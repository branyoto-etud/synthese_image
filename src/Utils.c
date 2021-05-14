#include "../include/../include/Utils.h"

/** ----------------------------------------- **/
/** --------------- Vectors ----------------- **/
/** ----------------------------------------- **/

void reverseVector(G3Xvector *u, G3Xpoint *A) {
    g3x_Normalize(u);
    double t = 4 * g3x_Dist(*A, (G3Xpoint) {0, 0, 0});
    *A = (G3Xpoint) {A->x + t * u->x, A->y + t * u->y, A->z + t * u->z};
    *u = (G3Xvector) {-u->x, -u->y, -u->z};
    g3x_Normalize(u);
}
G3Xvector reflectVector(G3Xvector u, G3Xvector N) {
    return g3x_SubVect(g3x_mapscal3(N, 2 * g3x_ProdScal(N, u)), u);
}
G3Xvector refractVector(G3Xvector u, G3Xvector N, double Dn) {
    double ps = g3x_ProdScal(u, N);
    double a = (SQR(Dn) * (1 - SQR(ps)));
    double s = a > 1 ? 0 : sqrt(1 - a);
    double c = s + (Dn * ps);
    return g3x_AddVect(g3x_mapscal3(N, c), g3x_mapscal3(u, Dn));
}

/** ----------------------------------------- **/
/** ---------------- Colors ----------------- **/
/** ----------------------------------------- **/

G3Xcolor multColor(G3Xcolor color, double value) {
    return (G3Xcolor) {
            CLIP(0, color.r * value, 1.),
            CLIP(0, color.g * value, 1.),
            CLIP(0, color.b * value, 1.),
            color.a
    };
}
G3Xcolor sumColor(G3Xcolor color1, G3Xcolor color2) {
    return (G3Xcolor) {
            CLIP(0, color1.r + color2.r, 1.),
            CLIP(0, color1.g + color2.g, 1.),
            CLIP(0, color1.b + color2.b, 1.),
            CLIP(0, color1.a + color2.a, 1)
    };
}