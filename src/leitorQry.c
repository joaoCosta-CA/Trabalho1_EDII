#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "leitores/Include_leitorQry/leitorQry.h"

/* Funções auxiliares primitivas para o esqueleto */
static void tratar_comando_qry(char *comando, char *params, HashFile hf_quadras) {
    if (strcmp(comando, "rq") == 0) {
        // TODO: deletar quadra
    } else if (strcmp(comando, "pq") == 0) {
        // TODO: calcula o número de pessoas na quadra
    } else if (strcmp(comando, "censo") == 0) {
        // TODO: gera uma estatístaca sobre todos os habitantes de bitnópolis
    } else if(strcmp(comando, "h?")) {
        // TODO: reporta os dados de um habitante, cpf **REPORTAR NO TXT, caso seja morador, reportar endereço**
    } else if(strcmp(comando, "nasc")){
        // TODO: pessoa nasce.
    }else if(strcmp(comando, "rip")){
        // TODO: pessoa morre. **REPORTAR NO TXT, caso seja morador, desenhar um cruz no endereço**
    }else if(strcmp(comando, "mud")){
        // TODO: morador muda de endereço. **Desenhar no SVG um retangulo vermelho
        //  no endereço de destino e, colcoar o endereço dentro do retnagulo**
    }else if(strcmp(comando, "dspj")){
        // TODO: morador é despejado **REPORTAR OS DADOS NO TXT, colocar **
    }else{
        perror("Comando não reconhecido, inválido!");
    }
}

LeitorQry leitor_qry(const char *diretorio_base, const char *nome_arquivo, HashFile hf_quadras)
{
    char caminho_completo[512];

    if (diretorio_base && strlen(diretorio_base) > 0)
    {
        snprintf(caminho_completo, sizeof(caminho_completo), "%s/%s", diretorio_base, nome_arquivo);
    }
    else
    {
        strncpy(caminho_completo, nome_arquivo, sizeof(caminho_completo));
    }

    FILE *arquivo = fopen(caminho_completo, "r");
    if (!arquivo)
    {
        perror("Erro ao abrir arquivo .qry");
        return NULL;
    }

    char linha[1024];
    while (fgets(linha, sizeof(linha), arquivo))
    {
        linha[strcspn(linha, "\r\n")] = 0;

        char *comando = strtok(linha, " \t");
        char *params = strtok(NULL, ""); 

        if (!comando)
            continue;
            
        tratar_comando_qry(comando, params, hf_quadras);
    }

    fclose(arquivo);
    
    // Retorna algo que represente o estado do qry, no momento NULL servirá como dummy
    return NULL;
}
