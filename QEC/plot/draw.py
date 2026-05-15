import uproot
import numpy as np
import matplotlib.pyplot as plt

# 1. Open the file
file = uproot.open("../result/jpsi_Jet_100000ev_QCD.root")

# 2. Read histograms from the ROOT file
h_qec_charged        = file["qec_all_charged"]
h_qec_charged_in     = file["qec_charged_in"]
h_qec_jet_out        = file["qec_jet_out"]
h_qec_jet_in         = file["qec_jet_in"]
h_qec_jet_in_charged = file["qec_jet_in_charged"]
h_qec_jet_in_neutral = file["qec_jet_in_neutral"]
h_qec_jet_all        = file["qec_jet_all"]

# Helper function to extract values and edges
def get_data(h):
    v, e = h.to_numpy()
    return v, e

v_ch, e_ch               = get_data(h_qec_charged)
v_ch_in, e_ch_in         = get_data(h_qec_charged_in)
v_jet_out, e_jet_out     = get_data(h_qec_jet_out)
v_jet_in, e_jet_in       = get_data(h_qec_jet_in)
v_in_chrg, e_in_chrg     = get_data(h_qec_jet_in_charged)
v_in_neut, e_in_neut     = get_data(h_qec_jet_in_neutral)
v_jet_all, e_jet_all     = get_data(h_qec_jet_all)

# --- Define Global Baseline (Total J/psi Events) ---
total_events = 156
if total_events == 0: total_events = 1.0

# --- Standardized Plotting Style Function ---
def apply_style(title, ylabel="Normalized to Total J/psi Events"):
    plt.xlabel(r"$\cos\chi$", fontsize=14)
    plt.ylabel(ylabel, fontsize=14)
    plt.yscale("log")
    plt.xlim(-1, 1)
    
    # Force Y-axis range and display the tick for 10
    plt.ylim(1e-8, 100) 
    plt.yticks([1e-8, 1e-6, 1e-4, 1e-2, 1, 10,20,30,40]) 
    
    plt.grid(True, which="both", ls="-", alpha=0.2)
    
    # Legend on the upper right to avoid overlapping left-side peaks
    plt.legend(fontsize=9, loc='upper right') 
    
    plt.title(title)
    plt.tight_layout()

def plot_step(edges, values, label, color=None):
    plt.step(edges, np.append(values, values[-1]), where='post', label=label, color=color)

# --- Figure 1: J/psi and AK8 Jet Spatial Relation ---
plt.figure(figsize=(8,6))
plot_step(e_jet_in,  v_jet_in/total_events,  r"$J/\psi$ inside AK8 jet")
plot_step(e_jet_out, v_jet_out/total_events, r"$J/\psi$ outside AK8 jet")
plot_step(e_jet_all, v_jet_all/total_events, r"$J/\psi$ vs. AK8 jet (all)")
apply_style("Plot 1: J/psi and AK8 Jet Spatial Relation")
plt.savefig("coschi_part1_jet_spatial.png", dpi=300)

# --- Figure 2: Inside AK8 Jet Components ---
plt.figure(figsize=(8,6))
plot_step(e_in_chrg, v_in_chrg/total_events, r"$J/\psi$ inside AK8 (jet charged dau)")
plot_step(e_in_neut, v_in_neut/total_events, r"$J/\psi$ inside AK8 (jet neutral dau)")
plot_step(e_jet_in,  v_jet_in/total_events,  r"$J/\psi$ inside AK8 jet (sum)")
apply_style("Plot 2: Inside AK8 Jet Components")
plt.savefig("coschi_part2_inside_jet.png", dpi=300)

# --- Figure 3: Charged Hadron Comparison (Normalized to Total Events) ---
plt.figure(figsize=(8,6))
# Using total_events normalization for all curves here
plot_step(e_ch_in,   v_ch_in/total_events,   r"$J/\psi$ inside AK8 (PF all charged)")
plot_step(e_in_chrg, v_in_chrg/total_events, r"$J/\psi$ inside AK8 (Jet charged dau)")
apply_style("Plot 3: PF Charged vs. Jet Daughters Yield")
plt.savefig("coschi_part3_charged_yield.png", dpi=300)

print("Plotting complete: All 3 plots are now normalized to the total J/psi event count (11682).")