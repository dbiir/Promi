#include <iostream>

using namespace std;

#define M 99

int main(){
  int a[M],b[M];
  for (int i=0 ; i<M ; i++){
    /*
    int a1,a2,a3,b1,b2,b3;
    cin >> a1 >> a2 >> a3 >> b1 >> b2 >> b3;
    a[i] = (a1+a2+a3) / 3;
    b[i] = (b1+b2+b3) / 3;
    */
    int a1,a2,b1,b2;
    cin >> a1 >> a2 >> b1 >> b2;
    a[i] = (a1+a2) / 2;
    b[i] = (b1+b2) / 2;
  }
  cout<<"--"<<endl;
  for (int i=0;i<M;i++){
    cout<<a[i]<<endl;
  }
  cout<<"*****"<<endl;
  for (int i=0;i<M;i++){
    cout<<b[i]<<endl;
  }
  return 0;
}