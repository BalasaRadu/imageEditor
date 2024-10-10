// Copyright Balasa Radu-Ion 311CAa 2023-2024
#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h> // citire standard
#include <stdlib.h> // alocarea dinamica a memoriei
#include <string.h> // manipularea sirurilor de caractere
#include <math.h> // pentru functia round

#define BUFFER_SIZE 200

typedef struct {
	char magic_nr[3];// retine tipul imaginii
	int width, height;// retine dimensiunile imaginii
	int nr_colors;// retine cate 0 daca imaginea e alb-negru sau 3 daca e color
	int maxval;// retine valoarea maxima e intensitatii (255)
	unsigned char ***data;// matrice pentru retinerea pixelilor
} image;

void swap(void *x, void *y, size_t size);

int alloc_pixels_array(image *curr_image);
void free_pixels_array(image *curr_image);

int load_file(char *filename, image *curr_image);

void select_all(image *img, int l, int *x1, int *y1, int *x2, int *y2);
void select(image *img, char *b, int l, int *x1, int *y1, int *x2, int *y2);

int check_nr_bins(int x);
void get_histogram(image *curr_image, char *b, int l);

void do_equalize(image *curr_image);

int check_angle(int x);
void rotate(image *img, char *b, int l, int *x1, int *y1, int *x2, int *y2);
void rotate_90_deg_cw_submat(image *curr_image, int x1, int y1, int x2, int y2);
void rotate_90_deg_cw_whole_img(image *curr_image);

void crop(image *curr_image, int x1, int y1, int x2, int y2);

void apply(image *curr_image, char *parameter, int x1, int y1, int x2, int y2);

void save_image(image *curr_image, char *filename, char *asci);

#endif
