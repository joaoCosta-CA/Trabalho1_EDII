/**
 * @file testList.c
 * @brief Testes unitários para o módulo de Lista Duplamente Encadeada.
 *
 * Cobre: criação, inserção, comprimento, travessia (getNext/getPrevious),
 * getLast, remoção por posição (head, meio, tail), killList com callback,
 * e comportamento com lista vazia.
 */
#include "unity/unity.h"
#include "estr_dados/Include_list/list.h"
#include <stdio.h>
#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {}

/* ─── Criação ─────────────────────────────────────────────────────────────── */

void test_ListCreation(void) {
    Lista l = createList();
    TEST_ASSERT_NOT_NULL(l);
    TEST_ASSERT_EQUAL_INT(0, length(l));
    killList(l, NULL);
}

void test_ListCreateEmpty_GetFirstReturnsNull(void) {
    Lista l = createList();
    TEST_ASSERT_NULL(getFirst(l));
    TEST_ASSERT_NULL(getLast(l));
    killList(l, NULL);
}

/* ─── Inserção e comprimento ──────────────────────────────────────────────── */

void test_ListInsertAndLength(void) {
    Lista l = createList();
    int val1 = 10, val2 = 20;

    Posic p1 = insert(l, &val1);
    TEST_ASSERT_NOT_NULL(p1);
    TEST_ASSERT_EQUAL_INT(1, length(l));

    Posic p2 = insert(l, &val2);
    TEST_ASSERT_NOT_NULL(p2);
    TEST_ASSERT_EQUAL_INT(2, length(l));

    killList(l, NULL);
}

/* ─── Travessia getNext ───────────────────────────────────────────────────── */

void test_ListGetAndTraversal(void) {
    Lista l = createList();
    int val1 = 10, val2 = 20, val3 = 30;
    insert(l, &val1);
    insert(l, &val2);
    insert(l, &val3);

    Posic p = getFirst(l);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(10, *((int*)get(l, p)));

    p = getNext(l, p);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(20, *((int*)get(l, p)));

    p = getNext(l, p);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(30, *((int*)get(l, p)));

    p = getNext(l, p);
    TEST_ASSERT_NULL(p);

    killList(l, NULL);
}

/* ─── getLast ─────────────────────────────────────────────────────────────── */

void test_ListGetLast(void) {
    Lista l = createList();
    int val1 = 1, val2 = 2, val3 = 3;
    insert(l, &val1);
    insert(l, &val2);
    insert(l, &val3);

    Posic last = getLast(l);
    TEST_ASSERT_NOT_NULL(last);
    TEST_ASSERT_EQUAL_INT(3, *((int*)get(l, last)));

    killList(l, NULL);
}

/* ─── Travessia getPrevious ───────────────────────────────────────────────── */

void test_ListGetPrevious(void) {
    Lista l = createList();
    int val1 = 10, val2 = 20, val3 = 30;
    insert(l, &val1);
    insert(l, &val2);
    insert(l, &val3);

    /* Começa no último e caminha para trás */
    Posic p = getLast(l);
    TEST_ASSERT_EQUAL_INT(30, *((int*)get(l, p)));

    p = getPrevious(l, p);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(20, *((int*)get(l, p)));

    p = getPrevious(l, p);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(10, *((int*)get(l, p)));

    p = getPrevious(l, p);
    TEST_ASSERT_NULL(p);

    killList(l, NULL);
}

/* ─── Remoção por posição ─────────────────────────────────────────────────── */

void test_ListRemoveMidElement(void) {
    Lista l = createList();
    int val1 = 10, val2 = 20, val3 = 30;
    Posic p1 = insert(l, &val1);
    Posic p2 = insert(l, &val2);
    insert(l, &val3);

    /* Remove do meio */
    int* rem = (int*)removePosic(l, p2);
    TEST_ASSERT_EQUAL_INT(20, *rem);
    TEST_ASSERT_EQUAL_INT(2, length(l));

    /* Remove do início */
    rem = (int*)removePosic(l, p1);
    TEST_ASSERT_EQUAL_INT(10, *rem);
    TEST_ASSERT_EQUAL_INT(1, length(l));

    killList(l, NULL);
}

void test_ListRemoveHead(void) {
    Lista l = createList();
    int a = 1, b = 2, c = 3;
    Posic ph = insert(l, &a);
    insert(l, &b);
    insert(l, &c);

    int *removed = (int *)removePosic(l, ph);
    TEST_ASSERT_EQUAL_INT(1, *removed);
    TEST_ASSERT_EQUAL_INT(2, length(l));

    /* Novo primeiro deve ser 2 */
    Posic first = getFirst(l);
    TEST_ASSERT_EQUAL_INT(2, *((int *)get(l, first)));

    killList(l, NULL);
}

void test_ListRemoveTail(void) {
    Lista l = createList();
    int a = 1, b = 2, c = 3;
    insert(l, &a);
    insert(l, &b);
    Posic pt = insert(l, &c);

    int *removed = (int *)removePosic(l, pt);
    TEST_ASSERT_EQUAL_INT(3, *removed);
    TEST_ASSERT_EQUAL_INT(2, length(l));

    /* Novo último deve ser 2 */
    Posic last = getLast(l);
    TEST_ASSERT_EQUAL_INT(2, *((int *)get(l, last)));

    killList(l, NULL);
}

void test_ListRemoveOnlyElement(void) {
    Lista l = createList();
    int a = 99;
    Posic p = insert(l, &a);

    int *removed = (int *)removePosic(l, p);
    TEST_ASSERT_EQUAL_INT(99, *removed);
    TEST_ASSERT_EQUAL_INT(0, length(l));
    TEST_ASSERT_NULL(getFirst(l));
    TEST_ASSERT_NULL(getLast(l));

    killList(l, NULL);
}

/* ─── killList com callback ───────────────────────────────────────────────── */

void test_KillListWithFreeCallback(void) {
    Lista l = createList();

    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    insert(l, a);
    insert(l, b);

    /* Não deve crashar — valgrind confirmaria ausência de leaks */
    killList(l, free);
}

/* ─── Segurança com NULL ──────────────────────────────────────────────────── */

void test_ListNullSafety(void) {
    /* Operações em lista nula não devem crashar */
    TEST_ASSERT_EQUAL_INT(0, length(NULL));
    TEST_ASSERT_NULL(getFirst(NULL));
    TEST_ASSERT_NULL(getLast(NULL));
    TEST_ASSERT_NULL(removePosic(NULL, NULL));
    killList(NULL, NULL); /* não deve crashar */
}

/* ─── main ────────────────────────────────────────────────────────────────── */

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_ListCreation);
    RUN_TEST(test_ListCreateEmpty_GetFirstReturnsNull);
    RUN_TEST(test_ListInsertAndLength);
    RUN_TEST(test_ListGetAndTraversal);
    RUN_TEST(test_ListGetLast);
    RUN_TEST(test_ListGetPrevious);
    RUN_TEST(test_ListRemoveMidElement);
    RUN_TEST(test_ListRemoveHead);
    RUN_TEST(test_ListRemoveTail);
    RUN_TEST(test_ListRemoveOnlyElement);
    RUN_TEST(test_KillListWithFreeCallback);
    RUN_TEST(test_ListNullSafety);
    return UNITY_END();
}
