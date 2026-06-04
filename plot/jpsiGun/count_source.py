import ROOT
import sys
from collections import Counter
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

# Enable Multi-threading
ROOT.EnableImplicitMT()

def get_particle_name(pdg_id):
    mapping = {
        21: "Gluon", 2212: "Proton", 22: "Photon",
        511: "B0", 521: "B+", 531: "Bs", 541: "Bc", 5122: "Lambda_b",
        443: "J/psi", 100443: "psi(2S)", 445: "chi_c2",
        441: "eta_c", 20443: "chi_c1", 10441: "chi_c0",
        30443: "psi(1D)", 10443: "hc(1P)",
        1: "d", 2: "u", 3: "s", 4: "c", 5: "b",
        91: "Cluster", 92: "String"
    }
    abs_id = abs(int(pdg_id))
    if abs_id in mapping: return mapping[abs_id]
    if (abs_id // 100 == 5) or (abs_id // 1000 == 5): return f"B-Hadron({pdg_id})"
    return f"PDG {pdg_id}"

def main(input_pattern):
    print(f"Processing data: {input_pattern}")
    df = ROOT.RDataFrame("MiniAnalyzer/OniaTree", input_pattern)
    df_valid = df.Filter("nGenJpsi > 0 && jpsi_mother_pdgId != -999")
    
    try:
        col_type = df.GetColumnType("jpsi_mother_pdgId")
        moms_result = df_valid.Take[col_type]("jpsi_mother_pdgId")
        moms_list = [m for m in moms_result]
    except Exception as e:
        print(f"Error: {e}")
        return

    total_valid = len(moms_list)
    if total_valid == 0: 
        print("No valid J/psi events found.")
        return

    raw_counts = Counter(moms_list)
    category_counts = Counter()
    
    # Detailed tracking containers
    prompt_hard_details = Counter()
    others_details = Counter()

    for pdg_id, count in raw_counts.items():
        abs_mom = abs(pdg_id)
        
        # 1. Non-Prompt (B-Hadron)
        if (abs_mom // 100 == 5) or (abs_mom // 1000 == 5):
            category_counts["Non-Prompt (B)"] += count
            
        # 2. Feed-down from other charmonium states
        elif abs_mom in [441, 445, 100443, 20443, 10441, 30443, 10443]:
            category_counts["Feed-down"] += count
            
        # 3. Prompt (Hard Interaction)
        elif abs_mom == 21 or abs_mom == 2212:
            category_counts["Prompt (Hard)"] += count
            prompt_hard_details[pdg_id] += count
            
        # 4. Quark Fragmentation
        elif 1 <= abs_mom <= 5:
            category_counts["Quark/Frag"] += count
            
        # 5. Others (Investigation needed)
        else:
            category_counts["Others"] += count
            others_details[pdg_id] += count

    # Print Terminal Diagnostics
    print("\n" + "="*30)
    print("DETAILED PARTICLE BREAKDOWN")
    print("="*30)
    
    print("\n[Prompt (Hard) Composition]")
    if not prompt_hard_details:
        print("None")
    for pid, c in prompt_hard_details.most_common():
        print(f"  - {get_particle_name(pid)} (PDG {pid}): {c} events")

    print("\n[Others Category Composition]")
    if not others_details:
        print("None")
    for pid, c in others_details.most_common():
        print(f"  - {get_particle_name(pid)} (PDG {pid}): {c} events")
    print("="*30 + "\n")

    output_pdf = "Jpsi_Detailed_Analysis_Report.pdf"
    with PdfPages(output_pdf) as pdf:
        # Page 1: Overview Table and Pie Chart
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 12))
        
        # Pie Chart
        if category_counts:
            labels = list(category_counts.keys())
            sizes = list(category_counts.values())
            ax1.pie(sizes, labels=labels, autopct='%1.1f%%', startangle=140)
            ax1.set_title(f"J/psi Origin Breakdown (Total: {total_valid})")

        # Summary Table
        table_data = [["Category", "Count", "Percentage (%)"]]
        for cat, count in category_counts.most_common():
            table_data.append([cat, count, f"{(count/total_valid)*100:.2f}%"])
        
        table = ax2.table(cellText=table_data, loc='center', cellLoc='center')
        table.scale(1, 2)
        ax2.axis('off')
        pdf.savefig()
        plt.close()

        # Page 2: Top 12 Mothers
        plt.figure(figsize=(12, 8))
        top_mothers = raw_counts.most_common(12)
        names = [get_particle_name(it[0]) for it in top_mothers]
        counts = [it[1] for it in top_mothers]
        plt.barh(names, counts, color='orchid', alpha=0.7)
        plt.gca().invert_yaxis()
        plt.title('Top 12 J/psi Mothers (Detailed View)')
        plt.xlabel('Count')
        plt.tight_layout()
        pdf.savefig()
        plt.close()

    print(f"Final report saved to: {output_pdf}")

if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else "*.root")