import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import PolynomialFeatures
from sklearn.linear_model import LinearRegression
from sklearn.svm import SVR
from sklearn.metrics import mean_squared_error

# 读取CSV文件
file_path = 'numdata.csv'
data = pd.read_csv(file_path)

# 提取自变量和因变量
X = data[['size', 'weight', 'latency', 'num']]
y = data['time']

# 拆分数据集为训练集和测试集
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# 多项式回归：创建多项式特征
poly = PolynomialFeatures(degree=2)  # 使用2次多项式
X_train_poly = poly.fit_transform(X_train)
X_test_poly = poly.transform(X_test)

# 创建和训练多项式回归模型
poly_model = LinearRegression()
poly_model.fit(X_train_poly, y_train)

# 创建和训练线性回归模型
linear_model = LinearRegression()
linear_model.fit(X_train, y_train)

# 创建和训练支持向量机回归模型
svr_model = SVR()
svr_model.fit(X_train, y_train)

# 进行预测
y_train_poly_pred = poly_model.predict(X_train_poly)
y_test_poly_pred = poly_model.predict(X_test_poly)

y_train_linear_pred = linear_model.predict(X_train)
y_test_linear_pred = linear_model.predict(X_test)

y_train_svr_pred = svr_model.predict(X_train)
y_test_svr_pred = svr_model.predict(X_test)

# 评估模型
poly_train_mse = mean_squared_error(y_train, y_train_poly_pred)
poly_test_mse = mean_squared_error(y_test, y_test_poly_pred)

linear_train_mse = mean_squared_error(y_train, y_train_linear_pred)
linear_test_mse = mean_squared_error(y_test, y_test_linear_pred)

svr_train_mse = mean_squared_error(y_train, y_train_svr_pred)
svr_test_mse = mean_squared_error(y_test, y_test_svr_pred)

results = {
    "Polynomial Regression": {"train_mse": poly_train_mse, "test_mse": poly_test_mse},
    "Linear Regression": {"train_mse": linear_train_mse, "test_mse": linear_test_mse},
    "SVR": {"train_mse": svr_train_mse, "test_mse": svr_test_mse}
}

print(results)
