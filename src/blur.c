#include "upload_image.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void median_blur(unsigned char* rgb_data){
    float blurring_kernel
}

int main(int argc, char *argv[]) {
    // Ensures there is at least one argument
    if (argc != 3) {
        (void)fprintf(stderr, "Usage: %s <input_jpeg_file> <method: mean or median>\n", argv[0]);
        return 1;
    }

    ImageSize image_size;

    // Take jpeg and converts to RGB array
    // Note to self: rgb_data is allocated so must be freed
    unsigned char* rgb_data = upload_image(argv[1], &image_size);

    // cleanup
    free(rgb_data);

    return 0;
}
