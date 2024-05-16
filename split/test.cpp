#include<iostream>
#include <vector>

using namespace std;

class MyClass {
public:
    std::vector<int> myVector; // 类的私有成员，类型为 std::vector<int>
    uint64_t size = 90;

    // 构造函数，初始化 myVector
    MyClass() {
        // 可以在构造函数中添加一些初始化操作
        myVector.push_back(1);
        myVector.push_back(2);
        myVector.push_back(3);
    }

    // 公有成员函数，用于访问和修改 myVector
    void printVector() {
        // 遍历并输出 myVector 的所有元素
        for (int i : myVector) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }

    void addToVector(int value) {
        // 将新的值添加到 myVector 中
        myVector.push_back(value);
    }
};

int main(){
  /*
  MyClass *ptr;

  ptr = new(MyClass);
  ptr->addToVector(4);
  cout<<sizeof(MyClass)<<endl;
  cout<<&ptr<<endl;
  cout<<&ptr->size<<endl;
  cout<<&ptr->myVector<<endl;
  cout<<&ptr->myVector[0]<<endl;
  cout<<&ptr->myVector[1]<<endl;
  cout<<&ptr->myVector[2]<<endl;
  cout<<ptr->myVector[3]<<endl;
  cout<<ptr->myVector.size()<<endl;

  MyClass *ptr1 = new(MyClass);
  cout<<sizeof(MyClass)<<endl;
  cout<<&ptr1<<endl;
  cout<<&ptr1->size<<endl;
  cout<<&ptr1->myVector<<endl;
  cout<<&ptr1->myVector[0]<<endl;
  cout<<&ptr1->myVector[1]<<endl;
  cout<<&ptr1->myVector[2]<<endl;

 
  MyClass *ptr = new(MyClass);
  //ptr->addToVector(5);
  cout<<sizeof(MyClass)<<endl;
  cout<<&ptr<<endl;
  cout<<&ptr->size<<endl;
  cout<<&ptr->myVector<<endl;
  cout<<&ptr->myVector[0]<<endl;
  cout<<&ptr->myVector[1]<<endl;
  cout<<&ptr->myVector[2]<<endl;
  cout<<&ptr->myVector[3]<<endl;
  cout<<ptr->myVector[3]<<endl;
  cout<<ptr->myVector.size()<<endl;
  */

  for (int i =0; i<5; i++){
    MyClass *ptr = new(MyClass);
    ptr->addToVector(4);
    cout<<sizeof(MyClass)<<endl;
    cout<<&ptr<<endl;
    cout<<&ptr->size<<endl;
    cout<<&ptr->myVector<<endl;
    cout<<&ptr->myVector[0]<<endl;
    cout<<&ptr->myVector[1]<<endl;
    cout<<&ptr->myVector[2]<<endl;
    cout<<ptr->myVector[3]<<endl;
    cout<<ptr->myVector.size()<<endl;    
  }

  return 0;
}