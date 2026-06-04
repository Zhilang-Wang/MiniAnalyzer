import uproot
import numpy as np
import matplotlib.pyplot as plt
import os

# --- 1. Configuration and Paths ---
input_file = "../../result/JPsiMuMu_qec_all_trigger.root"
output_dir = "plots_qec_comprehensive"
if not os.path.exists(output_dir):
    os.makedirs(output_dir)

file = uproot.open(input_file)

# --- 2. Core Plotting Functions ---
def get_norm_data(name):
    """
    Extract histogram and apply Unit Sum normalization.
    """
    if name not in file:
        return None, None
    h = file[name]
    v, e = h.to_numpy()
    
    total_counts = np.sum(v)
    if total_counts > 0:
        v_norm = v / total_counts
    else:
        v_norm = v
    
    return v_norm, e

def plot_comparison(base_name, title, filename):
    plt.figure(figsize=(9, 7))
    
    # suffix, label, color, linestyle, linewidth, alpha
    # High contrast settings for Direct and Feed-down
    sub_types = [
        ("",          "Total (All)",         "black",       "-",  2.0, 1.0),
        ("_prompt",   "Prompt (Total)",      "blue",        "-",  2.5, 0.4), # Semi-transparent
        ("_direct",   "Prompt: Direct",      "darkorange", "--",  2.2, 1.0), # High contrast
        ("_fd",       "Prompt: Feed-down",   "forestgreen", "--",  2.5, 1.0), # Distinct dots
        ("_np",       "Non-Prompt (from B)", "red",         "-",  1.8, 1.0)
    ]
    
    for suffix, label, color, lstyle, lwidth, alpha_val in sub_types:
        v_norm, edges = get_norm_data(f"{base_name}{suffix}")
        if v_norm is not None:
            plt.step(edges, np.append(v_norm, v_norm[-1]), where='post', 
                     label=label, color=color, linestyle=lstyle, lw=lwidth, alpha=alpha_val)

    plt.xlabel(r"$\cos\chi$", fontsize=14)
    plt.ylabel("Unit-sum normalized", fontsize=14)
    plt.yscale("log")
    plt.xlim(-1, 1)
    plt.ylim(1e-7, 5) 
    
    plt.grid(True, which="both", ls="-", alpha=0.2)
    plt.legend(fontsize=10, loc='upper right', frameon=True)
    plt.title(title, fontsize=14)
    plt.tight_layout()
    
    plt.savefig(os.path.join(output_dir, filename), dpi=300)
    plt.close()

# --- 3. Execute Tasks ---
tasks = [
    ("qec_jet_all",     "QEC: J/psi vs AK8 Jet (Overall)", "comparison_jet_all.png"),
    ("qec_jet_in",      "QEC: J/psi Inside AK8 Jet",       "comparison_jet_inside.png"),
    ("qec_jet_out",     "QEC: J/psi Outside AK8 Jet",      "comparison_jet_outside.png"),
    ("qec_all_charged", "QEC: Global Charged Hadrons",     "comparison_charged_global.png"),
    ("qec_charged_in",  "QEC: Charged Hadrons (J/psi In Jet)", "comparison_charged_in_jet.png")
]

for base, title, fname in tasks:
    print(f"Processing: {title}...")
    plot_comparison(base, title, fname)

print(f"\nDone! High-contrast plots saved in: '{output_dir}'")