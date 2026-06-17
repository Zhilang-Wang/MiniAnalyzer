import ROOT
import os
import glob

# --- 1. Paths and Configurations ---
input_files = glob.glob("/eos/user/z/zhilang/JPsiMuMu_Fil-JPsiNo-2MuPtEta_TuneCP5_13p6TeV_pythia8-evtgen/MiniAnalyzer_JPsiMuMu_Signal_2024_v3/260615_074128/*/*.root")
output_folder = "AnalysisPlots_HadronComparison"
tree_path = "MiniAnalyzer/OniaTree"

if not os.path.exists(output_folder):
    os.makedirs(output_folder)

# --- 2. Configuration Matrix for Target Histograms ---
# Format: (variable_suffix, x_axis_title, bins, xmin, xmax, use_log)
hadron_tasks = [
    ("pt",  "Charged Hadron p_{T} [GeV]", 100, 0, 50,  True),
    ("eta", "Charged Hadron #eta",          50, -2.4, 2.4, False)
]

# --- 3. Initialize TChain and Canvas Environment ---
chain = ROOT.TChain(tree_path)
for f in input_files:
    chain.Add(f)

ROOT.gROOT.SetBatch(True)
canvas = ROOT.TCanvas("canvas_hadron", "canvas_hadron", 800, 600)
ROOT.gStyle.SetOptStat(0)  # Relying on TLegend instead of statistics box

# Standard baseline filter matching your C++ logic
base_cut = "nGenJpsi > 0 && jpsi_mother_pdgId != -999 && is_full_jpsi_matched==1"

# --- 4. Loop Over Kinematic Tasks ---
for suffix, title, bins, xmin, xmax, use_log in hadron_tasks:
    print(f"--> Processing overlay plotting for variable: {suffix}...")
    
    # 1. Booking Histograms matching your C++ definitions
    h_all_pf = ROOT.TH1F(f"h_all_pf_{suffix}", f";{title};Events", bins, xmin, xmax)
    h_in_jet = ROOT.TH1F(f"h_in_jet_{suffix}", f";{title};Events", bins, xmin, xmax)
    
    # 2. Projecting the Global Charged Hadrons Branch ("ch_pt", "ch_eta")
    print(f"   Filling Global PF Candidates via branch: ch_{suffix}...")
    chain.Draw(f"ch_{suffix}>>h_all_pf_{suffix}", base_cut, "HIST")
    
    # 3. Projecting the Charged Jet Daughters Branch ("ak8_dau_pt", "ak8_dau_eta")
    # Applying the C++ criteria filter: ak8_dau_charge != 0
    print(f"   Filling Jet Charged Daughters via branch: ak8_dau_{suffix}...")
    jet_dau_cut = f"{base_cut} && ak8_dau_charge != 0"
    chain.Draw(f"ak8_dau_{suffix}>>h_in_jet_{suffix}", jet_dau_cut, "HIST")
    
    # 4. Color Definition and Aesthetic Tweaking
    h_all_pf.SetLineColor(ROOT.kBlack)
    h_all_pf.SetLineWidth(2)
    
    h_in_jet.SetLineColor(ROOT.kCrimson)
    h_in_jet.SetLineWidth(2)
    h_in_jet.SetLineStyle(2)  # Dashed presentation for subset distinction
    
    # 5. Handle Y-Axis Scaling Dynamically
    if use_log:
        canvas.SetLogy(1)
        h_all_pf.SetMinimum(0.5)
        h_in_jet.SetMinimum(0.5)
        max_val = max(h_all_pf.GetMaximum(), h_in_jet.GetMaximum())
        h_all_pf.SetMaximum(max_val * 3.0)
    else:
        canvas.SetLogy(0)
        h_all_pf.SetMinimum(0)
        max_val = max(h_all_pf.GetMaximum(), h_in_jet.GetMaximum())
        h_all_pf.SetMaximum(max_val * 1.2)
        
    # 6. Execute Layer Drawings
    h_all_pf.Draw("HIST")
    h_in_jet.Draw("HIST SAME")
    
    # 7. Generate TLegend Module
    legend = ROOT.TLegend(0.58, 0.74, 0.88, 0.88)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.AddEntry(h_all_pf, "Global PF Candidates (ch)", "l")
    legend.AddEntry(h_in_jet, "Jet Charged Daughters (ak8_dau)", "l")
    legend.Draw()
    
    # 8. Save Plot Structure and Free Memory Elements
    canvas.SaveAs(f"{output_folder}/comparison_hadron_{suffix}.png")
    
    h_all_pf.Delete()
    h_in_jet.Delete()

print(f"\nProcessing complete. Hadron variables mapped cleanly to '{output_folder}'.")
