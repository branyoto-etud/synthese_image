#include "Rayon.h"
#include <float.h>

Rayon Cree_Rayon(G3Xpoint A, G3Xvector u) {
  Rayon R;
  R.origine   = A;
  R.direction = u;
  g3x_Normalize(&R.direction);
  R.color     = G3Xk;
  R.objet     = NULL;
  R.distance  = DBL_MAX;
  return R;
}

void RayIntersectWith(Rayon *R, Objet* o, G3Xpoint *I, G3Xvector *N) {
  /* Projetté du rayon dans l'espace canonique */
  G3Xpoint  A = g3x_ProdHMatPoint (o->Mi, R->origine  );
  G3Xvector u = g3x_ProdHMatVector(o->Mi, R->direction);
  g3x_Normalize(&u);

  G3Xpoint J;  /* Point d'intersection */
  G3Xvector M; /* Normale au point d'intersection */
  /* Stop si pas d'intersections */
  if (!o->inter(&J, &M, A, u)) return;

  double d = g3x_SqrDist(g3x_ProdHMatPoint(o->Md, J), R->origine);
  /* Stop si trop loin */
  if (d >= R->distance) return;

  /* Mise à jours du rayon */
  R->objet    = o;
  R->distance = d;
  /* Mise à jours du meilleur point d'intersection et de sa normale */
  *I = g3x_ProdHMatPoint (o->Md, J);
  *N = g3x_ProdHMatVector(o->Md, M);
  g3x_Normalize(N);
}

void RayTracer(Rayon *R, Objet* objets, G3Xpoint L, int rec) {
  G3Xpoint I; G3Xvector N;
  Objet* obj = objets;

  if (rec == 0) return;
  rec -= 1;

  /* Calcul de l'objet le plus proche */
  do {
    RayIntersectWith(R, obj, &I, &N);
  } while ((obj = obj->next) != objets);

  /* Si pas d'intersection -> le rayon se perd donc c'est fini */
  if (R->objet == NULL) return;

  R->color = R->objet->color;
  /* Si Niveau 0 -> fin */
  if (RAYTRACER_DEG == 0) return;
}

void Draw_Rayon(Rayon *R) {
  glDisable(GL_LIGHTING);                                   /* "débranche" la lumière */
    glColor3f(R->color.r,R->color.g,R->color.b);            /* la couleur du rayon    */
    glBegin(GL_LINES);                                      /* démarre une "ligne"    */
      glVertex3d(R->origine.x,R->origine.y,R->origine.z);   /* le début de la ligne   */
      glVertex3d(R->origine.x+R->direction.x,               /* la fin de la ligne     */
                 R->origine.y+R->direction.y,
                 R->origine.z+R->direction.z);
    glEnd();                                                /* ferme la ligne         */
  glEnable(GL_LIGHTING);                                    /* "rebranche" la lumière */
}
