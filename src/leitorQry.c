#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "leitores/Include_leitorQry/leitorQry.h"
#include "estr_dados/Include_list/list.h"

typedef struct
{
    FILE *f_txt;
    HashFile hf_quadras;
    HashFile hf_pessoas;
    Lista decoracoes;
} CtxQry;

typedef struct
{
    const char *target_cep;
    int count;
    int count_n;
    int count_s;
    int count_l;
    int count_o;
} PqState;

static void pq_callback(const char *key, const char *value, void *userdata)
{
    (void)key;
    PqState *state = (PqState *)userdata;
    char buffer[450];
    strncpy(buffer, value, sizeof(buffer) - 1);
    buffer[449] = '\0';

    strtok(buffer, ";");            // nome
    strtok(NULL, ";");              // sobrenome
    strtok(NULL, ";");              // sexo
    strtok(NULL, ";");              // nasc
    char *cep = strtok(NULL, ";");  // cep
    char *face = strtok(NULL, ";"); // face

    if (cep && strcmp(cep, state->target_cep) == 0)
    {
        state->count++;
        if (face)
        {
            char f = face[0];
            if (strncmp(face, "Face.", 5) == 0 && strlen(face) >= 6)
            {
                f = face[5];
            }
            if (f == 'N' || f == 'n')
                state->count_n++;
            else if (f == 'S' || f == 's')
                state->count_s++;
            else if (f == 'L' || f == 'l')
                state->count_l++;
            else if (f == 'O' || f == 'o')
                state->count_o++;
        }
    }
}

typedef struct
{
    int total_habitantes;
    int moradores;
    int moradores_m;
    int moradores_f;
    int total_m;
    int total_f;
    int sem_teto;
    int sem_teto_m;
    int sem_teto_f;
} CensoState;

static void censo_callback(const char *key, const char *value, void *userdata){
    (void)key;
    CensoState *state = (CensoState *)userdata;
    state->total_habitantes++;
    
    char buffer[256];
    strncpy(buffer, value, sizeof(buffer) - 1);
    buffer[255] = '\0';

    strtok(buffer, ";");            // nome
    strtok(NULL, ";");              // sobrenome
    char *sexo = strtok(NULL, ";"); // sexo
    strtok(NULL, ";");              // nasc
    char *cep = strtok(NULL, ";");  // cep

    char sexo_char = 'X';
    if (sexo && sexo[0] != '\0')
    {
        sexo_char = sexo[0];
        if (sexo_char == 'M' || sexo_char == 'm')
            state->total_m++;
        else if (sexo_char == 'F' || sexo_char == 'f')
            state->total_f++;
    }

    if (cep && cep[0] != '\0')
    {
        state->moradores++;
        if (sexo_char == 'M' || sexo_char == 'm')
            state->moradores_m++;
        else if (sexo_char == 'F' || sexo_char == 'f')
            state->moradores_f++;
    }
    else
    {
        state->sem_teto++;
        if (sexo_char == 'M' || sexo_char == 'm')
            state->sem_teto_m++;
        else if (sexo_char == 'F' || sexo_char == 'f')
            state->sem_teto_f++;
    }
}

typedef struct
{
    const char *target_cep;
    FILE *f_txt;
} RqState;

static void rq_callback(const char *key, const char *value, void *userdata)
{
    RqState *state = (RqState *)userdata;
    char buffer[450];
    strncpy(buffer, value, sizeof(buffer) - 1);
    buffer[449] = '\0';

    char *nome = strtok(buffer, ";");
    char *sobrenome = strtok(NULL, ";"); // sobrenome
    strtok(NULL, ";");                   // sexo
    strtok(NULL, ";");                   // nasc
    char *cep = strtok(NULL, ";");       // cep

    if (cep && strcmp(cep, state->target_cep) == 0)
    {
        fprintf(state->f_txt, "rq: Morador afetado - CPF: %s, Nome: %s %s\n", key, nome, sobrenome ? sobrenome : "");
    }
}

static int calcular_coordenadas(HashFile hf_quadras, const char *cep, char face, int num, double *out_x, double *out_y)
{
    char buffer[256];
    if (!hash_search(hf_quadras, cep, buffer, sizeof(buffer)))
        return 0;

    char *t = strtok(buffer, ";"); // cep
    t = strtok(NULL, ";");
    double qx = t ? atof(t) : 0;
    t = strtok(NULL, ";");
    double qy = t ? atof(t) : 0;
    t = strtok(NULL, ";");
    double qw = t ? atof(t) : 0;
    t = strtok(NULL, ";");
    double qh = t ? atof(t) : 0;

    if (face == 'N')
    {
        *out_x = qx + num;
        *out_y = qy + qh;
    }
    else if (face == 'S')
    {
        *out_x = qx + num;
        *out_y = qy;
    }
    else if (face == 'L')
    {
        *out_x = qx;
        *out_y = qy + num;
    }
    else if (face == 'O')
    {
        *out_x = qx + qw;
        *out_y = qy + num;
    }
    else
    {
        *out_x = qx + qw / 2;
        *out_y = qy + qh / 2;
    } // Default center

    return 1;
}

static void tratar_comando_qry(char *comando, char *params, CtxQry *ctx)
{
    if (strcmp(comando, "rq") == 0)
    {
        char cep[50];
        if (sscanf(params, "%s", cep) == 1)
        {
            char buffer_quadra[300];
            if (hash_search(ctx->hf_quadras, cep, buffer_quadra, sizeof(buffer_quadra)))
            {
                char *t = strtok(buffer_quadra, ";"); // cep
                t = strtok(NULL, ";");
                double qx = t ? atof(t) : 0;
                t = strtok(NULL, ";");
                double qy = t ? atof(t) : 0;
                t = strtok(NULL, ";");
                double qw = t ? atof(t) : 0;
                t = strtok(NULL, ";");
                double qh = t ? atof(t) : 0;

                char *svg_str = malloc(512);
                snprintf(svg_str, 512, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"red\" stroke-width=\"2\"/>\n"
                                       "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"red\" stroke-width=\"2\"/>",
                         qx, qy, qx + qw, qy + qh,
                         qx + qw, qy, qx, qy + qh);
                insert(ctx->decoracoes, svg_str);

                fprintf(ctx->f_txt, "rq: Quadra %s destruida.\n", cep);

                RqState st = {cep, ctx->f_txt};
                hash_forall(ctx->hf_pessoas, rq_callback, &st);

                hash_remove(ctx->hf_quadras, cep);
            }
            else
            {
                fprintf(ctx->f_txt, "rq: Quadra %s não encontrada.\n", cep);
            }
        }
    }
    else if (strcmp(comando, "pq") == 0)
    {
        char cep[50];
        if (sscanf(params, "%s", cep) == 1)
        {
            char buffer_quadra[300];
            if (hash_search(ctx->hf_quadras, cep, buffer_quadra, sizeof(buffer_quadra)))
            {
                char *t = strtok(buffer_quadra, ";"); // cep
                t = strtok(NULL, ";");
                double qx = t ? atof(t) : 0;
                t = strtok(NULL, ";");
                double qy = t ? atof(t) : 0;
                t = strtok(NULL, ";");
                double qw = t ? atof(t) : 0;
                t = strtok(NULL, ";");
                double qh = t ? atof(t) : 0;

                PqState st = {cep, 0, 0, 0, 0, 0};
                hash_forall(ctx->hf_pessoas, pq_callback, &st);

                fprintf(ctx->f_txt, "pq: Quadra %s possui %d habitantes (N:%d S:%d L:%d O:%d).\n",
                        cep, st.count, st.count_n, st.count_s, st.count_l, st.count_o);

                char *svg_str = malloc(2048);
                snprintf(svg_str, 2048,
                         "<g>\n"
                         "<circle cx=\"%f\" cy=\"%f\" r=\"7\" fill=\"white\" />\n"
                         "<text x=\"%f\" y=\"%f\" font-size=\"10\" fill=\"black\" text-anchor=\"middle\" alignment-baseline=\"middle\" font-weight=\"bold\">%d</text>\n"
                         "<circle cx=\"%f\" cy=\"%f\" r=\"5\" fill=\"white\" />\n"
                         "<text x=\"%f\" y=\"%f\" font-size=\"8\" fill=\"blue\" text-anchor=\"middle\" alignment-baseline=\"middle\">%d</text>\n"
                         "<circle cx=\"%f\" cy=\"%f\" r=\"5\" fill=\"white\" />\n"
                         "<text x=\"%f\" y=\"%f\" font-size=\"8\" fill=\"blue\" text-anchor=\"middle\" alignment-baseline=\"middle\">%d</text>\n"
                         "<circle cx=\"%f\" cy=\"%f\" r=\"5\" fill=\"white\" />\n"
                         "<text x=\"%f\" y=\"%f\" font-size=\"8\" fill=\"blue\" text-anchor=\"middle\" alignment-baseline=\"middle\">%d</text>\n"
                         "<circle cx=\"%f\" cy=\"%f\" r=\"5\" fill=\"white\" />\n"
                         "<text x=\"%f\" y=\"%f\" font-size=\"8\" fill=\"blue\" text-anchor=\"middle\" alignment-baseline=\"middle\">%d</text>\n"
                         "</g>",
                         qx + qw / 2.0, qy + qh / 2.0, qx + qw / 2.0, qy + qh / 2.0, st.count,
                         qx + qw / 2.0, qy + qh - 5.0, qx + qw / 2.0, qy + qh - 5.0, st.count_n,
                         qx + qw / 2.0, qy + 5.0, qx + qw / 2.0, qy + 5.0, st.count_s,
                         qx + 5.0, qy + qh / 2.0, qx + 5.0, qy + qh / 2.0, st.count_l,
                         qx + qw - 5.0, qy + qh / 2.0, qx + qw - 5.0, qy + qh / 2.0, st.count_o);
                insert(ctx->decoracoes, svg_str);
            }
            else
            {
                fprintf(ctx->f_txt, "pq: Quadra %s não encontrada.\n", cep);
            }
        }
    }
    else if (strcmp(comando, "censo") == 0)
    {
        CensoState st = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        hash_forall(ctx->hf_pessoas, censo_callback, &st);
        
        double prop_moradores = st.total_habitantes > 0 ? (double)st.moradores / st.total_habitantes : 0.0;
        double pct_hab_m = st.total_habitantes > 0 ? (double)st.total_m * 100.0 / st.total_habitantes : 0.0;
        double pct_hab_f = st.total_habitantes > 0 ? (double)st.total_f * 100.0 / st.total_habitantes : 0.0;
        double pct_mor_m = st.moradores > 0 ? (double)st.moradores_m * 100.0 / st.moradores : 0.0;
        double pct_mor_f = st.moradores > 0 ? (double)st.moradores_f * 100.0 / st.moradores : 0.0;
        double pct_sem_teto_m = st.sem_teto > 0 ? (double)st.sem_teto_m * 100.0 / st.sem_teto : 0.0;
        double pct_sem_teto_f = st.sem_teto > 0 ? (double)st.sem_teto_f * 100.0 / st.sem_teto : 0.0;
        
        fprintf(ctx->f_txt, "censo:\n");
        fprintf(ctx->f_txt, "  Total de habitantes: %d\n", st.total_habitantes);
        fprintf(ctx->f_txt, "  Total de moradores: %d\n", st.moradores);
        fprintf(ctx->f_txt, "  Proporcao moradores/habitantes: %.4f\n", prop_moradores);
        fprintf(ctx->f_txt, "  Homens: %d\n", st.total_m);
        fprintf(ctx->f_txt, "  Mulheres: %d\n", st.total_f);
        fprintf(ctx->f_txt, "  %% habitantes homens: %.2f%%\n", pct_hab_m);
        fprintf(ctx->f_txt, "  %% habitantes mulheres: %.2f%%\n", pct_hab_f);
        fprintf(ctx->f_txt, "  %% moradores homens: %.2f%%\n", pct_mor_m);
        fprintf(ctx->f_txt, "  %% moradores mulheres: %.2f%%\n", pct_mor_f);
        fprintf(ctx->f_txt, "  Total de sem-tetos: %d\n", st.sem_teto);
        fprintf(ctx->f_txt, "  %% sem-tetos homens: %.2f%%\n", pct_sem_teto_m);
        fprintf(ctx->f_txt, "  %% sem-tetos mulheres: %.2f%%\n", pct_sem_teto_f);
    }
    else if (strcmp(comando, "h?") == 0)
    {
        char cpf[50], buffer[300];
        if (sscanf(params, "%s", cpf) == 1)
        {
            if (hash_search(ctx->hf_pessoas, cpf, buffer, sizeof(buffer)))
            {
                char *nome = strtok(buffer, ";");
                char *sobrenome = strtok(NULL, ";");
                char *sexo = strtok(NULL, ";");
                char *nasc = strtok(NULL, ";");
                char *cep = strtok(NULL, ";");
                char *face = strtok(NULL, ";");
                char *num = strtok(NULL, ";");
                char *compl = strtok(NULL, ";");
                fprintf(ctx->f_txt, "h?: CPF %s - %s %s, Sexo: %s, Nasc: %s, Endereço: CEP %s, Face %s, Nym %s, Compl %s\n",
                        cpf, nome, sobrenome, sexo, nasc, cep, face, num, compl);
            }
            else
            {
                fprintf(ctx->f_txt, "h?: Habitante com CPF %s não encontrado.\n", cpf);
            }
        }
    }
    else if (strcmp(comando, "nasc") == 0)
    {
        char cpf[50], nome[50], sobrenome[50], sexo[5], nasc[50], cep[50], face[5], compl[50];
        int num = 0;
        // nasc cpf nome sobrenome sexo data cep face num compl
        if (sscanf(params, "%s %s %s %s %s %s %s %d %s", cpf, nome, sobrenome, sexo, nasc, cep, face, &num, compl) >= 8)
        {
            char nova_string[300];
            snprintf(nova_string, sizeof(nova_string), "%s;%s;%s;%s;%s;%s;%d;%s",
                     nome, sobrenome, sexo, nasc, cep, face, num, compl);
            if (hash_insert(ctx->hf_pessoas, cpf, nova_string))
            {
                fprintf(ctx->f_txt, "nasc: Pessoa %s (%s) registrada no endereço %s\n", cpf, nome, cep);
            }
        }
    }
    else if (strcmp(comando, "rip") == 0)
    {
        char cpf[50], buffer[300];
        if (sscanf(params, "%s", cpf) == 1)
        {
            if (hash_search(ctx->hf_pessoas, cpf, buffer, sizeof(buffer)))
            {
                char *nome = strtok(buffer, ";");
                for (int i = 0; i < 3; i++)
                    strtok(NULL, ";"); // pula sobrenome, sexo, nasc
                char *cep = strtok(NULL, ";");
                char *face = strtok(NULL, ";");
                char *num_s = strtok(NULL, ";");

                double x = 0, y = 0;
                if (cep && face && num_s && calcular_coordenadas(ctx->hf_quadras, cep, face[0], atoi(num_s), &x, &y))
                {
                    char *svg_str = malloc(256);
                    snprintf(svg_str, 256, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"red\" stroke-width=\"2\"/>"
                                           "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"red\" stroke-width=\"2\"/>",
                             x - 5, y, x + 5, y, x, y - 5, x, y + 5);
                    insert(ctx->decoracoes, svg_str);
                }
                hash_remove(ctx->hf_pessoas, cpf);
                fprintf(ctx->f_txt, "rip: Morador %s (CPF %s) faleceu.\n", nome, cpf);
            }
        }
    }
    else if (strcmp(comando, "mud") == 0)
    {
        char cpf[50], novo_cep[50], nova_face[5], novo_compl[50];
        int novo_num = 0;
        if (sscanf(params, "%s %s %s %d %s", cpf, novo_cep, nova_face, &novo_num, novo_compl) >= 4)
        {
            char buffer[300];
            if (hash_search(ctx->hf_pessoas, cpf, buffer, sizeof(buffer)))
            {
                char *nome = strtok(buffer, ";");
                char *sobrenome = strtok(NULL, ";");
                char *sexo = strtok(NULL, ";");
                char *nasc = strtok(NULL, ";");

                char nova_string[300];
                snprintf(nova_string, sizeof(nova_string), "%s;%s;%s;%s;%s;%s;%d;%s",
                         nome, sobrenome, sexo, nasc, novo_cep, nova_face, novo_num, novo_compl);

                hash_update(ctx->hf_pessoas, cpf, nova_string);

                double x = 0, y = 0;
                if (calcular_coordenadas(ctx->hf_quadras, novo_cep, nova_face[0], novo_num, &x, &y))
                {
                    char *svg_str = malloc(512);
                    snprintf(svg_str, 512, "<rect x=\"%f\" y=\"%f\" width=\"20\" height=\"20\" fill=\"none\" stroke=\"red\"/>\n"
                                           "<text x=\"%f\" y=\"%f\" fill=\"red\" font-size=\"10\">%s</text>",
                             x - 10, y - 10, x - 10, y - 12, cpf);
                    insert(ctx->decoracoes, svg_str);
                }

                fprintf(ctx->f_txt, "mud: Morador %s (CPF %s) moveu-se para o CEP %s\n", nome, cpf, novo_cep);
            }
        }
    }
    else if (strcmp(comando, "dspj") == 0)
    {
        char cpf[50], buffer[300];
        if (sscanf(params, "%s", cpf) == 1)
        {
            if (hash_search(ctx->hf_pessoas, cpf, buffer, sizeof(buffer)))
            {
                char *nome = strtok(buffer, ";");
                char *sobrenome = strtok(NULL, ";");
                char *sexo = strtok(NULL, ";");
                char *nasc = strtok(NULL, ";");
                char *cep = strtok(NULL, ";");
                fprintf(ctx->f_txt, "dspj: Morador CPF %s - %s %s, Sexo: %s, Nasc: %s, CEP antigo: %s DESPEJADO!\n",
                        cpf, nome, sobrenome, sexo, nasc, cep);
                hash_remove(ctx->hf_pessoas, cpf);
            }
        }
    }
    else
    {
        printf("Comando ignorado ou inválido: %s\n", comando);
    }
}

LeitorQry leitor_qry(const char *diretorio_base, const char *nome_arquivo, HashFile hf_quadras, HashFile hf_pessoas, const char *dir_saida, const char *nome_base)
{
    char caminho_completo[512];
    if (diretorio_base && strlen(diretorio_base) > 0)
    {
        snprintf(caminho_completo, sizeof(caminho_completo), "%s/%s", diretorio_base, nome_arquivo);
    }
    else
    {
        strncpy(caminho_completo, nome_arquivo, sizeof(caminho_completo));
    }

    FILE *arquivo = fopen(caminho_completo, "r");
    if (!arquivo)
        return NULL;

    char caminho_txt[512];
    snprintf(caminho_txt, sizeof(caminho_txt), "%s/%s.txt", dir_saida, nome_base);
    FILE *f_txt = fopen(caminho_txt, "w");

    CtxQry ctx;
    ctx.f_txt = f_txt;
    ctx.hf_quadras = hf_quadras;
    ctx.hf_pessoas = hf_pessoas;
    ctx.decoracoes = createList();

    char linha[1024];
    while (fgets(linha, sizeof(linha), arquivo))
    {
        linha[strcspn(linha, "\r\n")] = 0;
        char *comando = strtok(linha, " \t");
        char *params = strtok(NULL, "");

        if (comando && f_txt)
        {
            fprintf(f_txt, "\n--- Comando: %s %s ---\n", comando, params ? params : "");
            tratar_comando_qry(comando, params, &ctx);
        }
    }

    fclose(arquivo);
    if (f_txt)
        fclose(f_txt);

    return ctx.decoracoes;
}
