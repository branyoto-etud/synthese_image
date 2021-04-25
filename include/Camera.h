#ifndef __CAMERA
#define __CAMERA

#define NBL 500
#define NBC 500

#include <g3x.h>
#include "Objet.h"

typedef struct
{
  G3Xhmat   Md;      /* matrice directe (la seule utile)             */
  int       nbc,nbl; /* tailles courantes du pixmap                  */
  G3Xcolor *col;     /* la "carte" des pixels                        */
} Cam;

Cam Cree_camera_can(void);
void Camera_setup(Cam* camera, double theta, double phi, double dist,
  G3Xpoint target, double foc, double resol, Objet* objets);
void Draw_camera(Cam* camera, double cam_dis);

#endif
