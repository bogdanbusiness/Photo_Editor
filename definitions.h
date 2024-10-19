// Gheorghe Andrei-Bogdan - 313CA
#ifndef _FUNCTII_H
#define _FUNCTII_H

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Encoding for the settings of the program
#define LOAD 1
#define SELECT 2
#define HISTOGRAM 3
#define EQUALIZE 4
#define ROTATE 5
#define CROP 6
#define APPLY 7
#define SAVE 8
#define EXIT 9
#define SHOW 10

// Encoding for the apply settings
#define EDGE 1001
#define BLUR 1002
#define SHARPEN 1003
#define GAUSSIAN_BLUR 1004

// Other definitions
#define u_ch unsigned char
#define KER_SIZE 3
#define COORD_NUM 4
#define PIXEL_CHAR 5
#define BUFFER 256
#define MAX_PIXEL 255
#define GET_NUMBER_RETURN_FAIL -2147483648

#define TRUE 1
#define FALSE 0

// Structures
typedef struct {
	    int type;
	    int lines, columns;
		int **R, **G, **B;
} img_struct;

typedef struct {
	int x_start, y_start;
	int x_end, y_end;
} sel_coord;

typedef struct {
		char *setting;
		int key;
} dict;

// File operations
// Reading

int metadata_read(img_struct *image, char *path);
int image_read(img_struct *img, char *path, int pos, u_ch(*read)(FILE * src));
u_ch read_ascii(FILE *source);
u_ch read_binary(FILE *source);

// Saving

int metadata_save(img_struct image, char *path, int is_bin);
int image_save(img_struct img, char *path, int poz, int is_bin);
void write_ascii(img_struct image, FILE *source);
void write_binary(img_struct image, FILE *source);

// Settings

void load(img_struct *image, sel_coord *coord);
void save(img_struct image);
void selection(img_struct image, sel_coord *coord);
void histogram(img_struct image, sel_coord coord);
void equalize(img_struct *image);
void crop(img_struct *image, sel_coord *coord);
void rotate(img_struct *image, sel_coord *coord);
void apply(img_struct *image, sel_coord *coord);

void show(img_struct image);

// Memory allocation

int create_matrix(int ***pointer, int nlines, int ncolumns);
void delete_matrix(int ***matrix, int size);
int create_image(img_struct *image);
void delete_image(img_struct *image);

// Helper funcitons

int rotate_image(img_struct *image, sel_coord *coord);
int rotate_section(img_struct *image, sel_coord *coord);
int key_from_setting(char *key);
int get_kernel(double ker[3][3], char *setting);
int clamp(int number, int min, int max);
int get_number(void);
void swap(int *number1, int *number2);

// Error functions

int check_image(img_struct image);
int check_bw(img_struct image);
int check_nospace(void);
int check_space(void);
int is_number(char *string);

#endif
