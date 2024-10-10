// Copyright Balasa Radu-Ion 311CAa 2023-2024
#include <stdio.h> // citire standard
#include <stdlib.h> // alocarea dinamica a memoriei
#include <string.h> // manipularea sirurilor de caractere

#include "image.h"

#define BUFFER_SIZE 200
#define FILE_NAME_SIZE 100

int main(void)
{
	image curr_image;
	char buffer[BUFFER_SIZE], filename[FILE_NAME_SIZE];
	int file_loaded = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	while (1) {
		// verificam daca se pot citi date de la tastatura
		if (!fgets(buffer, sizeof(buffer), stdin))
			return 0;
		if (strncmp(buffer, "LOAD", 4) == 0) {
			if (sscanf(buffer + 5, "%s", filename) == 1) {
				if (file_loaded)
					free_pixels_array(&curr_image);// eliberam memoria
				file_loaded = load_file(filename, &curr_image);
				if (file_loaded) {// verificam daca s-a incarcat in memorie
					printf("Loaded %s\n", filename);
					x1 = 0, y1 = 0;
					x2 = curr_image.width, y2 = curr_image.height;
				} else {
					printf("Failed to load %s\n", filename);
				}
			}
		}  else if (strncmp(buffer, "SELECT ALL", 10) == 0) {
			select_all(&curr_image, file_loaded, &x1, &y1, &x2, &y2);
		} else if (strncmp(buffer, "SELECT", 6) == 0) {
			select(&curr_image, buffer, file_loaded, &x1, &y1, &x2, &y2);
		} else if (strncmp(buffer, "HISTOGRAM", 9) == 0) {
			get_histogram(&curr_image, buffer, file_loaded);
		} else if (strncmp(buffer, "EQUALIZE", 8) == 0) {
			if (!file_loaded) {// verificam daca avem o imagine incarcata
				printf("No image loaded\n");
				continue;
			}
			do_equalize(&curr_image);
		} else if (strncmp(buffer, "ROTATE", 6) == 0) {
			rotate(&curr_image, buffer, file_loaded, &x1, &y1, &x2, &y2);
		} else if (strncmp(buffer, "CROP", 4) == 0) {
			if (!file_loaded) {
				printf("No image loaded\n");
				continue;
			}
			crop(&curr_image, x1, y1, x2, y2);
			x1 = 0, y1 = 0, x2 = curr_image.width, y2 = curr_image.height;
		} else if (strncmp(buffer, "APPLY", 5) == 0) {
			char parameter[15];
			int check = sscanf(buffer + 6, "%s", parameter);
			if (!file_loaded) {
				printf("No image loaded\n");
				continue;
			}
			// verificam daca comanda este valida
			if (strlen(parameter) == 0 || check > 2) {
				printf("Invalid command\n");
				continue;
			}
			apply(&curr_image, parameter, x1, y1, x2, y2);
		} else if (strncmp(buffer, "SAVE", 4) == 0) {
			char ascii[5] = "\n";
			if (sscanf(buffer + 5, "%s %s", filename, ascii) == 0)
				printf("Invalid command\n");
			if (!file_loaded) {
				printf("No image loaded\n");
				continue;
			}
			save_image(&curr_image, filename, ascii);
		} else if (strncmp(buffer, "EXIT", 4) == 0) {
			if (!file_loaded) {
				printf("No image loaded\n");
				continue;
			}
			free_pixels_array(&curr_image);// eliberam memoria imaginii
			exit(0);// inchidem programul
		} else {
			printf("Invalid command\n");
		}
	}
}
