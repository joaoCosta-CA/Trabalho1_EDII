/**
 * @file testSvg.c
 * @brief Testes unitários para o módulo SVG.
 *
 * Cobre: inicialização e geração básica, cálculo de bounding box,
 * svg_iniciar com caminho inválido, svg_fechar com NULL, e
 * svg_gerar_quadras com HashFile vazio.
 */
#include "unity/unity.h"
#include "svg/svg.h"
#include "estr_dados/Include_list/list.h"
#include "estr_dados/Include_HashFile/HashFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

/* Arquivos temporários usados pelos testes */
#define SVG_TMP      "test_out.svg"
#define HF_TMP_DAT   "test_svg_q.hf"
#define HF_TMP_IDX   "test_svg_q.idx"

void setUp(void) {}

void tearDown(void) {
    remove(SVG_TMP);
    remove(HF_TMP_DAT);
    remove(HF_TMP_IDX);
}

/* ─── Geração básica ──────────────────────────────────────────────────────── */

void test_SvgInitAndGeneration(void) {
    HashFile hf = hash_open(HF_TMP_DAT, HF_TMP_IDX);
    hash_insert(hf, "cep1", "cep1;10.0;10.0;100.0;100.0;2.0;red;blue");

    FILE *f = svg_iniciar(SVG_TMP, 0, 0, 500, 500);
    TEST_ASSERT_NOT_NULL(f);

    svg_gerar_quadras(hf, f);

    Lista dec = createList();
    char *str = malloc(100);
    snprintf(str, 100, "<line x1=\"0\" y1=\"0\" x2=\"10\" y2=\"10\"/>");
    insert(dec, str);

    svg_gerar_decoracoes(dec, f);
    killList(dec, free);

    svg_fechar(f);
    hash_close(hf);

    struct stat st;
    TEST_ASSERT_EQUAL_INT(0, stat(SVG_TMP, &st));
    TEST_ASSERT_TRUE(st.st_size > 0);
}

/* ─── Bounding box ────────────────────────────────────────────────────────── */

void test_SvgCalcularBbox(void) {
    HashFile hf = hash_open(HF_TMP_DAT, HF_TMP_IDX);
    /* Quadra em (10,20) com w=100, h=50  →  max_x=110, max_y=70 */
    hash_insert(hf, "cep1", "cep1;10.0;20.0;100.0;50.0;1.0;white;black");
    /* Quadra em (5,5)  com w=30,  h=30   →  max_x=35,  max_y=35  */
    hash_insert(hf, "cep2", "cep2;5.0;5.0;30.0;30.0;1.0;white;black");

    double min_x, min_y, max_x, max_y;
    svg_calcular_bbox(hf, &min_x, &min_y, &max_x, &max_y);

    TEST_ASSERT_EQUAL_DOUBLE(5.0,  min_x);
    TEST_ASSERT_EQUAL_DOUBLE(5.0,  min_y);
    TEST_ASSERT_EQUAL_DOUBLE(110.0, max_x);
    TEST_ASSERT_EQUAL_DOUBLE(70.0,  max_y);

    hash_close(hf);
}

void test_SvgBboxHashVazio_UsaDefault(void) {
    HashFile hf = hash_open(HF_TMP_DAT, HF_TMP_IDX);

    double min_x, min_y, max_x, max_y;
    /* Sem registros, deve retornar valores default (0,0,1000,1000) */
    svg_calcular_bbox(hf, &min_x, &min_y, &max_x, &max_y);

    TEST_ASSERT_EQUAL_DOUBLE(0.0,    min_x);
    TEST_ASSERT_EQUAL_DOUBLE(0.0,    min_y);
    TEST_ASSERT_EQUAL_DOUBLE(1000.0, max_x);
    TEST_ASSERT_EQUAL_DOUBLE(1000.0, max_y);

    hash_close(hf);
}

/* ─── svg_iniciar com caminho inválido ───────────────────────────────────── */

void test_SvgIniciarPathInvalidoRetornaNull(void) {
    /* Diretório que não existe — fopen deve falhar */
    FILE *f = svg_iniciar("/diretorio_inexistente_xyz/out.svg", 0, 0, 100, 100);
    TEST_ASSERT_NULL(f);
}

/* ─── svg_fechar com NULL não crasha ─────────────────────────────────────── */

void test_SvgFecharNullNaoCrasha(void) {
    svg_fechar(NULL); /* não deve crashar */
}

/* ─── svg_gerar_quadras com argumentos NULL não crasha ───────────────────── */

void test_SvgGerarQuadrasNullNaoCrasha(void) {
    svg_gerar_quadras(NULL, NULL); /* não deve crashar */

    HashFile hf = hash_open(HF_TMP_DAT, HF_TMP_IDX);
    svg_gerar_quadras(hf, NULL);   /* arquivo NULL — não deve crashar */
    hash_close(hf);
}

/* ─── main ────────────────────────────────────────────────────────────────── */

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_SvgInitAndGeneration);
    RUN_TEST(test_SvgCalcularBbox);
    RUN_TEST(test_SvgBboxHashVazio_UsaDefault);
    RUN_TEST(test_SvgIniciarPathInvalidoRetornaNull);
    RUN_TEST(test_SvgFecharNullNaoCrasha);
    RUN_TEST(test_SvgGerarQuadrasNullNaoCrasha);
    return UNITY_END();
}
