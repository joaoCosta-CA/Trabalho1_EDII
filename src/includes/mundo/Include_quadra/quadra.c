#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quadra.h"

typedef struct {
    char cep[30];
    double x;
    double y;
    double width;
    double height;
    double sw;
    char cfill[32];
    char cstrk[32];
} InternalQuadra;

Quadra criar_quadra(const char* cep, double x, double y, double width, double height, double sw, const char* cfill, const char* cstrk) {
    InternalQuadra* q = (InternalQuadra*) malloc(sizeof(InternalQuadra));
    if (q != NULL) {
        strncpy(q->cep, cep, 29);
        q->cep[29] = '\0';
        q->x = x;
        q->y = y;
        q->width = width;
        q->height = height;
        q->sw = sw;
        
        if (cfill) {
            strncpy(q->cfill, cfill, 31);
            q->cfill[31] = '\0';
        } else {
            q->cfill[0] = '\0';
        }

        if (cstrk) {
            strncpy(q->cstrk, cstrk, 31);
            q->cstrk[31] = '\0';
        } else {
            q->cstrk[0] = '\0';
        }
    }
    return (Quadra) q;
}

void destruir_quadra(Quadra quadra) {
    if (quadra != NULL) {
        free(quadra);
    }
}

const char* quadra_get_cep(Quadra quadra) {
    if (quadra == NULL) return NULL;
    return ((InternalQuadra*)quadra)->cep;
}

double quadra_get_x(Quadra quadra) {
    if (quadra == NULL) return 0.0;
    return ((InternalQuadra*)quadra)->x;
}

double quadra_get_y(Quadra quadra) {
    if (quadra == NULL) return 0.0;
    return ((InternalQuadra*)quadra)->y;
}

double quadra_get_w(Quadra quadra) {
    if (quadra == NULL) return 0.0;
    return ((InternalQuadra*)quadra)->width;
}

double quadra_get_h(Quadra quadra) {
    if (quadra == NULL) return 0.0;
    return ((InternalQuadra*)quadra)->height;
}

double quadra_get_sw(Quadra quadra) {
    if (quadra == NULL) return 0.0;
    return ((InternalQuadra*)quadra)->sw;
}

const char* quadra_get_cfill(Quadra quadra) {
    if (quadra == NULL) return NULL;
    return ((InternalQuadra*)quadra)->cfill;
}

const char* quadra_get_cstrk(Quadra quadra) {
    if (quadra == NULL) return NULL;
    return ((InternalQuadra*)quadra)->cstrk;
}