#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "leitores/Include_leitorQry/leitorQry.h"
#include "estr_dados/Include_list/list.h"

/* ─── Tipos internos ─────────────────────────────────────────────────────── */

typedef struct
{
    FILE *f_txt;
    HashFile hf_quadras;
    HashFile hf_pessoas;
    Lista decoracoes;
    char ultima_pq[50];
    char ultimo_cpf_op[50];
    char ultima_operacao[10];
} CtxQry;

typedef struct
{
    const char *target_cep;
    int count, count_n, count_s, count_l, count_o;
} PqState;

typedef struct
{
    int total_habitantes, moradores, moradores_m, moradores_f;
    int total_m, total_f, sem_teto, sem_teto_m, sem_teto_f;
} CensoState;

typedef struct
{
    const char *target_cep;
    FILE *f_txt;
    HashFile hf_pessoas;
} RqState;

/* ─── Utilitários ────────────────────────────────────────────────────────── */

/* Retorna o caractere da face aceitando "L" ou "Face.L" */
static char parse_face(const char *s)
{
    if (strncmp(s, "Face.", 5) == 0 && s[5] != '\0')
        return s[5];
    return s[0];
}

/* Insere string SVG alocada na lista de decorações */
static void decora(CtxQry *ctx, char *svg_str)
{
    insert(ctx->decoracoes, svg_str);
}

/* Calcula coordenadas (x,y) de um endereço na quadra */
static int calcular_coordenadas(HashFile hf_quadras, const char *cep,
                                char face, int num,
                                double *out_x, double *out_y)
{
    char buf[256];
    if (!hash_search(hf_quadras, cep, buf, sizeof(buf)))
        return 0;

    char *t = strtok(buf, ";"); /* campo cep */
    t = strtok(NULL, ";");
    double qx = t ? atof(t) : 0;
    t = strtok(NULL, ";");
    double qy = t ? atof(t) : 0;
    t = strtok(NULL, ";");
    double qw = t ? atof(t) : 0;
    t = strtok(NULL, ";");
    double qh = t ? atof(t) : 0;

    switch (face)
    {
    case 'N':
        *out_x = qx + num;
        *out_y = qy + qh;
        break;
    case 'S':
        *out_x = qx + num;
        *out_y = qy;
        break;
    case 'L':
        *out_x = qx + qw;
        *out_y = qy + num;
        break;
    case 'O':
        *out_x = qx;
        *out_y = qy + num;
        break;
    default:
        *out_x = qx + qw / 2;
        *out_y = qy + qh / 2;
    }
    return 1;
}

/* Rastreia última operação para a legenda SVG */
static void rastrear_operacao(CtxQry *ctx, const char *cpf, const char *op)
{
    strncpy(ctx->ultimo_cpf_op, cpf, 49);
    ctx->ultimo_cpf_op[49] = '\0';
    strncpy(ctx->ultima_operacao, op, 9);
    ctx->ultima_operacao[9] = '\0';
}

/* ─── Callbacks do hash_forall ───────────────────────────────────────────── */

static void pq_callback(const char *key, const char *value, void *userdata)
{
    (void)key;
    PqState *st = (PqState *)userdata;
    char buf[450];
    strncpy(buf, value, 449);
    buf[449] = '\0';

    strtok(buf, ";");
    strtok(NULL, ";");
    strtok(NULL, ";");
    strtok(NULL, ";");
    char *cep = strtok(NULL, ";");
    char *face = strtok(NULL, ";");

    if (!cep || strcmp(cep, st->target_cep) != 0)
        return;

    st->count++;
    if (!face)
        return;
    char f = parse_face(face);
    if (f == 'N' || f == 'n')
        st->count_n++;
    else if (f == 'S' || f == 's')
        st->count_s++;
    else if (f == 'L' || f == 'l')
        st->count_l++;
    else if (f == 'O' || f == 'o')
        st->count_o++;
}

static void censo_callback(const char *key, const char *value, void *userdata)
{
    (void)key;
    CensoState *st = (CensoState *)userdata;
    st->total_habitantes++;

    char buf[256];
    strncpy(buf, value, 255);
    buf[255] = '\0';

    strtok(buf, ";");
    strtok(NULL, ";");
    char *sexo = strtok(NULL, ";");
    strtok(NULL, ";");
    char *cep = strtok(NULL, ";");

    char s = (sexo && sexo[0]) ? sexo[0] : 'X';
    if (s == 'M' || s == 'm')
        st->total_m++;
    else if (s == 'F' || s == 'f')
        st->total_f++;

    if (cep && cep[0] != '\0')
    {
        st->moradores++;
        if (s == 'M' || s == 'm')
            st->moradores_m++;
        else if (s == 'F' || s == 'f')
            st->moradores_f++;
    }
    else
    {
        st->sem_teto++;
        if (s == 'M' || s == 'm')
            st->sem_teto_m++;
        else if (s == 'F' || s == 'f')
            st->sem_teto_f++;
    }
}

static void rq_callback(const char *key, const char *value, void *userdata)
{
    RqState *st = (RqState *)userdata;
    char buf[450];
    strncpy(buf, value, 449);
    buf[449] = '\0';

    char *nome = strtok(buf, ";");
    char *sobrenome = strtok(NULL, ";");
    char *sexo = strtok(NULL, ";");
    char *nasc = strtok(NULL, ";");
    char *cep = strtok(NULL, ";");

    if (cep && strcmp(cep, st->target_cep) == 0)
    {
        fprintf(st->f_txt, "rq: Morador afetado - CPF: %s, Nome: %s %s\n",
                key, nome, sobrenome ? sobrenome : "");
        char nova_str[450];
        snprintf(nova_str, sizeof(nova_str), "%s;%s;%s;%s;;;0;",
                 nome ? nome : "", sobrenome ? sobrenome : "",
                 sexo ? sexo : "", nasc ? nasc : "");
        hash_update(st->hf_pessoas, key, nova_str);
    }
}

/* ─── Handlers de cada comando ───────────────────────────────────────────── */

static void cmd_rq(char *params, CtxQry *ctx)
{
    char cep[50];
    if (sscanf(params, "%49s", cep) != 1)
        return;

    char buf[300];
    if (!hash_search(ctx->hf_quadras, cep, buf, sizeof(buf)))
    {
        fprintf(ctx->f_txt, "rq: Quadra %s não encontrada.\n", cep);
        return;
    }

    char *t = strtok(buf, ";");
    t = strtok(NULL, ";");
    double qx = t ? atof(t) : 0;
    t = strtok(NULL, ";");
    double qy = t ? atof(t) : 0;
    t = strtok(NULL, ";");
    double qw = t ? atof(t) : 0;
    t = strtok(NULL, ";");
    double qh = t ? atof(t) : 0;

    char *svg = malloc(512);
    snprintf(svg, 512,
             "<svg:line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"red\" stroke-width=\"2\"/>"
             "<svg:line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"red\" stroke-width=\"2\"/>",
             qx, qy, qx + qw, qy + qh, qx + qw, qy, qx, qy + qh);
    decora(ctx, svg);
    fprintf(ctx->f_txt, "rq: Quadra %s destruida.\n", cep);

    RqState st = {cep, ctx->f_txt, ctx->hf_pessoas};
    hash_forall(ctx->hf_pessoas, rq_callback, &st);
    hash_remove(ctx->hf_quadras, cep);
}

static void cmd_pq(char *params, CtxQry *ctx)
{
    char cep[50];
    if (sscanf(params, "%49s", cep) != 1)
        return;

    strncpy(ctx->ultima_pq, cep, 49);
    ctx->ultima_pq[49] = '\0';

    char buf[300];
    if (!hash_search(ctx->hf_quadras, cep, buf, sizeof(buf)))
    {
        fprintf(ctx->f_txt, "pq: Quadra %s não encontrada.\n", cep);
        return;
    }

    char *t = strtok(buf, ";");
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

    char *svg = malloc(2048);
    snprintf(svg, 2048,
             "<svg:g>\n"
             "<svg:circle cx=\"%f\" cy=\"%f\" r=\"7\" fill=\"white\" />\n"
             "<svg:text x=\"%f\" y=\"%f\" font-size=\"10\" fill=\"black\" text-anchor=\"middle\" alignment-baseline=\"middle\" font-weight=\"bold\">%d</svg:text>\n"
             "<svg:circle cx=\"%f\" cy=\"%f\" r=\"5\" fill=\"white\" />\n"
             "<svg:text x=\"%f\" y=\"%f\" font-size=\"8\" fill=\"blue\" text-anchor=\"middle\" alignment-baseline=\"middle\">%d</svg:text>\n"
             "<svg:circle cx=\"%f\" cy=\"%f\" r=\"5\" fill=\"white\" />\n"
             "<svg:text x=\"%f\" y=\"%f\" font-size=\"8\" fill=\"blue\" text-anchor=\"middle\" alignment-baseline=\"middle\">%d</svg:text>\n"
             "<svg:circle cx=\"%f\" cy=\"%f\" r=\"5\" fill=\"white\" />\n"
             "<svg:text x=\"%f\" y=\"%f\" font-size=\"8\" fill=\"blue\" text-anchor=\"middle\" alignment-baseline=\"middle\">%d</svg:text>\n"
             "<svg:circle cx=\"%f\" cy=\"%f\" r=\"5\" fill=\"white\" />\n"
             "<svg:text x=\"%f\" y=\"%f\" font-size=\"8\" fill=\"blue\" text-anchor=\"middle\" alignment-baseline=\"middle\">%d</svg:text>\n"
             "</svg:g>",
             qx + qw / 2, qy + qh / 2, qx + qw / 2, qy + qh / 2, st.count,
             qx + qw / 2, qy + qh - 5, qx + qw / 2, qy + qh - 5, st.count_n,
             qx + qw / 2, qy + 5, qx + qw / 2, qy + 5, st.count_s,
             qx + 5, qy + qh / 2, qx + 5, qy + qh / 2, st.count_l,
             qx + qw - 5, qy + qh / 2, qx + qw - 5, qy + qh / 2, st.count_o);
    decora(ctx, svg);
}

static void cmd_censo(CtxQry *ctx)
{
    CensoState st = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    hash_forall(ctx->hf_pessoas, censo_callback, &st);

    int th = st.total_habitantes;
    double prop_mor = th > 0 ? (double)st.moradores / th : 0.0;
    double pct_hab_m = th > 0 ? st.total_m * 100.0 / th : 0.0;
    double pct_hab_f = th > 0 ? st.total_f * 100.0 / th : 0.0;
    double pct_mor_m = st.moradores > 0 ? st.moradores_m * 100.0 / st.moradores : 0.0;
    double pct_mor_f = st.moradores > 0 ? st.moradores_f * 100.0 / st.moradores : 0.0;
    double pct_st_m = st.sem_teto > 0 ? st.sem_teto_m * 100.0 / st.sem_teto : 0.0;
    double pct_st_f = st.sem_teto > 0 ? st.sem_teto_f * 100.0 / st.sem_teto : 0.0;

    FILE *f = ctx->f_txt;
    fprintf(f, "censo:\n");
    fprintf(f, "  Total de habitantes: %d\n", th);
    fprintf(f, "  Total de moradores: %d\n", st.moradores);
    fprintf(f, "  Proporcao moradores/habitantes: %.4f\n", prop_mor);
    fprintf(f, "  Homens: %d\n", st.total_m);
    fprintf(f, "  Mulheres: %d\n", st.total_f);
    fprintf(f, "  %% habitantes homens: %.2f%%\n", pct_hab_m);
    fprintf(f, "  %% habitantes mulheres: %.2f%%\n", pct_hab_f);
    fprintf(f, "  %% moradores homens: %.2f%%\n", pct_mor_m);
    fprintf(f, "  %% moradores mulheres: %.2f%%\n", pct_mor_f);
    fprintf(f, "  Total de sem-tetos: %d\n", st.sem_teto);
    fprintf(f, "  %% sem-tetos homens: %.2f%%\n", pct_st_m);
    fprintf(f, "  %% sem-tetos mulheres: %.2f%%\n", pct_st_f);
}

static void cmd_h(char *params, CtxQry *ctx)
{
    char cpf[50], buf[300];
    if (sscanf(params, "%49s", cpf) != 1)
        return;

    if (!hash_search(ctx->hf_pessoas, cpf, buf, sizeof(buf)))
    {
        fprintf(ctx->f_txt, "h?: Habitante com CPF %s não encontrado.\n", cpf);
        return;
    }

    char *nome = strtok(buf, ";");
    char *sobrenome = strtok(NULL, ";");
    char *sexo = strtok(NULL, ";");
    char *nasc = strtok(NULL, ";");
    char *cep = strtok(NULL, ";");
    char *face = strtok(NULL, ";");
    char *num = strtok(NULL, ";");
    char *compl = strtok(NULL, ";");

    fprintf(ctx->f_txt,
            "h?: CPF %s - %s %s, Sexo: %s, Nasc: %s, Endereco: CEP %s, Face %s, Num %s, Compl %s\n",
            cpf, nome, sobrenome, sexo, nasc, cep, face, num, compl);
}

static void cmd_nasc(char *params, CtxQry *ctx)
{
    char cpf[50], nome[50], sobrenome[50], sexo[5], nasc[50];
    if (sscanf(params, "%49s %49s %49s %4s %49s", cpf, nome, sobrenome, sexo, nasc) < 5)
        return;

    char nova[300];
    snprintf(nova, sizeof(nova), "%s;%s;%s;%s;;;0;", nome, sobrenome, sexo, nasc);

    if (hash_insert(ctx->hf_pessoas, cpf, nova))
    {
        rastrear_operacao(ctx, cpf, "nasc");
        fprintf(ctx->f_txt, "nasc: Pessoa %s (%s) registrada\n", cpf, nome);
    }
}

static void cmd_rip(char *params, CtxQry *ctx)
{
    char cpf[50], buf[300];
    if (sscanf(params, "%49s", cpf) != 1)
        return;
    if (!hash_search(ctx->hf_pessoas, cpf, buf, sizeof(buf)))
        return;

    char *nome = strtok(buf, ";");
    for (int i = 0; i < 3; i++)
        strtok(NULL, ";"); /* sobrenome, sexo, nasc */
    char *cep = strtok(NULL, ";");
    char *face = strtok(NULL, ";");
    char *num_s = strtok(NULL, ";");

    if (cep && face && num_s)
    {
        double x = 0, y = 0;
        if (calcular_coordenadas(ctx->hf_quadras, cep, parse_face(face), atoi(num_s), &x, &y))
        {
            char *svg = malloc(256);
            snprintf(svg, 256,
                     "<svg:line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"red\" stroke-width=\"2\"/>"
                     "<svg:line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"red\" stroke-width=\"2\"/>",
                     x - 5, y, x + 5, y, x, y - 5, x, y + 5);
            decora(ctx, svg);
        }
    }

    rastrear_operacao(ctx, cpf, "rip");
    hash_remove(ctx->hf_pessoas, cpf);
    fprintf(ctx->f_txt, "rip: Morador %s (CPF %s) faleceu.\n", nome, cpf);
}

static void cmd_mud(char *params, CtxQry *ctx)
{
    char cpf[50], novo_cep[50], nova_face[20], novo_compl[100];
    int novo_num = 0;
    if (sscanf(params, "%49s %49s %19s %d %99s",
               cpf, novo_cep, nova_face, &novo_num, novo_compl) < 4)
        return;

    char buf[300];
    if (!hash_search(ctx->hf_pessoas, cpf, buf, sizeof(buf)))
        return;

    /* Extrai dados demográficos */
    char *nome = strtok(buf, ";");
    char *sobrenome = strtok(NULL, ";");
    char *sexo = strtok(NULL, ";");
    char *nasc = strtok(NULL, ";");

    /* Grava novo endereço no hash */
    char nova_str[300];
    snprintf(nova_str, sizeof(nova_str), "%s;%s;%s;%s;%s;%s;%d;%s",
             nome, sobrenome, sexo, nasc, novo_cep, nova_face, novo_num, novo_compl);
    hash_update(ctx->hf_pessoas, cpf, nova_str);

    char face_dest = parse_face(nova_face);

    /* Quadrado na posição de DESTINO (nova casa) */
    double dx = 0, dy = 0;
    if (calcular_coordenadas(ctx->hf_quadras, novo_cep, face_dest, novo_num, &dx, &dy))
    {
        char *svg = malloc(512);
        snprintf(svg, 512,
                 "<svg:rect width=\"10\" height=\"10\" x=\"%f\" y=\"%f\""
                 " fill=\"red\" stroke=\"red\" stroke-width=\"1\""
                 " fill-opacity=\"0.3\" stroke-dasharray=\"1,1\" />"
                 "<svg:text x=\"%f\" y=\"%f\" font-size=\"6\" fill=\"black\""
                 " text-anchor=\"middle\" alignment-baseline=\"middle\">%s</svg:text>",
                 dx - 5, dy - 5, dx, dy + 1, cpf);
        decora(ctx, svg);
    }

    fprintf(ctx->f_txt, "mud: Morador %s (CPF %s) moveu-se para o CEP %s\n",
            nome, cpf, novo_cep);
}

static void cmd_dspj(char *params, CtxQry *ctx)
{
    char cep_quadra[50], buf[300];
    if (sscanf(params, "%49s", cep_quadra) != 1)
        return;
    if (!hash_search(ctx->hf_pessoas, cep_quadra, buf, sizeof(buf)))
        return;

    char *nome = strtok(buf, ";");
    char *sobrenome = strtok(NULL, ";");
    char *sexo = strtok(NULL, ";");
    char *nasc = strtok(NULL, ";");
    char *cep = strtok(NULL, ";");
    char *face = strtok(NULL, ";");
    char *num_s = strtok(NULL, ";");

    if (cep && face && num_s)
    {
        double x = 0, y = 0;
        if (calcular_coordenadas(ctx->hf_quadras, cep, parse_face(face), atoi(num_s), &x, &y))
        {
            char *svg = malloc(256);
            snprintf(svg, 256,
                     "<svg:circle cx=\"%f\" cy=\"%f\" r=\"8\""
                     " fill=\"black\" stroke=\"black\" stroke-width=\"1\"/>",
                     x, y);
            decora(ctx, svg);
        }
    }

    rastrear_operacao(ctx, cep_quadra, "dspj");
    fprintf(ctx->f_txt,
            "dspj: Morador CPF %s - %s %s, Sexo: %s, Nasc: %s, CEP antigo: %s DESPEJADO!\n",
            cep_quadra, nome, sobrenome, sexo, nasc, cep);
    hash_remove(ctx->hf_pessoas, cep_quadra);
}

/* ─── Dispatcher principal ───────────────────────────────────────────────── */

static void tratar_comando_qry(char *comando, char *params, CtxQry *ctx)
{
    if (strcmp(comando, "rq") == 0)
        cmd_rq(params, ctx);
    else if (strcmp(comando, "pq") == 0)
        cmd_pq(params, ctx);
    else if (strcmp(comando, "censo") == 0)
        cmd_censo(ctx);
    else if (strcmp(comando, "h?") == 0)
        cmd_h(params, ctx);
    else if (strcmp(comando, "nasc") == 0)
        cmd_nasc(params, ctx);
    else if (strcmp(comando, "rip") == 0)
        cmd_rip(params, ctx);
    else if (strcmp(comando, "mud") == 0)
        cmd_mud(params, ctx);
    else if (strcmp(comando, "dspj") == 0)
        cmd_dspj(params, ctx);
    else
        printf("Comando ignorado ou inválido: %s\n", comando);
}

/* ─── Geração da legenda SVG ─────────────────────────────────────────────── */

static void gerar_legenda(CtxQry *ctx)
{
    if (ctx->ultimo_cpf_op[0] == '\0')
        return;

    const char *box_color = "white";
    const char *text_symbol = "";
    const char *text_color = "black";

    if (strncmp(ctx->ultima_operacao, "nasc", 4) == 0)
    {
        box_color = "#AAAAFF";
        text_symbol = "***";
        text_color = "#000080";
    }
    else if (strncmp(ctx->ultima_operacao, "rip", 3) == 0)
    {
        box_color = "black";
        text_symbol = "R.I.P.";
        text_color = "white";
    }
    else if (strncmp(ctx->ultima_operacao, "dspj", 4) == 0)
    {
        box_color = "#786721";
        text_symbol = "OUT";
        text_color = "#F4EED7";
    }

    char *box = malloc(512);
    snprintf(box, 512,
             "<!-- LEGENDA -->"
             "<svg:rect width=\"100\" height=\"50\" x=\"1460\" y=\"0\""
             " fill=\"%s\" stroke=\"black\" stroke-width=\"1\""
             " fill-opacity=\"1\" rx=\"20\" ry=\"20\" />",
             box_color);
    decora(ctx, box);

    char *t1 = malloc(256);
    snprintf(t1, 256,
             "<svg:text x=\"1510\" y=\"12\" fill=\"%s\" stroke=\"%s\""
             " font-size=\"14\" text-anchor=\"middle\">%s</svg:text>",
             text_color, text_color, text_symbol);
    decora(ctx, t1);

    char *t2 = malloc(256);
    snprintf(t2, 256,
             "<svg:text x=\"1510\" y=\"28\" fill=\"%s\" stroke=\"%s\""
             " font-size=\"9\" text-anchor=\"middle\">%s</svg:text>",
             text_color, text_color, ctx->ultimo_cpf_op);
    decora(ctx, t2);
}

/* ─── Ponto de entrada público ───────────────────────────────────────────── */

static void monta_caminho_completo(const char *diretorio_base, const char *nome_arquivo,
                                   char *caminho, int tamanho)
{
    if (nome_arquivo == NULL || nome_arquivo[0] == '\0')
    {
        caminho[0] = '\0';
        return;
    }

    if (strchr(nome_arquivo, '/') || strchr(nome_arquivo, '\\'))
    {
        strncpy(caminho, nome_arquivo, tamanho - 1);
        caminho[tamanho - 1] = '\0';
    }
    else if (diretorio_base && strlen(diretorio_base) > 0)
    {
        snprintf(caminho, tamanho, "%s/%s", diretorio_base, nome_arquivo);
    }
    else
    {
        strncpy(caminho, nome_arquivo, tamanho - 1);
        caminho[tamanho - 1] = '\0';
    }
}

LeitorQry leitor_qry(const char *diretorio_base, const char *nome_arquivo,
                     HashFile hf_quadras, HashFile hf_pessoas,
                     const char *dir_saida, const char *nome_base)
{
    char caminho[512];
    monta_caminho_completo(diretorio_base, nome_arquivo, caminho, sizeof(caminho));

    FILE *arquivo = fopen(caminho, "r");
    if (!arquivo)
        return NULL;

    char caminho_txt[512];
    snprintf(caminho_txt, sizeof(caminho_txt), "%s/%s.txt", dir_saida, nome_base);
    FILE *f_txt = fopen(caminho_txt, "w");

    CtxQry ctx = {
        .f_txt = f_txt,
        .hf_quadras = hf_quadras,
        .hf_pessoas = hf_pessoas,
        .decoracoes = createList(),
        .ultima_pq = {'\0'},
        .ultimo_cpf_op = {'\0'},
        .ultima_operacao = {'\0'},
    };

    char linha[1024];
    while (fgets(linha, sizeof(linha), arquivo))
    {
        linha[strcspn(linha, "\r\n")] = '\0';
        char *cmd = strtok(linha, " \t");
        char *params = strtok(NULL, "");

        if (cmd && f_txt)
        {
            fprintf(f_txt, "\n--- Comando: %s %s ---\n", cmd, params ? params : "");
            tratar_comando_qry(cmd, params, &ctx);
        }
    }

    fclose(arquivo);
    if (f_txt)
        fclose(f_txt);

    gerar_legenda(&ctx);

    return ctx.decoracoes;
}
