#pragma once

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#define EPSILON (1e-6)

bool is_zero(double x) {
	return -EPSILON < x && x < EPSILON;
}

typedef size_t dimension;
typedef size_t row;
typedef size_t column;

typedef struct {
	const dimension dim;
	double* const scalars;
} Vector;

typedef struct {
	const row rs;
	const column cs;
	double* const scalars;
} Matrix;

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

// NOTE: owns memory. Main utility is to use it as a type cast
Matrix as_row_matrix(Vector v) {
	return (Matrix){.rs = 1, .cs = v.dim, .scalars = v.scalars};
}

Matrix as_column_matrix(Vector v) {
	return (Matrix){.rs = v.dim, .cs = 1, .scalars = v.scalars};
}

Vector as_vector_from_row(Matrix m) {
	assert(m.rs == 1);
	return (Vector){.dim = m.cs, .scalars = m.scalars};
}

Vector as_vector_from_column(Matrix m) {
	assert(m.cs == 1);
	return (Vector){.dim = m.rs, .scalars = m.scalars};
}

Matrix matrix_make(row rs, column cs) {
	double* data = (double*)malloc(sizeof(*data) * rs * cs);
	assert(data);
	return (Matrix){.rs = rs, .cs = cs, .scalars = data};
}

Matrix matrix_from(double* data, row rs, column cs) {
	assert(data);
	return (Matrix){.rs = rs, .cs = cs, .scalars = data};
}

double matrix_get(Matrix m, row r, column c) {
	return m.scalars[r * m.cs + c];
}

double matrix_set(Matrix m, row r, column c, double s) {
	return m.scalars[r * m.cs + c] = s;
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

void row_swap(Matrix m, row i, row j) {
	if (i == j)
		return;
	for (column c = 0; c < m.cs; c++) {
		double aux = matrix_get(m, i, c);
		matrix_set(m, i, c, matrix_get(m, j, c));
		matrix_set(m, j, c, aux);
	}
}

void row_scale(Matrix m, row i, double s) {
	for (column c = 0; c < m.cs; c++)
		matrix_set(m, i, c, matrix_get(m, i, c) * s);
}

void row_add_multiple(Matrix m, row i, row j, double s) {
	if (s == 0)
		return;
	for (column c = 0; c < m.cs; c++)
		matrix_set(m, i, c, matrix_get(m, i, c) + matrix_get(m, j, c) * s);
}

// NOTE: grabs the one that is lowest, for speed
bool swap_for_pivot_row(Matrix m, row i, column j, Matrix v) {
	if (!is_zero(matrix_get(m, i, j)))
		return true;
	for (row pivot = m.rs - 1; pivot > i; pivot--) {
		if (!is_zero(matrix_get(m, pivot, j))) {
			row_swap(m, i, pivot);
			row_swap(v, i, pivot);
			return true;
		}
	}
	return false;
}

void reduce_column(Matrix m, row i, column j, Matrix v) {
	double pivot = matrix_get(m, i, j);
	assert(pivot);

	row_scale(m, i, 1/pivot);
	row_scale(v, i, 1/pivot);
	matrix_set(m, i, j, 1);

	for (row r = 0; r < m.rs; r++) {
		if (r == i)
			continue;
		row_add_multiple(m, r, i, -matrix_get(m, r, j));
		row_add_multiple(v, r, i, -matrix_get(m, r, j));
		matrix_set(m, r, j, 0);
	}
}

Vector matrix_solve(Matrix m, Vector v) {
	Matrix vm = as_column_matrix(v);

	bool* free_vars = (bool*)malloc(sizeof(*free_vars) * m.cs);
	for (size_t i = 0; i < m.cs; i++)
		free_vars[i] = false;

	size_t free_var_count = 0;

	// reduced row echelon form
	for (size_t rp = 0, cp = 0; rp < m.rs && cp < m.cs; cp++) {
		// failed with this column
		if (!swap_for_pivot_row(m, rp, cp, vm)) {
			free_vars[cp] = true;
			free_var_count += 1;
			continue;
		}

		reduce_column(m, rp, cp, vm);
		rp++;
	}

	Vector solution = vector_make(m.cs);
	for (column i = 0; i < solution.dim; i++)
		vector_set(solution, i, 0);

	size_t pivot_count = m.rs - free_var_count;
	for (size_t c = solution.dim, r = pivot_count - 1; c--;) {
		if (free_vars[c]) {
			vector_set(solution, c, 1);
			continue;
		}

		Vector row = vector_from(&m.scalars[r * m.cs], m.cs);
		double sol = matrix_get(vm, r, 0) - vector_inner_product(row, solution);
		vector_set(solution, c, sol);
		r--;
	}

	free(free_vars);
	return solution;
}

void matrix_free(Matrix m) {
	free(m.scalars);
}
