// Copyright Balasa Radu-Ion 311CAa 2023-2024
#include "image.h"

void swap(void *x, void *y, size_t size)
// functie generica de swap
{
	void *tmp = malloc(size);
	if (!tmp) {
		printf("swap malloc failed\n");
		exit(EXIT_FAILURE);
	}
	memcpy(tmp, x, size);
	memcpy(x, y, size);
	memcpy(y, tmp, size);

	free(tmp);
}

int alloc_pixels_array(image *curr_image)
// functie care aloca dinamic memorie pentru matricea de pixeli
{
	int width = curr_image->width;
	int height = curr_image->height;
	int nr_colors = curr_image->nr_colors;
	curr_image->data = (unsigned char ***)
					malloc(sizeof(unsigned char **) * height);
	if (!curr_image->data) {
		printf("alloc_pixels_array malloc1 failed\n");
		free(curr_image->data);
		return 0;
	}
	for (int i = 0; i < height; i++) {
		curr_image->data[i] = (unsigned char **)
							malloc(sizeof(unsigned char *) * width);
		if (!curr_image->data[i]) {
			printf("alloc_pixels_array malloc2 failed\n");
			free(curr_image->data[i]);
			return 0;
		}
		for (int j = 0; j < width; j++) {
			curr_image->data[i][j] = (unsigned char *)
									malloc(sizeof(unsigned char) * nr_colors);
			if (!curr_image->data[i][j]) {
				printf("alloc_pixels_array malloc3 failed\n");
				free(curr_image->data);
				return 0;
			}
		}
	}
	return 1;
}

int load_file(char *filename, image *curr_image)
// functie pentru a incarca in memorie o imagine
{
	FILE *file = fopen(filename, "r");

	if (!file)// nu s-a putut deschide fisierul
		return 0;

	fscanf(file, "%s", curr_image->magic_nr);

	fscanf(file, "%d %d", &curr_image->width, &curr_image->height);

	curr_image->nr_colors = 1;
	if (strchr("36", curr_image->magic_nr[1]))
	// verificam daca imaginea este color
		curr_image->nr_colors = 3;

	if (strchr("14", curr_image->magic_nr[1])) {
	// verificam daca imaginea este alb-negru
		curr_image->maxval = 1;
	} else {
		fscanf(file, "%d", &curr_image->maxval);
		char newline;
		fscanf(file, "%c", &newline);// citim newlinul care este in plus
	}

	if (!alloc_pixels_array(curr_image))// nu s-a putut aloca memoria
		return 0;

	for (int i = 0; i < curr_image->height; i++)
		for (int j = 0; j < curr_image->width; j++)
			for (int k = 0; k < curr_image->nr_colors; k++) {
				unsigned char pixel = 0;
				if (strchr("123", curr_image->magic_nr[1]))
				// citire pentru datele binare
					fscanf(file, "%hhu", &pixel);
				else
				// citire pentru datele in format ascii
					fread(&pixel, sizeof(unsigned char), 1, file);
				curr_image->data[i][j][k] = pixel;
			}

	fclose(file);
	return 1;
}

void select_all(image *img, int l, int *x1, int *y1, int *x2, int *y2)
{
	if (!l) {
	// verificam daca avem o imagine incarcata in memorie
		printf("No image loaded\n");
		return;
	}
	*x1 = 0, *x2 = img->width, *y1 = 0, *y2 = img->height;
	printf("Selected ALL\n");
}

void select(image *img, char *b, int f, int *x1, int *y1, int *x2, int *y2)
{
	int new_x1, new_y1, new_x2, new_y2;
	int ok = 1;
	if (sscanf(b + 7, "%d%d%d%d", &new_x1, &new_y1, &new_x2, &new_y2) != 4)
	// verificam daca s-au citit datele corect
		ok = 0;
	if (!f) {
	// verificam daca avem o imagine incarcata in memorie
		printf("No image loaded\n");
		return;
	}
	if (!ok) {
		printf("Invalid command\n");
		return;
	}
	if (new_x1 > new_x2)
	//verificam oridnea coordonatelor x
		swap(&new_x1, &new_x2, sizeof(int));
	if (new_y1 > new_y2)
	//verificam oridnea coordonatelor y
		swap(&new_y1, &new_y2, sizeof(int));
	if (new_x1 < 0 || new_x2 > img->width ||
		new_y1 < 0 || new_y2 > img->height ||
		new_x1 == new_x2 || new_y1 == new_y2) {
		printf("Invalid set of coordinates\n");
		return;
	}
	*x1 = new_x1, *x2 = new_x2;
	*y1 = new_y1, *y2 = new_y2;
	printf("Selected %d %d %d %d\n", *x1, *y1, *x2, *y2);
}

int check_nr_bins(int x)
{
	if (x < 2 || x > 256)
		return 0;

	while (x % 2 == 0)
	// verificam daca numarul este putere de 2
		x /= 2;

	return x == 1;
}

void get_histogram(image *curr_image, char *b, int l)
{
	int max_stars, nr_bins, extra, ok = 1;
	if (sscanf(b + 10, "%d %d %d", &max_stars, &nr_bins, &extra) != 2)
	// verificam daca s-au citit corect datele de intrare
		ok = 0;
	if (!l) {
	// verificam daca avem o imagine incarcata in memorie
		printf("No image loaded\n");
		return;
	}
	if (!ok) {
		printf("Invalid command\n");
		return;
	}
	if (strchr("36", curr_image->magic_nr[1])) {
	// verificam daca imaginea este color
		printf("Black and white image needed\n");
		return;
	}

	if (!check_nr_bins(nr_bins)) {
		printf("Invalid set of parameters\n");
		return;
	}

	// alocam dinamic memoria vectorului de frecventa
	int *fv;
	fv = (int *)calloc(nr_bins, sizeof(int));

	if (!fv) {
		printf("get_histogram malloc failed\n");
		exit(EXIT_FAILURE);
	}

	int max_fv = 0;

	// actualizam vectorul de frecventa pe bin-uri
	for (int i = 0; i < curr_image->height; i++)
		for (int j = 0; j < curr_image->width; j++)
			fv[curr_image->data[i][j][0] / (256 / nr_bins)]++;

	// determinam frecventa maxima
	for (int i = 0; i < nr_bins; i++)
		if (fv[i] > max_fv)
			max_fv = fv[i];

	// afisam histograma
	for (int i = 0; i < nr_bins; i++) {
		int cnt_stars = fv[i] * max_stars / max_fv;
		printf("%d\t|\t", cnt_stars);
		for (int j = 0; j < cnt_stars; j++)
			printf("*");
		printf("\n");
	}

	free(fv);
}

void do_equalize(image *curr_image)
{
	if (strchr("36", curr_image->magic_nr[1])) {
	// verificam daca imaginea este color
		printf("Black and white image needed\n");
		return;
	}

	// alocam dinamic memoria pt vectorul h
	// in care memoram suma partiala pe vectorul de frecvente
	int *h;
	h = (int *)calloc(256, sizeof(int));

	// actualizam vectorul de frecvente
	for (int i = 0; i < curr_image->height; i++)
		for (int j = 0; j < curr_image->width; j++)
			h[curr_image->data[i][j][0]]++;

	// calculam sumele partiale
	for (int i = 1; i <= curr_image->maxval; i++)
		h[i] = h[i - 1] + h[i];

	for (int i = 0; i < curr_image->height; i++)
		for (int j = 0; j < curr_image->width; j++) {
			double x = curr_image->height, y = curr_image->width;
			double area = x * y;
			x = h[curr_image->data[i][j][0]], y = 255;
			double ans = x * y;
			ans /= area;
			if (ans < 0)
				ans = 0;

			if (ans > 255)
				ans = 255;
			// actualizam valoare din matricea de pixeli
			curr_image->data[i][j][0] = (unsigned char)ans;
		}

	free(h);
	printf("Equalize done\n");
}

int check_angle(int x)
{
	if (x % 90 != 0)
		return 0;
	if (x < -360 || x > 360)
		return 0;
	return 1;
}

void rotate_90_deg_cw_submat(image *curr_image, int x1, int y1, int x2, int y2)
// functie pentru rotirea unei submatrici
{
	// transpunem submatricea
	for (int dy = 0; dy < y2 - y1; dy++)
		for (int dx = 0; dx < x2 - x1; dx++)
			for (int k = 0; k < curr_image->nr_colors; k++) {
				if (dy <= dx)
					continue;
				swap(&curr_image->data[y1 + dy][x1 + dx][k],
					 &curr_image->data[y1 + dx][x1 + dy][k],
					 sizeof(unsigned char));
			}
	// inversam ordinea de pe linile submatricii
	for (int i = y1; i < y2; i++)
		for (int j1 = x1, j2 = x2 - 1; j1 < j2; j1++, j2--)
			for (int k = 0; k < curr_image->nr_colors; k++)
				swap(&curr_image->data[i][j1][k],
					 &curr_image->data[i][j2][k],
					 sizeof(unsigned char));
}

void rotate_90_deg_cw_whole_img(image *curr_image)
{
	// declaram o matrice auxiliara
	image aux_image;
	aux_image.height = curr_image->width;
	aux_image.width = curr_image->height;
	aux_image.nr_colors = curr_image->nr_colors;
	aux_image.maxval = curr_image->maxval;
	aux_image.data = NULL;
	alloc_pixels_array(&aux_image);

	// rotim matricea prin transpunere si schimbarea ordinii liniilor
	for (int i = 0; i < curr_image->height; i++)
		for (int j = 0; j < curr_image->width; j++)
			for (int k = 0; k < curr_image->nr_colors; k++) {
				int val = curr_image->data[i][j][k];
				aux_image.data[j][curr_image->height - 1 - i][k] = val;
			}
	free_pixels_array(curr_image);
	curr_image->height = aux_image.height;
	curr_image->width = aux_image.width;
	alloc_pixels_array(curr_image);

	for (int i = 0; i < curr_image->height; i++)
		for (int j = 0; j < curr_image->width; j++)
			for (int k = 0; k < curr_image->nr_colors; k++)
				curr_image->data[i][j][k] = aux_image.data[i][j][k];

	free_pixels_array(&aux_image);
}

void rotate(image *img, char *b, int l, int *x1, int *y1, int *x2, int *y2)
{
	int angle, ok = 1;
	if (sscanf(b + 7, "%d", &angle) != 1)
	// verificam daca s-a citit corect ungiul de rotatie
		ok = 0;
	if (!l) {
	// verificam daca avem o imagine incarcata in memorie
		printf("No image loaded\n");
		return;
	}
	if (!ok) {
		printf("Invalid command\n");
		return;
	}
	if (*x1 == 0 && *x2 == img->width &&
		*y1 == 0 && *y2 == img->height) {
	// cazul de rotire a intregii rotatii
		if (!check_angle(angle)) {
			printf("Unsupported rotation angle\n");
			return;
		}
		for (int i = 0; i < angle + 360; i += 90) {
			rotate_90_deg_cw_whole_img(img);
			swap(x2, y2, sizeof(int));
		}
		printf("Rotated %d\n", angle);
		return;
	}
	if (*x2 - *x1 != *y2 - *y1) {
	// verificam daca submatricea este patratica
		printf("The selection must be square\n");
		return;
	}
	if (!check_angle(angle)) {
		printf("Unsupported rotation angle\n");
		return;
	}
	for (int i = 0; i < angle + 360; i += 90)
		rotate_90_deg_cw_submat(img, *x1, *y1, *x2, *y2);
	printf("Rotated %d\n", angle);
}

void free_pixels_array(image *curr_image)
// functie pentru eliberarea matricii pixelilor
{
	for (int i = 0; i < curr_image->height; i++) {
		for (int j = 0; j < curr_image->width; j++)
			free(curr_image->data[i][j]);
		free(curr_image->data[i]);
	}
	free(curr_image->data);
}

void crop(image *curr_image, int x1, int y1, int x2, int y2)
{
	// declaram o imagine auxiliara
	image aux_image;
	aux_image.width = x2 - x1;
	aux_image.height = y2 - y1;
	aux_image.nr_colors = curr_image->nr_colors;
	aux_image.maxval = curr_image->maxval;
	aux_image.data = NULL;
	alloc_pixels_array(&aux_image);

	// mutam pixelii zonei selectate in imaginea auxiliara
	for (int i = y1; i < y2; i++)
		for (int j = x1; j < x2; j++)
			for (int k = 0; k < curr_image->nr_colors; k++)
				aux_image.data[i - y1][j - x1][k] = curr_image->data[i][j][k];

	free_pixels_array(curr_image);

	// redimensionam imaginea curenta
	curr_image->width = x2 - x1;
	curr_image->height = y2 - y1;
	curr_image->maxval = aux_image.maxval;
	curr_image->nr_colors = aux_image.nr_colors;
	alloc_pixels_array(curr_image);

	// copiam datele in imaginea initiala
	for (int i = 0; i < curr_image->height; i++)
		for (int j = 0; j < curr_image->width; j++)
			for (int k = 0; k < curr_image->nr_colors; k++)
				curr_image->data[i][j][k] = aux_image.data[i][j][k];

	free_pixels_array(&aux_image);
	printf("Image cropped\n");
}

void apply(image *curr_image, char *parameter, int x1, int y1, int x2, int y2)
// functie pentru aplicarea unui filtru
{
	int kernel[3][3];// matricea nucleu
	double divide = 1;
	if (strcmp(parameter, "EDGE") == 0) {
		kernel[0][0] = -1, kernel[0][1] = -1, kernel[0][2] = -1;
		kernel[1][0] = -1, kernel[1][1] = 8, kernel[1][2] = -1;
		kernel[2][0] = -1, kernel[2][1] = -1, kernel[2][2] = -1;
	} else if (strcmp(parameter, "SHARPEN") == 0) {
		kernel[0][0] = 0, kernel[0][1] = -1, kernel[0][2] = 0;
		kernel[1][0] = -1, kernel[1][1] = 5, kernel[1][2] = -1;
		kernel[2][0] = 0, kernel[2][1] = -1, kernel[2][2] = 0;
	} else if (strcmp(parameter, "BLUR") == 0) {
		kernel[0][0] = 1, kernel[0][1] = 1, kernel[0][2] = 1;
		kernel[1][0] = 1, kernel[1][1] = 1, kernel[1][2] = 1;
		kernel[2][0] = 1, kernel[2][1] = 1, kernel[2][2] = 1;
		divide = 9;
	} else if (strcmp(parameter, "GAUSSIAN_BLUR") == 0) {
		kernel[0][0] = 1, kernel[0][1] = 2, kernel[0][2] = 1;
		kernel[1][0] = 2, kernel[1][1] = 4, kernel[1][2] = 2;
		kernel[2][0] = 1, kernel[2][1] = 2, kernel[2][2] = 1;
		divide = 16;
	} else {
		printf("APPLY parameter invalid\n");
		return;
	}
	if (strchr("1245", curr_image->magic_nr[1])) {
	// verificam daca imaginea este alb-negru
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	// declaram o matrice auxiliara
	image aux_image;
	aux_image.height = curr_image->height;
	aux_image.width = curr_image->width;
	alloc_pixels_array(&aux_image);

	// vectori de directie pentru a accesa vecinii unui pixel
	int dx[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
	int dy[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};

	for (int i = y1; i < y2; i++)
		for (int j = x1; j < x2; j++)
			for (int k = 0; k < 3; k++)
				// verificam daca pixelii se afla pe margine
				if (i != 0 && i != curr_image->height - 1 &&
					j != 0 && j != curr_image->width - 1) {
					double sum = 0;
					for (int l = 0; l < 9; l++) {
						double coef = kernel[1 + dx[l]][1 + dy[l]];
						double val = curr_image->data[i + dx[l]][j + dy[l]][k];
						sum += coef * val;
					}
					sum /= divide;
					sum = round(sum);
					if (sum < 0)
						sum = 0;
					if (sum > 255)
						sum = 255;
					aux_image.data[i][j][k] = (unsigned char)sum;
				}

	// copiem pixelii dupa aplicarea filtrului in matricea initiala
	for (int i = y1; i < y2; i++)
		for (int j = x1; j < x2; j++)
			for (int k = 0; k < 3; k++)
				if (i != 0 && i != curr_image->height - 1 &&
					j != 0 && j != curr_image->width - 1)
					curr_image->data[i][j][k] = aux_image.data[i][j][k];

	free_pixels_array(&aux_image);
	printf("APPLY %s done\n", parameter);
}

void save_image(image *curr_image, char *filename, char *ascii)
{
	FILE *file = fopen(filename, "w");
	if (!file) {
	// verificam daca s-a putut deschide fisierul
		printf("Failed to save %s\n", filename);
		return;
	}
	int binary = 1;
	if (strcmp(ascii, "ascii") == 0)
	// verificam daca salvam imaginea in format binary
		binary = 0;

	// actualizam tipul imaginii
	if (strchr("123", curr_image->magic_nr[1]) && binary)
		curr_image->magic_nr[1] += 3;
	if (strchr("456", curr_image->magic_nr[1]) && !binary)
		curr_image->magic_nr[1] -= 3;

	// afisam datele imaginii
	fprintf(file, "%s\n", curr_image->magic_nr);
	fprintf(file, "%d %d\n", curr_image->width, curr_image->height);
	fprintf(file, "%d\n", curr_image->maxval);
	for (int i = 0; i < curr_image->height; i++) {
		for (int j = 0; j < curr_image->width; j++)
			for (int k = 0; k < curr_image->nr_colors; k++)
				if (binary)
					fwrite(&curr_image->data[i][j][k],
						   sizeof(unsigned char), 1, file);
				else
					fprintf(file, "%d ", curr_image->data[i][j][k]);
		if (!binary)
			fprintf(file, "\n");
	}
	fclose(file);
	printf("Saved %s\n", filename);
}
