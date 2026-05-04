#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/estr_dados/Include_HashFile/HashFile.h"

// --- Definições Internas (Privadas) ---
#define BUCKET_SIZE 3
#define KEY_SIZE 50

typedef struct
{
    char key[KEY_SIZE];
    char data[450];
} Record;

typedef struct
{
    int local_depth;
    int count;
    Record records[BUCKET_SIZE];
} Bucket;

typedef struct
{
    int global_depth;
    long *bucket_offsets;
} Directory;

typedef struct
{
    Directory *dir;
    FILE *fp;
    char idx_path[256];
    char *expansions_log;
    int expansions_log_size;
} InternalHashFile;

/**
 * Função de hash para strings: soma os valores dos caracteres
 * e aplica a máscara de bits conforme a profundidade.
 */
static unsigned int internal_hash(const char *key, int depth)
{
    unsigned int h = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        h = h * 31 + (unsigned char)key[i];
    }
    if (depth == 0)
        return 0;
    return h & ((unsigned int)((1 << depth) - 1));
}

/**
 * Máscara de bits para índice inteiro do diretório (usada internamente no split).
 */
static int internal_hash_index(int index, int depth)
{
    if (depth == 0)
        return 0;
    return index & ((1 << depth) - 1);
}

static void internal_redistribute(FILE *fp, long old_offset, long new_offset, int new_depth)
{

    Bucket old_b, new_b;
    memset(&new_b, 0, sizeof(Bucket));

    fseek(fp, old_offset, SEEK_SET);
    fread(&old_b, sizeof(Bucket), 1, fp);

    new_b.local_depth = new_depth;
    new_b.count = 0;

    Record staying[BUCKET_SIZE];
    memset(staying, 0, sizeof(staying));
    int s_count = 0;

    for (int i = 0; i < old_b.count; i++)
    {
        unsigned int h = internal_hash(old_b.records[i].key, new_depth);
        if ((h >> (new_depth - 1)) & 1)
        {
            new_b.records[new_b.count++] = old_b.records[i];
        }
        else
        {
            staying[s_count++] = old_b.records[i];
        }
    }

    old_b.local_depth = new_depth;
    old_b.count = s_count;
    memcpy(old_b.records, staying, sizeof(Record) * (size_t)s_count);

    fseek(fp, old_offset, SEEK_SET);
    fwrite(&old_b, sizeof(Bucket), 1, fp);
    fseek(fp, new_offset, SEEK_SET);
    fwrite(&new_b, sizeof(Bucket), 1, fp);
}

static void internal_split(HashFile hf, int index)
{
    InternalHashFile *ihf = (InternalHashFile *)hf;

    Directory *dir = ihf->dir;
    FILE *fp = ihf->fp;

    long old_offset = dir->bucket_offsets[index];
    Bucket temp;
    fseek(fp, old_offset, SEEK_SET);
    fread(&temp, sizeof(Bucket), 1, fp);

    int new_l_depth = temp.local_depth + 1;

    if (new_l_depth > dir->global_depth)
    {
        int old_size = 1 << dir->global_depth;
        dir->global_depth++;
        int new_size = 1 << dir->global_depth;
        dir->bucket_offsets = realloc(dir->bucket_offsets, sizeof(long) * (size_t)new_size);
        for (int i = 0; i < old_size; i++)
            dir->bucket_offsets[i + old_size] = dir->bucket_offsets[i];
    }

    fseek(fp, 0, SEEK_END);
    long new_offset = ftell(fp);
    Bucket nb;
    memset(&nb, 0, sizeof(Bucket));
    nb.local_depth = new_l_depth;
    nb.count = 0;
    fwrite(&nb, sizeof(Bucket), 1, fp);

    int step = 1 << new_l_depth;
    int first = internal_hash_index(index, temp.local_depth) + (1 << temp.local_depth);
    for (int i = first; i < (1 << dir->global_depth); i += step)
        dir->bucket_offsets[i] = new_offset;

    internal_redistribute(fp, old_offset, new_offset, new_l_depth);

    char log_entry[128];
    snprintf(log_entry, sizeof(log_entry), "Split no bloco %d (nova prof. local: %d)\r\n", index, new_l_depth);
    int len = strlen(log_entry);
    if (!ihf->expansions_log) {
        ihf->expansions_log = malloc(len + 1);
        strcpy(ihf->expansions_log, log_entry);
        ihf->expansions_log_size = len + 1;
    } else {
        ihf->expansions_log = realloc(ihf->expansions_log, ihf->expansions_log_size + len);
        strcat(ihf->expansions_log, log_entry);
        ihf->expansions_log_size += len;
    }
}

HashFile hash_open(const char *dat_path, const char *idx_path)
{
    InternalHashFile *hf = malloc(sizeof(InternalHashFile));
    strncpy(hf->idx_path, idx_path, 255);
    hf->idx_path[255] = '\0';

    hf->fp = fopen(dat_path, "rb+");
    FILE *fidx = fopen(idx_path, "rb");

    if (!hf->fp || !fidx)
    {
        if (hf->fp) fclose(hf->fp);
        if (fidx) fclose(fidx);

        hf->fp = fopen(dat_path, "wb+");
        hf->dir = malloc(sizeof(Directory));
        hf->dir->global_depth = 0;
        hf->dir->bucket_offsets = malloc(sizeof(long));

        Bucket first;
        memset(&first, 0, sizeof(Bucket));
        first.local_depth = 0;
        first.count = 0;
        fwrite(&first, sizeof(Bucket), 1, hf->fp);
        hf->dir->bucket_offsets[0] = 0;
    }
    else
    {
        hf->dir = malloc(sizeof(Directory));
        fread(&hf->dir->global_depth, sizeof(int), 1, fidx);
        int size = 1 << hf->dir->global_depth;
        hf->dir->bucket_offsets = malloc(sizeof(long) * (size_t)size);
        fread(hf->dir->bucket_offsets, sizeof(long), (size_t)size, fidx);
        fclose(fidx);
    }
    hf->expansions_log = NULL;
    hf->expansions_log_size = 0;
    return hf;
}

int hash_insert(HashFile hf, const char *key, const char *value)
{
    InternalHashFile *ihf = (InternalHashFile *)hf;

    unsigned int idx = internal_hash(key, ihf->dir->global_depth);
    long offset = ihf->dir->bucket_offsets[idx];

    Bucket b;
    fseek(ihf->fp, offset, SEEK_SET);
    fread(&b, sizeof(Bucket), 1, ihf->fp);

    /* Verifica duplicata — hash_search com NULL é seguro agora */
    if (hash_search(hf, key, NULL, 0))
    {
        return 0;
    }

    if (b.count < BUCKET_SIZE)
    {
        strncpy(b.records[b.count].key, key, KEY_SIZE - 1);
        b.records[b.count].key[KEY_SIZE - 1] = '\0';
        strncpy(b.records[b.count].data, value, 449);
        b.records[b.count].data[449] = '\0';
        b.count++;
        fseek(ihf->fp, offset, SEEK_SET);
        fwrite(&b, sizeof(Bucket), 1, ihf->fp);
        return 1;
    }
    else
    {
        internal_split(hf, (int)idx);
        return hash_insert(hf, key, value);
    }
}

int hash_search(HashFile hf, const char *key, char *out_buffer, int buffer_size)
{
    InternalHashFile *ihf = (InternalHashFile *)hf;

    unsigned int idx = internal_hash(key, ihf->dir->global_depth);
    long offset = ihf->dir->bucket_offsets[idx];

    Bucket b;
    fseek(ihf->fp, offset, SEEK_SET);
    fread(&b, sizeof(Bucket), 1, ihf->fp);

    for (int i = 0; i < b.count; i++)
    {
        if (strcmp(b.records[i].key, key) == 0)
        {
            /* Correção do crash: só copia se o buffer não for NULL */
            if (out_buffer != NULL && buffer_size > 0)
            {
                strncpy(out_buffer, b.records[i].data, (size_t)(buffer_size - 1));
                out_buffer[buffer_size - 1] = '\0';
            }
            return 1;
        }
    }
    return 0;
}

void hash_close(HashFile hf)
{
    InternalHashFile *ihf = (InternalHashFile *)hf;

    FILE *fidx = fopen(ihf->idx_path, "wb");
    fwrite(&ihf->dir->global_depth, sizeof(int), 1, fidx);
    int size = 1 << ihf->dir->global_depth;
    fwrite(ihf->dir->bucket_offsets, sizeof(long), (size_t)size, fidx);
    fclose(fidx);

    fclose(ihf->fp);
    if (ihf->expansions_log)
        free(ihf->expansions_log);
    free(ihf->dir->bucket_offsets);
    free(ihf->dir);
    free(ihf);
}

int hash_remove(HashFile hf, const char *key)
{
    InternalHashFile *ihf = (InternalHashFile *)hf;
    if (!ihf)
        return 0;

    unsigned int idx = internal_hash(key, ihf->dir->global_depth);
    long offset = ihf->dir->bucket_offsets[idx];

    Bucket b;
    fseek(ihf->fp, offset, SEEK_SET);
    if (fread(&b, sizeof(Bucket), 1, ihf->fp) != 1)
        return 0;

    int found_idx = -1;
    for (int i = 0; i < b.count; i++)
    {
        if (strcmp(b.records[i].key, key) == 0)
        {
            found_idx = i;
            break;
        }
    }

    if (found_idx != -1)
    {
        for (int i = found_idx; i < b.count - 1; i++)
        {
            b.records[i] = b.records[i + 1];
        }
        b.count--;

        fseek(ihf->fp, offset, SEEK_SET);
        fwrite(&b, sizeof(Bucket), 1, ihf->fp);

        return 1;
    }

    return 0;
}


int hash_update(HashFile hf, const char *key, const char *new_value)
{
    InternalHashFile *ihf = (InternalHashFile *)hf;
    if (!ihf || !new_value)
        return 0;

    unsigned int idx = internal_hash(key, ihf->dir->global_depth);
    long offset = ihf->dir->bucket_offsets[idx];

    Bucket b;
    fseek(ihf->fp, offset, SEEK_SET);
    fread(&b, sizeof(Bucket), 1, ihf->fp);

    for (int i = 0; i < b.count; i++)
    {
        if (strcmp(b.records[i].key, key) == 0)
        {
            strncpy(b.records[i].data, new_value, 449);
            b.records[i].data[449] = '\0';
            fseek(ihf->fp, offset, SEEK_SET);
            fwrite(&b, sizeof(Bucket), 1, ihf->fp);
            return 1;
        }
    }
    return 0;
}

void hash_forall(HashFile hf, void (*callback)(const char *key, const char *value, void *userdata), void *userdata)
{
    InternalHashFile *ihf = (InternalHashFile *)hf;
    if (!ihf || !callback)
        return;

    int num_indices = 1 << ihf->dir->global_depth;
    /* Caso global_depth == 0, num_indices = 1, alocamos pelo menos 1 */
    long *visited_offsets = malloc(sizeof(long) * (size_t)(num_indices > 0 ? num_indices : 1));
    int visited_count = 0;

    for (int i = 0; i < num_indices; i++)
    {
        long current_offset = ihf->dir->bucket_offsets[i];

        int already_visited = 0;
        for (int j = 0; j < visited_count; j++)
        {
            if (visited_offsets[j] == current_offset)
            {
                already_visited = 1;
                break;
            }
        }

        if (!already_visited)
        {
            Bucket b;
            fseek(ihf->fp, current_offset, SEEK_SET);
            if (fread(&b, sizeof(Bucket), 1, ihf->fp) == 1)
            {
                for (int r = 0; r < b.count; r++)
                {
                    callback(b.records[r].key, b.records[r].data, userdata);
                }
            }
            visited_offsets[visited_count++] = current_offset;
        }
    }

    free(visited_offsets);
}

static void write_utf16le_string(FILE *f, const char *str)
{
    for (int i = 0; str[i]; i++) {
        unsigned char c = (unsigned char)str[i];
        fputc(c, f);
        fputc(0, f);
    }
}

void hash_dump(HashFile hf, const char *hfd_path)
{
    InternalHashFile *ihf = (InternalHashFile *)hf;
    if (!ihf || !ihf->dir || !hfd_path)
        return;

    FILE *out = fopen(hfd_path, "wb");
    if (!out)
        return;

    Directory *dir = ihf->dir;
    int num_indices = 1 << dir->global_depth;

    /* BOM UTF-16LE */
    unsigned char bom[] = {0xFF, 0xFE};
    fwrite(bom, 1, 2, out);

    char buf[1024];

    /* "DUMP\r\n" */
    write_utf16le_string(out, "DUMP\r\n");

    /* *Dump cabecalho */
    write_utf16le_string(out, "*Dump cabecalho\r\n");
    
    snprintf(buf, sizeof(buf), "numBucketsd %d \r\n", num_indices);
    write_utf16le_string(out, buf);
    
    snprintf(buf, sizeof(buf), "sizeRecordd %d \r\n", KEY_SIZE);
    write_utf16le_string(out, buf);
    
    snprintf(buf, sizeof(buf), "sizeBlock 2048 \r\n");
    write_utf16le_string(out, buf);
    
    snprintf(buf, sizeof(buf), "offsetKey 0 \r\n");
    write_utf16le_string(out, buf);
    
    snprintf(buf, sizeof(buf), "sizeKey 4 \r\n");
    write_utf16le_string(out, buf);
    
    snprintf(buf, sizeof(buf), "offsetTable 46 \r\n");
    write_utf16le_string(out, buf);
    
    snprintf(buf, sizeof(buf), "offsetBuckets %ld \r\n", 
             (long)(46 + num_indices * 8));
    write_utf16le_string(out, buf);
    
    snprintf(buf, sizeof(buf), "offsetOverflow -1\r\n");
    write_utf16le_string(out, buf);

    /* *Dump table */
    write_utf16le_string(out, "* Dump table\r\n");

    for (int i = 0; i < num_indices; i++)
    {
        snprintf(buf, sizeof(buf), "[%d] %ld\r\n", i, dir->bucket_offsets[i]);
        write_utf16le_string(out, buf);
    }

    /* *Dump buckets */
    write_utf16le_string(out, "*Dump buckets\r\n");

    /* Aloca array de offsets visitados */
    long *visited = malloc(sizeof(long) * (size_t)(num_indices > 0 ? num_indices : 1));
    int visited_count = 0;

    for (int i = 0; i < num_indices; i++)
    {
        long current_offset = dir->bucket_offsets[i];

        /* Pula se já imprimimos este bucket */
        int already = 0;
        for (int j = 0; j < visited_count; j++)
        {
            if (visited[j] == current_offset)
            {
                already = 1;
                break;
            }
        }
        if (already)
            continue;

        visited[visited_count++] = current_offset;

        Bucket b;
        fseek(ihf->fp, current_offset, SEEK_SET);
        if (fread(&b, sizeof(Bucket), 1, ihf->fp) != 1)
            continue;

        snprintf(buf, sizeof(buf), "BLOCO: %d\r\n", i);
        write_utf16le_string(out, buf);

        for (int r = 0; r < b.count; r++)
        {
            snprintf(buf, sizeof(buf), "%d | %s | %s | 0.000000 |\r\n",
                    1, b.records[r].key, b.records[r].data);
            write_utf16le_string(out, buf);
        }
    }

    write_utf16le_string(out, "\r\n*Expansoes\r\n");
    if (ihf->expansions_log) {
        write_utf16le_string(out, ihf->expansions_log);
    }
    write_utf16le_string(out, "FIM DUMP");

    free(visited);
    fclose(out);
}