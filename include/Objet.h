#ifndef __OBJET
#define __OBJET

#define _SPHERE_ 0
#define _CUBE_ 1

#include <g3x.h>

  /* ambi   : réaction à la lumière ambiante (blanche par défaut)               */
  /* diff   : réaction à la lumière diffuse (blanche par défaut)                */
  /* spec   : réaction à la lumière de spéculosité (blanche par défaut)         */
  /* shin   : coeff. de spéculosité                                             */
  /* alfa   : valeur de transparence (1.: objet totalement transp. / 0.:opaque) */
  /* n      : indice de refraction (€ [1, 2.5])                                 */
typedef struct {
  double ambi, diff, spec, shin, alfa, n;
} Material;

typedef struct __objet_s {
  int id;
  G3Xhmat Md, Mn, Mi;
  G3Xcolor color;
  Material mat;

  bool (*inter)(G3Xpoint*, G3Xvector*, G3Xpoint, G3Xvector);

  struct __objet_s *next;
} Objet;

void draw_object(Objet *obj);

Objet* Cree_sphere_can(G3Xcolor col, Material mat);
Objet* Cree_cube_can(G3Xcolor col, Material mat);

void reset_objet(Objet *obj);
void rescale_objet(Objet *obj, double a, double b, double c);
void xrotate_objet(Objet *obj, double arad);
void yrotate_objet(Objet *obj, double arad);
void zrotate_objet(Objet *obj, double arad);
void translate_objet(Objet *obj, double x, double y, double z);

#endif
