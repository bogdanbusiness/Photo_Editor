// Gheorghe Andrei-Bogdan - 313CA
#include "definitions.h"

/// @brief  Reads the metadata of a file
/// @param image Image struct pointer to store metadata
/// @param path Path to the file that will be opened
/// @return Return position at the end of the function; -1 on any failure
int metadata_read(img_struct *image, char *path)
{
	char read[BUFFER];

	// Opens the file
	FILE *source;
	source = fopen(path, "r");
	if (!source)
		return -1;

	// For code readability, we store values in a temporary vector
	int tmp[3];
	for (int i = 0; i < 3; i++) {
		while (1) {
			// Scans first element in the line
			fscanf(source, "%s", read);
			// Check if line is a comment
			if (read[0] != '#') {
				if (i == 0)
					tmp[0] = atoi(&read[1]);
				else
					tmp[i] = atoi(read);
				break;
			}
			// If line is a comment, read the whole line and try again
			fscanf(source, "%255[^\n]", read);
		}
	}

	// Check for color and read the max pixel value (if it exists)
	if (tmp[0] != 1 && tmp[0] != 4) {
		while (1) {
			fscanf(source, "%s", read);
			if (read[0] != '#')
				break;

			fscanf(source, "%255[^\n]", read);
		}
	}

	image->type = tmp[0];
	image->columns = tmp[1];
	image->lines = tmp[2];

	// The current position + the newline byte
	int position = ftell(source) + 1;
	fclose(source);
	return position;
}

/// @brief Reads the data of the image from a ascii or binary file
/// @param image Image that will be loaded
/// @param path Path to the file that will be opened
/// @param pos Position from where the reading will start
/// @param read Function pointer to the read function
/// @return Standard error codes
int image_read(img_struct *img, char *path, int pos, u_ch(*read)(FILE *src))
{
	FILE *source;

	// Open the file to ascii read if type is P1, P2 or P3
	if (img->type - 3 <= 0)
		source = fopen(path, "r");
	else
		source = fopen(path, "rb");
	if (!source)
		return 1;

	// Start from where we closed previously
	fseek(source, pos, SEEK_SET);

	// For files of type P1, P2, P4 or P5
	if (img->type % 3 != 0) {
		for (int i = 0; i < img->lines; i++)
			for (int j = 0; j < img->columns; j++)
				img->R[i][j] = read(source);
	} else {
		for (int i = 0; i < img->lines; i++)
			for (int j = 0; j < img->columns; j++) {
				img->R[i][j] = read(source);
				img->G[i][j] = read(source);
				img->B[i][j] = read(source);
			}
	}

	fclose(source);
	return 0;
}

/// @brief Saves the metadata to a file
/// @param image Image that will be saved
/// @param path Path to the file that will be opened
/// @param is_bin Boolean for the type of file we are saving to
/// @return Return position at the end of file; returns -1 on failure
int metadata_save(img_struct image, char *path, int is_bin)
{
	FILE *source;
	source = fopen(path, "w");
	if (!source)
		return -1;

	// Saves the image type depending on the save type and the stored type
	int ty = image.type;
	if (is_bin == 1 && (ty - 3) <= 0)
		fprintf(source, "P%d\n", ty + 3);
	else if (is_bin == 0 && (ty - 3) > 0)
		fprintf(source, "P%d\n", ty - 3);
	else
		fprintf(source, "P%d\n", ty);

	// Saves the columns and lines
	fprintf(source, "%d %d\n", image.columns, image.lines);

	// Saves the max pixel brightness (if it exists)
	if (ty != 1 && ty != 4)
		fprintf(source, "%d\n", MAX_PIXEL);

	int position = ftell(source);
	fclose(source);
	return position;
}

/// @brief Saves the data of the image in a ascii or binary file
/// @param image Image that will be saved
/// @param path Path to the file that will be opened
/// @param is_bin Boolean for the type of file we are saving to
/// @return Standard error codes
int image_save(img_struct img, char *path, int poz, int is_bin)
{
	FILE *source;

	// Open in the appropriate format
	if (is_bin)
		source = fopen(path, "rb+");
	else
		source = fopen(path, "r+");
	if (!source)
		return 1;

	// Start from where we last closed
	fseek(source, poz, SEEK_SET);

	// Write to the file
	if (is_bin)
		write_binary(img, source);
	else
		write_ascii(img, source);

	fclose(source);
	return 0;
}

/// @brief Reads the set of characters from a file
/// @param source File pointer to where we are reading
/// @return Returns the characters transformed into an int
u_ch read_ascii(FILE *source)
{
	char buf[PIXEL_CHAR];
	fscanf(source, " %s", buf);
	return (u_ch)atoi(buf);
}

/// @brief Reads one byte from a file
/// @param source File pointer to where we are reading
/// @return Returns the byte read
u_ch read_binary(FILE *source)
{
	u_ch buf;
	fread(&buf, sizeof(u_ch), 1, source);
	return buf;
}

/// @brief Writes the entire image in ascii
/// @param image Image struct that will be written
/// @param source File pointer to where we are reading
void write_ascii(img_struct image, FILE *source)
{
	for (int i = 0; i < image.lines; i++) {
		for (int j = 0; j < image.columns; j++) {
			if (image.type % 3 == 0) {
				fprintf(source, "%u ", image.R[i][j]);
				fprintf(source, "%u ", image.G[i][j]);
				fprintf(source, "%u ", image.B[i][j]);
			} else {
				fprintf(source, "%u ", image.R[i][j]);
			}
		}
		fprintf(source, "\n");
	}
}

/// @brief Writes the entire image in binary
/// @param image Image struct that will be written
/// @param source File pointer to where we are reading
void write_binary(img_struct image, FILE *source)
{
	for (int i = 0; i < image.lines; i++) {
		for (int j = 0; j < image.columns; j++) {
			if (image.type % 3 == 0) {
				fwrite(&image.R[i][j], sizeof(unsigned char), 1, source);
				fwrite(&image.G[i][j], sizeof(unsigned char), 1, source);
				fwrite(&image.B[i][j], sizeof(unsigned char), 1, source);
			} else {
				fwrite(&image.R[i][j], sizeof(unsigned char), 1, source);
			}
		}
	}
}
