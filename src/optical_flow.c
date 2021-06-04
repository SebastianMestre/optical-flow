#include "optical_flow.h"
#include "matrix.h"

#include <assert.h>
#include <math.h>

static double intensity(struct rgb rgb) {
	return (0.299 / 255) * rgb.r + (0.587 / 255) * rgb.g + (0.114 / 255) * rgb.b;
}

static struct rgb at(struct bmp bmp, int x, int y) {
	return bmp.data[bmp.width * y + x];
}

static double clamp255(double x) {
	return x < 0 ? 0 : (x > 255 ? 255 : x);
}

#define BOX_RADIUS 8
#define BOX_SIZE (2*(BOX_RADIUS)+1)

struct bmp optical_flow(struct bmp frame, struct bmp next_frame) {
	assert(frame.width == next_frame.width);
	assert(frame.height == next_frame.height);

	uint32_t const width = frame.width - BOX_SIZE;
	uint32_t const height = frame.height - BOX_SIZE;

	struct bmp result = bmp_make(width, height);

	uint32_t const equation_count = BOX_SIZE * BOX_SIZE;

	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {

			Matrix A = matrix_make(equation_count, 2);
			Matrix b = matrix_make(equation_count, 1);

			int k = 0;
			for (int ii = 0; ii < BOX_SIZE; ++ii) {
				for(int jj = 0; jj < BOX_SIZE; ++jj) {

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

			// V -- optical flow vector (2 component)
			Vector V = matrix_solve(AtA, as_vector_from_column(Atb));
			assert(V.dim == 2);

			{
				double Vx = vector_get(V, 0);
				double Vy = vector_get(V, 1);
				double Vlen = sqrt(Vx * Vx + Vy * Vy);

#if 0
				result.data[j * width + i] = (struct rgb){
					.r = clamp255(Vlen),
					.g = clamp255(Vlen),
					.b = clamp255(Vlen),
				};
#else
				result.data[j * width + i] = (struct rgb){
					.r = clamp255(127 + Vx),
					.g = clamp255(127 + Vy),
					.b = clamp255(Vlen),
				};
#endif
			}

			vector_free(V);

			matrix_free(Atb);
			matrix_free(AtA);
			matrix_free(At);
			matrix_free(b);
			matrix_free(A);
		}
	}

	return result;
}
