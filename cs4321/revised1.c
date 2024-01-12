#include <stdio.h>

int main(){
	int curr;
	int max = -100;
	int maxStart = -1;
	int maxEnd = -1;
	int arr[] = {2,3,-5,1,-7,-5,3,7};
	for(int i = 0; i < 8; i++){
		curr = arr[i];
		for(int j = i; j < 8; j++){			
			if(curr > max){
				max = curr;
				maxStart = i;
				maxEnd = j;
			}
			curr += arr[j + 1];
		}
	}
	printf("The maximum sum is: %d, this was obtained from the sub array starting at: %d and ending at %d\n", max, maxStart, maxEnd);
	// complexity of n^2
}
