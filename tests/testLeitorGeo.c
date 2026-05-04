/**
 * @file testLeitorGeo.c
 * @brief Testes unitários para o módulo leitor_geo.
 *
 * Cobre: leitura de quadra simples, efeito do comando cq no estilo,
 * reinício de estilo entre chamadas, e arquivo inexistente.
 */
#include "unity/unity.h"
#include "leitores/Include_leitorGeo/leitor_geo.h"
#include "estr_dados/Include_HashFile/HashFile.h"
#include <stdio.h>
#include <string.h>

#define HF_DAT  "tg_quadras.hf"
#define HF_IDX  "tg_quadras.idx"
#define GEO_TMP "tg_test.geo"

static HashFile hf;

void setUp(void) {
    remove(HF_DAT);
    remove(HF_IDX);
    remove(GEO_TMP);
    hf = hash_open(HF_DAT, HF_IDX);
}

void tearDown(void) {
    if (hf) { hash_close(hf); hf = NULL; }
    remove(HF_DAT);
    remove(HF_IDX);
    remove(GEO_TMP);
}

/* ─── Leitura básica de quadra ────────────────────────────────────────────── */

void test_LerArquivoGeo_QuadraBasica(void) {
    FILE *f = fopen(GEO_TMP, "w");
    fprintf(f, "q cep01 10.0 20.0 50.0 30.0\n");
    fclose(f);

    ler_arquivo_geo(".", GEO_TMP, hf);

    char buf[200];
    TEST_ASSERT_EQUAL_INT(1, hash_search(hf, "cep01", buf, sizeof(buf)));

    /* Verifica as coordenadas serializadas */
    TEST_ASSERT_NOT_NULL(strstr(buf, "10.000000"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "20.000000"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "50.000000"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "30.000000"));
}

/* ─── Múltiplas quadras ───────────────────────────────────────────────────── */

void test_LerArquivoGeo_MultiplaQuadras(void) {
    FILE *f = fopen(GEO_TMP, "w");
    fprintf(f, "q cepA 0.0 0.0 10.0 10.0\n");
    fprintf(f, "q cepB 50.0 50.0 20.0 20.0\n");
    fclose(f);

    ler_arquivo_geo(".", GEO_TMP, hf);

    char buf[200];
    TEST_ASSERT_EQUAL_INT(1, hash_search(hf, "cepA", buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_INT(1, hash_search(hf, "cepB", buf, sizeof(buf)));
}

/* ─── Comando cq altera o estilo das quadras seguintes ───────────────────── */

void test_LerArquivoGeo_CqAlteraEstilo(void) {
    FILE *f = fopen(GEO_TMP, "w");
    fprintf(f, "cq 2.0 red blue\n");
    fprintf(f, "q cep_estilo 10.0 10.0 50.0 50.0\n");
    fclose(f);

    ler_arquivo_geo(".", GEO_TMP, hf);

    char buf[200];
    TEST_ASSERT_EQUAL_INT(1, hash_search(hf, "cep_estilo", buf, sizeof(buf)));
    /* O estilo personalizado deve aparecer no valor serializado */
    TEST_ASSERT_NOT_NULL(strstr(buf, "red"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "blue"));
}

/* ─── Reinício de estilo entre duas chamadas distintas ───────────────────── */

void test_LerArquivoGeo_EstiloPadraoReiniciadoEntreArquivos(void) {
    /* Primeira chamada — usa estilo personalizado */
    FILE *f1 = fopen("tg_a.geo", "w");
    fprintf(f1, "cq 3.0 green yellow\n");
    fprintf(f1, "q cep_a 0.0 0.0 10.0 10.0\n");
    fclose(f1);
    ler_arquivo_geo(".", "tg_a.geo", hf);

    /* Segunda chamada — deve usar estilo padrão (white/black) */
    FILE *f2 = fopen("tg_b.geo", "w");
    fprintf(f2, "q cep_b 20.0 20.0 10.0 10.0\n");
    fclose(f2);
    ler_arquivo_geo(".", "tg_b.geo", hf);

    char buf[200];
    TEST_ASSERT_EQUAL_INT(1, hash_search(hf, "cep_b", buf, sizeof(buf)));
    /* cep_b não recebeu cq → estilo padrão "white" e "black" */
    TEST_ASSERT_NOT_NULL(strstr(buf, "white"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "black"));

    remove("tg_a.geo");
    remove("tg_b.geo");
}

/* ─── Arquivo inexistente não crasha (exit() no leitor — teste de smoke) ─── */
/* Nota: ler_arquivo_geo chama exit() se o arquivo não existir.
   Este teste só pode verificar que a função lida normalmente com arquivos válidos.
   O comportamento com arquivo inexistente é propositalmente fatal (exit). */

/* ─── main ────────────────────────────────────────────────────────────────── */

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_LerArquivoGeo_QuadraBasica);
    RUN_TEST(test_LerArquivoGeo_MultiplaQuadras);
    RUN_TEST(test_LerArquivoGeo_CqAlteraEstilo);
    RUN_TEST(test_LerArquivoGeo_EstiloPadraoReiniciadoEntreArquivos);
    return UNITY_END();
}
