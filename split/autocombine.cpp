#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define N 500
#define CNT 4

int main() {
    // 文件路径
    std::string filePath = "../tps-sum.txt";
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

    // 检查文件是否成功打开
    if (!inFile0.is_open()) {
        std::cerr << "Error opening file: " << filePath0 << std::endl;
        return 1; // 退出程序，返回错误代码
    }
    if (!inFile1.is_open()) {
        std::cerr << "Error opening file: " << filePath1 << std::endl;
        return 1; // 退出程序，返回错误代码
    }

    // 读取文件内容，计算总和
    std::string line0,line1,line2,line3;
    int sum[N];
    int n=0;
    while (std::getline(inFile0, line0)) {
        std::getline(inFile1, line1);
        std::getline(inFile2, line2);
        std::getline(inFile3, line3);
        sum[n] = std::stoi(line0) + std::stoi(line1) + std::stoi(line2) + std::stoi(line3);
        //outFile << sum[n] << std::endl;
        n++;
    }

    int aver_sum[N];
    

    int iter =3;  //平均值循环次数
    while (iter>0){
        aver_sum[0] = sum[0];
        for (int i=1; i<n-1; i++){
            aver_sum[i] = (sum[i-1] + sum[i] + sum[i+1]) / 3;
        }   
        aver_sum[n-1] = sum[n-1];

        for (int i=0;i<n;i++) sum[i] = aver_sum[i];
        iter--;
    }

    for (int i=0;i<n;i++){
        outFile << aver_sum[i] << std::endl;
    }

    // 关闭文件
    inFile0.close();
    inFile1.close();
    inFile2.close();
    inFile3.close();    
    outFile.close();


    return 0; // 程序正常退出
}
