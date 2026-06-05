import ROOT
import os
import glob

# --- 1. Paths ---
input_files = glob.glob("/eos/user/z/zhilang/JPsiMuMu_Fil-JPsiNo-2MuPtEta_TuneCP5_13p6TeV_pythia8-evtgen/MiniAnalyzer_JPsiMuMu_Signal_2024/260604_130150/0001/*.root")
output_folder = "ComparisonPlots_NewCuts"
tree_path = "MiniAnalyzer/OniaTree"

if not os.path.exists(output_folder):
    os.makedirs(output_folder)

# --- 2. Variables & Correct High Energy Binning ---
# 1) 把 J/psi pt 的横轴上限从 100 提高到 1000 以匹配 QCD-PT-600to800 样本
# 2) 对 TMath::Abs(jpsi_mother_pdgId) 判定改用 TMath::Abs 以防止 ROOT 解析 Bug
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

# --- 3. Cut Definitions for Production Mechanisms (Perfect Synchronization) ---

# 强制所有分类都必须通过 Gen 信号有效性判定（同步 df_valid）
base_cut = "nGenJpsi > 0 && jpsi_mother_pdgId != -999"

# 1. Base Classifications (Wrapped with base_cut)
cut_total   = f"{base_cut}"
cut_prompt  = f"{base_cut} && isFromB == 0"
cut_np      = f"{base_cut} && isFromB == 1"

# 2. Feed-down Prompt (Strict brackets to protect OR logic)
fd_ids = [441, 445, 100443, 20443, 10441, 30443, 10443]
fd_condition = " || ".join([f"TMath::Abs(jpsi_mother_pdgId) == {id_}" for id_ in fd_ids])
cut_fd      = f"{base_cut} && isFromB == 0 && ({fd_condition})"

# 3. Prompt (Hard Interaction)
cut_hard    = f"{base_cut} && isFromB == 0 && !({fd_condition}) && (TMath::Abs(jpsi_mother_pdgId) == 21 || TMath::Abs(jpsi_mother_pdgId) == 2212)"

# 4. Quark Fragmentation
cut_frag    = f"{base_cut} && isFromB == 0 && !({fd_condition}) && !(TMath::Abs(jpsi_mother_pdgId) == 21 || TMath::Abs(jpsi_mother_pdgId) == 2212) && (TMath::Abs(jpsi_mother_pdgId) >= 1 && TMath::Abs(jpsi_mother_pdgId) <= 5)"

# 5. Others
cut_others  = f"{base_cut} && isFromB == 0 && !({fd_condition}) && !(TMath::Abs(jpsi_mother_pdgId) == 21 || TMath::Abs(jpsi_mother_pdgId) == 2212) && !(TMath::Abs(jpsi_mother_pdgId) >= 1 && TMath::Abs(jpsi_mother_pdgId) <= 5)"

# --- 4. Plotting Loop ---
for var_expr, title, bins, xmin, xmax in variables:
    file_name = var_expr.replace("/", "_").replace("*", "_").replace("(", "").replace(")", "").replace("0.5_log", "rapidity")
    
    # Initialize histograms
    h_total  = ROOT.TH1F(f"h_{file_name}_total",  f";{title};Fraction of Total Events", bins, xmin, xmax)
    h_prompt = ROOT.TH1F(f"h_{file_name}_prompt", f";{title};Fraction of Total Events", bins, xmin, xmax)
    h_np     = ROOT.TH1F(f"h_{file_name}_np",     f";{title};Fraction of Total Events", bins, xmin, xmax)
    h_fd     = ROOT.TH1F(f"h_{file_name}_fd",     f";{title};Fraction of Total Events", bins, xmin, xmax)
    h_hard   = ROOT.TH1F(f"h_{file_name}_hard",   f";{title};Fraction of Total Events", bins, xmin, xmax)
    h_frag   = ROOT.TH1F(f"h_{file_name}_frag",   f";{title};Fraction of Total Events", bins, xmin, xmax)
    h_others = ROOT.TH1F(f"h_{file_name}_others", f";{title};Fraction of Total Events", bins, xmin, xmax)

    # Filling histograms
    chain.Draw(f"{var_expr}>>h_{file_name}_total",  cut_total,  "goff")
    chain.Draw(f"{var_expr}>>h_{file_name}_prompt", cut_prompt, "goff")
    chain.Draw(f"{var_expr}>>h_{file_name}_np",     cut_np,     "goff")
    chain.Draw(f"{var_expr}>>h_{file_name}_fd",     cut_fd,     "goff")
    chain.Draw(f"{var_expr}>>h_{file_name}_hard",   cut_hard,   "goff")
    chain.Draw(f"{var_expr}>>h_{file_name}_frag",   cut_frag,   "goff")
    chain.Draw(f"{var_expr}>>h_{file_name}_others", cut_others, "goff")

    # Global Area Normalization
    norm_factor = h_total.Integral()
    if norm_factor > 0:
        for h in [h_total, h_prompt, h_np, h_fd, h_hard, h_frag, h_others]:
            h.Scale(1.0 / norm_factor)

    # --- Styling ---
    h_total.SetLineColor(ROOT.kBlack)
    h_total.SetLineWidth(3)
    
    h_prompt.SetLineColor(ROOT.kAzure+7)
    h_prompt.SetLineWidth(2)
    
    h_np.SetLineColor(ROOT.kRed-4)
    h_np.SetLineWidth(2)

    # Sub-components styled as clear dashed/dotted lines
    h_fd.SetLineColor(ROOT.kGreen+3)
    h_fd.SetLineWidth(2)
    h_fd.SetLineStyle(7) # Long dash
    
    h_hard.SetLineColor(ROOT.kOrange+7)
    h_hard.SetLineWidth(2)
    h_hard.SetLineStyle(2) # Dashed
    
    h_frag.SetLineColor(ROOT.kMagenta+1)
    h_frag.SetLineWidth(2)
    h_frag.SetLineStyle(3) # Dotted

    h_others.SetLineColor(ROOT.kGray+2)
    h_others.SetLineWidth(2)
    h_others.SetLineStyle(5) # Dash-dot

    # Set dynamic Y maximum
    max_val = h_total.GetMaximum()
    h_total.SetMaximum(max_val * 1.6)
    
    # Draw Layer Stack
    h_total.Draw("HIST")
    h_prompt.Draw("HIST SAME")
    h_np.Draw("HIST SAME")
    h_fd.Draw("HIST SAME")
    h_hard.Draw("HIST SAME")
    h_frag.Draw("HIST SAME")
    h_others.Draw("HIST SAME")

    # Legend Configuration
    legend = ROOT.TLegend(0.52, 0.60, 0.88, 0.88)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.AddEntry(h_total,  "Total Sample", "l")
    legend.AddEntry(h_prompt, "Prompt J/#psi (Total)", "l")
    legend.AddEntry(h_np,     "Non-prompt J/#psi", "l")
    legend.AddEntry(h_fd,     "  - Feed-down", "l")
    legend.AddEntry(h_hard,   "  - Prompt (Hard)", "l")
    legend.AddEntry(h_frag,   "  - Quark/Frag", "l")
    legend.AddEntry(h_others, "  - Others", "l")
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