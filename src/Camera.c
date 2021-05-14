#include <float.h>

#include "../include/Camera.h"
#include "../include/Rayon.h"

Cam Cree_camera_can(void) {
    Cam camera;
    camera.Md = g3x_Identity();
    camera.nbc = NBC;
    camera.nbl = NBL;
    /* allocation de l'image aux tailles max */
    if (!(camera.col = (G3Xcolor *) calloc(NBC * NBL, sizeof(G3Xcolor)))) g3x_Quit();
    return camera;
}

void free_camera(Cam *camera) {
    free(camera->col);
    camera->col = NULL;
}

void move_camera(Cam *camera, double focal, double rot_y, double rot_z, G3Xpoint position) {
    /* (reset)                                                                              */
    camera->Md = g3x_Identity();
    /* échelle (attention à la symétrie sur l'axe des x)                                    */
    camera->Md = g3x_ProdHMat(g3x_MakeHomothetieXYZ(-camera->nbc / (1. * camera->nbl), 1., focal), camera->Md);
    /* alignement                                                                           */
    camera->Md = g3x_ProdHMat(g3x_MakeRotationZ(PI / 2.), camera->Md);
    /* Orientation : le vecteur directeur de la cam. canonique (0.,0.,-1) s'aligne sur U    */
    camera->Md = g3x_ProdHMat(g3x_MakeRotationY(+rot_y), camera->Md);
    camera->Md = g3x_ProdHMat(g3x_MakeRotationZ(+rot_z), camera->Md);
    /* translation : positionnement                                                         */
    camera->Md = g3x_ProdHMat(g3x_MakeTranslationV(position), camera->Md);
}

void Camera_setup(Cam *camera, CameraConfig *config, Object *objects) {
    G3Xpoint position = get_camera_position(config);
    G3Xvector u = g3x_SetVect(position, config->target);
    g3x_Normalize(&u);
    /*                         |cos(rot_z)*sin(rot_y)   *
     * en coord. sphérique : u |sin(rot_z)*sin(rot_y)   *
     *                         |cos(rot_y)           */
    double rot_y = acos(u.z);
    double rot_z = G3Xiszero(rot_y) ? PI : ((u.y < 0. ? -1. : +1.) * acos(u.x / sin(rot_y)));
    move_camera(camera, config->focal, rot_y, rot_z, position);
    camera->nbc = NBC * config->resolution;
    camera->nbl = NBL * config->resolution;

    /* Get initial position */
    G3Xpoint origine = (G3Xpoint) {position.x - u.x * config->focal,
                                   position.y - u.y * config->focal,
                                   position.z - u.z * config->focal};


    /* Fill the pixmap */
    /* middle_ray(camera, objects, origine, config->light); */
    fill_pixmap(camera, objects, origine, config->light);
}

void middle_ray(Cam* camera, Object *objects, G3Xpoint origine, G3Xpoint light) {
    G3Xpoint target = g3x_ProdHMatPoint(camera->Md, (G3Xpoint) {0, 0, 0});
    G3Xvector v = g3x_SetVect(origine, target);
    Rayon R = Cree_Rayon(target, v);
    RayTracer(&R, objects, light, 4);

    int x,y;
    G3Xcolor *color = camera->col;
    for (x = 0; x < camera->nbl; x++) {     /* for each row             */
        for (y = 0; y < camera->nbc; y++) { /* for each columns         */
            *color = R.color;
            color++;
        }
    }
}

void fill_pixmap(Cam *camera, Object *objects, G3Xpoint origine, G3Xpoint light) {
    int x, y;                       /* indexes of the for loop  */
    double nbl = camera->nbl;       /* number of lines          */
    double nbc = camera->nbc;       /* number of columns        */
    G3Xcolor *color = camera->col;  /* first pixel              */

    for (x = 0; x < nbl; x++) {     /* for each row             */
        for (y = 0; y < nbc; y++) { /* for each columns         */
            /* The target pixel in the canonic screen */
            G3Xpoint target = (G3Xpoint) {(2 * x + 1) / nbl - 1, (2 * y + 1) / nbc - 1, 0};
            /* Canonic to real pos */
            target = g3x_ProdHMatPoint(camera->Md, target);
            Rayon R = Cree_Rayon(target, g3x_SetVect(origine, target));
            /* Shoot a ray */
            RayTracer(&R, objects, light, 4);
            if (R.object == NULL) { /* No hit -> chessboard pattern */
                R.color = ((x % 4 < 2 && y % 4 < 2) || (x % 4 >= 2 && y % 4 >= 2) ? G3Xk_c : G3Xw_c);
            }
            *color = R.color;
            color++;
        }
    }
}

void Draw_camera(Cam *camera, double distance) {
    glPushMatrix();
    glMultMatrixd(camera->Md.m);
    /* caméra canonique */
    /* Chargement Pixmap => Texture Gl */
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, camera->nbl, camera->nbc, GL_RGBA, GL_FLOAT, camera->col);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor4f(1., 1., 1., 1.);
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0);
    glVertex3d(-1., -1., 0.);
    glTexCoord2d(1, 0);
    glVertex3d(-1., +1., 0.);
    glTexCoord2d(1, 1);
    glVertex3d(+1., +1., 0.);
    glTexCoord2d(0, 1);
    glVertex3d(+1., -1., 0.);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    /* cadre */
    glDisable(GL_LIGHTING);
    glColor3f(1., 1., 1.);
    glBegin(GL_LINE_LOOP);
    glVertex3d(-1., -1., 0.);
    glVertex3d(-1., +1., 0.);
    glVertex3d(+1., +1., 0.);
    glVertex3d(+1., -1., 0.);
    glEnd();
    glColor3f(.5, .5, .5);
    glBegin(GL_LINES);
    glVertex3d(-1., -1., 0.);
    glVertex3d(0., 0., -1.);
    glVertex3d(-1., +1., 0.);
    glVertex3d(0., 0., -1.);
    glVertex3d(+1., +1., 0.);
    glVertex3d(0., 0., -1.);
    glVertex3d(+1., -1., 0.);
    glVertex3d(0., 0., -1.);
    glEnd();
    glColor3f(1., 0., 0.);
    glBegin(GL_LINES);
    glVertex3d(0., 0., -1.);
    glVertex3d(0., 0., 0.5 * distance);
    glEnd();
    glEnable(GL_LIGHTING);
    glPopMatrix();
}
