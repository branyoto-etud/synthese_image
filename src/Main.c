#define WWIDTH  1000
#define WHEIGHT 1000

#include <g3x.h>
#include "Camera.h"
#include "Objet.h"

/* la Caméra globale */
Cam    camera;
Objet* objets = NULL;

/* paramètres réglables de la Caméra : position, orientation, focale, résolution */
double     cam_the=-1.5/*0.05*/, cam_phi=1.5/*PI/4*/, cam_dis=1., cam_foc=4., cam_res=.4;
G3Xpoint   cam_tar={0.,0.,0.};

static void addObject(Objet* o) {
  Objet* last = objets;

  if (last == NULL) {
    objets = o;
  } else {
    while (last->next != objets) {
      last = last->next;
    }
    last->next = o;
  }
  o->next = objets;
}

static void Init_objets() {
  Objet *S1 = Cree_sphere_can(G3Xr, (Material){.5, .5, 0, 0, 0, 1}); 
  Objet *S2 = Cree_sphere_can(G3Xb, (Material){.2, .5, .5, .5, 0, 1}); 
  Objet *S3 = Cree_cube_can(G3Xg, (Material){.2, .5, .3, .5, .5, 1}); 
  Objet *S4 = Cree_sphere_can(G3Xc, (Material){.2, .5, .9, .9, .7, 1}); 
  Objet *L = Cree_sphere_can(G3Xw, (Material){.2, .5, 0, 0, 0.2, 1});
  Objet *M = Cree_sphere_can(G3Xm_a, (Material){.2, .5, .5, .7, 0, 1});
  rescale_objet(S1, .2, .2, .2);
  rescale_objet(S2, .1, .1, .1);
  rescale_objet(S3, .2, .2, .2);
  rescale_objet(S4, .3, .3, .3);
  rescale_objet(L, .05, .05, .05);
  rescale_objet(M, .05, .05, .05);
  translate_objet(S1, .5, .5, .5);
  translate_objet(S2, 0, 0, 0);
  translate_objet(S3, 0, 0, -.6);
  translate_objet(S4, 0, -.5, 0);
  translate_objet(L, .2, 0, 1);
  translate_objet(M, 0, 0, .8);
  addObject(S1);
  addObject(S2);
  addObject(S3);
  addObject(S4);
  addObject(L);
  addObject(M);
}


static void init(void)
{
  camera  = Cree_camera_can();
  Init_objets();
  g3x_CreateScrollh_d("dF" ,&cam_foc, 0.0, 30., 1.,"distance focale");
  g3x_CreateScrollh_d("Di" ,&cam_dis, 0.0, 30., 1.,"distance de vue");
  g3x_CreateScrollh_d("Ph" ,&cam_phi, -PI, +PI, 1.,"angle de vue (vertical)");
  g3x_CreateScrollh_d("Th" ,&cam_the, -PI, +PI, 1.,"angle de vue (horizontal)");

  g3x_CreateScrollv_d("res",&cam_res, 0.0,1.0,1.,"résolution");
}

/*! la fonction d'affichage !*/
static void draw() {
  Objet* o = objets;
  do {
    draw_object(o);
  } while ((o = o->next) != objets);

  Camera_setup(&camera,cam_the,cam_phi, cam_dis,cam_tar, cam_foc, cam_res, objets);
  Draw_camera(&camera, cam_dis);
}

/* fonction de sortie */
static void quit()
{
  /* TODO : Free les objets */
  free(camera.col); camera.col=NULL;
}

/***************************************************************************/
/*                                                                         */
/***************************************************************************/
int main(int argc, char **argv)
{
	g3x_InitWindow(*argv,WWIDTH,WHEIGHT);
  g3x_SetBkgCol(0.3);

  g3x_SetPerspective(40.,100.,1.);

  g3x_SetCameraSpheric(0.,PI/4.,5.,(G3Xpoint){0.,0.,0.});
  g3x_SetLightCartesian((G3Xpoint) {0, 0, 1});

	g3x_SetInitFunction(init);
	g3x_SetDrawFunction(draw);
	g3x_SetExitFunction(quit);

  return g3x_MainStart();
}
