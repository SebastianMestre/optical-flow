// bmp.h
#pragma once

#include <stdint.h>
#include <stdio.h>
#include "rgb.h"

struct bmp {
	uint32_t width;
	uint32_t height;
	struct rgb* data;
};

void bmp_write (char const* fname, struct bmp);

struct bmp bmp_read(FILE* f);

struct bmp bmp_make(int width, int height);
