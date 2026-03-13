import uproot
import numpy as np
import matplotlib.pyplot as plt

# ROOT 文件路径
file_path = "../result/MiniAnalyzer_output.root"

# 打开文件
file = uproot.open(file_path)

# 查看结构（调试用）
print("ROOT keys:", file.keys())

# 进入目录
tree = file["MiniAnalyzer/OniaTree"]
print("Number of events in tree:", tree.num_entries)

# 读取变量
mass = tree["mass"].array(library="np")
vProb = tree["vProb"].array(library="np")  # dimuon vertex probability
# 如果你保存了 SoftMuon 信息，也可以读取
# mu1_soft = tree["mu1_isSoftMuon"].array(library="np")
# mu2_soft = tree["mu2_isSoftMuon"].array(library="np")

# 画图
plt.figure(figsize=(8,6), dpi=300)
plt.hist(mass, bins=100, range=(2.5, 3.5), histtype='step', color='blue', label='Mass distribution')
plt.xlabel("mass")
plt.ylabel("Events")
plt.legend()

plt.savefig("mass.png", dpi=300)
