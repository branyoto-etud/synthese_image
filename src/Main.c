#define WWIDTH  1000
#define WHEIGHT 1000

#include <g3x.h>
#include <stdlib.h>
#include "../include/Camera.h"
#include "../include/Object.h"
#include "../include/CameraConfig.h"

Object *objects = NULL;
CameraConfig *config;
Cam camera;

static void Init_objets() {
    Object *sphere = Cree_sphere_can(G3Xr, (Material) {.5, .5, .5, 0.8, .3, 1.2});
    Object *s2 = Cree_sphere_can(G3Xb, (Material) {.5, .5, .5, 0.8, .9, 1.2});
    rescale_objet(sphere, .2, .2, .2);
    rescale_objet(s2, .2, .2, .2);
    translate_objet(sphere, .1, 0, 0);
    translate_objet(s2, .1, 0.5, 1.);
    queue_object(&objects, sphere);
    queue_object(&objects, s2);
}

static void init(void) {
    config = default_config();
    camera = Cree_camera_can();
    Init_objets();
    g3x_CreateScrollh_d("dF", &(config->focal), 0.0, 30., 1., "distance focale");
    g3x_CreateScrollh_d("Di", &(config->distance), 0.0, 30., 1., "distance de vue");
    g3x_CreateScrollh_d("Ph", &(config->phi), -PI, +PI, 1., "angle de vue (vertical)");
    g3x_CreateScrollh_d("Th", &(config->theta), -PI, +PI, 1., "angle de vue (horizontal)");

    g3x_CreateScrollv_d("res", &(config->resolution), 0.0, 1.0, 1., "résolution");
}

static void draw() {
    draw_objects(objects);
    /* g3x_Material((G3Xcolor) {0,0,0,1}, .3,.5,.2,0.2,1); */
    Camera_setup(&camera, config, objects);
    Draw_camera(&camera, config->distance);
}

static void quit() {
    free_camera(&camera);
    free_objets(&objects);
    free(config);
}

/***************************************************************************/
/*                                                                         */
/***************************************************************************/
int main(int argc, char **argv) {
    g3x_InitWindow(*argv, WWIDTH, WHEIGHT);
    g3x_SetBkgCol(0.3);

    g3x_SetPerspective(40., 100., 1.);

    g3x_SetCameraSpheric(-1.16, 0.8, 5., (G3Xpoint) {0., 0., 0.});
    g3x_SetLightCartesian((G3Xpoint) {0, 0, 1});

    g3x_SetInitFunction(init);
    g3x_SetDrawFunction(draw);
    g3x_SetExitFunction(quit);

    return g3x_MainStart();
}
