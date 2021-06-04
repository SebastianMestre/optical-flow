// bmp.c
#include "bmp.h"

// THIS CODE WAS TAKEN FROM HERE:
// https://stackoverflow.com/a/2654860
// I DID NOT WRITE IT MYSELF

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void bmp_write (
	char const* fname,
	struct bmp bmp
) {
	uint32_t h = bmp.height;
	uint32_t w = bmp.width;

	FILE *f;
	unsigned char *img = NULL;
	int filesize = 54 + 3*w*h;  //w is your image width, h is image height, both int

	img = (unsigned char *)malloc(3*w*h);
	memset(img,0,3*w*h);

	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			int x = i;
			int y = (h-1)-j;
			uint8_t r = bmp.data[j*h+i].r;
			uint8_t g = bmp.data[j*h+i].g;
			uint8_t b = bmp.data[j*h+i].b;
			img[(x+y*w)*3+2] = (unsigned char)(r);
			img[(x+y*w)*3+1] = (unsigned char)(g);
			img[(x+y*w)*3+0] = (unsigned char)(b);
		}
	}

	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
	unsigned char bmppad[3] = {0,0,0};

	bmpfileheader[ 2] = (unsigned char)(filesize    );
	bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);

	bmpinfoheader[ 4] = (unsigned char)(w    );
	bmpinfoheader[ 5] = (unsigned char)(w>> 8);
	bmpinfoheader[ 6] = (unsigned char)(w>>16);
	bmpinfoheader[ 7] = (unsigned char)(w>>24);
	bmpinfoheader[ 8] = (unsigned char)(h    );
	bmpinfoheader[ 9] = (unsigned char)(h>> 8);
	bmpinfoheader[10] = (unsigned char)(h>>16);
	bmpinfoheader[11] = (unsigned char)(h>>24);

	f = fopen(fname,"wb");
	fwrite(bmpfileheader,1,14,f);
	fwrite(bmpinfoheader,1,40,f);
	for(int i=0; i<h; i++)
	{
		fwrite(img+(w*(h-i-1)*3),3,w,f);
		fwrite(bmppad,1,(4-(w*3)%4)%4,f);
	}

	free(img);
	fclose(f);
}

struct bmp bmp_read(FILE *f) {

	unsigned char fileheader[14] = {};
	unsigned char info[40] = {};

	fread(fileheader,1,14,f);
	fread(info,1,40,f);

	uint32_t size = ((uint32_t)info[0] << 0) |
	                ((uint32_t)info[1] << 8) |
	                ((uint32_t)info[2] << 16) |
	                ((uint32_t)info[3] << 24);

	uint32_t width = ((uint32_t)info[4] << 0) |
	                 ((uint32_t)info[5] << 8) |
	                 ((uint32_t)info[6] << 16) |
	                 ((uint32_t)info[7] << 24);

	uint32_t height = ((uint32_t)info[8] << 0) |
	                  ((uint32_t)info[9] << 8) |
	                  ((uint32_t)info[10] << 16) |
	                  ((uint32_t)info[11] << 24);

	struct rgb* result_data = malloc(width * height * sizeof(struct rgb));

	uint32_t data_size = size - 54; // file size minus header size
	uint8_t* data = malloc(data_size);

	fread(data, 1, data_size, f);

	int k = 0;
	int q = 0;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			result_data[k++] = (struct rgb){
				.r = data[q+2],
				.g = data[q+1],
				.b = data[q+0],
			};
			q += 3;
		}

		// round up to multiple of 4
		q = (q + (4-1)) & ~(4-1);
	}

	free(data);

	return (struct bmp){
		.width = width,
		.height = height,
		.data = result_data,
	};
}

struct bmp bmp_make(int width, int height) {
	size_t data_size = width * height * sizeof(struct rgb);
	struct rgb* data = malloc(data_size);
	memset(data, 0, data_size);
	return (struct bmp){ .width = width, .height = height, .data = data };
}
