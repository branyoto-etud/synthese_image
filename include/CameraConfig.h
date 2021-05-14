#ifndef __CAMERA_CONFIG
#define __CAMERA_CONFIG

#include <g3x.h>

typedef struct {
    double theta, phi, distance, focal, resolution;
    G3Xpoint target;
    G3Xpoint light;
} CameraConfig;

CameraConfig *default_config();

void free_camera_config(CameraConfig** config);

G3Xpoint get_camera_position(CameraConfig *config);

void move_light(CameraConfig *config, G3Xpoint position);

#endif
