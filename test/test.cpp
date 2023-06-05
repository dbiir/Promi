#include <iostream>

using namespace std;

#define NUM 40

int main(){
  int a[NUM];
  for (int i=0;i<NUM;i++){
    cin>>a[i];
  }
  cout<<"******"<<endl;
  for (int i=0;i<NUM-1;i++){
    a[i] = (a[i] + a[i+1])/2;
    cout<<a[i]<<endl;
  }
  cout<<a[NUM-1]<<endl;
  return 0;
}