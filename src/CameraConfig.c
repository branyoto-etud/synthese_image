#include "../include/CameraConfig.h"

CameraConfig *default_config() {
    CameraConfig *config;
    if (NULL == (config = (CameraConfig *) malloc(sizeof(CameraConfig)))) g3x_Quit();

    config->theta = -1.26; /* 0.77 */
    config->phi = 0.7;  /* 1.5 */
    config->distance = 2.5;
    config->focal = 4.0;
    config->resolution = 1.0;
    config->target = (G3Xpoint) {0.0, 0.0, 0.0};
    config->light = (G3Xpoint) {0.0, 0.0, 1.0};
    return config;
}

void free_camera_config(CameraConfig** config) {
    free(*config);
    *config = NULL;
}

G3Xpoint get_camera_position(CameraConfig *config) {
    return (G3Xpoint) {config->target.x + config->distance * cos(config->theta) * sin(config->phi),
                       config->target.y + config->distance * sin(config->theta) * sin(config->phi),
                       config->target.z + config->distance * cos(config->phi)};
}

void move_light(CameraConfig *config, G3Xpoint position) {
    config->light = position;
}
