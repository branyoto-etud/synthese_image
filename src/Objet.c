#include "Objet.h"

void draw_object(Objet *obj)
{
	g3x_Material(obj->color, obj->mat.ambi,obj->mat.diff,obj->mat.spec,obj->mat.shin,1.-obj->mat.alfa);
  glMatrixMode(GL_MODELVIEW);     /* appelle la bonne pile de matrices GL                                     */
	glPushMatrix();                 /* "ouvre" la pile                                                          */
		glMultMatrixd(obj->Md.m);     /* ajoute la matrice de l'objet sur la pile                                 */
                                  /* et applique toute la pile de matrices à l'objet canonique                */
    if (obj->id == _SPHERE_) {
      glutSolidSphere(1.,40,40);  /* la sphère canonique GLUT (rayon,nb.de "méridiens", nb. de "parallèles"   */
    } else {
      glutSolidCube(2.);          /* le cube canonique GLUT (de côté 2)                                       */
    }
	glPopMatrix();                /* supprime la dernière matrice (celle de l'objet) de la pile (et la "ferme") */
}

bool RayonInterSphere(G3Xpoint* I, G3Xvector* N, G3Xpoint A, G3Xvector u) {
  double t = -g3x_ProdScal(u, (G3Xvector) A);
  if (t < 0) return false;

  double d = g3x_SqrNorm(g3x_ProdVect(u, (G3Xvector)A));
  if (d > 1.) return false;

  t -= sqrt(1 - d);
  I->x = A.x + t * u.x;
  I->y = A.y + t * u.y;
  I->z = A.z + t * u.z;
  *N = (G3Xvector)(*I);

  return true;
}
Objet* Cree_sphere_can(G3Xcolor col, Material mat) {
  Objet* obj;
  if (NULL == (obj = (Objet*)malloc(sizeof(Objet)))) {
    return NULL;
  }

  obj->id = _SPHERE_;
  reset_objet(obj);
  obj->color = col;
  obj->mat = mat;
  obj->inter = RayonInterSphere;
  obj->next = NULL;
  return obj;
}

static bool point_in_cube_can(G3Xpoint A) {
  return MAX3(fabs(A.x), fabs(A.y), fabs(A.z)) <= 1. + 1.e-8;
}

bool RayonInterCube(G3Xpoint* I, G3Xvector* N, G3Xpoint A, G3Xvector u) {
  static G3Xvector CN[6] = {{ 1, 0, 0}, { 0, 1, 0}, { 0, 0, 1},
                            {-1, 0, 0}, { 0,-1, 0}, { 0, 0,-1}};
  bool flags[6] = {false, false, false, false, false, false};
  if (A.x > 1) flags[0] = true; else if (A.x < -1) flags[3] = true;
  if (A.y > 1) flags[1] = true; else if (A.y < -1) flags[4] = true;
  if (A.z > 1) flags[2] = true; else if (A.z < -1) flags[5] = true;

  int i;
  for (i = 0; i < 6; i++) {
    if (flags[i] == false) continue;

    double ps1 = g3x_ProdScal(g3x_SetVect(A, CN[i]), CN[i]); if (ps1 >= 0) continue;
    double ps2 = g3x_ProdScal(                    u, CN[i]); if (ps2 >= 0) continue;

    double t = ps1 / ps2;
    I->x = A.x + t * u.x;
    I->y = A.y + t * u.y;
    I->z = A.z + t * u.z;
    *N = CN[i];
    if (! point_in_cube_can(*I)) continue;
    return true;
  }
  return false;
}

Objet* Cree_cube_can(G3Xcolor col, Material mat) {
  Objet* obj;
  if (NULL == (obj = (Objet*)malloc(sizeof(Objet)))) {
    return NULL;
  }
  obj->id = _CUBE_;
  reset_objet(obj);
  obj->color = col;
  obj->mat = mat;
  obj->inter = RayonInterCube;
  obj->next = NULL;
  return obj;
}
void reset_objet(Objet *obj) {
  obj->Md = g3x_Identity();
  obj->Mn = g3x_Identity();
  obj->Mi = g3x_Identity();
}
void rescale_objet(Objet *obj, double a, double b, double c) {
  obj->Md = g3x_ProdHMat(g3x_MakeHomothetieXYZ(a,b,c),       obj->Md);
  obj->Mn = g3x_ProdHMat(g3x_MakeHomothetieXYZ(b*c,a*c,b*b), obj->Mn);
  obj->Mi = g3x_ProdHMat(obj->Mi,  g3x_MakeHomothetieXYZ(1/a,1/b,1/c));
}
void xrotate_objet(Objet *obj, double arad) {
  obj->Md = g3x_ProdHMat(g3x_MakeRotationX(arad),  obj->Md);
  obj->Mn = g3x_ProdHMat(g3x_MakeRotationX(arad),  obj->Mn);
  obj->Mi = g3x_ProdHMat(obj->Mi,  g3x_MakeRotationX(arad));
}
void yrotate_objet(Objet *obj, double arad) {
  obj->Md = g3x_ProdHMat(g3x_MakeRotationY(arad),  obj->Md);
  obj->Mn = g3x_ProdHMat(g3x_MakeRotationY(arad),  obj->Mn);
  obj->Mi = g3x_ProdHMat(obj->Mi,  g3x_MakeRotationY(arad));
}
void zrotate_objet(Objet *obj, double arad) {
  obj->Md = g3x_ProdHMat(g3x_MakeRotationZ(arad),  obj->Md);
  obj->Mn = g3x_ProdHMat(g3x_MakeRotationZ(arad),  obj->Mn);
  obj->Mi = g3x_ProdHMat(obj->Mi,  g3x_MakeRotationZ(arad));
}
void translate_objet(Objet *obj, double x, double y, double z) {
  obj->Md = g3x_ProdHMat(g3x_MakeTranslationXYZ(x, y, z),    obj->Md);
  obj->Mi = g3x_ProdHMat(obj->Mi, g3x_MakeTranslationXYZ(-x, -y, -z));
}
