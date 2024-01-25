#include <iostream>
#include<stdlib.h>

using namespace std;
#define N 58

int main(){
  float a[N];
  for (int i=0;i<N;i++){
    cin>>a[i];
    //a[i] -= 5;
    a[i] += (rand() % 2);
    a[i] -= (rand() % 2);
    a[i] += 0.123;
    //a[i] += (rand() % 10000);
  }
  std::cout<<"*****"<<endl;
  for (int i=0;i<N;i++){
    cout<<a[i]<<endl;
  }  
  return 0;
}