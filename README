#### Gheorghe Andrei-Bogdan - 313CA

# Image editing program

## Introduction

This command line image editor program allows users to perform various
operations on images, such as loading, saving, cropping, rotating, and
applying filters.
The program supports both ASCII and binary image formats PGM and PPM and
provides a set of predefined filters for users to enhance their images.

## Table of contents

- [Introduction](#introduction)
- [Table of contents](#table-of-contents)
- [Settings](#settings)
  - [Program Settings](#program-settings)
  - [Filter Settings](#filter-settings)
- [Documentation](#documentation)
  - [Structures](#structures)
  - [Error Handling](#error-handling)
  - [Main](#main)
  - [Settings Functions](#setting-functions)

## Settings

### Program Settings:

LOAD <path>:                Load an image
SELECT <x1> <y1> <x2> <y2>: Select a region
SELECT ALL:                 Selects entire image
HISTOGRAM <stars> <bins>:   View histogram
EQUALIZE:                   Equalize image histogram
ROTATE <angle>:             Rotate an image or selected region
CROP:                       Crop an image
APPLY <filter>:             Apply a filter
SAVE <path> [ascii]:        Save the image
EXIT:                       Exit the program
SHOW [CONTENT]:             View the image

### Filter Settings:

EDGE:          Edge detection
BLUR:          Blur
SHARPEN:       Sharpen
GAUSSIAN_BLUR: 3x3 gaussian blur

## Documentation

### Structures

#### Image

Defined by a struct that holds the type, height, width and RGB values of the
image. Our image is spit into 3 images (matrices), a "red" image, that
has only the red value of a pixel, a "blue" image and a "green" image.

Grayscale images are stored using only the R value of the image.

#### Coordonates

Defined by a struct that stores 2 points of the selection.
(Upper left and lower right)

### Error handling

The order of errors:
Invalid image -> Invalid command -> Other errors

Order of checks:
1. In main, check if the image exists
2. Also in main, check the keyword of the command (e.g. LOAD, SAVE)
3. In the command function called, check for correct command syntax:
- spaces between keyword (or absence of space for commands like EQUALIZE)
- spaces between expected syntax blocks
- correct variables (if they are integers)
- check for expected end of the command

### Main

Reads the keyword from a command from the console.
The keyword is then translated to an integer using key_from_setting().
To translate between the commands and the integer, we use a dictionary.

When adding a new command one must:
1. Define the command keyword in the header with `#define <keyword> <value>`
2. Modify the dictionary in key_from_setting to include the new keyword and
value defined earlier
3. Modify the switch from main.c to include the new keyword
4. Create a function that reads the rest of the syntax of the command

### Settings funtions

#### LOAD

Loads an image stated by a path from the console.

The loading is done in 2 parts due to how the input is structured.
The metadata_read() function reads the first lines in the input,
the metadata, which is always in ascii.
The function then returns the position of the cursuor and closes the file,
letting us reopen the file either in ascii or in binary,
depending of the metadata read. The second part of the function reads the
pixels from the image.

#### SAVE

Writes the content of the image saved in memory into the specified file,
in the specified format (ascii or binary).

The saving is done in 2 parts due to how the output must be structured.
The metadata_save() function prints the first lines in the lines,
the metadata of hte image, which is always in ascii.
The image_save() function prints the actual image in the desired format.

#### SELECT

Lets the user select coordonates that future functions will effect.

The coordonate struct is initialized in main and is changed only
at the end of the SELECT (ALL) functions, meaning in the case of an error,
the previous coordonates are kept.

#### HISTOGRAM

Prints the histogram of an image.

We use a statically allocated frequency array,
and using it depending on the number of bins provided.
We then calculate the number of stars the user needs to be shown.

#### EQUALIZE

Does an image equalization.

The image equalization is done on the entire file, regardless of the selection.
As expected, we are using a frequency vector of the values in the image.

#### CROP

Crops the image according to the selection coordonates.

Copies the information we intend to store into a new image struct,
deletes the old image, and returns the address of the copied info.

#### ROTATE

Rotates the image or a square selection.

The rotation is done by 2 functions:
1. rotate_image() which rotates the entire image
2. rotate_section() which rotates the square selection

Multiples rotations (either clockwise or counterclockwise) are done
by repeatedly calling the 2 functions above.
The counterclockwise rotation (the one done by both functions) is
done by transposing and flipping the image.

#### APPLY

Applies a filter to the image.

Creates a new image with the size of the selection and the copies the
image into the new image. It then applies the kernel to this new image,
copies the image back into the original image and deletes the copy.
The function apply works regardless of what 3x3 kernel is provided.

The kernel is made by the function get_kernel(), which takes the
setting of the apply setting and returns the kernel.

#### EXIT

Deallocates all the resources that have been allocated in the program
and closes the program.

#### SHOW

Shows the metadata of the image.
If the command is SHOW CONTENT, it outputs the entire image to the console.
