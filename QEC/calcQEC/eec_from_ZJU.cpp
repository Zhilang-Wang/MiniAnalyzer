#include "TAxis.h"
#include "TFile.h"
#include "TChain.h"
#include "TH3D.h"
#include "TString.h"
#include <Math/GenVector/VectorUtil.h>
#include <Math/PtEtaPhiE4D.h>
#include <Math/PxPyPzE4D.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TH1F.h>
#include <TH2D.h>
#include <TLorentzVector.h>
#include "Math/Vector4D.h"
#include <TSystem.h>
#include <TTree.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <vector>
#include "random"
#include <fastjet/ClusterSequence.hh>
#include <fastjet/PseudoJet.hh>
#include "include/JetsAndDaughters.h"
#include "/storage/shuangyuan/code/headfile/ProgressBar.h"
#include "/storage/shuangyuan/code/headfile/Hists.h"
#include "/storage/shuangyuan/code/analysis_spin/Machine_learning/include/ParticleInfo.h"

struct JetsandDaughters
{
  // jet info
  TLorentzVector jet_tlz;
  void setJet(const fastjet::PseudoJet &j)
  {
    jet_tlz.SetPtEtaPhiE(j.pt(), j.eta(), j.phi(), j.e());
  }

  // jpsi info
  bool has_jpsi = false;
  TLorentzVector jpsi_tlz;

  // daughters info
  std::vector<ParticleInfo> duaughtersinfo;
  std::vector<fastjet::PseudoJet> duaughters;
};

struct EventInfo
{
  std::vector<JetsandDaughters> jets;
  bool isdijet = true;
  std::vector<ParticleInfo> particlesinfo;
  std::vector<fastjet::PseudoJet> particles;
  bool has_jpsi = false;
  TLorentzVector jpsi_tlz;
};

TLorentzVector createTLVFromPseudoJet(const fastjet::PseudoJet &subjet)
{
  TLorentzVector tlv;
  tlv.SetPtEtaPhiE(subjet.pt(), subjet.eta(), subjet.phi(), subjet.e());
  return tlv;
};

int main(int argc, char *argv[])
{
  TString basefolder = "/data/xiaoxue/data/herwig/";
  TString input_file = "";
  TString output_path = "";
  double alpha = 1.0;
  double omega = 3.0 - alpha;
  int chunknum = 2;
  int chunkindex = 0;
  double ymax = 1.0;
  int opt;

  // Parse command line arguments
  while ((opt = getopt(argc, argv, "i:o:n:e:")) != -1)
  {
    switch (opt)
    {
    case 'i':
      input_file = optarg;
      break;
    case 'o':
      output_path = optarg;
      break;
    case 'n':
      chunknum = std::atoi(optarg);
      break;
    case 'e':
      chunkindex = std::atoi(optarg);
      break;
    default:
      std::cerr << "Usage: " << argv[0] << " -i <input_file> -o <output_path> -n <chunk_num> -e <chunk_index>" << std::endl;
      return 1;
    }
  }

  // Check if input file is provided
  if (input_file == "")
  {
    std::cerr << "Error: Input file not specified!" << std::endl;
    std::cerr << "Usage: " << argv[0] << " -i <input_file> -o <output_path> -n <chunk_num> -e <chunk_index>" << std::endl;
    return 1;
  }

  // Create output directory
  // gSystem->Exec(TString::Format("mkdir -p %s", output_path.Data()));

  // Create and configure TChain
  TChain *t = new TChain("JetsAndDaughters");
  for (int i = 1; i <= 300; i++)
  {
    t->Add(input_file + TString::Format("/Chunk%d/*.root/JetsAndDaughters", i));
  }
  auto tchain = JetsAndDaughters(t);

  int entries = t->GetEntries();
  if (entries == 0)
  {
    std::cout << "Warning: No events found!" << std::endl;
    delete t;
    return 1;
  }

  // Calculate chunk ranges
  int chunksize = std::ceil(static_cast<double>(entries) / chunknum);
  int entrybegin = chunkindex * chunksize;
  int entryend = std::min((chunkindex + 1) * chunksize, entries);

  if (entrybegin >= entries)
  {
    std::cout << "Warning: chunkindex " << chunkindex << " is out of range" << std::endl;
    entrybegin = entries;
    entryend = entries;
  }

  std::cout << "Total entries: " << entries << " Processing range: " << entrybegin << " - " << entryend
            << " (" << (entryend - entrybegin) << " events)" << std::endl;

  // 分块写入功能
  int total_events = entryend - entrybegin;
  int events_per_part = std::max(total_events, 1);
  int part_index = 0;
  int events_processed = 0;
  int events_in_current_part = 0;
  int valid_events_in_current_part = 0;

  Hists hists;
  std::vector<double> ptbins = {0, 5, 25, 50, 100, 200, 300};
  Int_t nBins = ptbins.size() - 1;
  TAxis *ptaxis = new TAxis(nBins, &ptbins[0]);

  std::vector<TString> ptbin_names = {"", "_jpsipt_0_5", "_jpsipt_5_25", "_jpsipt_25_50",
                                      "_jpsipt_50_100", "_jpsipt_100_200", "_jpsipt_200_300", "_jpsipt_300_Inf"};
  std::vector<TString> coschi_hist_names = {"coschi", "coschi_jet", "coschi_alljet"};
  for (auto &histname : coschi_hist_names)
  {
    for (auto &ptbin_name : ptbin_names)
    {
      hists.addHist(histname + ptbin_name, 20, -1, 1);
    }
  }
  hists.addHist("pdgid", 2000, -1000, 1000);

  TH2D *coschi_migration = new TH2D("coschi_migration", "coschi_migration", 20, -1, 1, 20, -1, 1);

  auto SafeWritePart = [&](bool is_final = false) -> bool
  {
    if (valid_events_in_current_part == 0)
    {
      std::cout << "Part " << part_index << " has no valid events, skipping file creation." << std::endl;
      return true;
    }

    TString output_filename = output_path + TString::Format("_Chunk%d_Part%d.root", chunkindex, part_index);
    hists.Write(output_filename, {coschi_migration});
    std::cout << "Saved part " << part_index << " to " << output_filename
              << " (valid events: " << valid_events_in_current_part
              << ", processed events: " << events_in_current_part;
    if (is_final)
    {
      std::cout << ", final part";
    }
    std::cout << ")" << std::endl;

    return true;
  };

  ProgressBar process(total_events);
  int loop = 1;
  double radius = 0.8;

  for (int k = entrybegin; k < entryend; k++)
  {
    process.update2(loop);
    loop++;
    events_processed++;
    events_in_current_part++;

    bool event_was_written = false;

    try
    {
      t->GetEntry(k);
      if (tchain.Pt_JPsi->size() != 1)
        continue;

      EventInfo eventinfo;
      eventinfo.jpsi_tlz.SetPtEtaPhiE(tchain.Pt_JPsi->at(0), tchain.Eta_JPsi->at(0), tchain.Phi_JPsi->at(0), tchain.Energy_JPsi->at(0));
      eventinfo.has_jpsi = true;

      int particle_count = tchain.Pt_Hadron->size();
      int neutrino_count = 0;
      for (int part_idx = 0; part_idx < particle_count; ++part_idx)
      {
        double pt = tchain.Pt_Hadron->at(part_idx);
        double eta = tchain.Eta_Hadron->at(part_idx);
        double phi = tchain.Phi_Hadron->at(part_idx);
        double energy = tchain.Energy_Hadron->at(part_idx);

        if (std::isnan(pt) || std::isinf(pt) ||
            std::isnan(eta) || std::isinf(eta) ||
            std::isnan(phi) || std::isinf(phi) ||
            std::isnan(energy) || std::isinf(energy))
        {
          std::cout << "Invalid particles!!!" << std::endl;
          continue;
        }
        
        // skip neutrinos
        if (abs(tchain.PdgId_Hadron->at(part_idx)) == 12 ||
            abs(tchain.PdgId_Hadron->at(part_idx)) == 14 ||
            abs(tchain.PdgId_Hadron->at(part_idx)) == 16)
        {
          neutrino_count++;
          continue;
        }

        TLorentzVector p;
        p.SetPtEtaPhiE(pt, eta, phi, energy);
        fastjet::PseudoJet particle(p.Px(), p.Py(), p.Pz(), p.E());

        int pdgid = tchain.PdgId_Hadron->at(part_idx);
        int charge = tchain.Charge_Hadron->at(part_idx);
        if (std::abs(pdgid) == 443)
          std::cout << "stable Jpsi !!!" << std::endl;
        ParticleInfo particleInfo(pdgid, charge, pt, eta, phi, energy);
        particle.set_user_info(new FlavHistory(pdgid));
        eventinfo.particlesinfo.push_back(particleInfo);
        particle.set_user_index(part_idx - neutrino_count);
        eventinfo.particles.push_back(particle);
      }

      if (eventinfo.particles.empty())
      {
        std::cout << "Empty particles !!!" << std::endl;
        continue;
      }

      fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, radius);
      fastjet::ClusterSequence cs(eventinfo.particles, jet_def);
      std::vector<fastjet::PseudoJet> anktjets = fastjet::sorted_by_pt(cs.inclusive_jets(30));
      anktjets.erase(std::remove_if(anktjets.begin(), anktjets.end(),
                                    [](const fastjet::PseudoJet &jet)
                                    {
                                      return std::abs(jet.eta()) > 5;
                                    }),
                     anktjets.end());
      anktjets = fastjet::sorted_by_pt(anktjets);
      eventinfo.jets.resize(anktjets.size());

      for (int i = 0; i < anktjets.size(); i++)
      {
        eventinfo.jets.at(i).setJet(anktjets.at(i));
        double dr_jet_jpsi = eventinfo.jets.at(i).jet_tlz.DeltaR(eventinfo.jpsi_tlz);
        if (dr_jet_jpsi < radius)
        {
          eventinfo.jets.at(i).has_jpsi = true;
          eventinfo.jets.at(i).jpsi_tlz = eventinfo.jpsi_tlz;
        }
        eventinfo.jets.at(i).duaughters = anktjets.at(i).constituents();
        for (const auto &constituent : eventinfo.jets.at(i).duaughters)
        {
          int user_index = constituent.user_index();
          if (user_index >= 0 && user_index < eventinfo.particlesinfo.size())
          {
            eventinfo.jets.at(i).duaughtersinfo.push_back(eventinfo.particlesinfo[user_index]);
          }
        }
      }

      if (eventinfo.jets.size() < 2)
        eventinfo.isdijet = false;
      double pTCut = 30;
      double etaCut = 2.1;
      if (eventinfo.jets.at(0).jet_tlz.Pt() < pTCut || abs(eventinfo.jets.at(0).jet_tlz.Eta()) > etaCut)
        eventinfo.isdijet = false;
      if (eventinfo.jets.at(0).jet_tlz.Pt() < pTCut || abs(eventinfo.jets.at(0).jet_tlz.Eta()) > etaCut)
        eventinfo.isdijet = false;
      auto dphi = eventinfo.jets.at(0).jet_tlz.Phi() - eventinfo.jets.at(1).jet_tlz.Phi();
      if (dphi > M_PI)
        dphi -= 2.0 * M_PI;
      if (dphi <= -M_PI)
        dphi += 2.0 * M_PI;
      auto dpt = eventinfo.jets.at(0).jet_tlz.Pt() - eventinfo.jets.at(1).jet_tlz.Pt();
      auto spt = eventinfo.jets.at(0).jet_tlz.Pt() + eventinfo.jets.at(1).jet_tlz.Pt();
      if (!(abs(dphi) > 2 && abs(dpt) / spt < 0.3))
        eventinfo.isdijet = false;

      if (!eventinfo.isdijet)
        continue;

      int jpsiptbin = ptaxis->FindBin(eventinfo.jpsi_tlz.Pt());
      TString histname;
      if (jpsiptbin == ptbins.size())
        histname = "_jpsipt_300_Inf";
      else
        histname = TString::Format("_jpsipt_%.0f_%.0f", ptbins.at(jpsiptbin - 1), ptbins.at(jpsiptbin));

      TLorentzVector jpsi_tlz = eventinfo.jpsi_tlz;
      TVector3 boost_vector = -jpsi_tlz.BoostVector();

      // Jet overlap checking
      bool hasoverlap = false;
      for (int i = 0; i < eventinfo.jets.size(); i++)
      {
        for (int j = i + 1; j < eventinfo.jets.size(); j++)
        {
          TLorentzVector jeti = eventinfo.jets.at(i).jet_tlz;
          TLorentzVector jetj = eventinfo.jets.at(j).jet_tlz;
          if (jeti.DeltaR(jetj) < radius)
          {
            hasoverlap = true;
            break;
            // std::cout << "Overlap !!!!" << std::endl;
            // std::cout << "Jet1 pT: " << eventinfo.jets.at(i).jet_tlz.Pt() << " , eta: " << eventinfo.jets.at(i).jet_tlz.Eta() << std::endl;
            // std::cout << "Jet2 pT: " << eventinfo.jets.at(j).jet_tlz.Pt() << " , eta: " << eventinfo.jets.at(j).jet_tlz.Eta() << std::endl;
          }
        }
      }
      if (hasoverlap)
        continue;

      // Jpsi in Jet
      bool jpsiinjet = false;
      for (int i = 0; i < eventinfo.jets.size(); i++)
      {
        if (eventinfo.jets.at(i).has_jpsi)
          jpsiinjet = true;
      }
      if (!jpsiinjet)
        continue;

      // All particles vs. J/psi
      for (auto &particle : eventinfo.particles)
      {
        TLorentzVector p = createTLVFromPseudoJet(particle);
        double coschi_origin = jpsi_tlz.Vect().Dot(p.Vect()) / jpsi_tlz.Vect().Mag() / p.Vect().Mag();
        // Boost to J/psi
        p.Boost(boost_vector);
        double coschi = jpsi_tlz.Vect().Dot(p.Vect()) / jpsi_tlz.Vect().Mag() / p.Vect().Mag();
        double ec = p.E() / jpsi_tlz.M();
        hists["coschi"]->Fill(coschi, ec);
        hists["coschi" + histname]->Fill(coschi, ec);
        coschi_migration->Fill(coschi_origin, coschi);
        if (coschi > 0.9)
          // hists["pdgid"]->Fill(eventinfo.particlesinfo.at(particle.user_index()).pdgid);
          std::cout << eventinfo.particlesinfo.at(particle.user_index()).pdgid << std::endl;
      }

      // Matched jet vs. J/psi
      for (auto &jet : eventinfo.jets)
      {
        if (!jet.has_jpsi)
          continue;
        // Boost to J/psi
        for (auto &daughter : jet.duaughters)
        {
          TLorentzVector p = createTLVFromPseudoJet(daughter);
          p.Boost(boost_vector);
          double coschi = jpsi_tlz.Vect().Dot(p.Vect()) / jpsi_tlz.Vect().Mag() / p.Vect().Mag();
          double ec = p.E() / jpsi_tlz.M();
          hists["coschi_jet"]->Fill(coschi, ec);
          hists["coschi_jet" + histname]->Fill(coschi, ec);
        }
      }

      // All jets vs. J/psi
      std::vector<TLorentzVector> alldaughters;
      for (auto &jet : eventinfo.jets)
      {
        // Boost to J/psi
        for (auto &daughter : jet.duaughters)
        {
          alldaughters.push_back(createTLVFromPseudoJet(daughter));
        }
      }
      for (auto &daughter : alldaughters)
      {
        daughter.Boost(boost_vector);
        double coschi = jpsi_tlz.Vect().Dot(daughter.Vect()) / jpsi_tlz.Vect().Mag() / daughter.Vect().Mag();
        double ec = daughter.E() / jpsi_tlz.M();
        hists["coschi_alljet"]->Fill(coschi, ec);
        hists["coschi_alljet" + histname]->Fill(coschi, ec);
      }

      valid_events_in_current_part++;
    }

    catch (const std::exception &e)
    {
      std::cout << "Exception processing event " << k << ": " << e.what() << ", skipping..." << std::endl;
      continue;
    }
    catch (...)
    {
      std::cout << "Unknown exception processing event " << k << ", skipping..." << std::endl;
      continue;
    }

    if (k == entryend - 1)
    {
      std::cout << "Reached the last event: k = " << k << std::endl;
    }

    // 检查是否需要写入当前part
    bool should_write_part = false;
    // 条件1：达到了每个part的事件数限制
    if (events_in_current_part >= events_per_part)
    {
      should_write_part = true;
    }
    // 条件2：处理到了最后一个事件
    else if (k == entryend - 1)
    {
      should_write_part = true;
    }

    if (should_write_part)
    {
      if (SafeWritePart(k == entryend - 1))
      {
        part_index++;
        // 重置计数器
        events_in_current_part = 0;
        valid_events_in_current_part = 0;
      }
    }
  }
  if (events_in_current_part > 0)
  {
    if (SafeWritePart(true))
    {
      part_index++;
    }
  }

  delete t;

  std::cout << std::endl;
  std::cout << "Processing completed successfully!" << std::endl;
  std::cout << "Total events processed: " << events_processed << std::endl;
  std::cout << "Total parts created: " << part_index << std::endl;

  return 0;
}
