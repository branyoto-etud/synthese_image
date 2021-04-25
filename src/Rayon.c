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

void RayTracer(Rayon *R, Objet* objets, G3Xpoint L, int rec) {
  if (rec == 0) return; /* Not used for the moment */
  rec -= 1;

  Objet* obj = objets;
  G3Xpoint saveI; G3Xvector saveN;
  do {
    G3Xpoint  A = g3x_ProdHMatPoint (obj->Mi, R->origine  );
    G3Xvector u = g3x_ProdHMatVector(obj->Mi, R->direction);
    g3x_Normalize(&u);
    G3Xpoint I; G3Xvector N;
    if (obj->inter(&I, &N, A, u)) {
      double d = g3x_SqrDist(g3x_ProdHMatPoint(obj->Md, I), R->origine);
      if (d < R->distance) {
        R->objet    = obj;
        R->distance =   d;
        saveI = g3x_ProdHMatPoint (obj->Md, I);
        saveN = g3x_ProdHMatVector(obj->Md, N);
        g3x_Normalize(&saveN);
      }
    }
  } while ((obj = obj->next) != objets);
  if (R->objet == NULL) return;
  R->color = R->objet->color;
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
