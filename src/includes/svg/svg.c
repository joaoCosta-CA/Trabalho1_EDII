#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "svg.h"
#include "../estr_dados/Include_HashFile/HashFile.h"

FILE* svg_iniciar(const char* caminho_arquivo) {
    FILE *f = fopen(caminho_arquivo, "w");
    if (!f) {
        perror("Erro ao iniciar o arquivo SVG");
        return NULL;
    }
    fprintf(f, "<!-- SVG Gerado pelo TED (Trabalho ED2) -->\n");
    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\">\n");
    return f;
}

void svg_fechar(FILE *f) {
    if (f) {
        fprintf(f, "</svg>\n");
        fclose(f);
    }
}

/**
 * Função de callback para ser repassada ao `hash_forall`.
 * Recebe a string de dados da quadra e imprime-a diretamente no arquivo SVG.
 */
static void imprimir_quadra_svg_callback(const char* key, const char* value, void* userdata) {
    FILE *svg_file = (FILE*) userdata;
    
    // key = cep (ignorado no layout visual primario, talvez util se for fazer texto)
    // value = cep;x;y;w;h;sw;cfill;cstrk (serializado conforme no leitor geo)
    // Decodifica a string da quadra usando um buffer local (para não modificar a original)
    char buffer[300];
    strncpy(buffer, value, sizeof(buffer) - 1);
    buffer[sizeof(buffer)-1] = '\0';
    
    char *token;
    char cep[50] = {0}, cfill[50] = {0}, cstrk[50] = {0};
    double x = 0, y = 0, w = 0, h = 0, sw = 1.0;
    
    token = strtok(buffer, ";");
    if(token) strncpy(cep, token, 49);
    
    token = strtok(NULL, ";");
    if(token) x = atof(token);
    
    token = strtok(NULL, ";");
    if(token) y = atof(token);
    
    token = strtok(NULL, ";");
    if(token) w = atof(token);
    
    token = strtok(NULL, ";");
    if(token) h = atof(token);
    
    token = strtok(NULL, ";");
    if(token) sw = atof(token);
    
    token = strtok(NULL, ";");
    if(token) strncpy(cfill, token, 49);
    
    token = strtok(NULL, ";");
    if(token) strncpy(cstrk, token, 49);
    
    fprintf(svg_file, "  <rect id=\"%s\" x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" ",
            key, x, y, w, h);
    
    if(strlen(cfill) > 0) {
        fprintf(svg_file, "fill=\"%s\" ", cfill);
    } else {
        fprintf(svg_file, "fill=\"none\" "); // Default se vazio
    }
    
    if(strlen(cstrk) > 0) {
        fprintf(svg_file, "stroke=\"%s\" stroke-width=\"%f\"", cstrk, sw);
    } else {
        // Se sem cor de borda informada, mas espessura > 0 default p/ black
        fprintf(svg_file, "stroke=\"black\" stroke-width=\"%f\"", sw);
    }
    
    fprintf(svg_file, " />\n");
}

void svg_gerar_quadras(HashFile hf_quadras, FILE* svg_file) {
    if (!hf_quadras || !svg_file) return;
    
    // Injeta a referência ao arquivo diretamente pro callback através de userdata
    hash_forall(hf_quadras, imprimir_quadra_svg_callback, svg_file);
}
