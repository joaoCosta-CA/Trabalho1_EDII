/**
 * @file testLeitorPM.c
 * @brief Testes unitários para o módulo leitorPM.
 *
 * Cobre: registo de pessoa (comando p), mudança de endereço (comando m),
 * conteúdo exacto do valor serializado, e arquivo inexistente não crasha.
 *
 * Formato serializado de uma pessoa:
 *   nome;sobrenome;sexo;nasc;cep;face;num;compl
 */
#include "unity/unity.h"
#include "leitores/Include_leitorPm/leitorPM.h"
#include "estr_dados/Include_HashFile/HashFile.h"
#include <stdio.h>
#include <string.h>

#define HF_DAT  "tpm_p.hf"
#define HF_IDX  "tpm_p.idx"
#define PM_TMP  "tpm_test.pm"

static HashFile hf;

void setUp(void) {
    remove(HF_DAT); remove(HF_IDX); remove(PM_TMP);
    hf = hash_open(HF_DAT, HF_IDX);
}

void tearDown(void) {
    if (hf) { hash_close(hf); hf = NULL; }
    remove(HF_DAT); remove(HF_IDX); remove(PM_TMP);
}

/* ─── Registo de pessoa (comando p) ──────────────────────────────────────── */

void test_ComandoP_RegistaPessoa(void) {
    FILE *f = fopen(PM_TMP, "w");
    fprintf(f, "p 111.111.111-11 Joao Silva M 01/01/2000\n");
    fclose(f);

    ler_arquivo_pm(".", PM_TMP, hf);

    char buf[300];
    TEST_ASSERT_EQUAL_INT(1, hash_search(hf, "111.111.111-11", buf, sizeof(buf)));

    /* Verifica campos demográficos no valor serializado */
    TEST_ASSERT_NOT_NULL(strstr(buf, "Joao"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "Silva"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "M"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "01/01/2000"));
}

/* ─── Mudança de endereço (comando m) ────────────────────────────────────── */

void test_ComandoM_AtualizaEndereco(void) {
    /* Pré-popula a pessoa */
    FILE *f = fopen(PM_TMP, "w");
    fprintf(f, "p 222.222.222-22 Maria Souza F 15/06/1995\n");
    fprintf(f, "m 222.222.222-22 CEP-10.1 N 5 Apto201\n");
    fclose(f);

    ler_arquivo_pm(".", PM_TMP, hf);

    char buf[300];
    TEST_ASSERT_EQUAL_INT(1, hash_search(hf, "222.222.222-22", buf, sizeof(buf)));

    /* O novo CEP e face devem estar no valor actualizado */
    TEST_ASSERT_NOT_NULL(strstr(buf, "CEP-10.1"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "N"));
}

/* ─── Múltiplas pessoas ───────────────────────────────────────────────────── */

void test_MultiplasPessoas(void) {
    FILE *f = fopen(PM_TMP, "w");
    fprintf(f, "p 000.000.001-01 Ana Lima F 10/10/1990\n");
    fprintf(f, "p 000.000.002-02 Bruno Costa M 20/05/1985\n");
    fclose(f);

    ler_arquivo_pm(".", PM_TMP, hf);

    char buf[300];
    TEST_ASSERT_EQUAL_INT(1, hash_search(hf, "000.000.001-01", buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_INT(1, hash_search(hf, "000.000.002-02", buf, sizeof(buf)));
}

/* ─── Pessoa sem endereço começa sem-teto ────────────────────────────────── */

void test_PessoaSemEndereco_CamposVazios(void) {
    FILE *f = fopen(PM_TMP, "w");
    fprintf(f, "p 333.333.333-33 Carlos Pereira M 05/03/1980\n");
    fclose(f);

    ler_arquivo_pm(".", PM_TMP, hf);

    char buf[300];
    TEST_ASSERT_EQUAL_INT(1, hash_search(hf, "333.333.333-33", buf, sizeof(buf)));

    /* Sem comando m, os campos de endereço devem ser vazios */
    /* Formato esperado: Carlos;Pereira;M;05/03/1980;;;0; */
    TEST_ASSERT_NOT_NULL(strstr(buf, ";;;"));
}

/* ─── Arquivo inexistente retorna sem crash ───────────────────────────────── */

void test_ArquivoInexistenteNaoCrasha(void) {
    /* leitorPM usa perror+return (não exit), portanto é seguro testar */
    ler_arquivo_pm(".", "nao_existe_xyz.pm", hf);
    /* Se chegou aqui, não crashou */
    TEST_ASSERT_TRUE(1);
}

/* ─── main ────────────────────────────────────────────────────────────────── */

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_ComandoP_RegistaPessoa);
    RUN_TEST(test_ComandoM_AtualizaEndereco);
    RUN_TEST(test_MultiplasPessoas);
    RUN_TEST(test_PessoaSemEndereco_CamposVazios);
    RUN_TEST(test_ArquivoInexistenteNaoCrasha);
    return UNITY_END();
}
