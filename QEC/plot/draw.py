import uproot
import numpy as np
import matplotlib.pyplot as plt

file = uproot.open("../result/jpsi_InJet_20000ev.root")

# 读取 4 个 histogram
h_incl        = file["coschi_inclusive_chadrons"]
h_jet_all     = file["coschi_inclusive_jetconst"]
h_jet         = file["coschi_inclusive_jpsi_jet"]
h_jet_inside  = file["coschi_jet_in_jpsi_jet"]  

# 转成 numpy 数组
v_incl, e_incl         = h_incl.to_numpy()
v_jet_all, e_jet_all   = h_jet_all.to_numpy()
v_jet, e_jet           = h_jet.to_numpy()
v_jet_inside, e_jet_in = h_jet_inside.to_numpy() 

# 归一化
v_incl        = v_incl / np.sum(v_incl)
v_jet_all     = v_jet_all / np.sum(v_jet_all)
v_jet         = v_jet / np.sum(v_jet)
v_jet_inside  = v_jet_inside / np.sum(v_jet_inside) 

# ----------------------
# 画图
# ----------------------
plt.figure(figsize=(8,6))

plt.step(e_incl,    np.append(v_incl, v_incl[-1]),    where='post', label=r"Inclusive $J/\psi$ vs. all charged hadrons")
plt.step(e_jet_all, np.append(v_jet_all, v_jet_all[-1]), where='post', label=r"Inclusive $J/\psi$ vs. all particles inside jet")
plt.step(e_jet,     np.append(v_jet,     v_jet[-1]),     where='post', label=r"Inclusive $J/\psi$ vs. jet")
plt.step(e_jet_in,  np.append(v_jet_inside, v_jet_inside[-1]),  where='post', label=r"$J/\psi$ inside AK4 jet vs. jet") 

plt.xlabel(r"$\cos\chi$", fontsize=14)
plt.ylabel("Normalized", fontsize=14)
plt.yscale("log")
plt.xlim(-1, 1)
plt.ylim(1e-8, 1.0)  
plt.grid(alpha=0.3)
plt.legend(fontsize=12)
plt.tight_layout()

plt.savefig("coschi_4cases.png", dpi=300)
plt.close()

print("✅ Finished drawing coschi_4cases.png")