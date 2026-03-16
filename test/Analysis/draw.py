import uproot
import numpy as np
import matplotlib.pyplot as plt

file_path = "../result/MiniAnalyzer_output.root"

file = uproot.open(file_path)

print("ROOT keys:", file.keys())

tree = file["MiniAnalyzer/OniaTree"]
print("Number of events in tree:", tree.num_entries)
  
mass = tree["mass"].array(library="np")
vProb = tree["vProb"].array(library="np")

plt.figure(figsize=(8,6), dpi=300)
plt.hist(mass, bins=100, range=(2.5, 3.5), histtype='step', color='blue', label='Mass distribution')
plt.xlabel("mass [GeV]")
plt.ylabel("Events")
plt.legend()
plt.savefig("mass.png", dpi=300)
plt.close()


