// Gheorghe Andrei-Bogdan - 313CA
#include "definitions.h"

/// @brief Allocates memory for a matrix through a pointer
/// @param matrix Pointer to the matrix that will be allocated
/// @param nlines Number of lines that will be alocated
/// @param ncolumns Number of columns that will be allocated
/// @return Standard error codes
int create_matrix(int ***matrix, int nlines, int ncolumns)
{
	// Allocating memory for the first line of the matrix
	(*matrix) = (int **)calloc(nlines, sizeof(int *));
	if (!(*matrix))
		return 1;

	// Allocating memory for all the columns of the matrix
	for (int i = 0; i < nlines; i++) {
		(*matrix)[i] = (int *)calloc(ncolumns, sizeof(int));
		if (!(*matrix)[i])
			return 1;
	}

	return 0;
}

/// @brief Deallocates memory of a matrix
/// @param matrix Pointer to the matrix that will be deleted
/// @param size The number of lines the matrix has
void delete_matrix(int ***matrix, int lines)
{
	for (int i = 0; i < lines; i++)
		free((*matrix)[i]);
	free((*matrix));
	(*matrix) = NULL;
}

/// @brief Allocates memory for an image
/// @param image Pointer to the image that will be deleted
/// @return Standard error codes
int create_image(img_struct *image)
{
	int lin = image->lines;
	int col = image->columns;

	if (create_matrix(&image->R, lin, col))
		return 1;

	// Creates the additional matricies only if the image is colored
	if (image->type == 3 || image->type == 6) {
		if (create_matrix(&image->G, lin, col))
			return 1;

		if (create_matrix(&image->B, lin, col))
			return 1;
		return 0;
	}

	return 0;
}

/// @brief Deallocates all the memory associetated with an image
/// @param image Pointer to the image that will be deleted
void delete_image(img_struct *image)
{
	int lin = image->lines;
	image->type = 0;

	if (image->R)
		delete_matrix(&image->R, lin);

	if (image->G)
		delete_matrix(&image->G, lin);

	if (image->B)
		delete_matrix(&image->B, lin);
}
