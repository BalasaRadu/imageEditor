# Image Editor - Project Overview

This project implements a simple command-line image editor in C that allows users to perform basic image processing tasks. The editor supports operations such as loading an image, applying transformations (like rotation and equalization), cropping, and saving the image back to a file. The image is represented using a custom structure, and all operations are performed directly on the image's pixel data.

## Features

- **Load Image**: Load an image from a file into memory.
- **Select Region**: Select a specific region within the image to apply operations.
- **Histogram**: Generate and display the histogram of the image.
- **Equalization**: Perform histogram equalization on the image.
- **Rotate**: Rotate the image or a selected region by 90 degrees clockwise.
- **Crop**: Crop a selected region of the image.
- **Apply Filters**: Apply filters or transformations to a selected region.
- **Save Image**: Save the edited image to a file in ASCII or binary format.
- **Memory Management**: Proper memory management for loading, editing, and freeing images.

## Available Commands
- **LOAD <filename>**: Load an image from the specified file.
- **SELECT ALL**: Select the entire image for operations.
- **SELECT <x1> <y1> <x2> <y2>**: Select a rectangular region of the image.
- **HISTOGRAM**: Display the histogram of the current image.
- **EQUALIZE**: Perform histogram equalization on the image.
- **ROTATE**: Rotate the image or the selected region by 90 degrees clockwise.
- **CROP**: Crop the selected region of the image.
- **APPLY <filter>**: Apply a filter or transformation to the selected region.
- **SAVE <filename> <format>**: Save the edited image to the specified file. The format can be "ASCII" or "binary".
- **EXIT**: Exit the program.
## Image Structure

The `image` structure contains the following fields:

- **magic_nr**: A 2-byte string representing the image type (e.g., "P3" for a color image in ASCII format).
- **width**: The width of the image in pixels.
- **height**: The height of the image in pixels.
- **nr_colors**: 0 for black-and-white images or 3 for color images.
- **maxval**: The maximum pixel intensity (typically 255).
- **data**: A 3D array representing the pixel data of the image.


## Files

- **main.c**: The main driver for the image editor, which handles the user input and delegates operations to various functions.
- **image.h**: The header file defining the `image` structure and function prototypes for operations like loading, saving, selecting, and transforming the image.

## Compilation and Execution

1. **Compile the project**:
   ```bash
   gcc main.c -o image_editor -lm
