#ifndef __OBJET
#define __OBJET

#define _SPHERE_ 0
#define _CUBE_ 1

#include <g3x.h>

/* ambi   : réaction à la lumière ambiante (blanche par défaut)               */
/* diff   : réaction à la lumière diffuse (blanche par défaut)                */
/* spec   : réaction à la lumière de spéculosité (blanche par défaut)         */
/* shin   : coeff. de spéculosité                                             */
/* alfa   : valeur de transparence (1.: 100% transp. / 0.:opaque) */
/* n      : indice de refraction (€ [1, 2.5])                                 */
typedef struct {
    double ambi, diff, spec, shin, alfa, n;
} Material;

typedef struct __objet_s {
    int id;
    G3Xhmat Md, Mn, Mi;
    G3Xcolor color;
    Material mat;

    bool (*inter)(G3Xpoint *, G3Xvector *, G3Xpoint, G3Xvector);

    struct __objet_s *next;
} Object;

/** ---------------- Drawing ---------------- **/

void draw_canonic_object(Object *object);

void draw_object(Object *object);

void draw_objects(Object *objects);

/** --------- Creation/Destruction ---------- **/

Object *pre_creation(G3Xcolor col, Material mat);

Object *Cree_sphere_can(G3Xcolor col, Material mat);

Object *Cree_cube_can(G3Xcolor col, Material mat);

void free_objets(Object **objects);

/** ------------- Modifications ------------- **/

void queue_object(Object **objects, Object *object);

void reset_objet(Object *object);

void rescale_objet(Object *object, double a, double b, double c);

void xrotate_objet(Object *object, double arad);

void yrotate_objet(Object *object, double arad);

void zrotate_objet(Object *object, double arad);

void translate_objet(Object *object, double x, double y, double z);

/** ------------- Intersection -------------- **/

bool point_in_cube_can(G3Xpoint A);

bool RayonInterCube(G3Xpoint *I, G3Xvector *N, G3Xpoint A, G3Xvector u);

bool RayonInterSphere(G3Xpoint *I, G3Xvector *N, G3Xpoint A, G3Xvector u);

#endif
