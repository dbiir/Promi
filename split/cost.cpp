#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define N 300
#define cost_machine11  100 //200s机器租赁费用
#define cost_machine 0.5  //机器每秒的租赁费用
#define cost_tps  0.499   //满载的每秒费用
#define tps_full  30000.0   //满载的吞吐
#define T  154           //释放机器的时间

int main(){

  // 文件路径
  std::string filePath = "../tpscost.txt"; 
  std::string filePath0 = "../tps-sum.txt";

  // 打开文件以进行读取
  std::ifstream inFile0(filePath0);

  std::ofstream outFile(filePath, std::ios::out | std::ios::trunc);  

  // 读取文件内容，计算总和
    std::string line0;
    double cost[N];
    int n=0;
    while (std::getline(inFile0, line0)) {
        if (n < T){
          cost[n] = (std::stof(line0)) / (tps_full) * (cost_tps + cost_machine) * 4;
        } else {
          cost[n] = (std::stof(line0)) / (tps_full) * (cost_tps + cost_machine) * 3;          
        }
        outFile << cost[n] << std::endl;
        n++;
    }  

}

/*
int main(){

  // 文件路径
  std::string filePath = "../tpscost.txt";
  std::string filePath0 = "../tps0.txt";
  std::string filePath1 = "../tps1.txt";
  std::string filePath2 = "../tps2.txt";
  std::string filePath3 = "../tps3.txt";

  // 打开文件以进行读取
  std::ifstream inFile0(filePath0);
  std::ifstream inFile1(filePath1);
  std::ifstream inFile2(filePath2);
  std::ifstream inFile3(filePath3);     

  std::ofstream outFile(filePath, std::ios::out | std::ios::trunc);

  // 读取文件内容，计算总和
    std::string line0,line1,line2,line3;
    double cost[N];
    int n=0;
    while (std::getline(inFile0, line0)) {
        std::getline(inFile1, line1);
        std::getline(inFile2, line2);
        std::getline(inFile3, line3);
        if (n < T){
          cost[n] = (std::stof(line0) + std::stof(line1) + std::stof(line2) + std::stof(line3)) / (tps_full) * (cost_tps + cost_machine) * 4;
        } else {
          cost[n] = (std::stof(line1) + std::stof(line2) + std::stof(line3)) / (tps_full) * (cost_tps + cost_machine) * 4;          
        }
        outFile << cost[n] << std::endl;
        n++;
    }

    // 关闭文件
    inFile0.close();
    inFile1.close();
    inFile2.close();
    inFile3.close();    
    outFile.close();
    
    return 0;
}
*/