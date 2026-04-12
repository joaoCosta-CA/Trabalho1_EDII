#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "leitorPM.h"

static void tratar_comando_p(char *params, HashFile hf)
{
    /* p cpf nome sobrenome sexo nasc */
    char cpf[30] = {0}, nome[50] = {0}, sobrenome[50] = {0}, sexo = 0, nasc[20] = {0};

    if (sscanf(params, "%29s %49s %49s %c %19s", cpf, nome, sobrenome, &sexo, nasc) == 5)
    {
        /* Serializa os dados no formato: nome;sobrenome;sexo;nasc;cep_val;face_val;num_val;compl_val */
        char dados_serializados[200];
        /* Endereço em branco inicialmente (sem-teto) */
        snprintf(dados_serializados, sizeof(dados_serializados), "%s;%s;%c;%s;;;0;", nome, sobrenome, sexo, nasc);
        hash_insert(hf, cpf, dados_serializados);
    }
}

static void tratar_comando_m(char *params, HashFile hf)
{
    /* m cpf cep face num compl */
    char cpf[30] = {0}, cep[50] = {0}, face = 0;
    int num = 0;
    int bytes_lidos = 0;

    /* Tenta ler os atributos principais da mudança */
    if (sscanf(params, "%29s %49s %c %d %n", cpf, cep, &face, &num, &bytes_lidos) >= 4)
    {
        char compl[100] = {0};
        if (params[bytes_lidos] != '\0')
        {
            strncpy(compl, params + bytes_lidos, sizeof(compl) - 1);
            compl[sizeof(compl) - 1] = '\0';
        }

        /* Recupera os dados básicos da pessoa no HashFile usando CPF */
        char dados_atuais[200] = {0};
        if (hash_search(hf, cpf, dados_atuais, sizeof(dados_atuais)))
        {
            char nome[50] = {0}, sobrenome[50] = {0}, sexo = 0, nasc[20] = {0};

            /* Faz o parse da parte demográfica do registro existente */
            char *token = strtok(dados_atuais, ";");
            if (token) strncpy(nome, token, 49);

            token = strtok(NULL, ";");
            if (token) strncpy(sobrenome, token, 49);

            token = strtok(NULL, ";");
            if (token && strlen(token) > 0) sexo = token[0];

            token = strtok(NULL, ";");
            if (token) strncpy(nasc, token, 19);

            /* Remonta a string de dados combinando a pessoa com seu novo endereço */
            char nova_string[200];
            snprintf(nova_string, sizeof(nova_string), "%s;%s;%c;%s;%s;%c;%d;%s",
                     nome, sobrenome, sexo, nasc, cep, face, num, compl);
            
            /* Grava no HashFile o dado de morador */
            hash_update(hf, cpf, nova_string);
        }
    }
}

void ler_arquivo_pm(const char *diretorio_base, const char *nome_arquivo, HashFile hf_pessoas)
{
    char caminho_completo[512] = {0};

    if (diretorio_base && strlen(diretorio_base) > 0)
    {
        snprintf(caminho_completo, sizeof(caminho_completo), "%s/%s", diretorio_base, nome_arquivo);
    }
    else
    {
        strncpy(caminho_completo, nome_arquivo, sizeof(caminho_completo) - 1);
    }

    FILE *arquivo = fopen(caminho_completo, "r");
    if (!arquivo)
    {
        perror("Aviso: arquivo .pm não encontrado ou erro de leitura");
        return;
    }

    char linha[1024];
    while (fgets(linha, sizeof(linha), arquivo))
    {
        /* Remove o '\n' ou '\r' ao final */
        linha[strcspn(linha, "\r\n")] = '\0';

        char *comando = strtok(linha, " \t");
        char *params = strtok(NULL, ""); /* Traz o resto inalterado */

        if (!comando)
            continue;

        /* Direciona para o parser específico */
        if (comando[0] == 'p' && comando[1] == '\0')
        {
            tratar_comando_p(params, hf_pessoas);
        }
        else if (comando[0] == 'm' && comando[1] == '\0')
        {
            tratar_comando_m(params, hf_pessoas);
        }
    }

    fclose(arquivo);
}
