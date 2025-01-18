#ifndef BARCODE_UTILS_H
#define BARCODE_UTILS_H

#include <stdbool.h>

bool validate_identifier(const char *identifier);
bool file_exists(const char *filename);
bool generate_barcode_image(const char *identifier, int lateral_spacing, int pixels_per_area, int height, const char *filename);
bool is_valid_pbm(const char *filename);
bool extract_identifier(const char *filename, char *identifier);

#endif