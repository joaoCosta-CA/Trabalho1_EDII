#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parametros.h"

typedef struct
{
    char dir_entrada[512]; /* -e  (BED)          */
    char arq_geo[256];     /* -f  (obrigatório)  */
    char dir_saida[512];   /* -o  (obrigatório)  */
    char arq_qry[256];     /* -q  (opcional)     */
    char arq_pm[256];      /* -pm (opcional)     */
} InternalParametros;

static void imprimir_uso(void)
{
    fprintf(stderr,
            "Uso: ted -f <arq.geo> -o <dir_saida> [-e <dir_entrada>] [-q <arq.qry>] [-pm <arq.pm>]\n"
            "\n"
            "  -f   Arquivo .geo com a descrição da cidade       (obrigatório)\n"
            "  -o   Diretório-base de saída                      (obrigatório)\n"
            "  -e   Diretório-base de entrada                    (opcional)\n"
            "  -q   Arquivo .qry com consultas                   (opcional)\n"
            "  -pm  Arquivo .pm com pessoas e moradores          (opcional)\n");
}

Parametros parametros_parse(int argc, char *argv[])
{
    InternalParametros *p = calloc(1, sizeof(InternalParametros));
    if (!p)
        return NULL;

    int tem_geo = 0;
    int tem_saida = 0;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-e") == 0 && i + 1 < argc)
        {
            strncpy(p->dir_entrada, argv[++i], sizeof(p->dir_entrada) - 1);
        }
        else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc)
        {
            strncpy(p->arq_geo, argv[++i], sizeof(p->arq_geo) - 1);
            tem_geo = 1;
        }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc)
        {
            strncpy(p->dir_saida, argv[++i], sizeof(p->dir_saida) - 1);
            tem_saida = 1;
        }
        else if (strcmp(argv[i], "-q") == 0 && i + 1 < argc)
        {
            strncpy(p->arq_qry, argv[++i], sizeof(p->arq_qry) - 1);
        }
        else if (strcmp(argv[i], "-pm") == 0 && i + 1 < argc)
        {
            strncpy(p->arq_pm, argv[++i], sizeof(p->arq_pm) - 1);
        }
        else
        {
            fprintf(stderr, "Parâmetro desconhecido: %s\n", argv[i]);
            imprimir_uso();
            free(p);
            return NULL;
        }
    }

    if (!tem_geo)
    {
        fprintf(stderr, "Erro: parâmetro obrigatório -f (arquivo .geo) não fornecido.\n");
        imprimir_uso();
        free(p);
        return NULL;
    }

    if (!tem_saida)
    {
        fprintf(stderr, "Erro: parâmetro obrigatório -o (diretório de saída) não fornecido.\n");
        imprimir_uso();
        free(p);
        return NULL;
    }

    return (Parametros)p;
}

const char *parametros_get_entrada(Parametros param)
{
    InternalParametros *p = (InternalParametros *)param;
    return p->dir_entrada;
}

const char *parametros_get_geo(Parametros param)
{
    InternalParametros *p = (InternalParametros *)param;
    return p->arq_geo;
}

const char *parametros_get_saida(Parametros param)
{
    InternalParametros *p = (InternalParametros *)param;
    return p->dir_saida;
}

const char *parametros_get_qry(Parametros param)
{
    InternalParametros *p = (InternalParametros *)param;
    if (p->arq_qry[0] == '\0')
        return NULL;
    return p->arq_qry;
}

const char *parametros_get_pm(Parametros param)
{
    InternalParametros *p = (InternalParametros *)param;
    if (p->arq_pm[0] == '\0')
        return NULL;
    return p->arq_pm;
}

void parametros_print(Parametros param)
{
    InternalParametros *p = (InternalParametros *)param;
    printf("=== Parâmetros ===\n");
    printf("  Dir. Entrada (BED): %s\n", p->dir_entrada[0] ? p->dir_entrada : "(não informado)");
    printf("  Arquivo .geo:       %s\n", p->arq_geo);
    printf("  Dir. Saída (BSD):   %s\n", p->dir_saida);
    printf("  Arquivo .qry:       %s\n", p->arq_qry[0] ? p->arq_qry : "(não informado)");
    printf("  Arquivo .pm:        %s\n", p->arq_pm[0] ? p->arq_pm : "(não informado)");
    printf("==================\n");
}

void parametros_destroy(Parametros param)
{
    if (param)
        free(param);
}
