#pragma once

typedef struct {
    int width;
    int height;
} ImageSize;

unsigned char* upload_image(char* path, ImageSize* out_size);
