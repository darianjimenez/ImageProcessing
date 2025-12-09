/*
Most of this code was referenced from: https://github.com/aumuell/libjpeg-turbo/blob/master/example.c#L284
*/

// NOLINTBEGIN(llvm-include-order)
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <jpeglib.h>
// NOLINTEND(llvm-include-order)


int main(int argc, char *argv[]) {
    // Ensures there is at least one argument
    if (argc != 2) {
        (void)fprintf(stderr, "Usage: %s <input_jpeg_file>\n", argv[0]);
        return 1;
    }

    //JPEG depression parameters
    struct jpeg_decompress_struct cinfo;
    // Library error handler
    struct jpeg_error_mgr jerr;

    // FILE pointer
    FILE *infile = NULL;
    JSAMPARRAY buffer = NULL;
    int row_stride = 0;

    // Get file data and write it to infile pointer
    if ((infile = fopen(argv[1], "rbe")) == NULL) {
        (void)fprintf(stderr, "Can't open %s\n", argv[1]);
        return 1;
    }

    cinfo.err = jpeg_std_error(&jerr);
    // Initializes decompressor
    jpeg_create_decompress(&cinfo);
    // Set src file of decompressor to infile
    jpeg_stdio_src(&cinfo, infile);
    // Get header from JPEG file
    jpeg_read_header(&cinfo, TRUE);

    // Sets conversion from JPEG to RGB
    cinfo.out_color_space = JCS_RGB;
    // STARTS DECOMPRESSING YAY!
    jpeg_start_decompress(&cinfo);

    // Printing out file information for better debug
    printf("Filename: %s\n", argv[1]);
    printf("Width: %d\n", (int) cinfo.output_width);
    printf("Height: %d\n", (int) cinfo.output_height);
    printf("Bytes: %d\n", (int) cinfo.output_width * (int) cinfo.output_height * cinfo.output_components);

    // Note to self: output_components is the byte size of one pixel (RGB), should be 3?
    row_stride = (int) cinfo.output_width * cinfo.output_components;
    // Allocates a buffer big enough for one row (WIDTH) of the image
    // Note to self: cinfo.mem->alloc_array is a FUNCTION
    // j_common_ptr: jpeg common pointer (this is just how the function works???)
    // JPOOL_IMAGE means image is freed when decompressing is done??
    // Another note: JSAMPARRAY is an array of pointer and we are only using the first index
    buffer = cinfo.mem->alloc_sarray((j_common_ptr)&cinfo, JPOOL_IMAGE, (JDIMENSION) row_stride, 1);

    // Allocates memory to store all of RGB data
    unsigned char *rgb_data = (unsigned char *)malloc((size_t) cinfo.output_width * (size_t) cinfo.output_height * (size_t) cinfo.output_components);
    if (!rgb_data) {
        (void)fprintf(stderr, "Memory allocation failed\n");
        jpeg_destroy_decompress(&cinfo);
        (void)fclose(infile);
        return 1;
    }

    int current_row = 0;
    while (cinfo.output_scanline < cinfo.output_height) {
        // Read one line at a time into buffer
        jpeg_read_scanlines(&cinfo, buffer, 1);
        // Calculate next available slot in allocated memory, and copy data from buffer to that location
        memcpy(rgb_data + ((ptrdiff_t) current_row * (ptrdiff_t) row_stride), buffer[0], (size_t) row_stride);
        current_row++;
    }

    // Cleanup
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    (void)fclose(infile);
    free(rgb_data);

    return 0;
}
