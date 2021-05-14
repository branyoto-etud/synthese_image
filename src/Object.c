#include "../include/Object.h"

/** ----------------------------------------- **/
/** ---------------- Drawing ---------------- **/
/** ----------------------------------------- **/

void draw_canonic_object(Object *object) {
    switch (object->id) {
        case _SPHERE_:
            glutSolidSphere(1., 40, 40);
            break;
        case _CUBE_:
            glutSolidCube(2.);
            break;
        default:
            printf("Unknown type %d\n", object->id);
    }
}

void draw_object(Object *object) {
    g3x_Material(object->color, object->mat.ambi, object->mat.diff, object->mat.spec, object->mat.shin,
                 1. - object->mat.alfa);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixd(object->Md.m);
    draw_canonic_object(object);
    glPopMatrix();
}

void draw_objects(Object *objects) {
    Object *object = objects;
    do {
        draw_object(object);
    } while ((object = object->next) != objects);
}

/** ----------------------------------------- **/
/** --------- Creation/Destruction ---------- **/
/** ----------------------------------------- **/

Object *pre_creation(G3Xcolor col, Material mat) {
    Object *object;
    if (NULL == (object = (Object *) malloc(sizeof(Object)))) {
        g3x_Quit();
    }
    reset_objet(object);
    object->next = NULL;
    object->color = col;
    object->mat = mat;
    return object;
}

Object *Cree_sphere_can(G3Xcolor col, Material mat) {
    Object *object = pre_creation(col, mat);
    object->inter = RayonInterSphere;
    object->id = _SPHERE_;
    return object;
}

Object *Cree_cube_can(G3Xcolor col, Material mat) {
    Object *object = pre_creation(col, mat);
    object->inter = RayonInterCube;
    object->id = _CUBE_;
    return object;
}

void free_objets(Object **objects) {
    Object *object = *objects;
    do {
        free(object);
    } while ((object = object->next) != *objects);
    *objects = NULL;
}

/** ----------------------------------------- **/
/** ------------- Modifications ------------- **/
/** ----------------------------------------- **/

void queue_object(Object **objects, Object *object) {
    Object *previous = *objects;
    if (NULL == previous) {
        *objects = object;
    } else {
        while (previous->next != *objects) {
            previous = previous->next;
        }
        previous->next = object;
    }
    object->next = *objects;
}

void reset_objet(Object *object) {
    object->Md = g3x_Identity();
    object->Mn = g3x_Identity();
    object->Mi = g3x_Identity();
}

void rescale_objet(Object *object, double a, double b, double c) {
    object->Md = g3x_ProdHMat(g3x_MakeHomothetieXYZ(a, b, c), object->Md);
    object->Mn = g3x_ProdHMat(g3x_MakeHomothetieXYZ(b * c, a * c, b * b), object->Mn);
    object->Mi = g3x_ProdHMat(object->Mi, g3x_MakeHomothetieXYZ(1 / a, 1 / b, 1 / c));
}

void xrotate_objet(Object *object, double arad) {
    object->Md = g3x_ProdHMat(g3x_MakeRotationX(arad), object->Md);
    object->Mn = g3x_ProdHMat(g3x_MakeRotationX(arad), object->Mn);
    object->Mi = g3x_ProdHMat(object->Mi, g3x_MakeRotationX(arad));
}

void yrotate_objet(Object *object, double arad) {
    object->Md = g3x_ProdHMat(g3x_MakeRotationY(arad), object->Md);
    object->Mn = g3x_ProdHMat(g3x_MakeRotationY(arad), object->Mn);
    object->Mi = g3x_ProdHMat(object->Mi, g3x_MakeRotationY(arad));
}

void zrotate_objet(Object *object, double arad) {
    object->Md = g3x_ProdHMat(g3x_MakeRotationZ(arad), object->Md);
    object->Mn = g3x_ProdHMat(g3x_MakeRotationZ(arad), object->Mn);
    object->Mi = g3x_ProdHMat(object->Mi, g3x_MakeRotationZ(arad));
}

void translate_objet(Object *object, double x, double y, double z) {
    object->Md = g3x_ProdHMat(g3x_MakeTranslationXYZ(x, y, z), object->Md);
    object->Mi = g3x_ProdHMat(object->Mi, g3x_MakeTranslationXYZ(-x, -y, -z));
}

/** ----------------------------------------- **/
/** ------------- Intersection -------------- **/
/** ----------------------------------------- **/

bool point_in_cube_can(G3Xpoint A) {
    return MAX3(fabs(A.x), fabs(A.y), fabs(A.z)) <= 1. + 1.e-8;
}

bool RayonInterCube(G3Xpoint *I, G3Xvector *N, G3Xpoint A, G3Xvector u) {
    static G3Xvector CN[6] = {{1,  0,  0}, {0,  1,  0}, {0,  0,  1},
                              {-1, 0,  0}, {0,  -1, 0}, {0,  0,  -1}};
    bool flags[6] = {false, false, false, false, false, false};
    if (A.x > 1) flags[0] = true; else if (A.x < -1) flags[3] = true;
    if (A.y > 1) flags[1] = true; else if (A.y < -1) flags[4] = true;
    if (A.z > 1) flags[2] = true; else if (A.z < -1) flags[5] = true;

    int i;
    for (i = 0; i < 6; i++) {
        if (flags[i] == false) continue;

        double ps1 = g3x_ProdScal(g3x_SetVect(A, CN[i]), CN[i]);
        if (ps1 >= 0) continue;
        double ps2 = g3x_ProdScal(u, CN[i]);
        if (ps2 >= 0) continue;

        double t = ps1 / ps2;
        I->x = A.x + t * u.x;
        I->y = A.y + t * u.y;
        I->z = A.z + t * u.z;
        *N = CN[i];
        if (!point_in_cube_can(*I)) continue;
        return true;
    }
    return false;
}

bool RayonInterSphere(G3Xpoint *I, G3Xvector *N, G3Xpoint A, G3Xvector u) {
    double t = -g3x_ProdScal(u, (G3Xvector) A);
    if (t < 0) return false;

    double d = g3x_SqrNorm(g3x_ProdVect(u, (G3Xvector) A));
    if (d > 1.) return false;

    t -= sqrt(1 - d);
    I->x = A.x + t * u.x;
    I->y = A.y + t * u.y;
    I->z = A.z + t * u.z;
    *N = (G3Xvector)(*I);

    return true;
}
