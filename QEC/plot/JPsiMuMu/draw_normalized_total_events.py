import uproot
import numpy as np
import matplotlib.pyplot as plt
import os

# --- 1. Configuration and Paths ---
input_file = "../../result/JPsiMuMu_qec_all_trigger_MatchedJpsi.root"
output_dir = "plots_qec_matchedJpsi_normalized_events"
if not os.path.exists(output_dir):
    os.makedirs(output_dir)

file = uproot.open(input_file)

# --- 2. Determine Total J/psi Events for Normalization ---
# 采用你 Analyzer 统计出的真实 J/psi 物理事件总数
N_total_jpsi = 36461602

print(f"--> Global Normalization Factor (True J/psi Events): {N_total_jpsi}\n")

# --- 3. Core Plotting Functions ---
def get_norm_data_by_total(name, norm_factor):
    """
    Extract histogram and apply normalization relative to True J/psi events.
    """
    if name not in file:
        return None, None
    h = file[name]
    v, e = h.to_numpy()
    
    if norm_factor > 0:
        v_norm = v / norm_factor
    else:
        v_norm = v
    
    return v_norm, e

def plot_comparison(base_name, title, filename, ymin, ymax):
    """
    Plot comparison with dynamically adjusted Y-axis range.
    """
    plt.figure(figsize=(9, 7))
    
    # suffix, label, color, linestyle, linewidth, alpha
    sub_types = [
        ("",          "Total (All)",         "black",       "-",  2.0, 1.0),
        ("_prompt",   "Prompt (Total)",      "blue",        "-",  2.5, 0.4), 
        ("_direct",   "Direct",              "darkorange", "--",  2.2, 1.0), 
        ("_fd",       "Feed-down",           "forestgreen", "--",  2.5, 1.0), 
        ("_np",       "Non-Prompt (from B)", "red",         "-",  1.8, 1.0)
    ]
    
    for suffix, label, color, lstyle, lwidth, alpha_val in sub_types:
        v_norm, edges = get_norm_data_by_total(f"{base_name}{suffix}", N_total_jpsi)
        if v_norm is not None:
            plt.step(edges, np.append(v_norm, v_norm[-1]), where='post', 
                     label=label, color=color, linestyle=lstyle, lw=lwidth, alpha=alpha_val)

    plt.xlabel(r"$\cos\chi$", fontsize=14)
    plt.ylabel("Normalized per True $J/\psi$ Event", fontsize=14)
    plt.yscale("log")
    plt.xlim(-1, 1)

    plt.ylim(ymin, ymax)

    plt.grid(True, which="both", ls="-", alpha=0.2)
    plt.legend(fontsize=10, loc='upper right', frameon=True)
    plt.title(title, fontsize=14)
    plt.tight_layout()
    
    plt.savefig(os.path.join(output_dir, filename), dpi=300)
    plt.close()

# --- 4. Execute Tasks with Customized Y-axis Ranges ---
tasks = [
    ("qec_jpsiAll_ak8Jet",         "QEC: Total J/psi vs AK8 Jet",         "comparison_jpsiAll_ak8Jet.png",         1e-7, 10.0),
    ("qec_jpsiInJet_ak8Jet",       "QEC: J/psi Inside Jet vs AK8 Jet",    "comparison_jpsiInJet_ak8Jet.png",       1e-7, 10.0),
    ("qec_jpsiOutJet_ak8Jet",      "QEC: J/psi Outside Jet vs AK8 Jet",   "comparison_jpsiOutJet_ak8Jet.png",      1e-9, 10.0), 
    ("qec_jpsiAll_chargedHadron",  "QEC: Total J/psi vs Charged Hadrons", "comparison_jpsiAll_chargedHadron.png",  1e-4, 10000.0), 
    ("qec_jpsiInJet_chargedHadron","QEC: J/psi Inside Jet vs Charged Hadrons", "comparison_jpsiInJet_chargedHadron.png", 1e-6, 1000.0)
]

for base, title, fname, ymin, ymax in tasks:
    actual_base = base
    if actual_base not in file:
        legacy_mapping = {
            "qec_jpsiAll_ak8Jet": "qec_jet_all",
            "qec_jpsiInJet_ak8Jet": "qec_jet_in",
            "qec_jpsiOutJet_ak8Jet": "qec_jet_out",
            "qec_jpsiAll_chargedHadron": "qec_all_charged",
            "qec_jpsiInJet_chargedHadron": "qec_charged_in"
        }
        actual_base = legacy_mapping.get(base, base)
    
    print(f"Processing: {title} (Reading histogram: {actual_base})...")
    plot_comparison(actual_base, title, fname, ymin, ymax) 

print(f"\nDone! Plots normalized to {N_total_jpsi} true events saved in: '{output_dir}'")