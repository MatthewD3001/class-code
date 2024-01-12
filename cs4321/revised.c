#include <stdio.h>

int main(){
	int sum[8][8];
	int max = -100;
	int maxStart = -1;
	int maxEnd = -1;
	int arr[] = {2,3,-5,1,-7,-5,3,7};
	for(int i = 0; i < 8; i++){
		sum[i][i] = arr[i];
		if(sum[i][i] > max){
			max = sum[i][i];
			maxStart = i;
			maxEnd = i;
		}
		for(int j = i + 1; j < 8; j++){
			sum[i][j] = sum[i][j-1] + arr[j];
			if(sum[i][j] > max){
				max = sum[i][j];
				maxStart = i;
				maxEnd = j;
			}
		}
	}
	printf("The maximum sum is: %d, this was obtained from the sub array starting at: %d and ending at %d\n", max, maxStart, maxEnd);
	// complexity of n^2
}
