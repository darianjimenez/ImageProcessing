/*
Inspiration from https://medium.com/@julianawrites/the-thrilling-mathematics-of-image-blurring-c0ad582c8140
*/

#include "upload_image.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mean(const unsigned char* restrict rgb_data, unsigned char* output, const ImageSize* image_size, const int kernel_size){
    // Factor to apply to all bytes in rgb_arr
    // Choose float because it is less memory usage than double and we do not need that precision
    const float factor = (float) 1.0 / ((float)(kernel_size * kernel_size));

    // Loop through every pixel

    int offset = kernel_size/2;
    const int stride = image_size->width * 3;

    float sum[3] = {0, 0, 0};
    // Traverse across row first because better for cache
    for(int window_y = 0; window_y < image_size -> height - kernel_size + 1; window_y++){
        for(int window_x = 0; window_x < image_size -> width - kernel_size + 1; window_x++){
            // Calculate new rgb from mean of specific pixel
            sum[0] = (float) 0.0;
            sum[1] = (float) 0.0;
            sum[2] = (float) 0.0;

            // Traverse across row first (from column to column) because better for cache
            for(int row = 0; row < kernel_size; row++){
                for(int col = 0; col < kernel_size; col++){
                    for(int add_byte = 0; add_byte < 3; add_byte++){
                        sum[add_byte] = sum[add_byte] + rgb_data[(window_y + row) * stride + (window_x + col) * 3 + add_byte];
                    }
                }
            }

            output[(window_y + offset) * stride + (window_x + offset) * 3] = (unsigned char) (sum[0] * factor);
            output[(window_y + offset) * stride + (window_x + offset) * 3 + 1] = (unsigned char) (sum[1] * factor);
            output[(window_y + offset) * stride + (window_x + offset) * 3 + 2] = (unsigned char) (sum[2] * factor);
        }
    }
}

int main(int argc, char *argv[]){
    // Ensures there is at least four arguments
    if (argc != 4) {
        (void)fprintf(stderr, "Usage: %s <input_jpeg_file> <method: mean or median> <kernel size>\n", argv[0]);
        return 1;
    }

    char *endptr = NULL;
    const int BASE = 10;
    int kernel_size = (int) strtol(argv[3], &endptr, BASE);
    if (*endptr != '\0') {
        (void)fprintf(stderr, "Error converting third argument: '%s' is not a valid number.\n", argv[3]);
        return 1;
    }

    if(kernel_size % 2 == 0){
        (void)fprintf(stderr, "Error: Kernel size should be an odd number\n");
        return 1;
    }

    ImageSize image_size;

    // Take jpeg and converts to RGB array
    // Note to self: rgb_data is allocated so must be freed
    unsigned char* rgb_data = upload_image(argv[1], &image_size);

    size_t array_size = (size_t) image_size.width * (size_t) image_size.height * (size_t) 3;
    unsigned char* output = (unsigned char*) malloc(array_size * sizeof(unsigned char));
    if (!output) {
        (void)fprintf(stderr, "Memory allocation failed for output\n");
        exit(EXIT_FAILURE);
    }

    mean(rgb_data, output, &image_size, kernel_size);

    // SAVE THE JPEG!
    const int QUALITY = 100;
    save_jpeg("output.jpg", output, image_size.width, image_size.height, QUALITY);

    // cleanup
    free(output);
    free(rgb_data);

    return 0;
}
