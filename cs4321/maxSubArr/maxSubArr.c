#include <stdio.h>

struct ans{
	int max;
	int indexS;
	int indexE;
};

void printAns(struct ans ans) {
	printf("The maximum sum is: %d, this was obtained from the sub array starting at: %d and ending at %d\n", ans.max, ans.indexS, ans.indexE);
}

struct ans maxSubArr(int *arr, int n){
	int curr;
	struct ans ans;
	for(int i = 0; i < n; i++){
		curr = arr[i];
		for(int j = i; j < n; j++){	
			if(curr > ans.max){
				ans.max = curr;
				ans.indexS = i;
				ans.indexE = j;
			}
			curr += arr[j + 1];
		}
	}
	return ans;
}

int main(){
	int arr[] = {2,3,-5,1,-7,-5,3,7};
	int n = sizeof(arr) / sizeof(arr[0]);
	printAns(maxSubArr(arr, n));
}
