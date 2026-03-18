import uproot
import numpy as np
import matplotlib.pyplot as plt

file_path = "/eos/user/z/zhilang/JPsiTo2Mu_Pt-0To100_pythia8-gun/MiniAnalyzer_JPsi_Run3/260317_121721/0000/DUMMYFILENAME_118.root"

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


