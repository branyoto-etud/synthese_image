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
  *N = g3x_ProdHMatVector(o->Mn, M);
  g3x_Normalize(N);
}

G3Xcolor shadeColor(G3Xcolor color, double value) {
  return (G3Xcolor) { CLIP(0, color.r * value, 1.) , 
    CLIP(0, color.g * value, 1.) , CLIP(0, color.b * value, 1.) , color.a };
}

G3Xcolor addColors(G3Xcolor color1, G3Xcolor color2) {
  return (G3Xcolor) { CLIP(0, color1.r + color2.r, 1.) , 
    CLIP(0, color1.g + color2.g, 1.) , CLIP(0, color1.b + color2.b, 1.) , CLIP(0, color1.a + color2.a, 1) };

}

void RayTracer(Rayon *R, Objet* objets, G3Xpoint L, int rec) {
  G3Xpoint I; G3Xvector N;
  G3Xvector w;

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

  /* On évalue la norme du vecteur entre l'intersection I et la source de lumière L */
  w = g3x_SetVect(I, L);
  g3x_Normalize(&w);

  double prod_scal = g3x_ProdScal(N, w);

  if (prod_scal <= 0) {
    if (RAYTRACER_DEG <= 1) {
      R->color = G3Xk;
    } else {
      R->color = shadeColor(R->objet->color, R->objet->mat.ambi);
    }
  } else if (RAYTRACER_DEG <= 1) {
    R->color = shadeColor(R->objet->color, prod_scal);
  } else {
    R->color = addColors(shadeColor(R->objet->color, R->objet->mat.diff * prod_scal), shadeColor(R->objet->color, R->objet->mat.ambi));
  }

  if (RAYTRACER_DEG <= 2) return;


  
}

void Draw_Rayon(Rayon *R) {
  glDisable(GL_LIGHTING);                                      /* "débranche" la lumière */
    glColor3f(R->color.r,R->color.g,R->color.b);            /* la couleur du rayon    */
    glBegin(GL_LINES);                                      /* démarre une "ligne"    */
      glVertex3d(R->origine.x,R->origine.y,R->origine.z);   /* le début de la ligne   */
      glVertex3d(R->origine.x+R->direction.x,               /* la fin de la ligne     */
                 R->origine.y+R->direction.y,
                 R->origine.z+R->direction.z);
    glEnd();                                                /* ferme la ligne         */
  glEnable(GL_LIGHTING);                                    /* "rebranche" la lumière */
}
