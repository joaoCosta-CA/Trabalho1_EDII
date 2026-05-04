/**
 * @file testLeitorQry.c
 * @brief Testes unitários para o módulo leitorQry.
 *
 * Cobre todos os comandos .qry:
 *   rq   — remove quadra e lista moradores afetados
 *   pq   — conta habitantes por face na quadra
 *   nasc — regista nova pessoa
 *   rip  — remove pessoa (falecimento)
 *   mud  — atualiza endereço de pessoa
 *   dspj — despeja morador (remove pessoa)
 *   h?   — consulta endereço de habitante
 *   arquivo inexistente retorna NULL
 */
#include "unity/unity.h"
#include "leitores/Include_leitorQry/leitorQry.h"
#include "estr_dados/Include_HashFile/HashFile.h"
#include "estr_dados/Include_list/list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- Ficheiros temporários ---------------------------------------- */
#define HQ_DAT   "tq_quadras.hf"
#define HQ_IDX   "tq_quadras.idx"
#define HP_DAT   "tq_pessoas.hf"
#define HP_IDX   "tq_pessoas.idx"
#define QRY_TMP  "tq_test.qry"
#define OUT_DIR  "."
#define OUT_BASE "tq_baseout"

static HashFile hq, hp;

/* Dados de quadra serializada (cep;x;y;w;h;sw;cfill;cstrk) */
#define QUADRA_CEP1  "cep1;10.0;10.0;100.0;50.0;1.0;white;black"
/* Dados de pessoa serializada (nome;sobrenome;sexo;nasc;cep;face;num;compl) */
#define PESSOA_MORADOR "Joao;Silva;M;01/01/1990;cep1;N;5;Ap1"

static void setup_quadra(void)  { hash_insert(hq, "cep1",         QUADRA_CEP1); }
static void setup_morador(void) { hash_insert(hp, "111.111.111-11", PESSOA_MORADOR); }

static Lista rodar_qry(const char *conteudo)
{
    FILE *f = fopen(QRY_TMP, "w");
    fprintf(f, "%s\n", conteudo);
    fclose(f);
    Lista dec = leitor_qry(OUT_DIR, QRY_TMP, hq, hp, OUT_DIR, OUT_BASE);
    remove(OUT_BASE ".txt");
    return dec;
}

void setUp(void) {
    remove(HQ_DAT); remove(HQ_IDX);
    remove(HP_DAT); remove(HP_IDX);
    remove(QRY_TMP);
    hq = hash_open(HQ_DAT, HQ_IDX);
    hp = hash_open(HP_DAT, HP_IDX);
}

void tearDown(void) {
    if (hq) { hash_close(hq); hq = NULL; }
    if (hp) { hash_close(hp); hp = NULL; }
    remove(HQ_DAT); remove(HQ_IDX);
    remove(HP_DAT); remove(HP_IDX);
    remove(QRY_TMP);
    remove(OUT_BASE ".txt");
}

/* ─── rq — remove quadra ─────────────────────────────────────────────────── */

void test_ComandoRQ_RemoveQuadra(void) {
    setup_quadra();

    Lista dec = rodar_qry("rq cep1");
    TEST_ASSERT_NOT_NULL(dec);

    char buf[200];
    /* A quadra deve ter sido removida do HashFile */
    TEST_ASSERT_EQUAL_INT(0, hash_search(hq, "cep1", buf, sizeof(buf)));

    killList(dec, free);
}

void test_ComandoRQ_QuadraInexistente(void) {
    /* Não deve crashar quando o CEP não existe */
    Lista dec = rodar_qry("rq cep_inexistente");
    TEST_ASSERT_NOT_NULL(dec);
    killList(dec, free);
}

/* ─── nasc — regista pessoa ──────────────────────────────────────────────── */

void test_ComandoNasc_RegistaPessoa(void) {
    Lista dec = rodar_qry("nasc 333.333.333-33 Pedro Alves M 10/10/2000");
    TEST_ASSERT_NOT_NULL(dec);

    char buf[300];
    TEST_ASSERT_EQUAL_INT(1, hash_search(hp, "333.333.333-33", buf, sizeof(buf)));
    TEST_ASSERT_NOT_NULL(strstr(buf, "Pedro"));

    killList(dec, free);
}

void test_ComandoNasc_DuplicadoNaoInsere(void) {
    /* Inserir duas vezes o mesmo CPF — segundo deve ser ignorado */
    hash_insert(hp, "444.444.444-44", "Ana;Costa;F;01/01/1990;;;0;");

    Lista dec = rodar_qry("nasc 444.444.444-44 OutroNome Silva F 01/01/2000");
    TEST_ASSERT_NOT_NULL(dec);

    char buf[300];
    hash_search(hp, "444.444.444-44", buf, sizeof(buf));
    /* Valor original deve permanecer */
    TEST_ASSERT_NOT_NULL(strstr(buf, "Ana"));

    killList(dec, free);
}

/* ─── rip — falecimento ──────────────────────────────────────────────────── */

void test_ComandoRip_RemovePessoa(void) {
    setup_morador();

    Lista dec = rodar_qry("rip 111.111.111-11");
    TEST_ASSERT_NOT_NULL(dec);

    char buf[300];
    TEST_ASSERT_EQUAL_INT(0, hash_search(hp, "111.111.111-11", buf, sizeof(buf)));

    killList(dec, free);
}

/* ─── mud — mudança de endereço ──────────────────────────────────────────── */

void test_ComandoMud_AtualizaEndereco(void) {
    setup_quadra();
    setup_morador();

    Lista dec = rodar_qry("mud 111.111.111-11 cep1 S 10 Bl2");
    TEST_ASSERT_NOT_NULL(dec);

    char buf[300];
    TEST_ASSERT_EQUAL_INT(1, hash_search(hp, "111.111.111-11", buf, sizeof(buf)));
    TEST_ASSERT_NOT_NULL(strstr(buf, "cep1"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "S"));

    killList(dec, free);
}

/* ─── dspj — despejo ─────────────────────────────────────────────────────── */

void test_ComandoDspj_RemovePessoa(void) {
    setup_morador();

    Lista dec = rodar_qry("dspj 111.111.111-11");
    TEST_ASSERT_NOT_NULL(dec);

    char buf[300];
    TEST_ASSERT_EQUAL_INT(0, hash_search(hp, "111.111.111-11", buf, sizeof(buf)));

    killList(dec, free);
}

/* ─── h? — consulta habitante ────────────────────────────────────────────── */

void test_ComandoH_GeraRelatorio(void) {
    setup_morador();

    /* h? deve escrever no arquivo txt; aqui só verificamos que não crasha */
    Lista dec = rodar_qry("h? 111.111.111-11");
    TEST_ASSERT_NOT_NULL(dec);
    killList(dec, free);
}

/* ─── pq — população por face ────────────────────────────────────────────── */

void test_ComandoPQ_ContaHabitantes(void) {
    setup_quadra();
    setup_morador();  /* Joao está na face N da cep1 */

    Lista dec = rodar_qry("pq cep1");
    TEST_ASSERT_NOT_NULL(dec);
    /* Não crashou e retornou decorações */
    killList(dec, free);
}

/* ─── arquivo inexistente retorna NULL ───────────────────────────────────── */

void test_ArquivoInexistenteRetornaNull(void) {
    Lista dec = leitor_qry(OUT_DIR, "nao_existe_xyz.qry", hq, hp, OUT_DIR, OUT_BASE);
    TEST_ASSERT_NULL(dec);
}

/* ─── main ────────────────────────────────────────────────────────────────── */

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_ComandoRQ_RemoveQuadra);
    RUN_TEST(test_ComandoRQ_QuadraInexistente);
    RUN_TEST(test_ComandoNasc_RegistaPessoa);
    RUN_TEST(test_ComandoNasc_DuplicadoNaoInsere);
    RUN_TEST(test_ComandoRip_RemovePessoa);
    RUN_TEST(test_ComandoMud_AtualizaEndereco);
    RUN_TEST(test_ComandoDspj_RemovePessoa);
    RUN_TEST(test_ComandoH_GeraRelatorio);
    RUN_TEST(test_ComandoPQ_ContaHabitantes);
    RUN_TEST(test_ArquivoInexistenteRetornaNull);
    return UNITY_END();
}
