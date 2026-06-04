import ROOT
import os
import glob

input_files = glob.glob("/eos/user/z/zhilang/QCD_Bin-PT-600to800_TuneCP5_13p6TeV_pythia8/MiniAnalyzer_QCD_Run3/260525_041755/0000/*.root")
output_folder = "AnalysisPlots"
tree_path = "MiniAnalyzer/OniaTree"

if not os.path.exists(output_folder):
    os.makedirs(output_folder)

variables = [
    ("mass", "m(#mu#mu) [GeV]", 100, 2.8, 3.4),
    ("pt", "J/#psi p_{T} [GeV]", 100, 0, 100),
    ("eta", "J/#psi #eta", 50, -2.5, 2.5),
    ("phi", "J/#psi #phi", 50, -3.14, 3.14),
    ("energy", "J/#psi Energy [GeV]", 100, 0, 200),
    ("charge", "J/#psi Charge", 5, -5, 5),
    
    ("vNChi2", "Vertex Normalized #chi^{2}", 100, 0, 10),
    ("vProb", "Vertex Probability", 100, 0, 1),
    ("DCA", "DCA [cm]", 100, 0, 0.5),

    ("ppdlPV", "2D Decay Length (PV) [cm]", 100, -0.1, 0.5),
    ("ppdlPV3D", "3D Decay Length (PV) [cm]", 100, -0.1, 0.5),
    ("ppdlErrPV", "2D Decay Length Error (PV) [cm]", 100, 0, 0.05),
    ("ppdlErrPV3D", "3D Decay Length Error (PV) [cm]", 100, 0, 0.05),
    ("cosAlpha", "cos(#alpha)_{2D}", 100, 0.99, 1.0),
    ("cosAlpha3D", "cos(#alpha)_{3D}", 100, 0.99, 1.0),
    
    ("ppdlBS", "2D Decay Length (BS) [cm]", 100, -0.1, 0.5),
    ("ppdlBS3D", "3D Decay Length (BS) [cm]", 100, -0.1, 0.5),

    ("mu1_pt", "Muon 1 p_{T} [GeV]", 100, 0, 50),
    ("mu1_eta", "Muon 1 #eta", 50, -2.4, 2.4),
    ("mu1_phi", "Muon 1 #phi", 50, -3.14, 3.14),
    ("mu1_energy", "Muon 1 Energy [GeV]", 100, 0, 100),

    ("mu2_pt", "Muon 2 p_{T} [GeV]", 100, 0, 50),
    ("mu2_eta", "Muon 2 #eta", 50, -2.4, 2.4),
    ("mu2_phi", "Muon 2 #phi", 50, -3.14, 3.14),
    ("mu2_energy", "Muon 2 Energy [GeV]", 100, 0, 100),
]

chain = ROOT.TChain(tree_path)
for f in input_files:
    chain.Add(f)
    print(f"Adding file: {f}")

ROOT.gROOT.SetBatch(True)
canvas = ROOT.TCanvas("c", "c", 800, 600)
ROOT.gStyle.SetOptStat(111111)

base_cut = "nGenJpsi > 0 && jpsi_mother_pdgId != -999"

for var, title, bins, xmin, xmax in variables:
    print(f"Drawing {var}...")
    h_name = f"h_{var}"
    h = ROOT.TH1F(h_name, f"{var};{title};Events", bins, xmin, xmax)
    
    chain.Draw(f"{var}>>{h_name}", base_cut, "HIST")
    
    h.SetLineColor(ROOT.kAzure+7)
    h.SetLineWidth(2)
    h.Draw("HIST")
    
    canvas.SaveAs(f"{output_folder}/{var}.png")

print(f"\nProcessing complete. Plots saved in '{output_folder}'.")