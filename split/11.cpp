#include <iostream>
#include<stdlib.h>

using namespace std;
#define N 20

int main(){
  int a[N];
  for (int i=0;i<N;i++){
    cin>>a[i];
    a[i] -= 5000;
    //a[i] -= (rand() % 5000);
    //a[i] -= (rand() % 10000);
    //a[i] += (rand() % 10000);
  }
  std::cout<<"*****"<<endl;
  for (int i=0;i<N;i++){
    cout<<a[i]<<endl;
  }  
  return 0;
}