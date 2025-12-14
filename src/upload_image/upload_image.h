#pragma once

typedef struct {
    int width;
    int height;
} ImageSize;

unsigned char* upload_image(char* path, ImageSize* out_size);

void save_jpeg(const char *filename, unsigned char *rgb_data, int width, int height, int quality);
