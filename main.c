// Gheorghe Andrei-Bogdan - 313CA
#include "definitions.h"

int main(void)
{
	img_struct image;
	sel_coord  coord;
	char setting[BUFFER];

	// Initializing the image and coordonates structures
	image.type = 0;
	image.R = NULL; image.B = NULL; image.G = NULL;
	coord.x_start = 0; coord.x_end = 0;
	coord.y_start = 0; coord.y_end = 0;

	// Main loop of the program
	while (1) {
		scanf("%s", setting);
		switch (key_from_setting(setting)) {
		case LOAD:
			load(&image, &coord);
			break;
		case SELECT: // Does both SELECT and SELECT ALL
			selection(image, &coord);
			break;
		case HISTOGRAM:
			histogram(image, coord);
			break;
		case EQUALIZE:
			equalize(&image);
			break;
		case ROTATE:
			rotate(&image, &coord);
			break;
		case CROP:
			crop(&image, &coord);
			break;
		case APPLY:
			apply(&image, &coord);
			break;
		case SAVE:
			save(image);
			break;
		case EXIT:
			// Check for anything EXIT
			if (check_nospace())
				break;
			if (image.type != 0)
				delete_image(&image);
			else
				printf("No image loaded\n");
			return 0;
		case SHOW: // Case for debugging
			show(image);
			break;
		default:
			// Get the rest of the line
			scanf("%255[^\n]", setting);
			printf("Invalid command\n");
		}
	}
}
