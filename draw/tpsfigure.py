import matplotlib.pyplot as plt
import os

# 读取文件内容的函数
def read_file(filename):
    data = []
    try:
        with open(filename, 'r') as file:
            for line in file:
                # 将每行数据转换为浮点数，并添加到列表中
                data.append(float(line.strip()))
    except FileNotFoundError:
        print(f"文件 {filename} 未找到")
    return data

# 获取当前路径
current_path = os.getcwd()

# 文件名
file1 = "../tps0.txt"
file2 = "../tps1.txt"
##file1 = os.path.join(current_path, 'tps0.txt')
##file2 = os.path.join(current_path, 'tps1.txt')

# 读取文件数据
data1 = read_file(file1)
data2 = read_file(file2)

# 检查数据是否为空
if not data1 and not data2:
    print("未找到数据")
    exit()

# 定义X轴位置
x1 = range(len(data1))
x2 = range(len(data2))

# 创建图形
fig, ax = plt.subplots()

# 绘制折线图
ax.plot(x1, data1, label='TPS0', marker='o')
ax.plot(x2, data2, label='TPS1', marker='o')

# 添加图表标题和标签
ax.set_xlabel('Index')
ax.set_ylabel('Value')
ax.set_title('TPS0 and TPS1 Values')
ax.legend()

# 保存图形
output_path = os.path.join(current_path, 'output.png')
plt.savefig(output_path)

# 显示图形
plt.show()
