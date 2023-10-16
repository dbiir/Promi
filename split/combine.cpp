#include <iostream>

using namespace std;

#define M 99

int main(){
  int a[M],b[M];
  for (int i=0;i<M;i++){
    cin>>a[i]>>b[i];
    a[i] += b[i];
  }
  for (int i=0;i<M;i++){
    cout<<a[i]<<endl;
  }
  return 0;
}