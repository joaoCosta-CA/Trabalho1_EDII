# ============================================================
#  Makefile — Projeto 01 ED2 (SIG Bitnópolis)
# ============================================================

CC      = gcc
CFLAGS  = -std=c99 -fstack-protector-all -Wall -Wextra -g

# ---- Diretórios -------------------------------------------
SRC_DIR    = src
INC_DIR    = $(SRC_DIR)/includes
HASH_DIR   = $(INC_DIR)/estr_dados/Include_HashFile
LIST_DIR   = $(INC_DIR)/estr_dados/Include_list
GEO_DIR    = $(INC_DIR)/leitores/Include_leitorGeo
QRY_DIR    = $(INC_DIR)/leitores/Include_leitorQry
PESSOA_DIR = $(INC_DIR)/mundo/Include_pessoa
QUADRA_DIR = $(INC_DIR)/mundo/Include_quadra
PARAM_DIR  = $(INC_DIR)/parametros
TEST_DIR   = tests
UNITY_DIR  = unity
OUT_DIR    = outputs

# ---- Fontes dos módulos -----------------------------------
HASH_SRC   = $(HASH_DIR)/HashFile.c
LIST_SRC   = $(LIST_DIR)/list.c
GEO_SRC    = $(GEO_DIR)/leitor_geo.c
PARAM_SRC  = $(PARAM_DIR)/parametros.c
PESSOA_SRC = $(PESSOA_DIR)/pessoa.c
QUADRA_SRC = $(QUADRA_DIR)/quadra.c

# Adicione aqui novos módulos conforme criar:
# QRY_SRC  = $(QRY_DIR)/leitor_qry.c
# PM_SRC   = $(INC_DIR)/leitores/Include_leitorPm/leitor_pm.c

MOD_SRCS   = $(HASH_SRC) $(LIST_SRC) $(GEO_SRC) $(PARAM_SRC) $(PESSOA_SRC) $(QUADRA_SRC)

# ---- Objetos correspondentes -----------------------------
MOD_OBJS   = $(MOD_SRCS:.c=.o)

# ---- Programa principal ----------------------------------
MAIN_SRC   = $(SRC_DIR)/main.c
TARGET     = ted

# ---- Testes unitários ------------------------------------
UNITY_SRC  = $(UNITY_DIR)/unity.c
TEST_HASH  = $(TEST_DIR)/testHashFile.c
TEST_BIN   = $(TEST_DIR)/run_tests

# ---- Includes para compilação ----------------------------
INCLUDES   = -I$(SRC_DIR) -I$(INC_DIR) -I.

# ============================================================
#  Targets
# ============================================================

.PHONY: all clean test run_test dirs

# --- Build principal ---------------------------------------
all: dirs $(TARGET)
	@echo "=== Build concluído: ./$(TARGET) ==="

$(TARGET): $(MAIN_SRC) $(MOD_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ -lm

# --- Regra genérica para .o --------------------------------
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# --- Testes unitários do HashFile --------------------------
test: $(TEST_BIN)
	@echo ""
	@echo "========== Executando testes =========="
	./$(TEST_BIN)
	@echo "======================================="

$(TEST_BIN): $(TEST_HASH) $(HASH_SRC) $(UNITY_SRC)
	$(CC) $(CFLAGS) -I. -I$(HASH_DIR) -o $@ $^ -lm

run_test: test

# --- Criar diretório de saída, se não existir ---------------
dirs:
	@mkdir -p $(OUT_DIR)

# --- Limpeza ------------------------------------------------
clean:
	rm -f $(MOD_OBJS) $(TARGET) $(TEST_BIN)
	rm -f *.hf *.hfc *.hfd *.dat *.idx
	rm -f $(TEST_DIR)/*.dat $(TEST_DIR)/*.idx
	@echo "=== Limpo ==="
