import ROOT
import os
import glob

# --- 1. Paths ---
input_files = glob.glob("/eos/user/z/zhilang/JPsiMuMu_Fil-JPsiNo-2MuPtEta_TuneCP5_13p6TeV_pythia8-evtgen/MiniAnalyzer_JPsiMuMu_Signal_2024_v3/260615_074128/*/*.root")
output_folder = "ComparisonPlots_JPsiMatched"
tree_path = "MiniAnalyzer/OniaTree"

if not os.path.exists(output_folder):
    os.makedirs(output_folder)

# --- 2. Variables & Correct High Energy Binning ---
variables = [
    ("pt", "J/#psi p_{T} [GeV]", 100, 0, 120), 
    ("eta", "J/#psi #eta", 50, -2.5, 2.5),
    ("phi", "J/#psi #phi", 50, -3.14, 3.14),
    ("0.5*log((energy + pt*sinh(eta))/(energy - pt*sinh(eta)))", "J/#psi Rapidity y", 50, -2.5, 2.5),
    ("mass", "m(#mu#mu) [GeV]", 120, 2.8, 3.4),
    ("ppdlPV", "2D Decay Length [cm]", 100, -0.05, 0.2),
]

chain = ROOT.TChain(tree_path)
for f in input_files:
    chain.Add(f)

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
canvas = ROOT.TCanvas("c", "c", 800, 600)

# --- 3. Cut Definitions for Production Mechanisms (Restructured) ---

# Enforce gen signal validity and perfect matching requirements on all categories
base_cut = "nGenJpsi > 0 && jpsi_mother_pdgId != -999 && is_full_jpsi_matched == 1"

# Base Classifications
cut_total   = f"{base_cut}"
cut_prompt  = f"{base_cut} && isFromB == 0"
cut_np      = f"{base_cut} && isFromB == 1"

# Feed-down Charmonia IDs
fd_ids = [441, 445, 100443, 20443, 10441, 30443, 10443]
fd_condition = " || ".join([f"TMath::Abs(jpsi_mother_pdgId) == {id_}" for id_ in fd_ids])

# 1. Feed-down category (from chi_c, psi(2S), etc.)
cut_fd      = f"{base_cut} && isFromB == 0 && ({fd_condition})"

# 2. Direct category (Merged: Hard Interaction + Quark/Frag + Others)
cut_direct  = f"{base_cut} && isFromB == 0 && !({fd_condition})"

# --- 4. Plotting Loop ---
for var_expr, title, bins, xmin, xmax in variables:
    file_name = var_expr.replace("/", "_").replace("*", "_").replace("(", "").replace(")", "").replace("0.5_log", "rapidity")
    
    # Initialize histograms for the consolidated categories
    h_total  = ROOT.TH1F(f"h_{file_name}_total",  f";{title};Fraction of Total Events", bins, xmin, xmax)
    h_prompt = ROOT.TH1F(f"h_{file_name}_prompt", f";{title};Fraction of Total Events", bins, xmin, xmax)
    h_np     = ROOT.TH1F(f"h_{file_name}_np",     f";{title};Fraction of Total Events", bins, xmin, xmax)
    h_fd     = ROOT.TH1F(f"h_{file_name}_fd",     f";{title};Fraction of Total Events", bins, xmin, xmax)
    h_direct = ROOT.TH1F(f"h_{file_name}_direct", f";{title};Fraction of Total Events", bins, xmin, xmax)

    # Filling histograms
    chain.Draw(f"{var_expr}>>h_{file_name}_total",  cut_total,  "goff")
    chain.Draw(f"{var_expr}>>h_{file_name}_prompt", cut_prompt, "goff")
    chain.Draw(f"{var_expr}>>h_{file_name}_np",     cut_np,     "goff")
    chain.Draw(f"{var_expr}>>h_{file_name}_fd",     cut_fd,     "goff")
    chain.Draw(f"{var_expr}>>h_{file_name}_direct", cut_direct, "goff")

    # Global Area Normalization
    norm_factor = h_total.Integral()
    if norm_factor > 0:
        for h in [h_total, h_prompt, h_np, h_fd, h_direct]:
            h.Scale(1.0 / norm_factor)

    # --- Styling ---
    h_total.SetLineColor(ROOT.kBlack)
    h_total.SetLineWidth(3)
    
    h_prompt.SetLineColor(ROOT.kAzure+7)
    h_prompt.SetLineWidth(2)
    
    h_np.SetLineColor(ROOT.kRed-4)
    h_np.SetLineWidth(2)

    # Sub-components styling
    h_direct.SetLineColor(ROOT.kOrange+7)
    h_direct.SetLineWidth(2)
    h_direct.SetLineStyle(2) # Dashed line for Direct Prompt component
    
    h_fd.SetLineColor(ROOT.kGreen+3)
    h_fd.SetLineWidth(2)
    h_fd.SetLineStyle(7) # Long dash line for Feed-down component

    # Set dynamic Y maximum
    max_val = h_total.GetMaximum()
    h_total.SetMaximum(max_val * 1.6)
    
    # Draw Layer Stack
    h_total.Draw("HIST")
    h_prompt.Draw("HIST SAME")
    h_np.Draw("HIST SAME")
    h_direct.Draw("HIST SAME")
    h_fd.Draw("HIST SAME")

    # Legend Configuration
    legend = ROOT.TLegend(0.52, 0.65, 0.88, 0.88)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.AddEntry(h_total,  "Total Sample", "l")
    legend.AddEntry(h_prompt, "Prompt J/#psi (Total)", "l")
    legend.AddEntry(h_np,     "Non-prompt J/#psi", "l")
    legend.AddEntry(h_direct, "  - Direct", "l")
    legend.AddEntry(h_fd,     "  - Feed-down", "l")
    legend.Draw()

    # Log scale settings
    if any(x in var_expr for x in ["pt", "ppdl"]):
        canvas.SetLogy(1)
        h_total.SetMinimum(max(1e-6, max_val * 1e-5))
    else:
        canvas.SetLogy(0)
        h_total.SetMinimum(0)

    canvas.SaveAs(f"{output_folder}/{file_name}_global_norm.png")

print(f"Perfect matched exclusive plots saved in '{output_folder}'.")