import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader, Dataset
from sklearn.model_selection import train_test_split
import numpy as np
import pandas as pd

# 自定义数据集类
class VariableLengthDataset(Dataset):
    def __init__(self, data, labels):
        self.data = data
        self.labels = labels

    def __len__(self):
        return len(self.data)

    def __getitem__(self, idx):
        return self.data[idx], self.labels[idx]

# 自定义神经网络模型
class CNN_MLP(nn.Module):
    def __init__(self):
        super(CNN_MLP, self).__init__()
        self.conv1 = nn.Conv1d(in_channels=1, out_channels=32, kernel_size=3)
        self.pool = nn.AdaptiveMaxPool1d(1)
        self.fc1 = nn.Linear(32, 64)
        self.fc2 = nn.Linear(64, 64)
        self.fc3 = nn.Linear(64, 1)

    def forward(self, x):
        x = self.conv1(x)
        x = torch.relu(x)
        x = self.pool(x)
        x = x.view(-1, 32)
        x = torch.relu(self.fc1(x))
        x = torch.relu(self.fc2(x))
        x = self.fc3(x)
        return x

# 示例数据
# X_train = [np.array([1, 2, 3, 4]), np.array([1, 2, 3, 4, 5, 6]), np.array([1, 2, 3, 4, 5])]
# y_train = np.array([10, 15, 12])  # 对应的标签

# 读取CSV文件
file_path = 'numdata.csv'
data = pd.read_csv(file_path)

# 提取自变量和因变量
X = data[['size', 'weight', 'latency', 'num']]
y = data['time']

# 拆分数据集为训练集和测试集
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)


# 找到最长的输入数组长度
max_length = max(len(arr) for arr in X_train)


# 填充输入数组使其长度相同，并转换为PyTorch张量
X_train_padded = np.array([np.pad(arr, (0, max_length - len(arr)), 'constant') for arr in X_train])
X_train_padded = X_train_padded[:, np.newaxis, :]  # 增加一个维度以匹配输入通道
X_train_padded = torch.tensor(X_train_padded, dtype=torch.float32)
y_train = torch.tensor(y_train, dtype=torch.float32)

# 创建数据加载器
train_dataset = VariableLengthDataset(X_train_padded, y_train)
train_loader = DataLoader(train_dataset, batch_size=2, shuffle=True)

# 创建模型实例
model = CNN_MLP()

# 定义损失函数和优化器
criterion = nn.MSELoss()
optimizer = optim.Adam(model.parameters(), lr=0.001)

# 训练模型
num_epochs = 10
for epoch in range(num_epochs):
    model.train()
    running_loss = 0.0
    for inputs, labels in train_loader:
        optimizer.zero_grad()
        outputs = model(inputs)
        loss = criterion(outputs, labels.unsqueeze(1))
        loss.backward()
        optimizer.step()
        running_loss += loss.item()
    print(f"Epoch [{epoch+1}/{num_epochs}], Loss: {running_loss/len(train_loader)}")

# 打印模型结构
print(model)
