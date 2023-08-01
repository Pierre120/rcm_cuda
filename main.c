#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Headers for image input and output
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define MAX_CHAR 32
#define GRAY 1
#define RGB 3


void 
robCompMask(unsigned char *img_out, unsigned char *img_in, int width, int height, const int mask[][3]) {
    int sum, rowStart, colStart, maskRowIndex, maskColIndex, maxRow, maxCol;
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            // Reset sum 
            sum = 0;

            // Reset mask indices
            rowStart = -1;
            colStart = -1;
            maskRowIndex = 0;
            maskColIndex = 0;
            maxRow = 3;
            maxCol = 3;

            // Center mask to the current pixel
            if(h == 0) {
                rowStart = 0;
                maskRowIndex = 1;
                maxRow = 3;
            }
            if(h == height - 1) {
                rowStart = -1;
                maskRowIndex = 0;
                maxRow = 2;
            }
            if(w == 0) {
                colStart = 0;
                maskColIndex = 1;
                maxCol = 3;
            }
            if(w == width - 1) {
                colStart = -1;
                maskColIndex = 0;
                maxCol = 2;
            }

            // Apply the mask
            for (int r = rowStart, mr = maskRowIndex; mr < maxRow; r++, mr++) {
                for (int c = colStart, mc = maskColIndex; mc < maxCol; c++, mc++) {
                    sum += img_in[(h + r) * width + (w + c)] * mask[mr][mc];
                }
            }

            // Set the output pixel to the sum
            img_out[h * width + w] = (unsigned char) abs(sum);
        }
    }
}

void generateOutputFilename(char* filename, char* output_filename, const char* direction) {
    // copy the filename to the output filename
    int i;
    for (i = 0; i < MAX_CHAR && filename[i] != '.'; i++) {
        output_filename[i + 10] = filename[i];
    }
    output_filename[i + 10] = '_';
    i++;
    // copy the direction to the output filename
    int j;
    for (j = 0; direction[j] != 0; j++) {
        output_filename[i + 10 + j] = direction[j];
    }
    // add the filetype to the output filename
    output_filename[i + 10 + j] = '.';
    output_filename[i + 10 + j + 1] = 'j';
    output_filename[i + 10 + j + 2] = 'p';
    output_filename[i + 10 + j + 3] = 'g';
    output_filename[i + 10 + j + 4] = 0;
}

void generateInputFilename(char* filename, char* input_filename) {
    // copy the filename to the output filename
    int i;
    for (i = 0; i < MAX_CHAR && filename[i] != 0; i++) {
        input_filename[i + 9] = filename[i];
    }
}

int isFileTypeSupported(char* filename) {
    // Declare supported filetypes
    const char* supported_filetypes[] = {
        "jpg", "jpeg", "png", "bmp"
    };
    // Move to the period before the filetype
    int start = 0;
    for (int i = 0; i < MAX_CHAR && filename[start] != '.'; i++, start++);
    start++; // move to the first character of the filetype

    // Check if the filetype is supported
    for (int i = start, j = 0; j < 4 && filename[i] != 0; i++, j++) {
        // Debugging purposes: print the filename extension and the supported filetypes
        /* printf("%c - %c - %c - %c - %c\n", 
            filename[i], 
            supported_filetypes[0][j % 3], 
            supported_filetypes[1][j % 4], 
            supported_filetypes[2][j % 3], 
            supported_filetypes[3][j % 3]); */
        
        // Check if the character is not equal to any of the supported file types
        if (filename[i] != supported_filetypes[0][j % 3]
            && filename[i] != supported_filetypes[1][j % 4]
            && filename[i] != supported_filetypes[2][j % 3]
            && filename[i] != supported_filetypes[3][j % 3]) {
            return 0; // return 0 if the filetype is not supported
        }
    }
    return 1; // return 1 if the filetype is supported
}

int main() {
    // Declare Robinson's Compass mask
    const int ROBINSON_COMPASS_MASK[8][3][3] = {
        {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}}, // N
        {{0, 1, 2}, {-1, 0, 1}, {-2, -1, 0}}, // NW
        {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}}, // W
        {{2, 1, 0}, {1, 0, -1}, {0, -1, -2}}, // SW
        {{1, 0, -1}, {2, 0, -2}, {1, 0, -1}}, // S
        {{0, -1, -2}, {1, 0, -1}, {2, 1, 0}}, // SE
        {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}}, // E
        {{-2, -1, 0}, {-1, 0, 1}, {0, 1, 2}}  // NE
    };

    int width, height, channels;
    char filename[MAX_CHAR] = { 0 };
    char input_filename[MAX_CHAR + 16] = "./inputs/\0";
    char output_filename[MAX_CHAR + 16] = "./outputs/\0";
    unsigned char *img_in, *img_out;

    // Ask for the image filename
    printf("Enter the image filename: ");
    scanf("%[^\n]%*c", filename);
    // Debugging purposes: print filename
    printf("Filename: %s\n", filename); // expected to print only one newline char

    // Check if filetype is supported for this program
    if (!isFileTypeSupported(filename)) {
        printf("File type not supported.\n");
        printf("Image should either be in JPG, PNG or BMP format.\n");
        return 1; // exit the program with 1 error
    }

    // Generate the input filename
    generateInputFilename(filename, input_filename);
    // Debugging purposes: print the input filename
    printf("Input filename: %s\n", input_filename);

    // Read the image
    img_in = stbi_load(input_filename, &width, &height, &channels, GRAY); // 4th param: GRAY or RGB
    // check image if it exists
    if (img_in == NULL) {
        printf("Error in loading the image.\n");
        printf("Please check if the image exists.\n");
        return 1; // exit the program with 1 error
    }
    // Debugging purposes: print image width, height, and channels
    printf("Image width: %d\n", width);
    printf("Image height: %d\n", height);
    printf("Number of channels: %d\n", channels);

    // Debugging purposes: to check if the image is in grayscale
    if(!stbi_write_jpg("./grayscale/input.jpg", width, height, GRAY, img_in, 100)) {
        printf("Error in saving the output image.\n");
        return 1; // exit the program with 1 error
    }

    // Allocate memory for the output image
    img_out = (unsigned char *) malloc(width * height * channels * sizeof(unsigned char));
    // Check if memory is successfully allocated
    if (img_out == NULL) {
        printf("Error in allocating memory for the output image.\n");
        return 1; // exit the program with 1 error
    }

    // Apply the Robinson's Compass mask for N direction
    robCompMask(img_out, img_in, width, height, ROBINSON_COMPASS_MASK[0]);
    // Generate output filename
    generateOutputFilename(filename, output_filename, "N\0");
    // Debugging purposes: print output filename
    printf("Output filename: %s\n", output_filename);
    // Save output image
    if(!stbi_write_jpg(output_filename, width, height, GRAY, img_out, 100)) {
        printf("Error in saving the output image.\n");
        return 1; // exit the program with 1 error
    }

    // Apply the Robinson's Compass mask for NW direction
    robCompMask(img_out, img_in, width, height, ROBINSON_COMPASS_MASK[1]);
    // Generate output filename
    generateOutputFilename(filename, output_filename, "NW\0");
    // Debugging purposes: print output filename
    printf("Output filename: %s\n", output_filename);
    // Save output image
    if(!stbi_write_jpg(output_filename, width, height, GRAY, img_out, 100)) {
        printf("Error in saving the output image.\n");
        return 1; // exit the program with 1 error
    }

    // Apply the Robinson's Compass mask for W direction
    robCompMask(img_out, img_in, width, height, ROBINSON_COMPASS_MASK[2]);
    // Generate output filename
    generateOutputFilename(filename, output_filename, "W\0");
    // Debugging purposes: print output filename
    printf("Output filename: %s\n", output_filename);
    // Save output image
    if(!stbi_write_jpg(output_filename, width, height, GRAY, img_out, 100)) {
        printf("Error in saving the output image.\n");
        return 1; // exit the program with 1 error
    }

    // Apply the Robinson's Compass mask for SW direction
    robCompMask(img_out, img_in, width, height, ROBINSON_COMPASS_MASK[3]);
    // Generate output filename
    generateOutputFilename(filename, output_filename, "SW\0");
    // Debugging purposes: print output filename
    printf("Output filename: %s\n", output_filename);
    // Save output image
    if(!stbi_write_jpg(output_filename, width, height, GRAY, img_out, 100)) {
        printf("Error in saving the output image.\n");
        return 1; // exit the program with 1 error
    }

    // Apply the Robinson's Compass mask for S direction
    robCompMask(img_out, img_in, width, height, ROBINSON_COMPASS_MASK[4]);
    // Generate output filename
    generateOutputFilename(filename, output_filename, "S\0");
    // Debugging purposes: print output filename
    printf("Output filename: %s\n", output_filename);
    // Save output image
    if(!stbi_write_jpg(output_filename, width, height, GRAY, img_out, 100)) {
        printf("Error in saving the output image.\n");
        return 1; // exit the program with 1 error
    }

    // Apply the Robinson's Compass mask for SE direction
    robCompMask(img_out, img_in, width, height, ROBINSON_COMPASS_MASK[5]);
    // Generate output filename
    generateOutputFilename(filename, output_filename, "SE\0");
    // Debugging purposes: print output filename
    printf("Output filename: %s\n", output_filename);
    // Save output image
    if(!stbi_write_jpg(output_filename, width, height, GRAY, img_out, 100)) {
        printf("Error in saving the output image.\n");
        return 1; // exit the program with 1 error
    }

    // Apply the Robinson's Compass mask for E direction
    robCompMask(img_out, img_in, width, height, ROBINSON_COMPASS_MASK[6]);
    // Generate output filename
    generateOutputFilename(filename, output_filename, "E\0");
    // Debugging purposes: print output filename
    printf("Output filename: %s\n", output_filename);
    // Save output image
    if(!stbi_write_jpg(output_filename, width, height, GRAY, img_out, 100)) {
        printf("Error in saving the output image.\n");
        return 1; // exit the program with 1 error
    }

    // Apply the Robinson's Compass mask for NE direction
    robCompMask(img_out, img_in, width, height, ROBINSON_COMPASS_MASK[7]);
    // Generate output filename
    generateOutputFilename(filename, output_filename, "NE\0");
    // Debugging purposes: print output filename
    printf("Output filename: %s\n", output_filename);
    // Save output image
    if(!stbi_write_jpg(output_filename, width, height, GRAY, img_out, 100)) {
        printf("Error in saving the output image.\n");
        return 1; // exit the program with 1 error
    }

    return 0;
}
