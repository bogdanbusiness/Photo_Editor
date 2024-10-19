// Gheorghe Andrei-Bogdan - 313CA
#include "definitions.h"

/// @brief Translates from a program setting to a value
/// @param key String of the program setting
/// @return Value associated with the program setting
int key_from_setting(char *setting)
{
	// Dictionary with all program settings
	dict dictionary[] = {
		{"LOAD", LOAD},
		{"SELECT", SELECT},
		{"HISTOGRAM", HISTOGRAM},
		{"EQUALIZE", EQUALIZE},
		{"ROTATE", ROTATE},
		{"CROP", CROP},
		{"APPLY", APPLY},
		{"SAVE", SAVE},
		{"EXIT", EXIT},
		{"SHOW", SHOW}
	};

	int nkeys = sizeof(dictionary) / sizeof(dict);

	// Transforms our setting into a value and returns it
	for (int i = 0; i < nkeys; i++) {
		if (strcmp(dictionary[i].setting, setting) == 0)
			return dictionary[i].key;
	}

	return -1;
}

/// @brief Translates from an apply setting to kernel
/// @param ker The output kernel
/// @param setting Apply setting string
/// @return Standard error returns
int get_kernel(double ker[3][3], char *setting)
{
	int key = 0;

	// Dictionary with all apply settings
	dict dictionary[] = {
		{"EDGE", EDGE},
		{"BLUR", BLUR},
		{"SHARPEN", SHARPEN},
		{"GAUSSIAN_BLUR", GAUSSIAN_BLUR}
	};

	int nkeys = sizeof(dictionary) / sizeof(dict);

	// Transforms our setting into a value and returns it
	for (int i = 0; i < nkeys; i++) {
		if (strcmp(dictionary[i].setting, setting) == 0)
			key = dictionary[i].key;
	}

	double edge_ker[KER_SIZE][KER_SIZE] = { {-1, -1, -1},
						{-1, 8, -1},
						{-1, -1, -1} };
	double blur_ker[KER_SIZE][KER_SIZE] = { {1. / 9, 1. / 9, 1. / 9},
						{1. / 9, 1. / 9, 1. / 9},
						{1. / 9, 1. / 9, 1. / 9} };
	double sharpen_ker[KER_SIZE][KER_SIZE] = {{0, -1, 0},
						  {-1, 5, -1},
						  {0, -1, 0} };
	double gauss_ker[KER_SIZE][KER_SIZE] = {{1. / 16, 1. / 8, 1. / 16},
						{1. / 8, 1. / 4, 1. / 8},
						{1. / 16, 1. / 8, 1. / 16}};
	// Get the kernel used from the setting
	switch (key) {
	case EDGE:
		for (int i = 0; i < KER_SIZE; i++)
			for (int j = 0; j < KER_SIZE; j++)
				ker[i][j] = edge_ker[i][j];
		break;
	case BLUR:
		for (int i = 0; i < KER_SIZE; i++)
			for (int j = 0; j < KER_SIZE; j++)
				ker[i][j] = blur_ker[i][j];
		break;
	case SHARPEN:
		for (int i = 0; i < KER_SIZE; i++)
			for (int j = 0; j < KER_SIZE; j++)
				ker[i][j] = sharpen_ker[i][j];
		break;
	case GAUSSIAN_BLUR:
		for (int i = 0; i < KER_SIZE; i++)
			for (int j = 0; j < KER_SIZE; j++)
				ker[i][j] = gauss_ker[i][j];
		break;
	default:
		printf("APPLY parameter invalid\n");
		return 1;
	}

	return 0;
}

/// @brief Rotates the entire image counterclockwise
/// @param image The image is modified through this pointer
/// @param coord The coordonates are modified so they match the rotated image
/// @return 0 on success, 1 on failure
int rotate_image(img_struct *image, sel_coord *coord)
{
	img_struct rotated; // Create a new rotated image
	rotated.R = NULL;
	rotated.B = NULL;
	rotated.G = NULL;

	rotated.type = image->type;
	rotated.columns = image->lines;
	rotated.lines = image->columns;
	if (create_image(&rotated)) {
		delete_image(&rotated);
		return 1;
	}

	for (int i = 0; i < rotated.lines; i++)
		for (int j = 0; j < rotated.columns; j++) {
			rotated.R[i][j] = image->R[rotated.columns - j - 1][i];
			if (rotated.type % 3 == 0) {
				rotated.G[i][j] = image->G[rotated.columns - j - 1][i];
				rotated.B[i][j] = image->B[rotated.columns - j - 1][i];
			}
		}

	// Delete the old image and copy the rotation
	delete_image(image);
	image->type = rotated.type;
	image->columns = rotated.columns;
	image->lines = rotated.lines;
	image->R = rotated.R;
	image->B = rotated.B;
	image->G = rotated.G;

	// Calibrate coordonate struct
	coord->x_start = 0;
	coord->x_end = image->columns;
	coord->y_start = 0;
	coord->y_end = image->lines;

	return 0;
}

/// @brief Rotates counterclockwise a square section from an image
/// @param img Pointer to the image affected
/// @param crd Coordonate struct of the zone affected
/// @return 0 on success, 1 on failure
int rotate_section(img_struct *img, sel_coord *crd)
{
	img_struct rotated; // Create a new rotated image for the section
	rotated.R = NULL;
	rotated.B = NULL;
	rotated.G = NULL;

	rotated.type = img->type;
	rotated.columns = crd->y_end - crd->y_start;
	rotated.lines = crd->x_end - crd->x_start;
	if (create_image(&rotated)) {
		delete_image(&rotated);
		return 1;
	}

	// Copy and rotate the section
	for (int i = 0; i < rotated.lines; i++)
		for (int j = 0; j < rotated.columns; j++) {
			rotated.R[i][j] = img->R[crd->y_end - j - 1][i + crd->x_start];
			if (img->type % 3 == 0) {
				rotated.G[i][j] = img->G[crd->y_end - j - 1][i + crd->x_start];
				rotated.B[i][j] = img->B[crd->y_end - j - 1][i + crd->x_start];
			}
		}

	// Write back into image the rotated section
	for (int i = 0; i < rotated.lines; i++)
		for (int j = 0; j < rotated.columns; j++) {
			img->R[i + crd->y_start][j + crd->x_start] = rotated.R[i][j];
			if (img->type % 3 == 0) {
				img->G[i + crd->y_start][j + crd->x_start] = rotated.G[i][j];
				img->B[i + crd->y_start][j + crd->x_start] = rotated.B[i][j];
			}
		}

	delete_image(&rotated);
	return 0;
}

/// @brief Gets a string from console and checks if its a number
/// @return Returns the number or GET_NUMBER_RETURN_FAIL on failure
int get_number(void)
{
	char string[BUFFER];

	scanf("%s", string);
	if (!is_number(string))
		return GET_NUMBER_RETURN_FAIL;

	return atoi(string);
}

/// @brief Clamps a number between min and max inclusive
/// @param number Number that will be clamped
/// @param min Minimum number
/// @param max Maximum number
/// @return The number itself, min or max
int clamp(int number, int min, int max)
{
	if (number > max)
		return max;
	else if (number < min)
		return min;
	return number;
}

/// @brief Swaps the values between the 2 variables
/// @param number1 First number
/// @param number2 Second number
void swap(int *number1, int *number2)
{
	int tmp = *number2;
	*number2 = *number1;
	*number1 = tmp;
}

// ERROR FUNCTIONS

/// @brief Checks if the image exists; prints error message to console
/// @param image The image loaded in memory
/// @return Standard error returns
int check_image(img_struct image)
{
	char clear[BUFFER];
	if (image.type == 0) {
		printf("No image loaded\n");
		scanf("%255[^\n]", clear);
		return 1;
	}
	return 0;
}

/// @brief Checks if the image is black/white; prints error message to console
/// @param image The image loaded in memory
/// @return Standard exit codes
int check_bw(img_struct image)
{
	if (image.type % 3 == 0) {
		printf("Black and white image needed\n");
		return 1;
	}
	return 0;
}

/// @brief Check if the string is a number
/// @param string String to check
/// @return If yes, returns 1, if no, returns 0
int is_number(char *string)
{
	for (unsigned long i = 0; i < strlen(string); i++)
		if ((string[i] < '0' || string[i] > '9') && (string[i] != '-'))
			return 0;
	return 1;
}

/// @brief Check for absence of a space
/// @return Standard exit codes
int check_nospace(void)
{
	char clear[BUFFER];
	if ((char)getchar() == ' ') {
		printf("Invalid command\n");
		scanf("%255[^\n]", clear);
		return 1;
	}
	return 0;
}

/// @brief Checks for existance of a space
/// @return Standard exit codes
int check_space(void)
{
	if ((char)getchar() != ' ') {
		printf("Invalid command\n");
		return 1;
	}
	return 0;
}

// Show command

/// @brief Shows the metadata (and content) of the image loaded in memory
/// @param image The image loaded in memory
void show(img_struct image)
{
	if (check_image(image))
		return;

	// Check if there is anything left in the command
	char setting[BUFFER] = "";
	if ((char)getchar() == ' ')
		scanf("%s", setting);

	// Show the type, size and max pixel value
	printf("Type of image: P%d\n", image.type);
	printf("Size: %d %d\n", image.columns, image.lines);
	if (image.type != 1 || image.type != 4)
		printf("Max pixel value: %d\n", MAX_PIXEL);

	// Show the image only if "CONTENT" part of the command exists
	if (strcmp(setting, "CONTENT") != 0)
		return;

	// Show contents
	for (int i = 0; i < image.lines; i++) {
		for (int j = 0; j < image.columns; j++)
			printf("%x ", image.R[i][j]);
		printf("\n");
	}

	if (image.type % 3 == 0) {
		for (int i = 0; i < image.lines; i++) {
			for (int j = 0; j < image.columns; j++)
				printf("%x ", image.G[i][j]);
			printf("\n");
		}
		for (int i = 0; i < image.lines; i++) {
			for (int j = 0; j < image.columns; j++)
				printf("%x ", image.B[i][j]);
			printf("\n");
		}
	}
}
