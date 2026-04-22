#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TLorentzVector.h"
#include <vector>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {

    if (argc < 3) {
        cout << "Usage: " << argv[0] << " input.root output.root" << endl;
        return 1;
    }

    TFile* inputFile = TFile::Open(argv[1]);
    TTree* tree = (TTree*)inputFile->Get("MiniAnalyzer/OniaTree");

    float jpsi_pt, jpsi_eta, jpsi_phi, jpsi_energy;
    tree->SetBranchAddress("pt",      &jpsi_pt);
    tree->SetBranchAddress("eta",     &jpsi_eta);
    tree->SetBranchAddress("phi",     &jpsi_phi);
    tree->SetBranchAddress("energy",  &jpsi_energy);

    float mu1_pt, mu1_eta, mu1_phi, mu1_energy;
    float mu2_pt, mu2_eta, mu2_phi, mu2_energy;
    tree->SetBranchAddress("mu1_pt",    &mu1_pt);
    tree->SetBranchAddress("mu1_eta",   &mu1_eta);
    tree->SetBranchAddress("mu1_phi",   &mu1_phi);
    tree->SetBranchAddress("mu1_energy",&mu1_energy);
    tree->SetBranchAddress("mu2_pt",    &mu2_pt);
    tree->SetBranchAddress("mu2_eta",   &mu2_eta);
    tree->SetBranchAddress("mu2_phi",   &mu2_phi);
    tree->SetBranchAddress("mu2_energy",&mu2_energy);

    vector<float>* ak8_jet_pt      = nullptr;
    vector<float>* ak8_jet_eta     = nullptr;
    vector<float>* ak8_jet_phi     = nullptr;
    vector<float>* ak8_jet_dr_jpsi = nullptr;

    vector<float>* ak8_dau_pt      = nullptr;
    vector<float>* ak8_dau_eta     = nullptr;
    vector<float>* ak8_dau_phi     = nullptr;
    vector<float>* ak8_dau_energy  = nullptr;
    vector<int>*   ak8_dau_charge  = nullptr;

    tree->SetBranchAddress("ak8_jet_pt",      &ak8_jet_pt);
    tree->SetBranchAddress("ak8_jet_eta",     &ak8_jet_eta);
    tree->SetBranchAddress("ak8_jet_phi",     &ak8_jet_phi);
    tree->SetBranchAddress("ak8_jet_dr_jpsi", &ak8_jet_dr_jpsi);

    tree->SetBranchAddress("ak8_dau_pt",      &ak8_dau_pt);
    tree->SetBranchAddress("ak8_dau_eta",     &ak8_dau_eta);
    tree->SetBranchAddress("ak8_dau_phi",     &ak8_dau_phi);
    tree->SetBranchAddress("ak8_dau_energy",  &ak8_dau_energy);
    tree->SetBranchAddress("ak8_dau_charge",  &ak8_dau_charge);

    vector<float>* ch_pt     = nullptr;
    vector<float>* ch_eta    = nullptr;
    vector<float>* ch_phi    = nullptr;
    vector<float>* ch_energy = nullptr;

    tree->SetBranchAddress("ch_pt",     &ch_pt);
    tree->SetBranchAddress("ch_eta",    &ch_eta);
    tree->SetBranchAddress("ch_phi",    &ch_phi);
    tree->SetBranchAddress("ch_energy", &ch_energy);

    TH1D* h_qec_charged = new TH1D("qec_all_charged", "QEC: J/#psi + all charged hadrons;cos#chi", 20, -1, 1);
    TH1D* h_qec_jet_out = new TH1D("qec_jet_out", "QEC: J/#psi outside AK8 jet;cos#chi", 20, -1, 1);
    TH1D* h_qec_jet_in  = new TH1D("qec_jet_in",  "QEC: J/#psi inside AK8 jet;cos#chi", 20, -1, 1);

    TH1D* h_qec_jet_in_charged  = new TH1D("qec_jet_in_charged",  "QEC: J/#psi inside AK8 jet (charged);cos#chi", 20, -1, 1);
    TH1D* h_qec_jet_in_neutral  = new TH1D("qec_jet_in_neutral",  "QEC: J/#psi inside AK8 jet (neutral);cos#chi", 20, -1, 1);
    TH1D* h_qec_jet_all         = new TH1D("qec_jet_all",         "QEC: J/#psi + AK8 jet (all);cos#chi", 20, -1, 1);

    int nTotal = 0;
    int nInJet = 0;
    int nOutJet = 0;

    Long64_t nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);
        nTotal++;

        TLorentzVector jpsi;
        jpsi.SetPtEtaPhiE(jpsi_pt, jpsi_eta, jpsi_phi, jpsi_energy);
        TLorentzVector mu1, mu2;
        mu1.SetPtEtaPhiE(mu1_pt, mu1_eta, mu1_phi, mu1_energy);
        mu2.SetPtEtaPhiE(mu2_pt, mu2_eta, mu2_phi, mu2_energy);

        double mJpsi = jpsi.M();
        TVector3 boost = -jpsi.BoostVector();

        if (ch_pt) {
            for (size_t j = 0; j < ch_pt->size(); ++j) {
                TLorentzVector h;
                h.SetPtEtaPhiE(ch_pt->at(j), ch_eta->at(j), ch_phi->at(j), ch_energy->at(j));
                h.Boost(boost);
                double c = jpsi.Vect().Dot(h.Vect()) / (jpsi.Vect().Mag() * h.Vect().Mag());
                h_qec_charged->Fill(c, h.E() / mJpsi);
            }
        }

        bool jpsiInAnyJet = false;
        if (ak8_jet_pt) {
            for (size_t j = 0; j < ak8_jet_pt->size(); j++) {
                if (ak8_jet_dr_jpsi->at(j) < 0.8) {
                    jpsiInAnyJet = true;
                    break;
                }
            }
        }

        if (jpsiInAnyJet)
            nInJet++;
        else
            nOutJet++;

        if (ak8_dau_pt && ak8_dau_charge) {
            for (size_t idau = 0; idau < ak8_dau_pt->size(); ++idau) {
                TLorentzVector h;
                h.SetPtEtaPhiE(ak8_dau_pt->at(idau), ak8_dau_eta->at(idau), ak8_dau_phi->at(idau), ak8_dau_energy->at(idau));

                double dr1 = mu1.DeltaR(h);
                double dr2 = mu2.DeltaR(h);
                if (dr1 < 0.01 || dr2 < 0.01) continue;

                h.Boost(boost);
                double c = jpsi.Vect().Dot(h.Vect()) / (jpsi.Vect().Mag() * h.Vect().Mag());
                double w = h.E() / mJpsi;
                h_qec_jet_all->Fill(c, w);
            }
        }

        if (jpsiInAnyJet && ak8_dau_pt && ak8_dau_charge) {
            for (size_t idau = 0; idau < ak8_dau_pt->size(); ++idau) {
                TLorentzVector h;
                h.SetPtEtaPhiE(ak8_dau_pt->at(idau), ak8_dau_eta->at(idau), ak8_dau_phi->at(idau), ak8_dau_energy->at(idau));

                double dr1 = mu1.DeltaR(h);
                double dr2 = mu2.DeltaR(h);
                if (dr1 < 0.01 || dr2 < 0.01) continue;

                int charge = ak8_dau_charge->at(idau);
                h.Boost(boost);
                double c = jpsi.Vect().Dot(h.Vect()) / (jpsi.Vect().Mag() * h.Vect().Mag());
                double w = h.E() / mJpsi;

                h_qec_jet_in->Fill(c, w);

                if (charge != 0) {
                    h_qec_jet_in_charged->Fill(c, w);
                } else {
                    h_qec_jet_in_neutral->Fill(c, w);
                }
            }
        } else if (ak8_dau_pt) {
            for (size_t idau = 0; idau < ak8_dau_pt->size(); ++idau) {
                TLorentzVector h;
                h.SetPtEtaPhiE(ak8_dau_pt->at(idau), ak8_dau_eta->at(idau), ak8_dau_phi->at(idau), ak8_dau_energy->at(idau));

                h.Boost(boost);
                double c = jpsi.Vect().Dot(h.Vect()) / (jpsi.Vect().Mag() * h.Vect().Mag());
                double w = h.E() / mJpsi;
                h_qec_jet_out->Fill(c, w);
            }
        }
    }

    cout << "========================================" << endl;
    cout << "Total    : " << nTotal << endl;
    cout << "In AK8   : " << nInJet << endl;
    cout << "Out AK8  : " << nOutJet << endl;
    cout << "Frac in  : " << (double)nInJet / nTotal * 100.0 << " %" << endl;
    cout << "Frac out : " << (double)nOutJet / nTotal * 100.0 << " %" << endl;
    cout << "========================================" << endl;

    TFile* fout = new TFile(argv[2], "RECREATE");
    h_qec_charged->Write();
    h_qec_jet_out->Write();
    h_qec_jet_in->Write();
    h_qec_jet_in_charged->Write();
    h_qec_jet_in_neutral->Write();
    h_qec_jet_all->Write();
    fout->Close();
    inputFile->Close();

    return 0;
}