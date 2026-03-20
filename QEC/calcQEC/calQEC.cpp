#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TLorentzVector.h"
#include <vector>
#include <iostream>


int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <input ROOT file> <output ROOT file>" << std::endl;
        return 1;
    }

    TString inputFileName = argv[1];
    TString outputFileName = argv[2];

    TFile* inputFile = TFile::Open(inputFileName);
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "Error opening input file!" << std::endl;
        return 1;
    }

    TTree* tree = (TTree*)inputFile->Get("MiniAnalyzer/OniaTree");
    if (!tree) {
        std::cerr << "Error: TTree 'OniaTree' not found!" << std::endl;
        return 1;
    }

    // J/psi branches
    float jpsi_pt, jpsi_eta, jpsi_phi, jpsi_energy;
    tree->SetBranchAddress("energy", &jpsi_energy);
    tree->SetBranchAddress("pt", &jpsi_pt);
    tree->SetBranchAddress("eta", &jpsi_eta);
    tree->SetBranchAddress("phi", &jpsi_phi);

    // charge hadron baranches
    std::vector<float>* ch_pt = nullptr;
    std::vector<float>* ch_eta = nullptr;
    std::vector<float>* ch_phi = nullptr;
    std::vector<float>* ch_energy = nullptr;
    tree->SetBranchAddress("ch_pt", &ch_pt);
    tree->SetBranchAddress("ch_eta", &ch_eta);
    tree->SetBranchAddress("ch_phi", &ch_phi);
    tree->SetBranchAddress("ch_energy", &ch_energy);

    // construct histograms
    TH1D* h_coschi = new TH1D("coschi", "cos#chi;cos#chi;Counts", 20, -1, 1); 

    Long64_t nEntries = tree->GetEntries();
    for (Long64_t i=0; i<nEntries; ++i) {
        tree->GetEntry(i);

        // construct J/psi TLorentzVector
        TLorentzVector jpsi;
        jpsi.SetPtEtaPhiE(jpsi_pt, jpsi_eta, jpsi_phi, jpsi_energy);

        // construct charge hadron TLorentzVector
        std::vector<TLorentzVector> chs;
        for (size_t j=0; j<ch_pt->size(); ++j) {
            TLorentzVector ch;
            ch.SetPtEtaPhiE(ch_pt->at(j), ch_eta->at(j), ch_phi->at(j), ch_energy->at(j));
            chs.push_back(ch);
        }

        // Boost to J/psi rest frame
        TVector3 boost_vector = -jpsi.BoostVector();
        for (auto &ch : chs) ch.Boost(boost_vector);

        // Calculate coschi and E/M
        for (auto &ch : chs) {
            double coschi = jpsi.Vect().Dot(ch.Vect()) / (jpsi.Vect().Mag() * ch.Vect().Mag());
            double ec = ch.E() / jpsi.M();
            h_coschi->Fill(coschi, ec);
        }
    }


    TFile* outputFile = new TFile(outputFileName, "RECREATE");
    h_coschi->Write();
    outputFile->Close();

    inputFile->Close();

    std::cout << "QEC calculation finished. Output saved to " << outputFileName << std::endl;

    return 0;
}
