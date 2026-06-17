#include "TFile.h"
#include "TChain.h"
#include "TH1D.h"
#include "TLorentzVector.h"
#include <vector>
#include <iostream>
#include <set>
#include <cmath>

using namespace std;

int main(int argc, char* argv[]) {

    if (argc < 3) {
        cout << "Usage: " << argv[0] << " \"input_pattern*.root\" output.root" << endl;
        return 1;
    }

    TChain* tree = new TChain("MiniAnalyzer/OniaTree");
    int nFiles = tree->Add(argv[1]);

    if (nFiles == 0) {
        cout << "Error: No files found matching pattern: " << argv[1] << endl;
        delete tree;
        return 1;
    }
    cout << "Successfully added " << nFiles << " files to the chain." << endl;

    // --- Branch Variables ---
    float jpsi_pt, jpsi_eta, jpsi_phi, jpsi_energy;
    tree->SetBranchAddress("pt",      &jpsi_pt);
    tree->SetBranchAddress("eta",     &jpsi_eta);
    tree->SetBranchAddress("phi",     &jpsi_phi);
    tree->SetBranchAddress("energy",  &jpsi_energy);

    int isFromB = 0;
    int jpsi_mother_pdgId = 0; 
    int is_full_jpsi_matched = 0; 
    tree->SetBranchAddress("isFromB", &isFromB);
    tree->SetBranchAddress("jpsi_mother_pdgId", &jpsi_mother_pdgId);
    tree->SetBranchAddress("is_full_jpsi_matched", &is_full_jpsi_matched); 

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
    vector<int>* ak8_dau_charge  = nullptr;

    tree->SetBranchAddress("ak8_jet_pt",      &ak8_jet_pt);
    tree->SetBranchAddress("ak8_jet_eta",     &ak8_jet_eta);
    tree->SetBranchAddress("ak8_jet_phi",     &ak8_jet_phi);
    tree->SetBranchAddress("ak8_jet_dr_jpsi", &ak8_jet_dr_jpsi);
    tree->SetBranchAddress("ak8_dau_pt",      &ak8_dau_pt);
    tree->SetBranchAddress("ak8_dau_eta",     &ak8_dau_eta);
    tree->SetBranchAddress("ak8_dau_phi",     &ak8_dau_phi);
    tree->SetBranchAddress("ak8_dau_energy",  &ak8_dau_energy);
    tree->SetBranchAddress("ak8_dau_charge",  &ak8_dau_charge);

    vector<float>* ch_pt = nullptr;
    vector<float>* ch_eta = nullptr;
    vector<float>* ch_phi = nullptr;
    vector<float>* ch_energy = nullptr;
    tree->SetBranchAddress("ch_pt",     &ch_pt);
    tree->SetBranchAddress("ch_eta",    &ch_eta);
    tree->SetBranchAddress("ch_phi",    &ch_phi);
    tree->SetBranchAddress("ch_energy", &ch_energy);

    // --- Histograms: Inclusive ---
    TH1D* h_qec_charged = new TH1D("qec_all_charged", "QEC: J/psi + all charged;cosTheta", 20, -1, 1);
    TH1D* h_qec_charged_in = new TH1D("qec_charged_in", "QEC: J/psi in AK8;cosTheta", 20, -1, 1);
    TH1D* h_qec_charged_out = new TH1D("qec_charged_out", "QEC: J/psi out AK8;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_out = new TH1D("qec_jet_out", "QEC: J/psi out jet;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_in = new TH1D("qec_jet_in", "QEC: J/psi in jet;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_in_charged = new TH1D("qec_jet_in_charged", "QEC: Jet charged dau;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_in_neutral = new TH1D("qec_jet_in_neutral", "QEC: Jet neutral dau;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_all = new TH1D("qec_jet_all", "QEC: J/psi vs AK8 jet;cosTheta", 20, -1, 1);

    // --- Histograms: Prompt ---
    TH1D* h_qec_charged_prompt = new TH1D("qec_all_charged_prompt", "Prompt QEC;cosTheta", 20, -1, 1);
    TH1D* h_qec_charged_in_prompt = new TH1D("qec_charged_in_prompt", "Prompt QEC in AK8;cosTheta", 20, -1, 1);
    TH1D* h_qec_charged_out_prompt = new TH1D("qec_charged_out_prompt", "Prompt QEC out AK8;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_out_prompt = new TH1D("qec_jet_out_prompt", "Prompt QEC out jet;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_in_prompt = new TH1D("qec_jet_in_prompt", "Prompt QEC in jet;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_in_charged_prompt = new TH1D("qec_jet_in_charged_prompt", "Prompt QEC jet charged;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_in_neutral_prompt = new TH1D("qec_jet_in_neutral_prompt", "Prompt QEC jet neutral;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_all_prompt = new TH1D("qec_jet_all_prompt", "Prompt QEC jet all;cosTheta", 20, -1, 1);

    // --- Histograms: Direct Prompt ---
    TH1D* h_qec_charged_direct = new TH1D("qec_all_charged_direct", "Direct Prompt QEC;cosTheta", 20, -1, 1);
    TH1D* h_qec_charged_in_direct = new TH1D("qec_charged_in_direct", "Direct Prompt QEC in AK8;cosTheta", 20, -1, 1);
    TH1D* h_qec_charged_out_direct = new TH1D("qec_charged_out_direct", "Direct Prompt QEC out AK8;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_out_direct = new TH1D("qec_jet_out_direct", "Direct Prompt QEC out jet;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_in_direct = new TH1D("qec_jet_in_direct", "Direct Prompt QEC in jet;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_in_charged_direct = new TH1D("qec_jet_in_charged_direct", "Direct Prompt QEC jet charged;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_in_neutral_direct = new TH1D("qec_jet_in_neutral_direct", "Direct Prompt QEC jet neutral;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_all_direct = new TH1D("qec_jet_all_direct", "Direct Prompt QEC jet all;cosTheta", 20, -1, 1);

    // --- Histograms: Feed-down Prompt ---
    TH1D* h_qec_charged_fd = new TH1D("qec_all_charged_fd", "Feed-down Prompt QEC;cosTheta", 20, -1, 1);
    TH1D* h_qec_charged_in_fd = new TH1D("qec_charged_in_fd", "Feed-down Prompt QEC in AK8;cosTheta", 20, -1, 1);
    TH1D* h_qec_charged_out_fd = new TH1D("qec_charged_out_fd", "Feed-down Prompt QEC out AK8;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_out_fd = new TH1D("qec_jet_out_fd", "Feed-down Prompt QEC out jet;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_in_fd = new TH1D("qec_jet_in_fd", "Feed-down Prompt QEC in jet;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_in_charged_fd = new TH1D("qec_jet_in_charged_fd", "Feed-down Prompt QEC jet charged;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_in_neutral_fd = new TH1D("qec_jet_in_neutral_fd", "Feed-down Prompt QEC jet neutral;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_all_fd = new TH1D("qec_jet_all_fd", "Feed-down Prompt QEC jet all;cosTheta", 20, -1, 1);

    // --- Histograms: Non-Prompt ---
    TH1D* h_qec_charged_np = new TH1D("qec_all_charged_np", "Non-Prompt QEC;cosTheta", 20, -1, 1);
    TH1D* h_qec_charged_in_np = new TH1D("qec_charged_in_np", "Non-Prompt QEC in AK8;cosTheta", 20, -1, 1);
    TH1D* h_qec_charged_out_np = new TH1D("qec_charged_out_np", "Non-Prompt QEC out AK8;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_out_np = new TH1D("qec_jet_out_np", "Non-Prompt QEC out jet;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_in_np = new TH1D("qec_jet_in_np", "Non-Prompt QEC in jet;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_in_charged_np = new TH1D("qec_jet_in_charged_np", "Non-Prompt QEC jet charged;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_in_neutral_np = new TH1D("qec_jet_in_neutral_np", "Non-Prompt QEC jet neutral;cosTheta", 20, -1, 1);
    TH1D* h_qec_jet_all_np = new TH1D("qec_jet_all_np", "Non-Prompt QEC jet all;cosTheta", 20, -1, 1); // Fixed Typo here

    set<int> fd_ids = {441, 445, 100443, 20443, 10441, 30443, 10443};
    int nTotal = 0, nInJet = 0, nOutJet = 0, nPrompt = 0, nNonPrompt = 0;
    int nDirect = 0, nFD = 0;

    Long64_t nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);
        
        // --- Core Base Cuts ---
        if (is_full_jpsi_matched != 1) continue; 
        if (jpsi_mother_pdgId == -999) continue; 
        
        nTotal++;

        TLorentzVector jpsi, mu1, mu2;
        jpsi.SetPtEtaPhiE(jpsi_pt, jpsi_eta, jpsi_phi, jpsi_energy);
        mu1.SetPtEtaPhiE(mu1_pt, mu1_eta, mu1_phi, mu1_energy);
        mu2.SetPtEtaPhiE(mu2_pt, mu2_eta, mu2_phi, mu2_energy);
        TVector3 boost = -jpsi.BoostVector();
        double mJpsi = jpsi.M();

        bool jpsiInAnyJet = false;
        if (ak8_jet_pt && ak8_jet_dr_jpsi) {
            for (size_t j = 0; j < ak8_jet_pt->size(); j++) {
                if (ak8_jet_dr_jpsi->at(j) < 0.8) { jpsiInAnyJet = true; break; }
            }
        }
        if (jpsiInAnyJet) nInJet++; else nOutJet++;

        int abs_mom = abs(jpsi_mother_pdgId);
        
        // --- Production Mechanisms ---
        bool isFD     = (isFromB == 0 && fd_ids.count(abs_mom));
        bool isDirect = (isFromB == 0 && !isFD);

        if (isFromB == 1) nNonPrompt++;
        else {
            nPrompt++;
            if (isDirect) nDirect++;
            if (isFD) nFD++;
        }

        // --- Fill Logic (Global Charged) ---
        if (ch_pt) {
            for (size_t j = 0; j < ch_pt->size(); ++j) {
                TLorentzVector h;
                h.SetPtEtaPhiE(ch_pt->at(j), ch_eta->at(j), ch_phi->at(j), ch_energy->at(j));
                if (mu1.DeltaR(h) < 0.01 || mu2.DeltaR(h) < 0.01) continue;
                h.Boost(boost);
                double c = jpsi.Vect().Dot(h.Vect()) / (jpsi.Vect().Mag() * h.Vect().Mag());
                double w = h.E() / mJpsi;

                h_qec_charged->Fill(c, w); 
                if (jpsiInAnyJet) h_qec_charged_in->Fill(c, w); else h_qec_charged_out->Fill(c, w); 

                if (isFromB == 1) {
                    h_qec_charged_np->Fill(c, w);
                    if (jpsiInAnyJet) h_qec_charged_in_np->Fill(c, w); else h_qec_charged_out_np->Fill(c, w);
                } else {
                    h_qec_charged_prompt->Fill(c, w);
                    if (jpsiInAnyJet) h_qec_charged_in_prompt->Fill(c, w); else h_qec_charged_out_prompt->Fill(c, w);
                    if (isDirect) {
                        h_qec_charged_direct->Fill(c, w);
                        if (jpsiInAnyJet) h_qec_charged_in_direct->Fill(c, w); else h_qec_charged_out_direct->Fill(c, w);
                    } else if (isFD) {
                        h_qec_charged_fd->Fill(c, w);
                        if (jpsiInAnyJet) h_qec_charged_in_fd->Fill(c, w); else h_qec_charged_out_fd->Fill(c, w);
                    }
                }
            }
        }

        // --- Fill Logic (Jet Daughters) ---
        if (ak8_dau_pt) {
            for (size_t idau = 0; idau < ak8_dau_pt->size(); ++idau) {
                TLorentzVector h;
                h.SetPtEtaPhiE(ak8_dau_pt->at(idau), ak8_dau_eta->at(idau), ak8_dau_phi->at(idau), ak8_dau_energy->at(idau));
                if (mu1.DeltaR(h) < 0.01 || mu2.DeltaR(h) < 0.01) continue;
                h.Boost(boost);
                double c = jpsi.Vect().Dot(h.Vect()) / (jpsi.Vect().Mag() * h.Vect().Mag());
                double w = h.E() / mJpsi;

                h_qec_jet_all->Fill(c, w); 
                if (jpsiInAnyJet) {
                    h_qec_jet_in->Fill(c, w);
                    if (ak8_dau_charge && ak8_dau_charge->at(idau) != 0) h_qec_jet_in_charged->Fill(c, w); else h_qec_jet_in_neutral->Fill(c, w);
                } else h_qec_jet_out->Fill(c, w);

                if (isFromB == 1) {
                    h_qec_jet_all_np->Fill(c, w);
                    if (jpsiInAnyJet) {
                        h_qec_jet_in_np->Fill(c, w);
                        if (ak8_dau_charge && ak8_dau_charge->at(idau) != 0) h_qec_jet_in_charged_np->Fill(c, w); else h_qec_jet_in_neutral_np->Fill(c, w);
                    } else h_qec_jet_out_np->Fill(c, w);
                } else {
                    h_qec_jet_all_prompt->Fill(c, w);
                    if (jpsiInAnyJet) {
                        h_qec_jet_in_prompt->Fill(c, w);
                        if (ak8_dau_charge && ak8_dau_charge->at(idau) != 0) h_qec_jet_in_charged_prompt->Fill(c, w); else h_qec_jet_in_neutral_prompt->Fill(c, w);
                    } else h_qec_jet_out_prompt->Fill(c, w);
                    if (isDirect) {
                        h_qec_jet_all_direct->Fill(c, w);
                        if (jpsiInAnyJet) {
                            h_qec_jet_in_direct->Fill(c, w);
                            if (ak8_dau_charge && ak8_dau_charge->at(idau) != 0) h_qec_jet_in_charged_direct->Fill(c, w); else h_qec_jet_in_neutral_direct->Fill(c, w);
                        } else h_qec_jet_out_direct->Fill(c, w);
                    } else if (isFD) {
                        h_qec_jet_all_fd->Fill(c, w);
                        if (jpsiInAnyJet) {
                            h_qec_jet_in_fd->Fill(c, w);
                            if (ak8_dau_charge && ak8_dau_charge->at(idau) != 0) h_qec_jet_in_charged_fd->Fill(c, w); else h_qec_jet_in_neutral_fd->Fill(c, w);
                        } else h_qec_jet_out_fd->Fill(c, w);
                    }
                }
            }
        }
    }

    // --- Final Summary Output ---
    cout << "========================================" << endl;
    cout << "Total Events      : " << nTotal << endl;
    cout << "Prompt J/psi      : " << nPrompt << " (" << (double)nPrompt/nTotal*100.0 << " %)" << endl;
    cout << "  - Direct        : " << nDirect << " (" << (double)nDirect/nTotal*100.0 << " %)" << endl;
    cout << "  - Feed-down     : " << nFD << " (" << (double)nFD/nTotal*100.0 << " %)" << endl;
    cout << "Non-Prompt J/psi  : " << nNonPrompt << " (" << (double)nNonPrompt/nTotal*100.0 << " %)" << endl;
    cout << "----------------------------------------" << endl;
    cout << "Inside AK8 Jet    : " << nInJet << endl;
    cout << "Outside AK8 Jet   : " << nOutJet << endl;
    if (nTotal > 0) cout << "Frac inside Jet   : " << (double)nInJet / nTotal * 100.0 << " %" << endl;
    cout << "========================================" << endl;

    TFile* fout = new TFile(argv[2], "RECREATE");
    h_qec_charged->Write(); h_qec_charged_in->Write(); h_qec_charged_out->Write();
    h_qec_jet_out->Write(); h_qec_jet_in->Write(); h_qec_jet_in_charged->Write(); h_qec_jet_in_neutral->Write(); h_qec_jet_all->Write();
    h_qec_charged_prompt->Write(); h_qec_charged_in_prompt->Write(); h_qec_charged_out_prompt->Write();
    h_qec_jet_out_prompt->Write(); h_qec_jet_in_prompt->Write(); h_qec_jet_in_charged_prompt->Write(); h_qec_jet_in_neutral_prompt->Write(); h_qec_jet_all_prompt->Write();
    h_qec_charged_direct->Write(); h_qec_charged_in_direct->Write(); h_qec_charged_out_direct->Write();
    h_qec_jet_out_direct->Write(); h_qec_jet_in_direct->Write(); h_qec_jet_in_charged_direct->Write(); h_qec_jet_in_neutral_direct->Write(); h_qec_jet_all_direct->Write();
    h_qec_charged_fd->Write(); h_qec_charged_in_fd->Write(); h_qec_charged_out_fd->Write();
    h_qec_jet_out_fd->Write(); h_qec_jet_in_fd->Write(); h_qec_jet_in_charged_fd->Write(); h_qec_jet_in_neutral_fd->Write(); h_qec_jet_all_fd->Write();
    h_qec_charged_np->Write(); h_qec_charged_in_np->Write(); h_qec_charged_out_np->Write();
    h_qec_jet_out_np->Write(); h_qec_jet_in_np->Write(); h_qec_jet_in_charged_np->Write(); h_qec_jet_in_neutral_np->Write(); h_qec_jet_all_np->Write();

    fout->Close();
    return 0;
}