int main(){
    int s = 0;
    int f = 0;
    int n = 10;
    for(int i = 1; i <= n; i++){
        f = i*i*i + i*i;
        for(int j = 1; j <= f; j++){
            s++;
        }
    }
}
