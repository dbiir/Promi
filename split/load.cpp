#include <iostream>
#include<stdlib.h>
#include<time.h>

using namespace std;
#define N 218
#define Base 30000
#define Line1 0.9
#define Line2 0.8
#define Line3 0.7

int main(){
  srand((unsigned)time(NULL));
  int a[N];
  int num1,num2,num3;
  num1 = 0;
  num2 = 0;
  num3 = 0;
  for (int i=0;i<N;i++){
    cin>>a[i];
    if (a[i] > Base * Line1){
      num1 ++;
    } 
    if (a[i] > Base * Line2){
      num2 ++;
    }
    if (a[i] > Base * Line3){
      num3 ++;
    } 
  }
  std::cout<<"*****"<<endl;
  cout<< num1 <<endl;
  cout<< num2 <<endl;
  cout<< num3 <<endl;
  return 0;
}