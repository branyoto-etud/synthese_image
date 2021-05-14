#ifndef __CAMERA
#define __CAMERA

#define NBL 500
#define NBC 500

#include <g3x.h>
#include "../include/Object.h"
#include "../include/CameraConfig.h"

typedef struct {
    G3Xhmat Md;      /* matrice directe (la seule utile)             */
    int nbc, nbl; /* tailles courantes du pixmap                  */
    G3Xcolor *col;     /* la "carte" des pixels                        */
} Cam;


Cam Cree_camera_can(void);

void free_camera(Cam *camera);

void move_camera(Cam *camera, double focal, double rot_y, double rot_z, G3Xpoint position);

void Camera_setup(Cam *camera, CameraConfig *config, Object *objects);

void middle_ray(Cam* camera, Object *objects, G3Xpoint origine, G3Xpoint light);
void fill_pixmap(Cam *camera, Object *objects, G3Xpoint origine, G3Xpoint light);

void Draw_camera(Cam *camera, double cam_dis);

#endif
