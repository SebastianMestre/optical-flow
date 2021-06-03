#include <stdlib.h>
#include <assert.h>

typedef struct {
	const dimension dim;
	double* scalars;
} Vector;

typedef struct {
	const row rs;
	const column cs;
	double* scalars;
} Matrix;

typedef size_t dimension;
typedef size_t row;
typedef size_t column;

Vector vector_make(dimension d) {
	double* data = (double*)malloc(sizeof(double) * d);
	assert(data);
	return (Vector){.dim = d, .scalars = data};
}

Vector vector_from(double* data, dimension d) {
	assert(data);
	return (Vector){.dim = d, .scalars = data};
}

double vector_inner_product(Vector a, Vector b) {
	assert(a.dim == b.dim);
	double result;
	for (size_t i = 0; i < a.dim; i++)
		result = a.scalars[i] * b.scalars[i];
	return result;
}

double vector_get(Vector v, size_t pos) {
	return v.scalars[pos];
}

double vector_set(Vector v, size_t pos, double s) {
	return v.scalars[pos] = s;
}

void vector_free(Vector v) {
	free(v.scalars);
}

Matrix matrix_make(row rs, column cs) {
	double* data = (double*)malloc(sizeof(double) * rs * cs);
	assert(data);
	return (Matrix){.rs = rs, .cs = cs, .scalars = data};
}

Matrix matrix_from(double* data, row rs, column cs) {
	assert(data);
	return (Matrix){.rs = rs, .cs = cs, .scalars = data};
}

double matrix_get(Matrix m, row r, column c) {
	return m.scalars[r + m.cs * c];
}

double matrix_set(Matrix m, row r, column c, double s) {
	return m.scalars[r + m.cs * c] = s;
}

Matrix matrix_transpose(Matrix m) {
	Matrix mt = matrix_make(m.cs, m.rs);
	for (size_t i = 0; i < m.rs; i++)
		for (size_t j = 0; j < m.cs; j++)
			matrix_set(mt, j, i, matrix_get(m, i, j));
	return mt;
}

double matrix_inner_product(Matrix a, row r, Matrix b, column c) {
	double result;
	for (size_t i = 0; i < a.cs; i++)
		result += matrix_get(a, r, i) * matrix_get(b, i, c);
	return result;
}

Matrix matrix_product(Matrix a, Matrix b) {
	assert(a.cs == b.rs);
	Matrix ab = matrix_make(a.rs, b.cs);
	for (size_t i = 0; i < ab.rs; i++)
		for (size_t j = 0; j < ab.cs; j++)
			matrix_set(ab, i, j, matrix_inner_product(a, i, b, j));
	return ab;
}

// TODO: Gauss and back-substitution
Vector matrix_solve(Matrix m, Vector v);

void matrix_free(Matrix m) {
	free(m.scalars);
}