// merge_simc_g4.C
// Usage:
// root -l -b -q 'merge_simc_g4.C("dis_hms_e.root","HodoFile.root","out.root","hms")'

#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>
#include <TString.h>

#include <cmath>
#include <iostream>
#include <string>
#include <unordered_map>

static void die_merge(const std::string& msg) {
  std::cerr << "ERROR: " << msg << std::endl;
  gSystem->Exit(1);
}

void merge_simc_g4(const char* simc_file,
                   const char* g4_file,
                   const char* out_file,
                   const char* simc_tree_out_name = "hms") {
  TFile fsimc(simc_file, "READ");
  if (fsimc.IsZombie()) die_merge(std::string("cannot open SimC file: ") + simc_file);

  TFile fg4(g4_file, "READ");
  if (fg4.IsZombie()) die_merge(std::string("cannot open Geant4 file: ") + g4_file);

  TTree* tsimc = (TTree*)fsimc.Get("h10");
  if (!tsimc) die_merge("cannot find SimC tree h10");

  TTree* thodo_pos = (TTree*)fg4.Get("hodoposition");
  TTree* thodo_ene = (TTree*)fg4.Get("hodoenergy");
  TTree* tgem      = (TTree*)fg4.Get("gemana");

  if (!thodo_pos) die_merge("cannot find Geant4 tree hodoposition");
  if (!thodo_ene) die_merge("cannot find Geant4 tree hodoenergy");
  if (!tgem)      die_merge("cannot find Geant4 tree gemana");

  const Long64_t n_g4 = thodo_ene->GetEntries();
  if (thodo_pos->GetEntries() != n_g4 || tgem->GetEntries() != n_g4) {
    die_merge("Geant4 trees have different entry counts");
  }

  Float_t simc_event_id = -1;
  if (tsimc->SetBranchAddress("event_id", &simc_event_id) < 0) {
    die_merge("cannot bind SimC branch event_id");
  }

  Int_t g4_event_id = -1;
  if (thodo_ene->SetBranchAddress("EventID", &g4_event_id) < 0) {
    die_merge("cannot bind Geant4 branch EventID");
  }

  std::unordered_map<int, Long64_t> g4_entry_by_event_id;
  g4_entry_by_event_id.reserve((size_t)n_g4);

  for (Long64_t i = 0; i < n_g4; ++i) {
    thodo_ene->GetEntry(i);
    if (g4_entry_by_event_id.count(g4_event_id)) {
      die_merge(Form("duplicate Geant4 EventID %d", g4_event_id));
    }
    g4_entry_by_event_id[g4_event_id] = i;
  }

  TString out_path(out_file);
  TString out_dir = gSystem->DirName(out_path);
  if (out_dir.Length() > 0 && out_dir != "." && gSystem->AccessPathName(out_dir)) {
    if (gSystem->mkdir(out_dir, kTRUE) != 0) {
      die_merge(std::string("cannot create output directory: ") + out_dir.Data());
    }
  }

  TFile fout(out_file, "RECREATE");
  if (fout.IsZombie()) die_merge(std::string("cannot create output file: ") + out_file);

  TTree* tsimc_out = tsimc->CloneTree(0);
  tsimc_out->SetName(simc_tree_out_name);
  tsimc_out->SetTitle(simc_tree_out_name);

  TTree* thodo_pos_out = thodo_pos->CloneTree(0);
  TTree* thodo_ene_out = thodo_ene->CloneTree(0);
  TTree* tgem_out      = tgem->CloneTree(0);

  Long64_t n_written = 0;
  const Long64_t n_simc = tsimc->GetEntries();

  for (Long64_t i = 0; i < n_simc; ++i) {
    tsimc->GetEntry(i);

    const int event_id = (int)std::llround(simc_event_id);
    if (std::fabs(simc_event_id - (Float_t)event_id) > 1.0e-3) {
      die_merge(Form("non-integer SimC event_id at entry %lld: %f", i, simc_event_id));
    }

    auto it = g4_entry_by_event_id.find(event_id);
    if (it == g4_entry_by_event_id.end()) {
      die_merge(Form("SimC event_id %d not found in Geant4 hodoenergy.EventID", event_id));
    }

    const Long64_t g4_entry = it->second;

    tsimc_out->Fill();

    thodo_pos->GetEntry(g4_entry);
    thodo_ene->GetEntry(g4_entry);
    tgem->GetEntry(g4_entry);

    thodo_pos_out->Fill();
    thodo_ene_out->Fill();
    tgem_out->Fill();

    ++n_written;
  }

  fout.Write();
  fout.Close();

  std::cout << "Merged " << n_written << " accepted events." << std::endl;
  std::cout << "Output: " << out_file << std::endl;
}