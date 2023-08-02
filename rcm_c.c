#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Headers for image input and output
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define MAX_CHAR 32
#define GRAY 1
// #define RGB 3
// #define GAUSSIAN_MASK_SIZE 5
// #define GAUSSIAN_SIGMA 5.0

// For the PI value in C
#ifndef M_PI
#   define M_PI 3.14159265358979323846
#endif

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

void gaussianBlurMask(unsigned char *img_in, int width, int height, int GAUSSIAN_MASK_SIZE, double mask[GAUSSIAN_MASK_SIZE][GAUSSIAN_MASK_SIZE]) {
    const int maxIndex = GAUSSIAN_MASK_SIZE / 2;
    int sum, rowStart, colStart, maskRowIndex, maskColIndex, maxRow, maxCol;
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            // Reset sum 
            sum = 0;

            // Reset mask indices
            rowStart = -maxIndex;
            colStart = -maxIndex;
            maskRowIndex = 0;
            maskColIndex = 0;
            maxRow = GAUSSIAN_MASK_SIZE;
            maxCol = GAUSSIAN_MASK_SIZE;

            // Center mask to the current pixel
            if(h == 0) {
                rowStart = 0;
                maskRowIndex = maxIndex;
                maxRow = GAUSSIAN_MASK_SIZE;
            }
            if(h > 0 && h < maxIndex) {
                rowStart = -h;
                maskRowIndex = maxIndex - h;
                maxRow = GAUSSIAN_MASK_SIZE;
            }
            if(h >= height - maxIndex && h <= height - 1) {
                rowStart = -maxIndex;
                maskRowIndex = 0;
                maxRow = maxIndex + (height - h);
            }
            if(w == 0) {
                colStart = 0;
                maskColIndex = maxIndex;
                maxCol = GAUSSIAN_MASK_SIZE;
            }
            if(w > 0 && w < maxIndex) {
                colStart = -w;
                maskColIndex = maxIndex - w;
                maxCol = GAUSSIAN_MASK_SIZE;
            }
            if(w >= width - maxIndex && w <= height - 1) {
                colStart = -maxIndex;
                maskColIndex = 0;
                maxCol = maxIndex + (width - w);
            }

            // Apply the mask
            for (int r = rowStart, mr = maskRowIndex; mr < maxRow; r++, mr++) {
                for (int c = colStart, mc = maskColIndex; mc < maxCol; c++, mc++) {
                    sum += img_in[(h + r) * width + (w + c)] * mask[mr][mc];
                }
            }

            // Set the output pixel to the sum
            img_in[h * width + w] = (unsigned char) abs(sum);
        }
    }
}

void generateGaussianBlurMask(int GAUSSIAN_MASK_SIZE, double GAUSSIAN_SIGMA, double mask[GAUSSIAN_MASK_SIZE][GAUSSIAN_MASK_SIZE]) {
    // Declare the Gaussian mask
    double sum = 0;
    int max_index = GAUSSIAN_MASK_SIZE / 2;
    for (int r = -GAUSSIAN_MASK_SIZE/2; r <= max_index; r++) {
        for (int c = -GAUSSIAN_MASK_SIZE/2; c <= max_index; c++) {
            mask[r + max_index][c + max_index] = exp(-((double)(r * r + c * c)) / (double)(2 * GAUSSIAN_SIGMA * GAUSSIAN_SIGMA)) / (double)(2 * M_PI * GAUSSIAN_SIGMA * GAUSSIAN_SIGMA);
            sum += mask[r + max_index][c + max_index];
        }
    }

    // Normalize the Gaussian mask
    for (int r = 0; r < GAUSSIAN_MASK_SIZE; r++) {
        for (int c = 0; c < GAUSSIAN_MASK_SIZE; c++) {
            mask[r][c] /= sum;
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
    const int NUM_DIRECTIONS = 8;
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
    const char filename_suffix[8][3] = {
        "N\0", "NW\0", "W\0", "SW\0", "S\0", "SE\0", "E\0", "NE\0"
    };
    // Declare Gaussian Blur Mask initialized to zero
    // double gaussianMask[GAUSSIAN_MASK_SIZE][GAUSSIAN_MASK_SIZE] = { { 0 } };

    int width, height, channels;
    char filename[MAX_CHAR] = { 0 };
    char input_filename[MAX_CHAR + 16] = "./inputs/\0";
    char output_filename[MAX_CHAR + 16] = "./outputs/\0";
    unsigned char *img_in, *img_out;

    // timer variables
    clock_t start, end;
    double 
        rcm_avg = 0.0, 
        rcm_total = 0.0,
        gb_mask = 0.0,
        gb_op = 0.0,
        gb_total = 0.0,
        rcm_gb_total = 0.0;

    // Ask for the image filename
    printf("Enter the image filename: ");
    scanf("%[^\n]%*c", filename);
    // Debugging purposes: print filename
    printf("\nIMAGE INFORMATION\nFilename: %s\n", filename); // expected to print only one newline char

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


    // --- GAUSSIAN BLUR MASK (SMOOTHING FILTER) ---
    // Ask user if use a smoothing filter
    int useSmoothingFilter = -1;
    while(useSmoothingFilter != 1 && useSmoothingFilter != 0) {
        printf("\nDo you want to use a smoothing filter? (1 - Yes, 0 - No): ");
        scanf("%d%*c", &useSmoothingFilter);
    }
    if(useSmoothingFilter) {
        // Ask for the mask size
        int maskSize = 0;
        while(maskSize % 2 == 0 && maskSize < 3) {
            printf("Enter the mask size (odd number >= 3): ");
            scanf("%d%*c", &maskSize);
        }
        // Ask for sigma value
        double sigma = 0;
        while(sigma < 0.3) {
            printf("Enter the sigma value (positive number >= 0.3): ");
            scanf("%lf%*c", &sigma); 
        }

        // Declare Gaussian Blur Mask initialized to zero
        double gaussianMask[maskSize][maskSize];
        // Generate Gaussian Blur mask
        printf("\nGENERATING GAUSSIAN BLUR MASK...\n");
        start = clock();
        generateGaussianBlurMask(maskSize, sigma, gaussianMask);
        end = clock();
        printf("DONE\n");
        gb_mask = ((double)(end - start)) * 1e6 / CLOCKS_PER_SEC; // microseconds
        // Apply Gaussian Blur mask to input image
        printf("\nAPPLYING GAUSSIAN BLUR MASK TO INPUT IMAGE...\n");
        start = clock();
        gaussianBlurMask(img_in, width, height, maskSize, gaussianMask);
        end = clock();
        printf("DONE\n");
        gb_op = ((double)(end - start)) * 1e6 / CLOCKS_PER_SEC; // microseconds
        // Get total for Gaussian Blur Mask Operation
        gb_total = gb_mask + gb_op;
    }


    // Allocate memory for the output image
    img_out = (unsigned char *) malloc(width * height * channels * sizeof(unsigned char));
    // Check if memory is successfully allocated
    if (img_out == NULL) {
        printf("Error in allocating memory for the output image.\n");
        return 1; // exit the program with 1 error
    }


    // --- ROBINSON'S COMPASS MASK EDGE DETECTION ---
    // Perform robinson compass mask in all directions
    printf("\nPERFORMING ROBINSON COMPASS MASK EDGE DETECTION...\n");
    for(int d = 0; d < NUM_DIRECTIONS; d++) {
        // Perform Robinson Compass Mask
        start = clock();
        robCompMask(img_out, img_in, width, height, ROBINSON_COMPASS_MASK[d]);
        end = clock();
        rcm_total += ((double)(end - start)) * 1e6 / CLOCKS_PER_SEC; // microseconds
        // Generate output filename
        generateOutputFilename(filename, output_filename, filename_suffix[d]);
        // Debugging purposes: print output filename
        printf("Output filename: %s\n", output_filename);
        // Save output image
        if(!stbi_write_jpg(output_filename, width, height, GRAY, img_out, 100)) {
            printf("Error in saving the output image.\n");
            return 1; // exit the program with 1 error
        }
    }
    printf("DONE\n");

    // Calculate time elapsed for Robinson's Compass Mask Edge Detection
    rcm_avg = rcm_total / NUM_DIRECTIONS;
    rcm_gb_total = rcm_total + gb_total;
    // Display time elapsed for Robinson's Compass Mask Edge Detection
    printf("\n --- TIME ELAPSED --- \n");
    printf("Gaussian Blur Mask Generation (microseconds): %lf us\n", gb_mask);
    printf("Gaussian Blur Mask Operation (microseconds): %lf us\n", gb_op);
    printf("Gaussian Blur Mask Total (microseconds): %lf us\n", gb_total);
    printf("Robinson's Compass Mask Average (microseconds): %lf us\n", rcm_avg);
    printf("Robinson's Compass Mask Total (microseconds): %lf us\n", rcm_total);
    printf("Robinson's Compass Mask + Gaussian Blur Mask Total (microseconds): %lf us\n\n", rcm_gb_total);

    return 0;
}
