# ============================================================
#  Makefile — Projeto 01 ED2 (SIG Bitnópolis)
# ============================================================

CC      = gcc
CFLAGS  = -std=c99 -fstack-protector-all -Wall -Wextra -g

# ---- Diretórios -------------------------------------------
SRC_DIR    = src
INC_DIR    = includes
TEST_DIR   = tests
UNITY_DIR  = unity
OUT_DIR    = outputs

# ---- Fontes dos módulos -----------------------------------
MOD_SRCS   = $(SRC_DIR)/HashFile.c \
             $(SRC_DIR)/list.c \
             $(SRC_DIR)/leitor_geo.c \
             $(SRC_DIR)/parametros.c \
             $(SRC_DIR)/pessoa.c \
             $(SRC_DIR)/quadra.c \
             $(SRC_DIR)/leitorPM.c \
             $(SRC_DIR)/leitorQry.c \
             $(SRC_DIR)/svg.c

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

# --- Testes unitários do HashFile e Pessoa --------------------------
test: $(TEST_BIN) $(TEST_DIR)/run_tests_pessoa
	@echo ""
	@echo "========== Executando testes HashFile =========="
	./$(TEST_BIN)
	@echo "========== Executando testes Pessoa =========="
	./$(TEST_DIR)/run_tests_pessoa
	@echo "==============================================="

$(TEST_BIN): $(TEST_HASH) $(SRC_DIR)/HashFile.c $(UNITY_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ -lm

$(TEST_DIR)/run_tests_pessoa: $(TEST_DIR)/testPessoa.c $(SRC_DIR)/pessoa.c $(UNITY_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ -lm

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
