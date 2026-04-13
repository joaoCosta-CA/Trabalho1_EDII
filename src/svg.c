#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "svg/svg.h"
#include "estr_dados/Include_HashFile/HashFile.h"
#include "estr_dados/Include_list/list.h"

typedef struct {
    double min_x;
    double min_y;
    double max_x;
    double max_y;
    int started;
} BBox;

static void calcular_bbox_callback(const char* key, const char* value, void* userdata) {
    BBox* box = (BBox*)userdata;
    char buffer[300];
    strncpy(buffer, value, sizeof(buffer) - 1);
    buffer[sizeof(buffer)-1] = '\0';
    
    char *token;
    double x = 0, y = 0, w = 0, h = 0;
    
    token = strtok(buffer, ";"); // cep
    token = strtok(NULL, ";");
    if(token) x = atof(token);
    token = strtok(NULL, ";");
    if(token) y = atof(token);
    token = strtok(NULL, ";");
    if(token) w = atof(token);
    token = strtok(NULL, ";");
    if(token) h = atof(token);
    
    double mx = x + w;
    double my = y + h;
    
    if (!box->started) {
        box->min_x = x;
        box->min_y = y;
        box->max_x = mx;
        box->max_y = my;
        box->started = 1;
    } else {
        if (x < box->min_x) box->min_x = x;
        if (y < box->min_y) box->min_y = y;
        if (mx > box->max_x) box->max_x = mx;
        if (my > box->max_y) box->max_y = my;
    }
}

void svg_calcular_bbox(HashFile hf_quadras, double *min_x, double *min_y, double *max_x, double *max_y) {
    if (!hf_quadras) return;
    
    BBox box = {0, 0, 0, 0, 0};
    hash_forall(hf_quadras, calcular_bbox_callback, &box);
    
    if (box.started) {
        *min_x = box.min_x;
        *min_y = box.min_y;
        *max_x = box.max_x;
        *max_y = box.max_y;
    } else {
        // Valores default caso não existam quadras
        *min_x = 0;
        *min_y = 0;
        *max_x = 1000;
        *max_y = 1000;
    }
}

FILE* svg_iniciar(const char* caminho_arquivo, double min_x, double min_y, double w, double h) {
    FILE *f = fopen(caminho_arquivo, "w");
    if (!f) {
        perror("Erro ao iniciar o arquivo SVG");
        return NULL;
    }
    fprintf(f, "<!-- SVG Gerado pelo TED (Trabalho ED2) -->\n");
    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"%f %f %f %f\">\n", min_x, min_y, w, h);
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
        fprintf(svg_file, "fill=\"%s\" fill-opacity=\"0.8\" ", cfill);
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
    fprintf(svg_file, "  <text x=\"%f\" y=\"%f\" fill=\"%s\" stroke=\"black\" font-size=\"12\">%s</text>\n", 
            x + 5.0, y + 9.0, (strlen(cstrk) > 0 ? cstrk : "black"), key);
}

void svg_gerar_quadras(HashFile hf_quadras, FILE* svg_file) {
    if (!hf_quadras || !svg_file) return;
    
    // Injeta a referência ao arquivo diretamente pro callback através de userdata
    hash_forall(hf_quadras, imprimir_quadra_svg_callback, svg_file);
}

void svg_gerar_decoracoes(void* decoracoes_list, FILE* f) {
    if (!decoracoes_list || !f) return;
    void* pos = getFirst(decoracoes_list);
    while (pos != NULL) {
        char* svg_str = (char*)get(decoracoes_list, pos);
        if (svg_str) {
            fprintf(f, "%s\n", svg_str);
        }
        pos = getNext(decoracoes_list, pos);
    }
}
