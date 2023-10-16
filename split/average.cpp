#include <iostream>

using namespace std;

#define N 99

int main(){
  int a[N],b[N];
  for (int i=0;i<N;i++){
    cin>>a[i];
  }
  cout<<"**************"<<endl;
  int k=3;
  while(k>0){
    b[0] = a[0];      
    for (int i=1;i<N-1;i++){
      b[i] = (a[i-1]+a[i]+a[i+1])/3;
    }
    b[N-1] = a[N-1];
    for (int i=0;i<N;i++){
      a[i]=b[i];
    }
    k--;
  }
  for (int i=1;i<N;i++){
    cout<<a[i]<<endl;
  }
  return 0;
}