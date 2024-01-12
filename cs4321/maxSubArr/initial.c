#include<stdio.h>

int main(){
	int test[] = {2,3,-5,2,-7,-5,3,7};
	int curr = 0;
	int max = 0;
	int maxStartIndex = 0;
	int maxEndIndex = 0;
	for(int i = 0; i < 8; i++){
		for(int j = 1; j < 8; j++){
			for(int k = i; k <= j; k++){
				curr += test[k];	
			}
			if(curr > max){
				max = curr;
				maxStartIndex = i;
				maxEndIndex = j;
			}
			curr = 0;
		}
	}
	printf("The max value is %d, this was obtained from starting at index %d and going until index %d.\n", max, maxStartIndex, maxEndIndex);
	return 0;
}
