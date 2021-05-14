#ifndef UTILS_H
#define UTILS_H

#include <g3x.h>

/** --------------- Vectors ----------------- **/

void reverseVector(G3Xvector *u, G3Xpoint *A);
G3Xvector reflectVector(G3Xvector u, G3Xvector N);
G3Xvector refractVector(G3Xvector u, G3Xvector N, double Dn);

/** ---------------- Colors ----------------- **/

G3Xcolor multColor(G3Xcolor color, double value);
G3Xcolor sumColor(G3Xcolor color1, G3Xcolor color2);

#endif
