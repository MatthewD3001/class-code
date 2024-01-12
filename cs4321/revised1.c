#include <stdio.h>

int main(){
	int curr;
	int max = -100;
	int maxStart = -1;
	int maxEnd = -1;
	int arr[] = {2,3,-5,1,-7,-5,3,7};
	for(int i = 0; i < 8; i++){
		curr = arr[i];
		if(curr > max){
			max = curr;
			maxStart = i;
			maxEnd = i;
		}
		for(int j = i + 1; j < 8; j++){
			curr += arr[j];
			if(curr > max){
				max = curr;
				maxStart = i;
				maxEnd = j;
			}
		}
	}
	printf("The maximum sum is: %d, this was obtained from the sub array starting at: %d and ending at %d\n", max, maxStart, maxEnd);
	// complexity of n^2
}
