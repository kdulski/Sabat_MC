#define Sabat_Analysis_cxx
#include <boost/random/normal_distribution.hpp>
#include <boost/random.hpp>
#include <fstream>
#include <math.h>

#include "Sabat_Analysis.h"
#include <TCanvas.h>
#include <TStyle.h>
#include <TH2D.h>
#include <TH2.h>

void Sabat_Analysis::Loop()
{
  TFile fNew("Out_" + inputName, "RECREATE");

  Double_t minEnergy = 0, maxEnergy = 14, binSize = 0.01; // in MeV

  TH1D *EngDep = new TH1D("EngDep","Energy Deposition", (maxEnergy-minEnergy)/binSize,
                          minEnergy - 0.5*binSize, maxEnergy - 0.5*binSize);
  TH1D *EngDepSmr = new TH1D("EngDepSmr","Energy Deposition Smr", (maxEnergy-minEnergy)/binSize,
                             minEnergy - 0.5*binSize, maxEnergy - 0.5*binSize);
  
  double a = 2.0*pow(10, -4); // in MeV
  double b = 2.22*pow(10, -2);
  double c = 0.5;


  if (fChain == 0)
    return;

  Long64_t nentries = fChain->GetEntriesFast();
  Long64_t nbytes = 0, nb = 0, GammaCounter = 0, AlphaCounter = 0;

  int t;

  double AlphaDetTime = 0, GammaDetTime = 0, NeutronDetTime = 0, AlphaDistanceTravel=0;
  double AlphaX_temp = 0, AlphaY_temp = 0, AlphaZ_temp = 0;
  double GammaX_temp = 0, GammaY_temp = 0, GammaZ_temp = 0;
  double NeutronX_temp = 0, NeutronY_temp = 0, NeutronZ_temp = 0;
  double GammaArrivalTime = 0, EngSmr_tem = 0;
  int AlphaFlag = 0, GammaFlag = 0, NeutronFlag = 0;
   
  std::cout << nentries << std::endl;

  for (Long64_t jentry=0; jentry<nentries; jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0)
      break;

    nb = fChain->GetEntry(jentry);
    nbytes += nb;

    if (Time>0) {
      if (Event != Event_ID) {
        if (EnergyDepositFinal>0. && EnergyDepositFinal<14.0) {
          EngSmr = gRandom->Gaus(0,1)*(a + b*sqrt(EnergyDepositFinal + c*pow(EnergyDepositFinal, 2)))/(2.35482004503);//original
          EngDep->Fill(EnergyDepositFinal);
          EngDepSmr->Fill(EnergyDepositFinal + EngSmr);
        }

        EnergyDepositFinal = 0;
        EnergyDepositFinal_smr = 0;
        EngSmr = 0;
        Event = Event_ID;
      }

    if (Event == Event_ID) {
      if (strcmp(Volume2, "DetectorLaBr") == 0 && (strcmp(Particles, "gamma") == 0 || strcmp(Particles, "e-") == 0))
        EnergyDepositFinal = EnergyDepositFinal + Energy_Deposit;
      }
    }
  }
  fNew.Write();
}
