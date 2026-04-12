#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "leitor_geo.h"

typedef struct
{
    double sw;
    char cfill[32];
    char cstrk[32];
} EstiloAtual;

/* Funções auxiliares privadas */
static void tratar_comando_q(char *params, void *hash_quadras);
static void tratar_comando_cq(char *params, void *hash_quadras);

/* Função Principal */
void ler_arquivo_geo(const char *diretorio_base, const char *nome_arquivo, void *hash_quadras)
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
        char *params = strtok(NULL, ""); // Pega o resto da linha inalterado para processamento interno

        if (!comando)
            continue;
        switch (comando[0])
        {
        case 'q':
            tratar_comando_q(params, hash_quadras);
            break;
        case 'c':
            if(strcmp(comando, "cq") == 0){
                tratar_comando_cq(params, hash_quadras);
            }
            break;
        }
    }

    fclose(arquivo);
}

static void tratar_comando_q(char *params, void *hass_quadras){}

static void tratar_comando_cq(char *params, void *hash_quadras){}