import uproot
import numpy as np
import matplotlib.pyplot as plt

file_path = "../result/MiniAnalyzer_output.root"

# 打开 ROOT 文件
file = uproot.open(file_path)

print("ROOT keys:", file.keys())

# 获取 TTree
tree = file["MiniAnalyzer/OniaTree"]
print("Number of events in tree:", tree.num_entries)

# ===============================
# 读取 branch
# ===============================
mass = tree["mass"].array(library="np")
vProb = tree["vProb"].array(library="np")

# 新加的 PF 关联 branch
jpsi_nPF = tree["jpsi_nPF"].array(library="np")
jpsi_sumPtPF = tree["jpsi_sumPtPF"].array(library="np")

# ===============================
# 画 Mass 分布
# ===============================
plt.figure(figsize=(8,6), dpi=300)
plt.hist(mass, bins=100, range=(2.5, 3.5), histtype='step', color='blue', label='Mass distribution')
plt.xlabel("mass [GeV]")
plt.ylabel("Events")
plt.legend()
plt.savefig("mass.png", dpi=300)
plt.close()

# ===============================
# 画 jpsi_nPF 分布
# ===============================
plt.figure(figsize=(8,6), dpi=300)
plt.hist(jpsi_nPF, bins=50, range=(0,50), histtype='step', color='green', label='Number of PF candidates')
plt.xlabel("nPF")
plt.ylabel("Events")
plt.legend()
plt.savefig("jpsi_nPF.png", dpi=300)
plt.close()

# ===============================
# 画 jpsi_sumPtPF 分布
# ===============================
plt.figure(figsize=(8,6), dpi=300)
plt.hist(jpsi_sumPtPF, bins=50, range=(0,200), histtype='step', color='red', label='Sum pT of PF candidates [GeV]')
plt.xlabel("sumPtPF [GeV]")
plt.ylabel("Events")
plt.legend()
plt.savefig("jpsi_sumPtPF.png", dpi=300)
plt.close()

plt.figure(figsize=(6,5))
plt.scatter(jpsi_nPF, jpsi_sumPtPF, alpha=0.5, s=10, color='blue')
plt.xlabel("Number of PF candidates around J/psi")
plt.ylabel("Sum pT of PF candidates [GeV]")
plt.title("J/psi - PF candidate association")
plt.grid(True)
plt.savefig("jpsi_PF_association.png", dpi=300)
plt.close()
