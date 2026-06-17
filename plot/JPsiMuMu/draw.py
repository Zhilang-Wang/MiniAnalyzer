import ROOT
import os
import glob

# Input and output paths configuration
input_files = glob.glob("/eos/user/z/zhilang/JPsiMuMu_Fil-JPsiNo-2MuPtEta_TuneCP5_13p6TeV_pythia8-evtgen/MiniAnalyzer_JPsiMuMu_Signal_2024_v3/260615_074128/*/*.root")
output_folder = "AnalysisPlots_JPsiMatched"
tree_path = "MiniAnalyzer/OniaTree"

if not os.path.exists(output_folder):
    os.makedirs(output_folder)

# List of variables to plot: (branch_name, x_axis_title, bins, xmin, xmax)
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

    ("gen_reco_dr", "DeltaR JPsi (Gen,Reco)", 100, 0.0, 0.2),

    ("mu1_pt", "Muon 1 p_{T} [GeV]", 100, 0, 50),
    ("mu1_eta", "Muon 1 #eta", 50, -2.4, 2.4),
    ("mu1_phi", "Muon 1 #phi", 50, -3.14, 3.14),
    ("mu1_energy", "Muon 1 Energy [GeV]", 100, 0, 100),

    ("mu2_pt", "Muon 2 p_{T} [GeV]", 100, 0, 50),
    ("mu2_eta", "Muon 2 #eta", 50, -2.4, 2.4),
    ("mu2_phi", "Muon 2 #phi", 50, -3.14, 3.14),
    ("mu2_energy", "Muon 2 Energy [GeV]", 100, 0, 100),
]

# Initialize TChain
chain = ROOT.TChain(tree_path)
for f in input_files:
    chain.Add(f)
    print(f"Adding file: {f}")

# Global ROOT canvas and style settings
ROOT.gROOT.SetBatch(True)
canvas = ROOT.TCanvas("c", "c", 800, 600)
ROOT.gStyle.SetOptStat(111111)

# Definition of the core background filter (Base Cut)
base_cut = "nGenJpsi > 0 && jpsi_mother_pdgId != -999 && is_full_jpsi_matched==1"

# Event loop over plotting variables
for var, title, bins, xmin, xmax in variables:
    print(f"Drawing {var}...")
    h_name = f"h_{var}"
    h = ROOT.TH1F(h_name, f"{var};{title};Events", bins, xmin, xmax)
    
    # Project tree branch into the histogram
    chain.Draw(f"{var}>>{h_name}", base_cut, "HIST")
    
    # Visual aesthetics configuration
    h.SetLineColor(ROOT.kAzure+7)
    h.SetLineWidth(2)
    
    # Force y-axis origin to zero and scale the upper margin by 20%
    h.SetMinimum(0)
    if h.GetMaximum() > 0:
        h.SetMaximum(h.GetMaximum() * 1.2)
    
    # Draw and write to file system
    h.Draw("HIST")
    canvas.SaveAs(f"{output_folder}/{var}.png")

print(f"\nProcessing complete. All plots saved in '{output_folder}'.")