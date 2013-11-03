/* 
 * File:   main.cpp
 * Author: mavewei
 *
 * Created on October 31, 2013, 5:28 PM
 */

#include <cstdlib>

#include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <cmath>
// #include <algorithm>
#define VERBOSE 1
#define HEIGHT  512
#define WIDTH   512
#define BLACK   0
#define WHITE   255
 using namespace std;

 void erode(unsigned char *image, unsigned char *erodeImage);
 void boundaryExtract(unsigned char *image, unsigned char *erodeImage);
 void componentCount(int x, int y, unsigned char *image, bool *visitImage, unsigned char pixelValue); 
 void labelComponent(unsigned char *image, unsigned char *labelImage);
 void binaryImage(unsigned char *image, unsigned char *binaryImage);
 int writeImagePGM(char *filename, int height, int width, unsigned char *image);

 int main(int argc, char** argv) {
    char *imgFilename;
    char oriOutputFilename[128];
    char outputFilename[128];
    unsigned char *imgIn, *imgBin, *imgOut;
    bool *visit;
    int count = 0;
    FILE *fp;

    /* code */
    if (argc != 2) {
        printf("USAGE: problem_01 [image-file-name]\n");
        printf("       image-file-name: Input image file. (circle.raw)\n");
        return -1;
    } 

    imgFilename = argv[1];
    
    fp = fopen(imgFilename, "rb");

    if (fp) {
        imgIn = (unsigned char *) calloc(HEIGHT * WIDTH, sizeof (unsigned char));
        imgOut = (unsigned char *) calloc(HEIGHT * WIDTH, sizeof(unsigned char));
        visit = (bool *) calloc(HEIGHT * WIDTH, sizeof(bool));

        if(VERBOSE) {
            printf("\n  --EXTRACT THE OBJECTS' BOUNDARIES--\n");
            printf("     *Reading input image\n");
        }
        fread(imgIn, sizeof (unsigned char), HEIGHT * WIDTH, fp);
        fclose(fp);        

        if(VERBOSE) printf("   NOTE: Convert image to binary\n");
        imgBin = (unsigned char *) calloc(HEIGHT * WIDTH, sizeof(unsigned char));
        binaryImage(imgIn, imgBin);

        sprintf(oriOutputFilename, "oriCircle.pgm");
        if(writeImagePGM(oriOutputFilename, HEIGHT, WIDTH, imgBin) == 0) {
            fprintf(stderr, "***ERROR: Failed writting output image!");
            return -1;
        }

        if(VERBOSE) printf("     *Performing 8-neighbor calculation\n");
        erode(imgBin, imgOut);
        if(VERBOSE)printf("     *Eroding the image\n");
        boundaryExtract(imgBin, imgOut);

        if(VERBOSE) printf("     *Writting output image to M.pgm\n");
        sprintf(outputFilename, "M.pgm");
        if(writeImagePGM(outputFilename, HEIGHT, WIDTH, imgOut) == 0) {
            fprintf(stderr, "  ***ERROR: Failed writting output image!");
            return -1;
        }
        
        if(VERBOSE) printf("  --LABELING CONNECTED COMPONENT WITH DIF. COLORS--\n");
        labelComponent(imgBin, imgOut);
        if(VERBOSE) printf("     *Writting output image to L.pgm\n");
        sprintf(outputFilename, "L.pgm");
        if(writeImagePGM(outputFilename, HEIGHT, WIDTH, imgOut) == 0) {
            fprintf(stderr, "  ***ERROR: Failed writting output image!");
            return -1;
        }
        
        if(VERBOSE) printf("  --MORPHOLOGICAL OPERATOR TO COUNT NUMBER CIRCLE--\n");
        for(int x = 0; x < HEIGHT; x++) {
            for(int y = 0; y < WIDTH; y++) {
                visit[x * WIDTH + y] = false;
            }
        }
        for(int x = 0; x < HEIGHT; x++) {
            for(int y = 0; y < WIDTH; y++) {
                if(!visit[x * WIDTH + y]) {
                    count++;
                    componentCount(x, y, imgBin, visit, imgBin[x * WIDTH + y]);
                }
            }
        }
        printf("     [RESULT] Circle.raw ave %d components\n", count);
    }
    free(imgIn);
    free(imgOut);
    free(imgBin);
    free(visit);
    return 0;
}

// void labelComponent(unsigned char *image, unsigned char *labelImage) {
//     int x, y, counter, min, labelID = 1;
//     for(x = 0; x < HEIGHT; x++) {
//         for(y = 0; y < WIDTH; y++) {
//             labelImage[x * WIDTH + y] = image[x * WIDTH + y];
//         }
//     }

//     for(x = 0; x < HEIGHT; x++) {
//         for (y = 0; y < WIDTH; y++) {
//             if(image[x * WIDTH + y] == WHITE) {
//                 int m, n;
//                 counter = 0;
//                 min = 255;
//                 for(m = -1; m < 2; m++){
//                     for (n = -1; n < 2; n++) {
//                         // if((x + m >= 0) && ((x + m) < HEIGHT) && (y + n >= 0) && ((y + n) < WIDTH) && image[(x + m) * WIDTH + (y + n)] == WHITE) {
//                         if((x + m >= 0) && ((x + m) < HEIGHT) && (y + n >= 0) && ((y + n) < WIDTH) && labelImage[(x + m) * WIDTH + (y + n)] != BLACK) {
//                             if(labelImage[(x + m) * WIDTH + (y + n)] < min) {
//                                 min = labelImage[(x + m) * WIDTH + (y + n)];
//                             }
//                             if(labelImage[(x + m) * WIDTH + (y + n)] == WHITE) {
//                                 counter++;    
//                             }
//                         }
//                     }
//                 }

//                 if(min == 255) {
//                     if(counter > 1) {
//                         // labelImage[x * WIDTH + y] = labelID;
//                         printf("[%d]\tcounter: %d\n", x * WIDTH + y, counter);
//                         for(m = -1; m < 2; m++){
//                             for (n = -1; n < 2; n++) {
//                                 if((x + m >= 0) && ((x + m) < HEIGHT) && (y + n >= 0) && ((y + n) < WIDTH) && labelImage[(x + m) * WIDTH + (y + n)] == WHITE) {
//                                     labelImage[(x + m) * WIDTH + (y + n)] = labelID;
//                                     printf("labelImage: %d ; labelID: %d\n", labelImage[(x + m) * WIDTH + (y + n)], labelID);
//                                 }
//                             }
//                         }
//                     }
//                     else
//                     {
//                         printf("[%d]\tcounter: %d\n", x * WIDTH + y, counter);
//                         labelImage[x * WIDTH + y] = labelID;
//                         printf("labelImage: %d ; labelID: %d\n", labelImage[(x + m) * WIDTH + (y + n)], labelID);
//                     }
//                     labelID++;
//                 }
//                 else
//                 {
//                     if(counter > 1) {
//                         printf("[%d]\tcounter: %d ; min: %d\n", x * WIDTH + y, counter, min);
//                         for(m = -1; m < 2; m++){
//                             for (n = -1; n < 2; n++) {
//                                 if((x + m >= 0) && ((x + m) < HEIGHT) && (y + n >= 0) && ((y + n) < WIDTH) && labelImage[(x + m) * WIDTH + (y + n)] == WHITE) {
//                                     labelImage[(x + m) * WIDTH + (y + n)] = min;
//                                     printf("labelImage: %d ; labelID: %d\n", labelImage[(x + m) * WIDTH + (y + n)], labelID);
//                                 }
//                             }
//                         }
//                     }
//                     else
//                     {
//                         printf("[%d]\tcounter: %d ; min: %d\n", x * WIDTH + y, counter, min);
//                         labelImage[x * WIDTH + y] = min;
//                         printf("labelImage: %d ; labelID: %d\n", labelImage[(x + m) * WIDTH + (y + n)], labelID);
//                     }
//                 }   



//                 ////////////////////////
//                 // if(counter > 0) {
//                 //     // int pValue = labelImage[x * WIDTH + y];
//                 //     if(min == 255) {
//                 //         labelImage[x * WIDTH + y] = labelID;
//                 //         // printf("[NEW]\tlabelImage[%d]: %d ; labelID: %d ; Min: %d\n", x * WIDTH + y, labelImage[x * WIDTH + y], labelID, min);
//                 //         labelID++;
//                 //         // printf("%s\n", );
//                 //     } else {
//                 //         labelImage[x * WIDTH + y] = min;
//                 //         // printf("[ELSE]\tlabelImage[%d]: %d ; labelID: %d ; min: %d\n", x * WIDTH + y, labelImage[x * WIDTH + y], labelID, min);
//                 //     }
//                 // }
//                 /////////////////////////




//             }
//         }
//         printf("===== Line: %d\n", x);
//     }

//     // for(x = 0; x < HEIGHT; x++) {
//     //     for (y = 0; y < WIDTH; y++) {
//     //         if(image[x * WIDTH + y] == WHITE) {
//     //             int m, n, min = 255;
//     //             for(m = -1; m < 2; m++){
//     //                 for (n = -1; n < 2; n++) {
//     //                     if((x + m >= 0) && ((x + m) < HEIGHT) && (y + n >= 0) && ((y + n) < WIDTH) && image[(x + m) * WIDTH + (y + n)] != BLACK) {
//     //                         if(labelImage[(x + m) * WIDTH + (y + n)] < min) {
//     //                             min = labelImage[(x + m) * WIDTH + (y + n)];
//     //                         }
//     //                     }
//     //                 }
//     //             }
//     //             labelImage[x * WIDTH + y] = min;
//     //             printf("labelImage[%d]: %d ; min: %d\n", x * WIDTH + y, labelImage[x * WIDTH + y], min);
//     //         }
//     //     }
//     //     printf("----- Line : %d\n", x);
//     // }
// }

void componentCount(int x, int y, unsigned char *image, bool *visitImage, unsigned char pixelValue) {
    if(x >= 0 && x < HEIGHT && y >= 0 && y < WIDTH) {
        if(image[x * WIDTH + y] == pixelValue && !visitImage[x * WIDTH + y]) {
            visitImage[x * WIDTH + y] = true;
            componentCount(x + 1, y, image, visitImage, pixelValue);
            componentCount(x - 1, y, image, visitImage, pixelValue);
            componentCount(x, y + 1, image, visitImage, pixelValue);
            componentCount(x, y - 1, image, visitImage, pixelValue);
        }
    }
}



void labelComponent(unsigned char *image, unsigned char *labelImage) {
    int x, y, numberCircle = 0, labelID;

    for(x = 0; x < HEIGHT; x++) {
        for(y = 0; y < WIDTH; y++) {
            labelImage[x * WIDTH + y] = image[x * WIDTH + y];
        }
    }

    labelID = 35;

    for(x = 0; x < HEIGHT; x++) {
        for (y = 0; y < WIDTH; y++) {
            if(image[x * WIDTH + y] == WHITE) {
                if(x - 1 >= 0 && y - 1 >= 0) {
                    if(image[x * WIDTH + (y - 1)] == 0 && image[(x - 1) * WIDTH + y] == 0) {
                        labelImage[x * WIDTH + y] = labelID;
                        labelID++;
                    } else {
                        if(image[x * WIDTH + (y - 1)] == 255) {
                            labelImage[x * WIDTH + y] = labelImage[x * WIDTH + (y - 1)];
                        } 
                        else 
                        {
                            labelImage[x * WIDTH + y] = labelImage[(x - 1) * WIDTH + y];
                        }
                    }
                }
            }
        }
    }

    for(x = 0; x < HEIGHT; x++) {
        for (y = 0; y < WIDTH; y++) {
            if(image[x * WIDTH + y] == WHITE) {
                int m, n, min = 255;
                for(m = -1; m < 2; m++){
                    for (n = -1; n < 2; n++) {
                        if((x + m >= 0) && ((x + m) < HEIGHT) && (y + n >= 0) && ((y + n) < WIDTH) && image[(x + m) * WIDTH + (y + n)] != BLACK) {
                            if(labelImage[(x + m) * WIDTH + (y + n)] < min) {
                                min = labelImage[(x + m) * WIDTH + (y + n)];
                            }
                        }
                    }
                }
                labelImage[x * WIDTH + y] = min;
            }
        }
    }
}

void boundaryExtract(unsigned char *image, unsigned char *erodeImage) {
    int x, y;
    for(x = 0; x < HEIGHT; x++) {
        for(y = 0; y < WIDTH; y++) {
            erodeImage[x * WIDTH + y] = image[x * WIDTH + y] - erodeImage[x * WIDTH + y];
        }
    }
}

void erode(unsigned char *image, unsigned char *erodeImage) {
    int counter;
    int x, y;

    for(x = 0; x < HEIGHT; x++) {
        for(y = 0; y < WIDTH; y++) {
            erodeImage[x * WIDTH + y] = image[x * WIDTH + y];
        }
    }

    for(x = 0; x < HEIGHT; x++) {
        for (y = 0; y < WIDTH; y++) {
            if(image[x * WIDTH + y] == WHITE) {
                int m, n;
                counter = 0;
                for(m = -1; m < 2; m++){
                    for (n = -1; n < 2; n++) {
                        if((x + m >= 0) && ((x + m) < HEIGHT) && (y + n >= 0) && ((y + n) < WIDTH) && image[(x + m) * WIDTH + (y + n)] == BLACK) {
                            counter++;
                        }
                    }
                }
                if(counter > 0) {
                    erodeImage[x * WIDTH + y] = BLACK;
                }
            }
        }
    }
}

void binaryImage(unsigned char *image, unsigned char *binaryImage) {
    int x, y;
    for(x = 0; x < HEIGHT; x++) {
        for(y = 0; y < WIDTH; y++) {
            binaryImage[x * WIDTH + y] = image[x * WIDTH + y];
        }
    }
    for(x = 0; x < HEIGHT; x++) {
        for(y = 0; y < WIDTH; y++) {
            binaryImage[x * WIDTH + y] = (image[x * WIDTH + y] < 125 ? 0 : 255);
        }
    }
}

int writeImagePGM(char *filename, int height, int width, unsigned char *image) {
    FILE *fp;
    fp = fopen(filename, "wb");
    if (fp) {
        fprintf(fp, "P5\n\n%d %d 255\n", height, width);
        fwrite(image, sizeof(unsigned char), height * width, fp);
        fclose(fp);
        return 1;
    } else {
        return 0;        
    }
}