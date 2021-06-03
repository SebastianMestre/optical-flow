// bmp.h
#pragma once

#include <stdint.h>
#include <stdio.h>

struct rgb {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

struct bmp {
	uint32_t width;
	uint32_t height;
	struct rgb* data;
};


void bmp_write (char const* fname, struct bmp);

struct bmp bmp_read(FILE* f);
