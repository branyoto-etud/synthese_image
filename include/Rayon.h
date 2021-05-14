#ifndef __RAYON
#define __RAYON

#include <g3x.h>
#include <math.h>
#include "../include/Object.h"

#define RAYTRACER_DEG 4

typedef struct {
    G3Xpoint origine;
    G3Xvector direction;
    G3Xcolor color;
    Object *object;
    double distance;
} Rayon;

Rayon Cree_Rayon(G3Xpoint A, G3Xvector u);

void RayTracer(Rayon *R, Object *objects, G3Xpoint L, int rec);

void Draw_Rayon(Rayon *R);


#endif
