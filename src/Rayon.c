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
  return (G3Xcolor) { 
    CLIP(0, color.r * value, 1.) , 
    CLIP(0, color.g * value, 1.) , 
    CLIP(0, color.b * value, 1.) , 
    color.a 
  };
}

G3Xcolor colorSum(G3Xcolor color1, G3Xcolor color2) {
  return (G3Xcolor) { 
    CLIP(0, color1.r + color2.r, 1.) , 
    CLIP(0, color1.g + color2.g, 1.) , 
    CLIP(0, color1.b + color2.b, 1.) , 
    CLIP(0, color1.a + color2.a, 1) 
  };
}

G3Xvector mulVect(G3Xvector V, double value) {
  return (G3Xvector) { V.x * value, V.y * value, V.z * value };
}

void RayTracer(Rayon *R, Objet* objets, G3Xpoint L, int rec) {
  G3Xpoint I; G3Xvector N;
  Objet* obj = objets;
  int i;

  if (rec == 0) return;
  rec -= 1;

  /* Calcul de l'objet le plus proche */
  do {
    RayIntersectWith(R, obj, &I, &N);
  } while ((obj = obj->next) != objets);

  /* Si pas d'intersection -> le rayon se perd donc c'est fini */
  if (R->objet == NULL) return;

  R->color = R->objet->color;

  if (RAYTRACER_DEG < 1) return;
  /* DEGRÉS 1 & 2 */

  /* On évalue la norme du vecteur entre l'intersection I et la source de lumière L */
  G3Xvector w = g3x_SetVect(I, L);
  double prod_scal;

  g3x_Normalize(&w);
  prod_scal = g3x_ProdScal(N, w);

  if (prod_scal <= 0) {
    if (RAYTRACER_DEG <= 1) {
      R->color = G3Xk;
    } else {
      R->color = shadeColor(R->objet->color, R->objet->mat.ambi);
    }
  } else {
    if (RAYTRACER_DEG <= 1) {
      R->color = shadeColor(R->objet->color, prod_scal);
    } else {
      R->color = colorSum(shadeColor(R->objet->color, R->objet->mat.diff * prod_scal), shadeColor(R->objet->color, R->objet->mat.ambi));
    }
  }

  if (RAYTRACER_DEG < 3) return;
  
  /* DEGRÉ 3 */

  /* symétrique du vecteur IL par rapport à la normale N */
  G3Xvector sym_w = g3x_SubVect(mulVect(N, 2 * prod_scal), w);

  G3Xcolor shine = shadeColor(G3Xw, 
    R->objet->mat.shin * R->objet->mat.spec * 
    MAX(0, pow( - g3x_ProdScal(R->direction, sym_w), (double) (1. / (1 - R->objet->mat.shin)) )) 
  );


  R->color = colorSum(R->color, shine);

  /* vecteur réfléchi */

  /*
  for (i = 0; i < 1; i++) {
    G3Xvector vr = g3x_AddVect(mulVect(N, 2 * g3x_ProdScal(R->direction, N)), R->direction);
    Objet* objs = objets;

    Rayon Rr = Cree_Rayon(I, vr);
    do {
      RayIntersectWith(&Rr, objs, &I, &N);
    } while ((objs = objs->next) != objets);

    Rr.color = Rr.objet->color;
    R->color = colorSum(R->color, shadeColor(Rr.color, Rr.objet->mat.spec));
    Draw_Rayon(&Rr);
  }
  */
  
  if (RAYTRACER_DEG < 4) return;




  /* DEGRÉ 4 */

  Rayon T = Cree_Rayon(I, R->direction);
  G3Xpoint J; 
  Objet* obj2 = objets;

  do {
    RayIntersectWith(&T, obj2, &J, &N);
  } while ((obj2 = obj2->next) != objets);

  if (T.objet == NULL) return;

  T.color = T.objet->color;
  R->color = T.color;

  Draw_Rayon(&T);
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
