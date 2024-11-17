#include <stdio.h>

int main() {
	int arr[10] = { 1,2,3,4,5,6,7,8,9,10 };
	int* start_ptr = arr;
	int* end_ptr = &arr[10];
	while(start_ptr != end_ptr) {
		printf("%d ", *start_ptr);
		start_ptr++;
	}
	printf("\n");
	return 0;
}