#include <stdio.h>
#include <stdlib.h>

typedef struct _box_t
{
	int id;

	int dimensions;
	int dimension[10];

	struct _box_t* fits_in[30];
	int fits_in_length;

	struct _box_t* fits_out[30];
	int fits_out_length;
} box_t;

int box_print(box_t* box)
{
	int i;

	printf("id: %d\n", box->id);
/*	printf("dimensions: %d\n", box->dimensions); */

/*	printf("dimension = [ "); */
/*	for (i = 0; i < box->dimensions; i++) { */
/*		printf("%d ", box->dimension[i]); */
/*	} */
/*	printf("]\n"); */

	printf("fits in = [ ");
	for (i = 0; i < box->fits_in_length; i++) {
		printf("%d ", box->fits_in[i]->id);
	}
	printf("]\n");

/*	printf("fits out = [ "); */
/*	for (i = 0; i < box->fits_out_length; i++) { */
/*		printf("%d ", box->fits_out[i]->id); */
/*	} */
/*	printf("]\n"); */

	return 0;
}

int box_init(box_t* box, int id, int* dimension, int dimensions)
{
	int i;

	box->id = id;
	box->dimensions = dimensions;

	for (i = 0; i < dimensions; i++) {
		box->dimension[i] = dimension[i];
	}

	for (i = 0; i < 30; i++) {
		box->fits_in[i] = NULL;
		box->fits_out[i] = NULL;
	}

	box->fits_in_length = 0;
	box->fits_out_length = 0;

	return 0;
}

int box_append_fits_in(box_t* box, box_t* other_box)
{
	box->fits_in[box->fits_in_length++] = other_box;

	return 0;
}

int box_append_fits_out(box_t* box, box_t* other_box)
{
	box->fits_out[box->fits_out_length++] = other_box;

	return 0;
}

int intcmp(const void* arg_a, const void* arg_b)
{
	return *((int*) arg_a) - *((int*) arg_b);
}

/* no one fits each other, -1 if a < b, or 1 if a > b. */
/* size vector must be ordered */
int boxcmp(box_t* a, box_t* b)
{
	int dimensions = a->dimensions;
	int res = 0;
	int k;

	if (a->dimension[0] < b->dimension[0]) {
		res = -1;
	}
	else if (a->dimension[0] > b->dimension[0]) {
		res = 1;
	}
	else {
		return 0;
	}

	for (k = 1; k < dimensions; k++) {
		if (a->dimension[k] == b->dimension[k])
			return 0;

		if ((a->dimension[k] < b->dimension[k]) && (res != -1))
			return 0;

		if ((a->dimension[k] > b->dimension[k]) && (res != 1))
			return 0;
	}

	return res;
}

int do_magic(box_t* curr_box, int* result, int* result_length, int depth)
{
	int i;
	int write;

	if ((curr_box->fits_in_length == 0) && (depth > *result_length)) {
		*result_length = depth;
		write = 1;
	}
	else {
		write = 0;
	}

	for (i = 0; i < curr_box->fits_in_length; i++) {
		write |= do_magic(curr_box->fits_in[i], result, result_length, depth + 1);
	}

	if (write) {
		result[depth-1] = curr_box->id;
	}

	return write;
}

int print_result(int* result, int length)
{
	int i;

	printf("%d\n", length);
	for (i = 0; i < length - 1; i++) {
		printf("%d ", result[i]);
	}
	printf("%d\n", result[length-1]);

	return 0;
}

int main(int argc, char** argv)
{
	char input[128];
	int boxes;
	int dimensions;
	int dimension[10];
	box_t box[30];
	int i, j;
	int result[30];
	int result_length;

	while (fgets(input, 128, stdin)) {
		sscanf(input, "%d %d", &boxes, &dimensions);

		result_length = 0;
		for (i = 0; i < 30; i++) {
			result[i] = 0;
		}

		for (i = 0; i < boxes; i++) {
			fgets(input, 128, stdin);
			sscanf(input, "%d %d %d %d %d %d %d %d %d %d", dimension+0, dimension+1, dimension+2, dimension+3, dimension+4, dimension+5, dimension+6, dimension+7, dimension+8, dimension+9);

			qsort(dimension, dimensions, sizeof(int), intcmp);

			box_init(&box[i], i + 1, dimension, dimensions);
		}

		for (i = 0; i < boxes; i++) {
			for (j = i + 1; j < boxes; j++) {
				if (boxcmp(&box[i], &box[j]) < 0) {
					box_append_fits_in(&box[i], &box[j]);
					box_append_fits_out(&box[j], &box[i]);
				}
				else if (boxcmp(&box[i], &box[j]) > 0) {
					box_append_fits_in(&box[j], &box[i]);
					box_append_fits_out(&box[i], &box[j]);
				}
			}
		}

		for (i = 0; i < boxes; i++) {
			do_magic(&box[i], result, &result_length, 1);
		}

		print_result(result, result_length);
	}

	return EXIT_SUCCESS;
}

