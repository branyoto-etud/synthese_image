#include "Camera.h"
#include "Rayon.h"

Cam Cree_camera_can(void)
{
  Cam camera;
  camera.Md  = g3x_Identity();
  camera.nbc = NBC;
  camera.nbl = NBL;
  /* allocation de l'image aux tailles max */
  if (! (camera.col = (G3Xcolor*)calloc(NBC*NBL,sizeof(G3Xcolor))) ) g3x_Quit();

  return camera;
}

void Camera_setup(Cam* camera, double theta, double phi, double dist, G3Xpoint target,
  double foc, double resol, Objet* objets)
{
  G3Xpoint light = (G3Xpoint) {0,0,0};
  G3Xpoint  pos = (G3Xpoint){ target.x + dist*cos(theta)*sin(phi),
                              target.y + dist*sin(theta)*sin(phi),
                              target.z + dist*cos(phi) };
  G3Xvector u   = g3x_SetVect(pos,target);
  g3x_Normalize(&u);
  /*                         |cos(rot_z)*sin(rot_y)   *
   * en coord. sphérique : u |sin(rot_z)*sin(rot_y)   *
   *                         |cos(rot_y)           */
  /* angle principal  (Y) */
  double rot_y  = acos(u.z);
  /* angle secondaire (Z) */
  double rot_z = ( G3Xiszero(rot_y) ? PI : (u.y<0.?-1.:+1.)*acos(u.x/sin(rot_y)) );

  /* (reset) */
  camera->Md = g3x_Identity();
  /* échelle (attention à la symétrie sur l'axe des x) */
  camera->Md = g3x_ProdHMat(g3x_MakeHomothetieXYZ(-camera->nbc/(1.*camera->nbl) , 1., foc),camera->Md);
  /* alignement      */
  camera->Md = g3x_ProdHMat(g3x_MakeRotationZ(PI/2.),camera->Md);

  /* Orientation : le vecteur directeur de la cam. canonique (0.,0.,-1) s'aligne sur U */
  camera->Md = g3x_ProdHMat(g3x_MakeRotationY(+rot_y),camera->Md);
  camera->Md = g3x_ProdHMat(g3x_MakeRotationZ(+rot_z),camera->Md);

  /* translation : positionnement    */
  camera->Md = g3x_ProdHMat(g3x_MakeTranslationV(pos),camera->Md);

  camera->nbc = NBC*resol;
  camera->nbl = NBL*resol;

  /* Get initial position */
  G3Xpoint o = (G3Xpoint) {pos.x - u.x * foc, pos.y - u.y * foc, pos.z - u.z * foc};

  /* Fill the pixmap */
  int x,y;
  double xpas = 2./camera->nbl;
  double ypas = 2./camera->nbc;
  G3Xcolor *col = camera->col;
  for (x=0;x<camera->nbl; x++) {
    for (y=0;y<camera->nbc; y++)
    {
      G3Xpoint p = (G3Xpoint) {(x - camera->nbl / 2.)*xpas, (y - camera->nbc / 2.)*ypas, 0};
      p = g3x_ProdHMatPoint(camera->Md, p);
      G3Xvector v = g3x_SetVect(o, p);
      Rayon R = Cree_Rayon(p, v);
      RayTracer(&R, objets, light, 1);
      /*Draw_Rayon(&R);*/
      *col = R.color;
      col++;
    }
  }
}

void Draw_camera(Cam* camera, double cam_dis)
{
  glPushMatrix();
    glMultMatrixd(camera->Md.m);

    /* caméra canonique */

    /*= Chargement Pixmap => Texture Gl =*/
    glDisable(GL_LIGHTING);
      glEnable(GL_TEXTURE_2D);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    	gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,camera->nbl,camera->nbc,GL_RGBA,GL_FLOAT,camera->col);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glColor4f(1.,1.,1.,1.);
      glBegin(GL_QUADS);
        glTexCoord2d(0,0); glVertex3d(-1.,-1.,0.);
        glTexCoord2d(1,0); glVertex3d(-1.,+1.,0.);
        glTexCoord2d(1,1); glVertex3d(+1.,+1.,0.);
        glTexCoord2d(0,1); glVertex3d(+1.,-1.,0.);
      glEnd();
      glDisable(GL_TEXTURE_2D);
  	glEnable(GL_LIGHTING);

    /* cadre */
    glDisable(GL_LIGHTING);
      glColor3f(1.,1.,1.);
      glBegin(GL_LINE_LOOP);
        glVertex3d(-1.,-1.,0.);
        glVertex3d(-1.,+1.,0.);
        glVertex3d(+1.,+1.,0.);
        glVertex3d(+1.,-1.,0.);
      glEnd();
      glColor3f(.5,.5,.5);
      glBegin(GL_LINES);
        glVertex3d(-1.,-1., 0.);
        glVertex3d( 0., 0.,-1.);
        glVertex3d(-1.,+1., 0.);
        glVertex3d( 0., 0.,-1.);
        glVertex3d(+1.,+1., 0.);
        glVertex3d( 0., 0.,-1.);
        glVertex3d(+1.,-1., 0.);
        glVertex3d( 0., 0.,-1.);
      glEnd();
      glColor3f(1.,0.,0.);
      glBegin(GL_LINES);
        glVertex3d(0., 0.,-1.);
        glVertex3d(0., 0.,0.5*cam_dis);
      glEnd();
  	glEnable(GL_LIGHTING);
  glPopMatrix();
}