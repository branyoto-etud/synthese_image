#ifndef __RAYON
#define __RAYON

#include <g3x.h>
#include "Objet.h"

typedef struct {
  G3Xpoint origine;
  G3Xvector direction;
  G3Xcolor color;
  Objet* objet;
  double distance;
} Rayon;

Rayon Cree_Rayon(G3Xpoint A, G3Xvector u);
void  RayTracer(Rayon *R, Objet* objets, G3Xpoint L, int rec);
void  Draw_Rayon(Rayon *R);


#endif
