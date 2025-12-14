/*
Inspiration from https://medium.com/@julianawrites/the-thrilling-mathematics-of-image-blurring-c0ad582c8140
*/

#include "upload_image.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mean(unsigned char* restrict rgb_data, const ImageSize* image_size, const int kernel_size){
    // Factor to apply to all bytes in rgb_arr
    // Choose float because it is less memory usage than double and we do not need that precision
    float factor = (float) 1.0 / ((float)(kernel_size * kernel_size));

    // Initialize factored rgb_arr
    size_t array_size = (size_t) image_size->width * (size_t) image_size->height * (size_t) 3;
    float* factored_rgb_data = (float*) malloc(array_size * sizeof(float));
    if (!factored_rgb_data) {
        (void)fprintf(stderr, "Memory allocation failed for factored_rgb_data\n");
        exit(EXIT_FAILURE);
    }

    // apply factor to all elements in rgb array (design decision: saves processing time but increases memory usage)
    for(int i = 0; i < image_size->width * image_size->height * 3; i++){
        factored_rgb_data[i] = ((float) rgb_data[i] * factor);
    }

    // Loop through every pixel

    int init_val = kernel_size/2;

    float sum[3] = {0, 0, 0};
    for(int col_px = init_val; col_px < image_size -> height - init_val; col_px++){
        for(int row_px = init_val; row_px < image_size -> width - init_val; row_px++){
            // Calculate new rgb from mean of specific pixel
            sum[0] = (float) 0.0;
            sum[1] = (float) 0.0;
            sum[2] = (float) 0.0;
            for(int col = 0; col < (kernel_size + 1)/2; col++){
                for(int row = 0; row < (kernel_size - 1) / 2 * 3 + 1; row = row + 3){
                    int center = (col_px * image_size->width + row_px) * 3;
                    for(int add_byte = 0; add_byte < 3; add_byte++){
                        // For row <= 0 and col <= 0
                        sum[add_byte] = sum[add_byte] + factored_rgb_data[center + image_size -> width * 3 * col + row + add_byte];
                        // To add reflective of rows less than 0
                        if(row != 0){
                            sum[add_byte] = sum[add_byte] + factored_rgb_data[center + image_size -> width * 3 * col - row + add_byte];
                        }
                        // To add reflective of cols less than 0 
                        if(col != 0){
                            sum[add_byte] = sum[add_byte] + factored_rgb_data[center - image_size -> width * 3 * col + row + add_byte];
                            if(row != 0){
                                sum[add_byte] = sum[add_byte] + factored_rgb_data[center - image_size -> width * 3 * col - row + add_byte];
                            }
                        }
                    }
                }
            }
            rgb_data[(col_px * image_size->width + row_px) * 3] = (unsigned char) sum[0];
            rgb_data[(col_px * image_size->width + row_px) * 3 + 1] = (unsigned char) sum[1];
            rgb_data[(col_px * image_size->width + row_px) * 3 + 2] = (unsigned char) sum[2];
        }
    }

    free(factored_rgb_data);
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

    mean(rgb_data, &image_size, kernel_size);

    // SAVE THE JPEG!
    const int QUALITY = 100;
    save_jpeg("output.jpg", rgb_data, image_size.width, image_size.height, QUALITY);

    // cleanup
    free(rgb_data);

    return 0;
}
