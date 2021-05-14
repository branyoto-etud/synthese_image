# Project image synthesis

This project is a simulation of a 3D render in a 2D image with a 3D library (glut).

## How to run it

First you need to install the custom library g3x done by Eric Incerti (eric.incerti@univ-eiffel.fr).
(Todo: Post it in git to ensure persistence)

Then run the cmake file. If the message is something like : `cannot open shared object file: No such file or directory`,
the build is successful but the file cannot be launched automatically. So you need to open a terminal 
and run the built file `./cmake-build-debug/synthesis`

If the Cmake fails to build the project, make sure the library is successfully installed (check if the environment variables are correct).

Last possible solution is to run the makefile `make all` inside a terminal. (not in an IDE!)

If none of these methods works, please contact us : bgc77160@gmail.com and aadedjoumon@gmail.com.
Or leave a issue in our [git repository](https://github.com/branyoto-etud/synthese_image)

## Historic

### 25/04/2021 : Base implementation

Implementation of the base rendering engine (only the shape is rendered):

- Objects (Sphere or Cube) can be created `Cree_sphere_can` (or `Cree_cube_can`) with a `G3Xcolor` and a `material`
  and then resized/rotated/moved (with `rescale_objet`/`(x/y/z)rotate_objet`/`translate_objet`)
- Objects can be displayed (only for debug) with `draw_object`.
- Rays can be created using `Cree_Rayon` with the start point and the direction.
- Rays can be displayed (only for debug) with `Draw_Rayon`, the displayed ray has the color that he contains.
- Raytracing can be done based using `RayTracer` with the ray, a loop-linked-list of objects, the light source and the
  number of reflections. (currently the last two are not used).
- Camera can be created with `Cree_camera_can` and then can be initialized with `Camera_setup`.
- Camera can be displayed with `Draw_camera` that draw the outlines (if set to true) and the camera screen.
  

### 13/05/2021 : Material interaction + Refactor
- (deg 0) No interaction with light
- (deg 1) Interaction between light and shape
- (deg 2) Interaction between light and material (with ambient light and diffuse light)
- (deg 3) Reflections (if shining)
- (deg 4) Refraction (if transparent)

## Missing
- (deg 5) Shadow, the light is no longer visible trough objects
  - Level 1 : Without alpha
  - Level 2 : With alpha but without refraction
  - Level 3 : With refraction but without object overlapping
  - Level 4 : Everything
- Multiple rays for 1 pixel (better quality)
