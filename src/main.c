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

#include "parametros/parametros.h"
#include "estr_dados/Include_HashFile/HashFile.h"
#include "leitores/Include_leitorGeo/leitor_geo.h"
#include "leitores/Include_leitorPm/leitorPM.h"
#include "leitores/Include_leitorQry/leitorQry.h"
#include "svg/svg.h"

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
        ler_arquivo_pm(dir_entrada, arq_pm, hf_pessoas);
    }

    /* --- 5. Processar arquivo .qry (se fornecido) --- */
    if (arq_qry)
    {
        printf("Processando arquivo .qry: %s\n", arq_qry);
        /* leitor_qry processa comandos e altera o estado em hf_quadras */
        leitor_qry(dir_entrada, arq_qry, hf_quadras);
    }

    /* --- 6. Gerar SVG Base --- */
    char path_svg[512];
    monta_caminho_saida(dir_saida, nome_base, "-quadras.svg", path_svg, sizeof(path_svg));
    printf("Gerando SVG das quadras em: %s\n", path_svg);
    
    double min_x, min_y, max_x, max_y;
    svg_calcular_bbox(hf_quadras, &min_x, &min_y, &max_x, &max_y);
    
    // Adiciona uma margem para evitar cortes na borda das figuras
    double margin = 20.0;
    double vw = (max_x - min_x) + 2 * margin;
    double vh = (max_y - min_y) + 2 * margin;
    double vx = min_x - margin;
    double vy = min_y - margin;

    FILE* f_svg = svg_iniciar(path_svg, vx, vy, vw, vh);
    if (f_svg) {
        svg_gerar_quadras(hf_quadras, f_svg);
        svg_fechar(f_svg);
    }
    
    /* --- 7. Gerar dump legível dos hashfiles (.hfd) --- */
    /* TODO: hash_dump(hf_quadras, path_hfd_quadras);  */
    /* TODO: hash_dump(hf_pessoas, path_hfd_pessoas);  */

    /* --- 8. Fechar tudo --- */
    hash_close(hf_quadras);
    hash_close(hf_pessoas);
    parametros_destroy(params);

    printf("Execução finalizada com sucesso.\n");
    return EXIT_SUCCESS;
}
