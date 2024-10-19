// Gheorghe Andrei-Bogdan - 313CA
#include "definitions.h"

/// @brief Loads an image from a file
/// @param image Struct pointer for the loaded image
/// @param coord Coordonate struct pointer
void load(img_struct *image, sel_coord *coord)
{
	char path[BUFFER];

	// Deletes the image loaded in memory (if it exists)
	if (image->type != 0)
		delete_image(image);

	// Check for space, get the path, check if command has ended
	if (check_space())
		return;

	scanf("%s", path);

	if (check_nospace())
		return;

	// Reads the metadata of the file
	int position = metadata_read(image, path);
	if (position == -1) {
		printf("Failed to load %s\n", path);
		return;
	}

	// Creates the image in memory
	if (create_image(image)) {
		printf("Failed to load %s\n", path);
		delete_image(image);
		return;
	}

	// Reads the image either in ascii or in binary
	if (image->type - 3 <= 0) {
		if (image_read(image, path, position, read_ascii)) {
			printf("Failed to load %s\n", path);
			delete_image(image);
			return;
		}
	} else {
		if (image_read(image, path, position, read_binary)) {
			printf("Failed to load %s\n", path);
			delete_image(image);
			return;
		}
	}

	// Sets the selection coordonates to select everything
	coord->x_start = 0;
	coord->y_start = 0;
	coord->x_end = image->columns;
	coord->y_end = image->lines;

	// Print the success to console
	printf("Loaded %s\n", path);
}

/// @brief Saves the image in a file
/// @param image The image that will be saved
void save(img_struct image)
{
	// Check if the image exists
	if (check_image(image))
		return;

	// Check for space, get the path, check if command has ended
	if (check_space())
		return;

	char path[BUFFER];
	short int is_bin;

	scanf("%s", path);

	// Check if the save is in binary or not
	if ((char)getchar() == ' ') {
		is_bin = TRUE;

		// Check if its the end of the line
		char c = getchar();
		if (c != '\n') {
			// Get the rest of the command
			char tmp[BUFFER];
			tmp[0] = c;
			scanf("%s", tmp + 1);
			// Check if we have the string "ascii"
			if (strcmp(tmp, "ascii") == 0)
				is_bin = FALSE;
		}
	} else {
		is_bin = TRUE;
	}

	// Saves the metadata
	int position = metadata_save(image, path, is_bin);
	if (position == -1) {
		printf("Failed to save in %s\n", path);
		return;
	}

	// Saves the image itself
	if (image_save(image, path, position, is_bin)) {
		printf("Failed to save in %s\n", path);
		return;
	}

	// Print the success to console
	printf("Saved %s\n", path);
}

/// @brief Crops the image according to the coordonates selected
/// @param image Pointer to the image that will be cropped
/// @param crd Coordonate struct pointer
void crop(img_struct *image, sel_coord *crd)
{
	// Check if the image exists
	if (check_image(*image))
		return;

	// Check if the command has ended
	if (check_nospace())
		return;

	// Creates the cropped image structure
	img_struct crop;
	crop.type = image->type;
	crop.columns = crd->x_end - crd->x_start;
	crop.lines = crd->y_end - crd->y_start;
	crop.R = NULL; crop.B = NULL; crop.G = NULL;

	// Alocates memory for the cropped image
	if (create_image(&crop)) {
		printf("Crop failed\n");
		delete_image(&crop);
		return;
	}

	// Copies the information
	for (int i = 0; i < crop.lines; i++) {
		for (int j = 0; j < crop.columns; j++) {
			crop.R[i][j] = image->R[i + crd->y_start][j + crd->x_start];
			if (crop.type % 3 == 0) {
				crop.G[i][j] = image->G[i + crd->y_start][j + crd->x_start];
				crop.B[i][j] = image->B[i + crd->y_start][j + crd->x_start];
			}
		}
	}

	// Deletes the image and copies the crop into image
	delete_image(image);
	image->type = crop.type;
	image->columns = crop.columns;
	image->lines = crop.lines;
	image->R = crop.R; image->B = crop.B; image->G = crop.G;

	// Modify the coordonate struct
	crd->x_start = 0; crd->x_end = image->columns;
	crd->y_start = 0; crd->y_end = image->lines;

	// Print the success to console
	printf("Image cropped\n");
}

/// @brief Equalize a black and white image
/// @param image Pointer to the image that will be equalized
void equalize(img_struct *image)
{
	// Check if the image exists
	if (check_image(*image))
		return;

	// Check if the command is complete
	if (check_nospace())
		return;

	// Check if the image is black and white
	if (check_bw(*image))
		return;

	int freq[MAX_PIXEL + 1] = {0}, area;
	double pixel_sum = 0, pixel;

	// Calculate the frequency vector
	for (int i = 0; i < image->lines; i++)
		for (int j = 0; j < image->columns; j++)
			freq[image->R[i][j]]++;

	area = image->lines * image->columns;

	// Caluculate the equalization formula for each pixel
	for (int i = 0; i < image->lines; i++)
		for (int j = 0; j < image->columns; j++) {
			pixel = image->R[i][j];
			for (int k = 0; k <= pixel; k++)
				pixel_sum += freq[k];

			pixel = 255 * pixel_sum / area;
			image->R[i][j] = clamp((int)round(pixel), 0, MAX_PIXEL);
			pixel_sum = 0;
		}

	// Print the success
	printf("Equalize done\n");
}

/// @brief Rotates the image by an angle
/// @param image Image pointer to the image that will be rotated
/// @param coord Coordonate struct
void rotate(img_struct *image, sel_coord *coord)
{
	// Check if the image exists
	if (check_image(*image))
		return;

	if (check_space())
		return;

	// Scan the angle in the command
	int angle;
	angle = get_number();
	if (angle == GET_NUMBER_RETURN_FAIL) {
		char clear[BUFFER];
		scanf("%255[^\n]", clear);
		printf("Invalid command\n");
	}

	// Check if angle is supported
	if (abs(angle) % 90 != 0 || abs(angle) > 360) {
		printf("Unsupported rotation angle\n");
		return;
	}

	// Check if the selection is either square or the entire image
	int col = coord->x_end - coord->x_start;
	int lin = coord->y_end - coord->y_start;
	if ((col != image->columns || lin != image->lines) && col != lin) {
		printf("The selection must be square\n");
		return;
	}

	// Finds the number of counterclockwise rotations needed
	int rotation = ((360 + angle) % 360) / 90;

	// Rotates the image according to the number of rotations
	for (int i = 0; i < rotation; i++) {
		col = coord->x_end - coord->x_start;
		lin = coord->y_end - coord->y_start;
		if (col == image->columns && lin == image->lines) {
			if (rotate_image(image, coord)) {
				printf("Rotation failed\n");
				return;
			}
		} else {
			if (rotate_section(image, coord)) {
				printf("Rotation failed\n");
				return;
			}
		}
	}

	// Print the success
	printf("Rotated %d\n", angle);
}

/// @brief Apply an effect to an image
/// @param image Pointer to the image that will be affected
/// @param crd Coordonate struct pointer
void apply(img_struct *image, sel_coord *crd)
{
	char setting[BUFFER];

	if (check_image(*image) || check_space())
		return;

	scanf("%s", setting);

	// Check if image is greyscale
	if (image->type % 3 != 0) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	double kernel[3][3];
	if (get_kernel(kernel, setting))
		return;

	// Creates a copy of the image
	img_struct copy; copy.type = image->type;
	copy.lines = crd->y_end - crd->y_start;
	copy.columns = crd->x_end - crd->x_start;
	copy.R = NULL; copy.B = NULL; copy.G = NULL;
	create_image(&copy);

	int lin = copy.lines; int col = copy.columns;

	// Applies the filter for the image
	for (int i = 0; i < lin; i++)
		for (int j = 0; j < col; j++) {
			// If on the edge of the image, copy the pixel value
			int check1 = (i == 0 && crd->y_start == 0);
			int check2 = (j == 0 && crd->x_start == 0);
			int check3 = (i == (lin - 1) && crd->y_end == image->lines);
			int check4 = (j == (col - 1) && crd->x_end == image->columns);

			if (check1 || check2 || check3 || check4) {
				copy.R[i][j] = image->R[crd->y_start + i][crd->x_start + j];
				copy.G[i][j] = image->G[crd->y_start + i][crd->x_start + j];
				copy.B[i][j] = image->B[crd->y_start + i][crd->x_start + j];
				continue;
			}

			double value_r = 0, value_b = 0, value_g = 0;
			int tmp_x, tmp_y;
			// Applies the kernel
			for (int y = 0; y <= 2; y++) {
				for (int x = 0; x <= 2; x++) {
					tmp_x = crd->x_start + j + x - 1;
					tmp_y = crd->y_start + i + y - 1;
					value_r += image->R[tmp_y][tmp_x] * kernel[y][x];
					value_g += image->G[tmp_y][tmp_x] * kernel[y][x];
					value_b += image->B[tmp_y][tmp_x] * kernel[y][x];
				}
			}

			// Copy the values into the image
			value_r = clamp((int)round(value_r), 0, MAX_PIXEL);
			copy.R[i][j] = value_r;
			value_g = clamp((int)round(value_g), 0, MAX_PIXEL);
			copy.G[i][j] = value_g;
			value_b = clamp((int)round(value_b), 0, MAX_PIXEL);
			copy.B[i][j] = value_b;
		}

	// Copy the modified copy into our image
	for (int i = 0; i < lin; i++)
		for (int j = 0; j < col; j++) {
			image->R[i + crd->y_start][j + crd->x_start] = copy.R[i][j];
			image->B[i + crd->y_start][j + crd->x_start] = copy.B[i][j];
			image->G[i + crd->y_start][j + crd->x_start] = copy.G[i][j];
		}

	delete_image(&copy);
	// Print the success to console
	printf("APPLY %s done\n", setting);
}

/// @brief Command used restrict other commands
/// @param image Image loaded in memory
/// @param coord Coordonate struct that will restrict the other commands
void selection(img_struct image, sel_coord *coord)
{
	int tmp[COORD_NUM]; // Temporary vector for code readability
	char setting[BUFFER];

	if (check_space())
		return;
	scanf("%s", setting);
	// Check if the command is SELECT ALL
	if (strcmp(setting, "ALL") == 0) {
		if (check_image(image))
			return;

		coord->x_start = 0; coord->x_end = image.columns;
		coord->y_start = 0; coord->y_end = image.lines;
		// Print the success
		printf("Selected ALL\n");
		return;
	}

	// Translate the coordonate into a number
	if (!is_number(setting)) {
		char clear[BUFFER]; scanf("%255[^\n]", clear);
		printf("Invalid command\n");
		return;
	}
	tmp[0] = atoi(setting);

	// Get the rest of the coordonates and check if they are numbers
	for (int i = 1; i < COORD_NUM; i++) {
		if (check_space())
			return;
		tmp[i] = get_number();
		if (tmp[i] == GET_NUMBER_RETURN_FAIL) {
			char clear[BUFFER]; scanf("%255[^\n]", clear);
			printf("Invalid command\n");
			return;
		}
	}

	if (check_image(image))
		return;

	// Check if the coordonates are inside the image
	for (int i = 0; i < COORD_NUM; i++) {
		if (i % 2 == 0) {
			if (tmp[i] < 0 || tmp[i] > image.columns) {
				printf("Invalid set of coordinates\n");
				return;
			}
		} else {
			if (tmp[i] < 0 || tmp[i] > image.lines) {
				printf("Invalid set of coordinates\n");
				return;
			}
		}
	}

	// Check if the coordonates are in the correct order (small -> big)
	if (tmp[0] > tmp[2])
		swap(&tmp[0], &tmp[2]);
	if (tmp[1] > tmp[3])
		swap(&tmp[1], &tmp[3]);

	// Check if the bigger coordonate is 0 0 or if coordonates are equal
	if (tmp[3] == 0 || tmp[2] == 0 || (tmp[0] == tmp[2] || tmp[1] == tmp[3])) {
		printf("Invalid set of coordinates\n");
		return;
	}

	// Translate between tmp and xy coordonates
	coord->x_start = tmp[0]; coord->x_end = tmp[2];
	coord->y_start = tmp[1]; coord->y_end = tmp[3];

	if (coord->x_start == coord->x_end && coord->y_start == coord->y_end) {
		printf("Invalid set of coordinates\n");
		return;
	}
	// Print the success
	printf("Selected %d %d ", coord->x_start, coord->y_start);
	printf("%d %d\n", coord->x_end, coord->y_end);
}

/// @brief Constructs a histogram of a black and white image
/// @param image The image loaded in memory
/// @param coord Coordonate struct used to restrict the command
void histogram(img_struct image, sel_coord coord)
{
	int max_stars, bins;

	// Check if the image exists
	if (check_image(image))
		return;

	if (check_space())
		return;

	// Try to get max_stars and bins and throw an error on failure
	max_stars = get_number();
	if (max_stars == GET_NUMBER_RETURN_FAIL) {
		char clear[BUFFER];
		scanf("%255[^\n]", clear);
		printf("Invalid command\n");
	}

	if (check_space())
		return;

	bins = get_number();
	if (bins == GET_NUMBER_RETURN_FAIL) {
		char clear[BUFFER];
		scanf("%255[^\n]", clear);
		printf("Invalid command\n");
	}

	// Check if to see if the command is complete
	if (check_nospace())
		return;

	// Check if the image is black and white
	if (check_bw(image))
		return;

	// Frequency vector and maximum frequency of this vector
	int freq[MAX_PIXEL + 1] = {0}, max_freq = 0;

	// Places every pixel in our image in a bin;
	for (int i = coord.y_start; i < coord.y_end; i++) {
		for (int j = coord.x_start; j < coord.x_end; j++) {
			int bin_size = (MAX_PIXEL + 1) / bins;
			freq[image.R[i][j] / bin_size]++;

			int update = freq[image.R[i][j] / bin_size];
			// Updates the maximum frequency if needed
			if (update > max_freq)
				max_freq = update;
		}
	}

	for (int i = 0; i < bins; i++) {
		// Calculates the number of stars that have to be shown
		int stars;
		stars = freq[i] * max_stars / max_freq;

		// Print the result
		printf("%d\t|\t", stars);
		for (int j = 0; j < stars; j++)
			printf("*");
		printf("\n");
	}
}
