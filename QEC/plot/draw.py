import uproot
import numpy as np
import matplotlib.pyplot as plt

file = uproot.open("../result/jpsi_in_jet.root")

# 读取 3 个 histogram
h_incl    = file["coschi_inclusive_chadrons"]
h_jet_all = file["coschi_inclusive_jetconst"]
h_jet_mat = file["coschi_jet_jpsi"]

# 转成 numpy 数组
v_incl, e_incl     = h_incl.to_numpy()
v_jet_all, e_jet_all = h_jet_all.to_numpy()
v_jet_mat, e_jet_mat = h_jet_mat.to_numpy()

# 归一化（每条都归一化到 1）
v_incl    = v_incl / np.sum(v_incl)
v_jet_all = v_jet_all / np.sum(v_jet_all)
v_jet_mat = v_jet_mat / np.sum(v_jet_mat)

# ----------------------
# 画图
# ----------------------
plt.figure(figsize=(8,6))

plt.step(e_incl,    np.append(v_incl, v_incl[-1]),    where='post', label=r"Inclusive $J/\psi$ vs. all charged hadrons")
plt.step(e_jet_all, np.append(v_jet_all, v_jet_all[-1]), where='post', label=r"Inclusive $J/\psi$ vs. jet")
plt.step(e_jet_mat, np.append(v_jet_mat, v_jet_mat[-1]), where='post', label=r"$J/\psi$ inside jet vs. jet")

plt.xlabel(r"$\cos\chi$", fontsize=14)
plt.ylabel("Normalized", fontsize=14)
plt.yscale("log")
plt.xlim(-1, 1)
plt.ylim(1e-10, 10)  
plt.grid(alpha=0.3)
plt.legend(fontsize=12)
plt.tight_layout()

plt.savefig("coschi_3cases.png", dpi=300)
plt.close()

print("✅ Finished drawing coschi_3cases.png")