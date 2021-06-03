#include "optical_flow.h"
#include "matrix.h"

#include <assert.h>

static double intensity(struct rgb rgb) {
	return (0.299 / 255) * rgb.r + (0.587 / 255) * rgb.g + (0.114 / 255) * rgb.b;
}

static struct rgb at(struct bmp bmp, int x, int y) {
	return bmp.data[bmp.width * y + x];
}

#define BOX_RADIUS 2
#define BOX_SIZE (2*(BOX_RADIUS)+1)

void optical_flow(struct bmp frame, struct bmp next_frame) {
	assert(frame.width == next_frame.width);
	assert(frame.height == next_frame.height);

	uint32_t const width = frame.width - BOX_SIZE;
	uint32_t const height = frame.height - BOX_SIZE;

	uint32_t const equation_count = (BOX_SIZE - 1) * (BOX_SIZE - 1);

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {

			Matrix A = matrix_make(equation_count, 2);
			Matrix b = matrix_make(equation_count, 1);

			int k = 0;
			for (int ii = 0; ii < BOX_SIZE-1; ++ii) {
				for(int jj = 0; jj < BOX_SIZE-1; ++jj) {
					double const fp = intensity(at(frame,      i+ii,   j+jj));
					double const fx = intensity(at(frame,      i+ii+1, j+jj));
					double const fy = intensity(at(frame,      i+ii,   j+jj+1));
					double const ft = intensity(at(next_frame, i+ii,   j+jj));

					double const Ix = fx - fp;
					double const Iy = fy - fp;
					double const It = ft - fp;

					matrix_set(A, k, 0,  Ix);
					matrix_set(A, k, 1,  Iy);
					matrix_set(b, k, 0, -It);

					k += 1;
				}
			}

			Matrix At = matrix_transpose(A);
			Matrix AtA = matrix_product(At, A);

			Matrix Atb = matrix_product(At, b);

			// solve(AtA * x = Atb);
		}
	}
}
