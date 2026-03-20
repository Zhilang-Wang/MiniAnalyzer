import uproot
import numpy as np
import matplotlib.pyplot as plt

file = uproot.open("../result/output_QEC.root")
hist = file["coschi"]

values, edges = hist.to_numpy()

values = values / np.sum(values)

plt.figure(figsize=(8,6))
plt.step(edges, np.append(values, values[-1]), where='post', label="all particles")

plt.xlabel("coschi")
plt.yscale("log")
plt.ylim(1e-8, 10)
plt.xlim(-1, 1)
plt.legend()
plt.savefig("coschi.png", dpi=300)
plt.close()

print("Saved figure as coschi.png") 