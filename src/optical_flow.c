#include "optical_flow.h"
#include "matrix.h"

static double intensity(struct rgb rgb) {
	return 0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b;
}

static struct rgb at(struct bmp bmp, int x, int y) {
	return bmp.data[bmp.width * y + x];
}

static struct rgb minus(struct rgb lhs, struct rgb rhs) {
	return (struct rgb){
		.r = lhs.r - rhs.r,
		.g = lhs.g - rhs.g,
		.b = lhs.b - rhs.b,
	};
}

#define BOX_RADIUS 2
#define BOX_SIZE (2*(BOX_RADIUS)+1)

void optical_flow(struct bmp bmp) {
	uint32_t const width = bmp.width - BOX_SIZE;
	uint32_t const height = bmp.height - BOX_SIZE;

	uint32_t const equation_count = (BOX_SIZE - 1) * (BOX_SIZE - 1);

	Matrix A = matrix_make(equation_count, 2);

	int k = 0;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			struct rgb pp = at(bmp, i,   j);
			struct rgb px = at(bmp, i+1, j);
			struct rgb py = at(bmp, i,   j+1);

			double const Ix = intensity(minus(px, pp));
			double const Iy = intensity(minus(py, pp));

			matrix_set(A, k, 0, Ix);
			matrix_set(A, k, 1, Ix);

			k += 1;
		}
	}
}
