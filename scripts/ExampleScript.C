{

  // This is an example script for analyzing a Root tree output from LUXSim.
  // Here we will plots some information about the energy deposits (energy and position)
  // and plot log10(S2/S1) using some simplifying assumptions.

  // that stupid stats box on Root plots is stupid
  gStyle->SetOptStat(0);

  // declare a TChain
  TChain *chain = new TChain("tree");
  chain->Add("../*.root");

  // declare important variables
  int iRecordSize_val; // number of particle records in the entry
  double fTotEDep_val; // total energy deposited by event in keV
  double fStepTime_val[10000]; // time in ns
  double fEnergyDep_keV_val[10000]; // energy deposited by scatter in keV
  double fPositionX_cm_val[10000]; // X position of scatter in cm
  double fPositionY_cm_val[10000]; // Y position of scatter in cm
  double fPositionZ_cm_val[10000]; // Z position of scatter in cm

  // point variables to chain branches
  chain->SetBranchAddress("iRecordSize", &iRecordSize_val);
  chain->SetBranchAddress("fTotEDep", &fTotEDep_val);
  chain->SetBranchAddress("fStepTime", &fStepTime_val);
  chain->SetBranchAddress("fEnergyDep_keV", &fEnergyDep_keV_val);
  chain->SetBranchAddress("fPositionX_cm", &fPositionX_cm_val);
  chain->SetBranchAddress("fPositionY_cm", &fPositionY_cm_val);
  chain->SetBranchAddress("fPositionZ_cm", &fPositionZ_cm_val);

  // let's declare some histograms
  TH1D *hETot = new TH1D("hETot", "Total Energy", 20, 0, 20);
  TH2D *hR2Z = new TH2D("hR2Z", "R^{2} versus Z", 25, 0, 625, 55, 5, 55);

  // other variables
  double R2; // radius squared
  double T0; // event time zero

  // loop over events
  for (int i = 0; i < chain->GetEntries(); i++) {
    // look at ith entry
    chain->GetEntry(i);
    if (fEnergyDep_keV_val[0] > 0) {
      // this entry is an energy deposit
      hETot->Fill(fTotEDep_val);
      for (int j = 0; j < iRecordSize_val; j++) {
        R2 = fPositionX_cm_val[j] * fPositionX_cm_val[j] + fPositionY_cm_val[j] * fPositionY_cm_val[j];
        hR2Z->Fill(R2, fPositionZ_cm_val[j]);
      }
    }
  }

  TCanvas *c1 = new TCanvas;
  hETot->GetXaxis()->SetTitle("Recoil Energy (keV)");
  hETot->GetYaxis()->SetTitle("Counts / 1 keV");
  hETot->Draw();

  TCanvas *c2 = new TCanvas;
  hR2Z->GetXaxis()->SetTitle("R^{2} (cm^{2})");
  hR2Z->GetYaxis()->SetTitle("Z (cm)");
  hR2Z->Draw("colz");

}
