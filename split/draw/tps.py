import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("tpccbalance.csv")
print(data[0])

##绘制多线图
x = np.linspace(0, 99, 20)  ##生成0到99的数字, 一共20个
"""
y_1 = x
y_2 = np.square(x)
y_3 = np.log(x)
y_4 = np.sin(x)
plt.plot(x,y_1)
plt.plot(x,y_2)
plt.plot(x,y_3)
plt.plot(x,y_4)
plt.show()


plt.figure(figsize=(14,6))
#在一个图内画多条折线，注意x轴数据源相同，y轴是不同数据源
plt.plot(df.index,df["Wookong+M"])
plt.plot(df.index,df["Remus"])
plt.plot(df.index,df["Squall"])
plt.xticks(range(len( df.index)), df.index,rotation=30)#rotation=30把标签倾斜30
plt.xlabel("时间",fontsize = 12)
plt.ylabel("股价",fontsize = 12)
plt.title("2012年8-9月股价折线图",fontsize = 14)
plt.legend(["收盘价","最高价","最低价","开盘价"],loc=4);#loc设定图例的位置，4为右下
"""