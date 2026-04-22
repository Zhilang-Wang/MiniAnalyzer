import uproot
import numpy as np
import matplotlib.pyplot as plt

file = uproot.open("../result/jpsi_InJet_50000ev.root")

# Read histograms from your output
h_qec_charged         = file["qec_all_charged"]
h_qec_jet_out          = file["qec_jet_out"]
h_qec_jet_in           = file["qec_jet_in"]
h_qec_jet_in_charged   = file["qec_jet_in_charged"]
h_qec_jet_in_neutral   = file["qec_jet_in_neutral"]
h_qec_jet_all          = file["qec_jet_all"]

# Convert to numpy arrays
v_chadrons,  e_chadrons     = h_qec_charged.to_numpy()
v_jet_out,   e_jet_out      = h_qec_jet_out.to_numpy()
v_jet_in,    e_jet_in       = h_qec_jet_in.to_numpy()
v_in_chrg,   e_in_chrg      = h_qec_jet_in_charged.to_numpy()
v_in_neut,   e_in_neut      = h_qec_jet_in_neutral.to_numpy()
v_jet_all,  e_jet_all       = h_qec_jet_all.to_numpy()

# Normalize to unity
v_chadrons     = v_chadrons / np.sum(v_chadrons)
v_jet_out      = v_jet_out  / np.sum(v_jet_out)
v_jet_in       = v_jet_in   / np.sum(v_jet_in)
v_in_chrg      = v_in_chrg  / np.sum(v_in_chrg)
v_in_neut      = v_in_neut  / np.sum(v_in_neut)
v_jet_all      = v_jet_all  / np.sum(v_jet_all)

# Plot
plt.figure(figsize=(8,6))

plt.step(e_chadrons, np.append(v_chadrons, v_chadrons[-1]),
         where='post', label=r"$J/\psi$ vs. all charged hadrons")

plt.step(e_jet_out, np.append(v_jet_out, v_jet_out[-1]),
         where='post', label=r"$J/\psi$ outside AK8 jet")

plt.step(e_jet_in, np.append(v_jet_in, v_jet_in[-1]),
         where='post', label=r"$J/\psi$ inside AK8 jet")

plt.step(e_in_chrg, np.append(v_in_chrg, v_in_chrg[-1]),
         where='post', label=r"$J/\psi$ inside AK8 jet (charged)")

plt.step(e_in_neut, np.append(v_in_neut, v_in_neut[-1]),
         where='post', label=r"$J/\psi$ inside AK8 jet (neutral)")

plt.step(e_jet_all, np.append(v_jet_all, v_jet_all[-1]),
         where='post', label=r"$J/\psi$ + AK8 jet (all)")

plt.xlabel(r"$\cos\chi$", fontsize=14)
plt.ylabel("Normalized", fontsize=14)
plt.yscale("log")
plt.xlim(-1, 1)
plt.ylim(1e-8, 1.0)
plt.grid(alpha=0.3)
plt.legend(fontsize=12)
plt.tight_layout()

plt.savefig("coschi.png", dpi=300)
plt.close()

print("Finished drawing coschi.png")