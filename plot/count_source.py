import ROOT
import os
import glob

# Configuration
# Update the path to point to your ROOT files
file_pattern = "/eos/user/z/zhilang/QCD_Bin-PT-600to800_TuneCP5_13p6TeV_pythia8/MiniAnalyzer_QCD_Run3/260507_072407/0000/*.root" 
# Ensure this matches your TFileService path: "folderName/treeName"
tree_path = "MiniAnalyzer/OniaTree" 

def run_statistics():
    # Initialize counters
    stats = {
        "Total_Reco": 0,
        "Non_Prompt_B": 0,    # From B Hadron decays
        "Prompt_Direct": 0,   # Directly from scattering or charmonium feed-down
        "Unmatched_Fake": 0   # No gen-match found (Background)
    }

    # Load files into TChain
    chain = ROOT.TChain(tree_path)
    files = glob.glob(file_pattern)
    
    if not files:
        print(f"Error: No files found matching {file_pattern}")
        return

    for f in files:
        chain.Add(f)

    entries = chain.GetEntries()
    print(f"Starting analysis on {len(files)} files with {entries} total entries...")

    # Loop over entries
    for i in range(entries):
        chain.GetEntry(i)
        stats["Total_Reco"] += 1

        # 1. Check isFromB flag (Backtracked to PDG ID containing 5)
        if chain.isFromB == 1:
            stats["Non_Prompt_B"] += 1
        
        # 2. Check if it matched a Gen Particle but not from B
        elif chain.jpsi_mother_pdgId != -999:
            stats["Prompt_Direct"] += 1
            
        # 3. No match found within dR < 0.3
        else:
            stats["Unmatched_Fake"] += 1

        if i % 50000 == 0 and i > 0:
            print(f"Processed {i} entries...")

    # Final Summary Output
    print("\n" + "="*40)
    print(f"{'J/psi Source Statistics':^40}")
    print("="*40)
    
    total = stats["Total_Reco"]
    if total == 0:
        print("No candidates found.")
        return

    print(f"Total Reco Candidates: {total:10d}")
    print("-" * 40)
    
    for category in ["Non_Prompt_B", "Prompt_Direct", "Unmatched_Fake"]:
        count = stats[category]
        percentage = (count / total) * 100
        print(f"{category:<20}: {count:10d} ({percentage:6.2f}%)")
    
    print("="*40)

if __name__ == "__main__":
    run_statistics()
