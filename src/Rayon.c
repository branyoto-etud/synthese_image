#include "../include/Rayon.h"
#include "../include/Utils.h"
#include <float.h>

Rayon Cree_Rayon(G3Xpoint A, G3Xvector u) {
    Rayon R;
    R.origine = A;
    R.direction = u;
    g3x_Normalize(&R.direction);
    R.color = G3Xk;
    R.object = NULL;
    R.distance = DBL_MAX;
    return R;
}
bool IsNearer(Rayon *R, Object* object, G3Xpoint A, G3Xvector u, G3Xpoint *I, G3Xvector *N) {
    if (!object->inter(I, N, A, u)) return false;

    double distance = g3x_SqrDist(g3x_ProdHMatPoint(object->Md, *I), R->origine);
    if (distance >= R->distance) return false;

    R->distance = distance;
    R->object = object;
    return true;
}
void RayIntersection(Rayon *R, Object *object, G3Xpoint *I, G3Xvector *N, bool reversed) {
    G3Xpoint A = g3x_ProdHMatPoint(object->Mi, R->origine);
    G3Xvector u = g3x_ProdHMatVector(object->Mi, R->direction);
    g3x_Normalize(&u);
    if (reversed) reverseVector(&u, &A);

    G3Xpoint J; G3Xvector M;
    if (IsNearer(R, object, A, u, &J, &M) == false) return;

    *I = g3x_ProdHMatPoint(object->Md, J);
    *N = g3x_ProdHMatVector(object->Mn, M);
    g3x_Normalize(N);
}

bool compute_nearest(Rayon *R, Object *objects, G3Xpoint *I, G3Xvector *N) {
    Object *obj = objects;
    do {
        RayIntersection(R, obj, I, N, false);
    } while ((obj = obj->next) != objects);
    return R->object == NULL ? false : true;
}

G3Xcolor compute_final_color(Material m, G3Xcolor objectColor, G3Xcolor lightColor, G3Xcolor reflectedColor,
                             G3Xcolor refractedColor, double ps_Nw, double ps_uWr) {
    return (G3Xcolor) {
            CLIP(0, (m.ambi + m.diff * ps_Nw) * objectColor.r + m.spec * m.shin * MAX(0, pow(-ps_uWr, 1 / (1-m.shin))) * lightColor.r + m.spec * reflectedColor.r + m.alfa * refractedColor.r, 1),
            CLIP(0, (m.ambi + m.diff * ps_Nw) * objectColor.g + m.spec * m.shin * MAX(0, pow(-ps_uWr, 1 / (1-m.shin))) * lightColor.g + m.spec * reflectedColor.g + m.alfa * refractedColor.g, 1),
            CLIP(0, (m.ambi + m.diff * ps_Nw) * objectColor.b + m.spec * m.shin * MAX(0, pow(-ps_uWr, 1 / (1-m.shin))) * lightColor.b + m.spec * reflectedColor.b + m.alfa * refractedColor.b, 1),
            CLIP(0, (m.ambi + m.diff * ps_Nw) * objectColor.a + m.spec * m.shin * MAX(0, pow(-ps_uWr, 1 / (1-m.shin))) * lightColor.a + m.spec * reflectedColor.a + m.alfa * refractedColor.a, 1)
    };
}

void RayTracer(Rayon *R, Object *objects, G3Xpoint L, int rec) { /* DEG 3 : Reflections */
    if (rec <= 0) return;
    rec -= 1;

    G3Xpoint I;
    G3Xvector N;
    Object *obj = objects;
    do {
        RayIntersection(R, obj, &I, &N, false);
    } while ((obj = obj->next) != objects);
    if (R->object == NULL) return;

    G3Xvector w = g3x_SetVect(I, L);
    g3x_Normalize(&w);

    double ps = g3x_ProdScal(N, w);
    if (ps <= 0) {
        R->color = compute_final_color(R->object->mat, R->object->color, G3Xw, G3Xk, G3Xk, ps, 0);
        return;
    }

    Rayon refl = Cree_Rayon(I, g3x_SubVect(R->direction, g3x_mapscal3(N, 2 * g3x_ProdScal(N, R->direction))));
    RayTracer(&refl, objects, L, rec);

    double ps_uWr = -g3x_ProdScal(R->direction, reflectVector(w, N));
    R->color = compute_final_color(R->object->mat, R->object->color, G3Xw, refl.color, G3Xk, ps, ps_uWr);
}

void Draw_Rayon(Rayon *R) {
    glDisable(GL_LIGHTING);                                   /* "débranche" la lumière */
    glColor3f(R->color.r, R->color.g, R->color.b);            /* la couleur du rayon    */
    glBegin(GL_LINES);                                      /* démarre une "ligne"    */
    glVertex3d(R->origine.x, R->origine.y, R->origine.z);   /* le début de la ligne   */
    glVertex3d(R->origine.x + R->direction.x,               /* la fin de la ligne     */
               R->origine.y + R->direction.y,
               R->origine.z + R->direction.z);
    glEnd();                                                /* ferme la ligne         */
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(R->origine.x, R->origine.y, R->origine.z);
    glutSolidSphere(.01, 40, 40);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}
