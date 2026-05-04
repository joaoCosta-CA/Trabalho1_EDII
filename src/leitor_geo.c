#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "leitores/Include_leitorGeo/leitor_geo.h"

typedef struct
{
    double sw;
    char cfill[32];
    char cstrk[32];
} EstiloAtual;

static void monta_caminho_completo(const char *diretorio_base, const char *nome_arquivo,
                                   char *caminho_completo, int tamanho)
{
    if (nome_arquivo == NULL || nome_arquivo[0] == '\0')
    {
        caminho_completo[0] = '\0';
        return;
    }

    if (strchr(nome_arquivo, '/') || strchr(nome_arquivo, '\\'))
    {
        strncpy(caminho_completo, nome_arquivo, tamanho - 1);
        caminho_completo[tamanho - 1] = '\0';
    }
    else if (diretorio_base && strlen(diretorio_base) > 0)
    {
        snprintf(caminho_completo, tamanho, "%s/%s", diretorio_base, nome_arquivo);
    }
    else
    {
        strncpy(caminho_completo, nome_arquivo, tamanho - 1);
        caminho_completo[tamanho - 1] = '\0';
    }
}

/* Estado da estilização corrente mantida sequencialmente pelo arquivo */
static EstiloAtual estilo_padrao = {1.0, "white", "black"};

/* Funções auxiliares privadas */
static void tratar_comando_q(char *params, HashFile hf_quadras);
static void tratar_comando_cq(char *params);

/* Função Principal */
void ler_arquivo_geo(const char *diretorio_base, const char *nome_arquivo, HashFile hash_quadras)
{
    /* Reinicia o estilo para o padrão a cada novo arquivo .geo */
    estilo_padrao.sw = 1.0;
    strncpy(estilo_padrao.cfill, "white", sizeof(estilo_padrao.cfill) - 1);
    strncpy(estilo_padrao.cstrk, "black", sizeof(estilo_padrao.cstrk) - 1);

    char caminho_completo[512];
    monta_caminho_completo(diretorio_base, nome_arquivo, caminho_completo, sizeof(caminho_completo));

    FILE *arquivo = fopen(caminho_completo, "r");
    if (!arquivo)
    {
        perror("Erro ao abrir arquivo .geo");
        exit(EXIT_FAILURE);
    }

    char linha[1024];
    while (fgets(linha, sizeof(linha), arquivo))
    {
        /* Remove quebra de linha do final se existir */
        linha[strcspn(linha, "\r\n")] = 0;

        /* Separa o comando do restante dos parâmetros */
        char *comando = strtok(linha, " \t");
        /* Pega o resto da linha inalterado para processamento interno */
        char *params = strtok(NULL, "");

        if (!comando)
            continue;
        switch (comando[0])
        {
        case 'q':
            tratar_comando_q(params, hash_quadras);
            break;
        case 'c':
            if (strcmp(comando, "cq") == 0)
            {
                tratar_comando_cq(params);
            }
            break;
        }
    }

    fclose(arquivo);
}

static void tratar_comando_q(char *params, HashFile hf_quadras)
{
    /* q cep x y w h */
    char cep[30] = {0};
    double x = 0, y = 0, w = 0, h = 0;

    if (sscanf(params, "%29s %lf %lf %lf %lf", cep, &x, &y, &w, &h) == 5)
    {
        /* Serializa num formato separado por ;
           cep;x;y;w;h;sw;cfill;cstrk */
        char dados_serializados[250];

        snprintf(dados_serializados, sizeof(dados_serializados), "%s;%f;%f;%f;%f;%f;%s;%s",
                 cep, x, y, w, h,
                 estilo_padrao.sw, estilo_padrao.cfill, estilo_padrao.cstrk);
        hash_insert(hf_quadras, cep, dados_serializados);
    }
}

static void tratar_comando_cq(char *params)
{
    /* cq sw cfill cstrk */
    char *sw_t = strtok(params, " \t");
    char *fill_t = strtok(NULL, " \t");
    char *strk_t = strtok(NULL, " \t");

    if (sw_t && fill_t && strk_t)
    {
        estilo_padrao.sw = atof(sw_t);
        strncpy(estilo_padrao.cfill, fill_t, sizeof(estilo_padrao.cfill) - 1);
        strncpy(estilo_padrao.cstrk, strk_t, sizeof(estilo_padrao.cstrk) - 1);
    }
}