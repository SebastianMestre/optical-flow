#include <stdio.h>
#include <stdlib.h>

#include "optical_flow.h"

int main() {
	FILE* f1 = fopen("img/soft1.bmp", "r");
	FILE* f2 = fopen("img/soft2.bmp", "r");

	struct bmp in1 = bmp_read(f1);
	struct bmp in2 = bmp_read(f2);

	fclose(f1);
	fclose(f2);

	struct bmp flow = optical_flow(in1, in2);

	bmp_write("result.bmp", flow);

	free(in1.data);
	free(in2.data);
	free(flow.data);
}
