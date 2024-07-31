import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import PolynomialFeatures
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_squared_error

# 读取CSV文件
file_path = 'singledata.csv'
data = pd.read_csv(file_path)

# 打印前几行数据以确认读取正确
print(data.head())

# 提取自变量和因变量
X = data[['size', 'weight', 'latency']]
y = data['time']

# 拆分数据集为训练集和测试集
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# 多项式回归：创建多项式特征
poly = PolynomialFeatures(degree=2)  # 使用2次多项式
X_train_poly = poly.fit_transform(X_train)
X_test_poly = poly.transform(X_test)

# 创建线性回归模型
model = LinearRegression()

# 训练模型
model.fit(X_train_poly, y_train)

# 进行预测
y_train_pred = model.predict(X_train_poly)
y_test_pred = model.predict(X_test_poly)

# 评估模型
train_mse = mean_squared_error(y_train, y_train_pred)
test_mse = mean_squared_error(y_test, y_test_pred)

print(f'Training MSE: {train_mse}')
print(f'Testing MSE: {test_mse}')

# 打印模型系数
print('Model coefficients:', model.coef_)
print('Model intercept:', model.intercept_)


