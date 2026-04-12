/**
 * @file main.c
 * @author João Henrique
 * @brief Programa principal TED — SIG de Bitnópolis.
 *
 * Fluxo:
 *  1. Parsear parâmetros de linha de comando
 *  2. Abrir hashfiles para quadras e pessoas
 *  3. Ler arquivo .geo  → popula hashfile de quadras
 *  4. Ler arquivo .pm   → popula hashfile de pessoas
 *  5. Ler arquivo .qry  → executa consultas/operações
 *  6. Gerar saídas (SVG, .hfd, relatórios)
 *  7. Fechar hashfiles e liberar memória
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "includes/parametros/parametros.h"
#include "includes/estr_dados/Include_HashFile/HashFile.h"
#include "includes/leitores/Include_leitorGeo/leitor_geo.h"

/**
 * Monta o caminho completo de um arquivo de saída no diretório BSD.
 * Exemplo: monta_caminho_saida("/tmp/out", "cidade", ".hf", buf, 512)
 *          → "/tmp/out/cidade.hf"
 */
static void monta_caminho_saida(const char *dir_saida, const char *base_name,
                                const char *extensao, char *out, int out_size)
{
    snprintf(out, out_size, "%s/%s%s", dir_saida, base_name, extensao);
}

/**
 * Extrai o nome base de um arquivo (sem diretório e sem extensão).
 * Ex: "dados/cidade01.geo" → "cidade01"
 */
static void extrair_nome_base(const char *caminho, char *base, int base_size)
{
    /* Encontra último separador de diretório */
    const char *inicio = strrchr(caminho, '/');
    if (inicio)
        inicio++;
    else
        inicio = caminho;

    strncpy(base, inicio, base_size - 1);
    base[base_size - 1] = '\0';

    /* Remove extensão */
    char *ponto = strrchr(base, '.');
    if (ponto)
        *ponto = '\0';
}

int main(int argc, char *argv[])
{
    /* --- 1. Parsear parâmetros --- */
    Parametros params = parametros_parse(argc, argv);
    if (!params)
    {
        return EXIT_FAILURE;
    }

    parametros_print(params);

    const char *dir_entrada = parametros_get_entrada(params);
    const char *arq_geo = parametros_get_geo(params);
    const char *dir_saida = parametros_get_saida(params);
    const char *arq_qry = parametros_get_qry(params);
    const char *arq_pm = parametros_get_pm(params);

    /* Criar diretório de saída se não existir */
    mkdir(dir_saida, 0755);

    /* Nome base para arquivos de saída (derivado do .geo) */
    char nome_base[256];
    extrair_nome_base(arq_geo, nome_base, sizeof(nome_base));

    /* --- 2. Abrir hashfiles --- */
    char path_hf_quadras[512], path_idx_quadras[512];
    monta_caminho_saida(dir_saida, nome_base, "-quadras.hf", path_hf_quadras, sizeof(path_hf_quadras));
    monta_caminho_saida(dir_saida, nome_base, "-quadras.hfc", path_idx_quadras, sizeof(path_idx_quadras));

    HashFile hf_quadras = hash_open(path_hf_quadras, path_idx_quadras);
    if (!hf_quadras)
    {
        fprintf(stderr, "Erro ao abrir hashfile de quadras.\n");
        parametros_destroy(params);
        return EXIT_FAILURE;
    }

    char path_hf_pessoas[512], path_idx_pessoas[512];
    monta_caminho_saida(dir_saida, nome_base, "-pessoas.hf", path_hf_pessoas, sizeof(path_hf_pessoas));
    monta_caminho_saida(dir_saida, nome_base, "-pessoas.hfc", path_idx_pessoas, sizeof(path_idx_pessoas));

    HashFile hf_pessoas = hash_open(path_hf_pessoas, path_idx_pessoas);
    if (!hf_pessoas)
    {
        fprintf(stderr, "Erro ao abrir hashfile de pessoas.\n");
        hash_close(hf_quadras);
        parametros_destroy(params);
        return EXIT_FAILURE;
    }

    /* --- 3. Ler arquivo .geo --- */
    printf("Lendo arquivo .geo: %s\n", arq_geo);
    ler_arquivo_geo(dir_entrada, arq_geo, hf_quadras);

    /* --- 4. Ler arquivo .pm (se fornecido) --- */
    if (arq_pm)
    {
        printf("Lendo arquivo .pm: %s\n", arq_pm);
        /* TODO: ler_arquivo_pm(dir_entrada, arq_pm, hf_pessoas); */
    }

    /* --- 5. Processar arquivo .qry (se fornecido) --- */
    if (arq_qry)
    {
        printf("Processando arquivo .qry: %s\n", arq_qry);
        /* TODO: ler_arquivo_qry(dir_entrada, arq_qry, hf_quadras, hf_pessoas, dir_saida, nome_base); */
    }

    /* --- 6. Gerar dump legível dos hashfiles (.hfd) --- */
    /* TODO: hash_dump(hf_quadras, path_hfd_quadras);  */
    /* TODO: hash_dump(hf_pessoas, path_hfd_pessoas);  */

    /* --- 7. Fechar tudo --- */
    hash_close(hf_quadras);
    hash_close(hf_pessoas);
    parametros_destroy(params);

    printf("Execução finalizada com sucesso.\n");
    return EXIT_SUCCESS;
}
