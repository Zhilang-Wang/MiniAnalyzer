import ROOT
import os
import glob
import re

# ==============================================================================
# 1. DIRECTORY CONFIGURATION
# ==============================================================================
dir_no   = "/eos/user/z/zhilang/JPsiMuMu_Fil-JPsiNo-2MuPtEta_TuneCP5_13p6TeV_pythia8-evtgen/MiniAnalyzer_JPsiMuMu_Signal_2024_No_vProb/260609_074929/"
dir_with = "/eos/user/z/zhilang/JPsiMuMu_Fil-JPsiNo-2MuPtEta_TuneCP5_13p6TeV_pythia8-evtgen/MiniAnalyzer_JPsiMuMu_Signal_2024_with_vProb/260607_151455"

output_dir = "Professor_Report_Plots"
if not os.path.exists(output_dir):
    os.makedirs(output_dir)

def get_job_id(filepath):
    match = re.search(r'_(\d+)\.root', os.path.basename(filepath))
    return int(match.group(1)) if match else None

# ==============================================================================
# 2. FILE ALIGNMENT VIA DEEP RECURSIVE SCANNING
# ==============================================================================
print("--> Scanning EOS directories for root files...")

files_no_list   = glob.glob(os.path.join(dir_no, "**/*.root"), recursive=True)
files_with_list = glob.glob(os.path.join(dir_with, "**/*.root"), recursive=True)

files_no   = {get_job_id(f): f for f in files_no_list if get_job_id(f) is not None}
files_with = {get_job_id(f): f for f in files_with_list if get_job_id(f) is not None}

common_ids = sorted(list(set(files_no.keys()) & set(files_with.keys())))

print("="*70)
print(f"--> Total successful files in No_vProb  : {len(files_no)}")
print(f"--> Total successful files in with_vProb: {len(files_with)}")
print(f"--> [CONTROL VARIABLE MATCH] Selected {len(common_ids)} identical jobs for analysis.")
print("="*70 + "\n")

if len(common_ids) == 0:
    print("ERROR: No matching Job IDs found between the two directories. Check your paths.")
    exit(1)

chain_no = ROOT.TChain("MiniAnalyzer/OniaTree")
chain_with = ROOT.TChain("MiniAnalyzer/OniaTree")

for jid in common_ids:
    chain_no.Add(files_no[jid])
    chain_with.Add(files_with[jid])

# ==============================================================================
# 3. HISTOGRAM FILLING AND PURITY ANALYSIS
# ==============================================================================
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

def fill_and_analyze(chain, label):
    h_all = ROOT.TH1F(f"h_all_{label}", "", 60, 2.8, 3.4)
    h_mat = ROOT.TH1F(f"h_mat_{label}", "", 60, 2.8, 3.4)
    
    # 1. 总样本：只要满足基本的运动学挑选
    baseline_all = "nGenJpsi > 0 && jpsi_mother_pdgId != -999"
    # 2. 真信号样本：必须额外满足是真匹配
    baseline_mat = "nGenJpsi > 0 && jpsi_mother_pdgId != -999 && is_gen_matched == 1"
    
    print(f"--> Processing and projecting tree for {label}...") 
    chain.Draw(f"mass>>h_all_{label}", baseline_all, "HIST")
    chain.Draw(f"mass>>h_mat_{label}", baseline_mat, "HIST")

    total_reco = h_all.Integral()
    matched_reco = h_mat.Integral()
    
    # 纯度定义：挑出来的总样本里，到底有多少比例是货真价实的 J/psi
    purity = (matched_reco / total_reco * 100.0) if total_reco > 0 else 0.0
    
    return h_all, h_mat, total_reco, matched_reco, purity

h_all_no, h_mat_no, tot_no, mat_no, pur_no = fill_and_analyze(chain_no, "No_vProb")
h_all_with, h_mat_with, tot_with, mat_with, pur_with = fill_and_analyze(chain_with, "with_vProb")

# ==============================================================================
# 4. PRINT SCIENTIFIC VERDICT REPORT (PURELY PURITY & RELATIVE EFFICIENCY)
# ==============================================================================
# 终极相对效率：用加限制后的真信号绝对个数，除以没加限制前的真信号绝对个数
relative_signal_efficiency = (mat_with / mat_no * 100.0) if mat_no > 0 else 0.0
signal_loss = 100.0 - relative_signal_efficiency
bkg_rejected = (tot_no - mat_no) - (tot_with - mat_with)

print("\n" + "="*70)
print("                       PHYSICS PERFORMANCE REPORT                       ")
print("="*70)
print(f"Workflow [No_vProb] (Before Cut):")
print(f"  --> Total Selected Candidates  : {tot_no:.0f}")
print(f"  --> TRUE Gen-Matched J/psi Signals (mat_no): {mat_no:.0f}")
print(f"  --> J/psi Probability (Purity)             : {pur_no:.4f}%")
print("-"*70)
print(f"Workflow [with_vProb] (After Cut):")
print(f"  --> Total Selected Candidates  : {tot_with:.0f}")
print(f"  --> TRUE Gen-Matched J/psi Signals (mat_with): {mat_with:.0f}")
print(f"  --> J/psi Probability (Purity)             : {pur_with:.4f}%")
print("="*70)
print(f"Conclusion for Professor:")
print(f"  1. The vProb cut increased J/psi Probability (Purity) by {pur_with - pur_no:.4f}%.")
print(f"  2. It successfully killed {bkg_rejected:.0f} random combinatorial background pairs.")
print(f"  3. [RELATIVE SIGNAL EFFICIENCY]: {relative_signal_efficiency:.4f}%")
print(f"     (True J/psi signal loss is only {signal_loss:.4f}%!)")
print("="*70 + "\n")

# ==============================================================================
# 5. GRAPHICAL OVERLAY GENERATION (PLOT: MASS COMPARISON)
# ==============================================================================
print("--> Generating overlaid mass spectrum plots...")
canvas = ROOT.TCanvas("c_combined", "Control Variable Study", 900, 700)

h_all_no.SetLineColor(ROOT.kRed-4)
h_all_no.SetLineWidth(2)
h_all_no.SetTitle("Mass Spectrum Comparison (Total Reco Samples);m(#mu#mu) [GeV];Events")

h_all_with.SetLineColor(ROOT.kAzure+7)
h_all_with.SetLineWidth(2)
h_all_with.SetFillColorAlpha(ROOT.kAzure+7, 0.1)

h_all_no.SetMinimum(0)
h_all_no.SetMaximum(max(h_all_no.GetMaximum(), h_all_with.GetMaximum()) * 1.3)

h_all_no.Draw("HIST")
h_all_with.Draw("HIST SAME")

legend = ROOT.TLegend(0.40, 0.70, 0.88, 0.88)
legend.SetBorderSize(0)
legend.SetFillStyle(0)
legend.AddEntry(h_all_no, f"No vProb Cut (Purity: {pur_no:.2f}%)", "l")
# 图例更新：直接展示纯度和向老师证明没伤信号的相对效率
legend.AddEntry(h_all_with, f"With vProb Cut (Purity: {pur_with:.2f}%, Rel_Eff: {relative_signal_efficiency:.2f}%)", "lf")
legend.Draw()

latex = ROOT.TLatex()
latex.SetNDC()
latex.SetTextSize(0.028)
latex.SetTextColor(ROOT.kGray+2)
latex.DrawLatex(0.15, 0.92, f"Dataset aligned: Using exactly {len(common_ids)} overlapping sub-jobs")

plot_path = f"{output_dir}/Strict_Control_Variable_Purity_Comparison.png"
canvas.SaveAs(plot_path)

# ==============================================================================
# 6. DELTA R GENERATION
# ==============================================================================
print("--> Generating 图一: deltaR (Gen-Reco) Comparison Plot...")
canvas_dr = ROOT.TCanvas("c_dr", "DeltaR Study", 900, 700)
canvas_dr.SetLogy()

h_dr_no = ROOT.TH1F("h_dr_no", "Matching Distance #DeltaR(Gen, Reco);#DeltaR;Normalized Entries", 50, 0, 0.2)
h_dr_with = ROOT.TH1F("h_dr_with", "Matching Distance #DeltaR(Gen, Reco);#DeltaR;Normalized Entries", 50, 0, 0.2)

chain_no.Draw("gen_reco_dr>>h_dr_no", "nGenJpsi > 0 && jpsi_mother_pdgId != -999", "HIST")
chain_with.Draw("gen_reco_dr>>h_dr_with", "nGenJpsi > 0 && jpsi_mother_pdgId != -999", "HIST")

if h_dr_no.Integral() > 0: h_dr_no.Scale(1.0 / h_dr_no.Integral())
if h_dr_with.Integral() > 0: h_dr_with.Scale(1.0 / h_dr_with.Integral())

h_dr_no.SetLineColor(ROOT.kRed-4)
h_dr_no.SetLineWidth(2)
h_dr_with.SetLineColor(ROOT.kAzure+7)
h_dr_with.SetLineWidth(2)

h_dr_no.SetMaximum(max(h_dr_no.GetMaximum(), h_dr_with.GetMaximum()) * 2.0)
h_dr_no.Draw("HIST")
h_dr_with.Draw("HIST SAME")

line_dr = ROOT.TLine(0.1, canvas_dr.GetUymin(), 0.1, h_dr_no.GetMaximum() * 0.5)
line_dr.SetLineStyle(3)
line_dr.SetLineColor(ROOT.kGray+1)
line_dr.SetLineWidth(2)
line_dr.Draw()

leg_dr = ROOT.TLegend(0.45, 0.75, 0.88, 0.88)
leg_dr.SetBorderSize(0)
leg_dr.SetFillStyle(0)
leg_dr.AddEntry(h_dr_no, "No vProb Cut (Loose)", "l")
leg_dr.AddEntry(h_dr_with, "With vProb Cut (Strict)", "l")
leg_dr.Draw()

dr_plot_path = f"{output_dir}/DeltaR_Gen_Reco_Comparison.png"
canvas_dr.SaveAs(dr_plot_path)
print(f"--> DeltaR Plot saved to: {dr_plot_path}")