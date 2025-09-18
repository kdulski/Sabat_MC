#include <sys/stat.h>
#include <functional>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <sstream>
#include <numeric>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdio>

#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TRandom3.h"
#include "TDatime.h"
#include "TNtuple.h"
#include "TObject.h"
#include "TBranch.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TStyle.h"
#include "TFile.h"
#include "TMath.h"
#include "TTree.h"

#include "Histo_Collection.h"

//Smearing parameters
double a = 2.0*pow(10, -4); // in MeV
double b = 2.22*pow(10, -2);
double c = 0.5;

double LaBrDecayTime = 0.025; // in us
double SiDecayTime = 0.09; // in us for silicon drift detectors

bool FileCheck(const std::string& NameOfFile);
double SmearEnergy(double energy);
std::vector<std::string> SplitString(std::string& s, const std::string& delimiter);
int GetMassNumberFromProducts(std::string label);
void AnalyzeFile(std::string NameOfFile, HistCollection histo);

int main(int argc, char* argv[])
{
//--------------------------------------------------------
//Reading parameters
  if (argc < 2) {
    std::cout << "Not enough arguments. Up to two needed: name of the file | name of the second file ..." << std::endl;
    return 0;
  }

  std::string fileOrPattern = argv[1];

//--------------------------------------------------------
//Setting environment for results
  Double_t minEnergy = 0, maxEnergy = 12, binSize = 0.01; // in MeV
  Double_t minTime = 0, timeSeparator = 1, maxTime = 1500, binSizeSmall = 0.001, binSizeLarge = 1; // in us
  HistCollection hist;
  hist.CreateEnergyHistos(minEnergy, maxEnergy, binSize);
  hist.CreateTimingHistos(minTime, timeSeparator, maxTime, binSizeSmall, binSizeLarge);
  hist.CreateEnergyVsTimingHistos(minEnergy, maxEnergy, binSize, minTime, maxTime, binSizeLarge);
  TString outputName = "";
//--------------------------------------------------------
//Analysis
  std::vector<std::string> filesToAnalyze;

  if (argc > 2) {
    filesToAnalyze.push_back(fileOrPattern);
    for (unsigned i=2; i<argc; i++) {
      fileOrPattern = argv[i];
      filesToAnalyze.push_back(fileOrPattern);
    }
    TString slash = "/";
    std::size_t slashPlace = fileOrPattern.rfind(slash);
    std::string pattern = fileOrPattern;
    std::string dirName = "";
    if (slashPlace > 0) {
      dirName = fileOrPattern.substr(0, slashPlace+1);
      pattern = fileOrPattern.substr(slashPlace+1, fileOrPattern.length());
    }
    outputName = dirName + "Out_LastFile_" + pattern;

    for (unsigned fileNo = 0; fileNo < filesToAnalyze.size(); fileNo++) {
      AnalyzeFile(filesToAnalyze.at(fileNo), hist);
    }
  } else {
    TString root_file = fileOrPattern;
    TString star = "*";
    TString slash = "/";
    std::size_t starPlace = fileOrPattern.rfind(star);
    std::size_t slashPlace = fileOrPattern.rfind(slash);

    if (root_file[strlen(root_file) - 1] == star) {
      std::string pattern;
      std::string dirName;
      if (slashPlace > 0) {
        dirName = fileOrPattern.substr(0, slashPlace+1);
        pattern = fileOrPattern.substr(slashPlace+1, starPlace-slashPlace-1);
      }
      else {
        dirName = "";
        pattern = fileOrPattern.substr(0, starPlace);

      }
      std::cout << "Getting files from directory: " << dirName << " and pattern " << pattern << std::endl;
      TString dirNameRoot = dirName;
      if (dirNameRoot == "")
        dirNameRoot = ".";
      TSystemDirectory dir(dirNameRoot, dirNameRoot);
      TList *files = dir.GetListOfFiles();

      if (files) {
        TSystemFile *file;
        TString fname;
        TIter next(files);
        while ((file = (TSystemFile*)next())) {
          fname = file->GetName();

          if (!file->IsDirectory() && fname.EndsWith(".root") && fname.BeginsWith(pattern)) {
            std::cout << "Adding file to the analysis queue " << fname.Data() << std::endl;
            filesToAnalyze.push_back(dirName + fname.Data());
          }
        }
      }
      outputName = "Out_Patt_" + pattern + ".root";
    } else {
      filesToAnalyze.push_back(fileOrPattern);
      outputName = "Out_" + fileOrPattern;
    }

    for (unsigned fileNo = 0; fileNo < filesToAnalyze.size(); fileNo++) {
      AnalyzeFile(filesToAnalyze.at(fileNo), hist);
    }
  }
  hist.SaveHistos(outputName);

  return 0;
}

void AnalyzeFile(std::string NameOfFile, HistCollection histo)
{
  TString fileName = NameOfFile;
  std::cout << " Reading file " << fileName << std::endl;
  TFile* hfile = new TFile(fileName, "READ");
  TTree *ntuple = (TTree *) hfile->Get("EventTree");
  Double_t Energy_Deposit, Neutron_Theta, Neutron_Phi, Time, Hit_X, Hit_Y, Hit_Z;
  Double_t Veto_Energy_Deposit, Veto_Time, Veto_Hit_X, Veto_Hit_Y ,Veto_Hit_Z;
  Int_t CurrentEvent, EventVeto, Parent_ID;
  Char_t Particles[7], Veto_Particles[7], Process[22], Label[32];
  Int_t Volume;
  Char_t Volume2[13];
  Int_t Event_ID;
  ntuple->SetBranchAddress("Energy_Deposition", &Energy_Deposit);
  ntuple->SetBranchAddress("Time", &Time);
  ntuple->SetBranchAddress("Hit_X", &Hit_X);
  ntuple->SetBranchAddress("Hit_Y", &Hit_Y);
  ntuple->SetBranchAddress("Hit_Z", &Hit_Z);
  ntuple->SetBranchAddress("Parent_ID", &Parent_ID);
  ntuple->SetBranchAddress("Particles", Particles);
  ntuple->SetBranchAddress("Process", Process);
  ntuple->SetBranchAddress("Volume", &Volume);
  ntuple->SetBranchAddress("Volume2", Volume2);
  ntuple->SetBranchAddress("Event_ID", &Event_ID);
  ntuple->SetBranchAddress("Neutron_Theta", &Neutron_Theta);
  ntuple->SetBranchAddress("Neutron_Phi", &Neutron_Phi);
  ntuple->SetBranchAddress("Hit_Label", Label);
  ntuple->SetBranchAddress("Veto_Energy_Deposition", &Veto_Energy_Deposit);
  ntuple->SetBranchAddress("Veto_Time", &Veto_Time);
  ntuple->SetBranchAddress("Veto_Hit_X", &Veto_Hit_X);
  ntuple->SetBranchAddress("Veto_Hit_Y", &Veto_Hit_Y);
  ntuple->SetBranchAddress("Veto_Hit_Z", &Veto_Hit_Z);
  ntuple->SetBranchAddress("Veto_Particles", Veto_Particles);

  Int_t nentries = (Int_t)ntuple->GetEntries();

  Double_t EnergyDepositFinal = 0, EnergyDepositVetoFinal = 0;
  Double_t FirstTime = 0, FirstTimeVeto = 0;
  CurrentEvent = -1;
  std::string currLabel = "";
  for (Int_t i=0; i<nentries; i++) {
    ntuple->GetEntry(i);

    std::string tempLabel(Label);

    if (Event_ID > CurrentEvent) {
      CurrentEvent = Event_ID;
      if (FirstTime > 0 && FirstTimeVeto > 0) {
        histo.FillTimeDifference(FirstTime - FirstTimeVeto);
      }
      if (EnergyDepositFinal > 0) {
        histo.FillEnergyDepositionForAProcess(EnergyDepositFinal, currLabel[1]);
        histo.FillTimeLaBrForAProcess(FirstTime, currLabel[1]);
        histo.FillEnergyDepositionVsMassNumber(EnergyDepositFinal, GetMassNumberFromProducts(currLabel));

        histo.FillEnergyDeposition(EnergyDepositFinal);
        double eneSmeared = EnergyDepositFinal + SmearEnergy(EnergyDepositFinal);
        histo.FillEnergyDepositionSmeared(eneSmeared);
        if (EnergyDepositVetoFinal > 0) {
          histo.FillEnergyDepositionWithVeto(EnergyDepositFinal);
          histo.FillEnergyDepositionWithVetoSmeared(eneSmeared);
          if (FirstTime > 0 && FirstTimeVeto > 0) {
            histo.FillEnergyDepositionVsTimeDiff(EnergyDepositFinal, FirstTime - FirstTimeVeto);
            histo.FillEnergyDepositionVsTimeDiffSmeared(eneSmeared, FirstTime - FirstTimeVeto);
            histo.FillEnergyDepositionVsTimeDiffCut(EnergyDepositFinal, FirstTime - FirstTimeVeto);
            histo.FillEnergyDepositionVsTimeDiffCutSmeared(eneSmeared, FirstTime - FirstTimeVeto);
          }
        }
      }
      if (EnergyDepositVetoFinal > 0) {
        histo.FillEnergyDepositionVeto(EnergyDepositVetoFinal);
      }

      FirstTime = 0;
      FirstTimeVeto = 0;
      EnergyDepositFinal = 0;
      EnergyDepositVetoFinal = 0;
      currLabel = "";
    }

    if (Event_ID == CurrentEvent) {
      if (Time > 0) {
        histo.FillTimeLaBr(Time);

        if (FirstTime == 0)
          FirstTime = Time;

        if (strcmp(Volume2, "DetectorLaBr") == 0 && (strcmp(Particles, "gamma") == 0 || strcmp(Particles, "e-") == 0))
          EnergyDepositFinal = EnergyDepositFinal + Energy_Deposit;
      }

      if (Veto_Time > 0) {
        histo.FillTimeVeto(Veto_Time);

        if (FirstTimeVeto == 0)
          FirstTimeVeto = Veto_Time;

        if ((strcmp(Veto_Particles, "alpha") == 0 || strcmp(Veto_Particles, "e-") == 0))
          EnergyDepositVetoFinal = EnergyDepositVetoFinal + Veto_Energy_Deposit;
      }

      if (currLabel == "") {
        std::string tempPart(Particles);

        if (tempPart != "neutron" && tempPart != "") {
          currLabel = tempLabel;
        }
      } else if (Label[0] != 's') { // Omitting secondaries
        if (Label[0] == 'n' && tempLabel != currLabel) {
          currLabel = "Mixed";
        }
      }
    }
  }
}

int GetMassNumberFromProducts(std::string label)
{
  if (label.at(0) != 'n')
    return 0;

  std::vector<std::string> products = SplitString(label, "_");
  int massNumber = -1;

  int firstNumberCode = 48;
  for (unsigned i=1; i<products.size(); i++) {
    int lengthOfString = products.at(i).size();
    if (products.at(i) == "proton" || products.at(i) == "neutron")
      massNumber += 1;
    else if (products.at(i) == "deuteron")
      massNumber += 2;
    else if (products.at(i) == "triton")
      massNumber += 3;
    else if (products.at(i) == "alpha")
      massNumber += 4;
    else if (lengthOfString <= 5 && lengthOfString > 1) {
      if (lengthOfString <= 3) {
        int decNum = -firstNumberCode + (int)products.at(i).at(lengthOfString-2);//std::atoi(products.at(i).at(lengthOfString-2));
        int intNum = -firstNumberCode + (int)products.at(i).at(lengthOfString-1);//std::atoi(products.at(i).at(lengthOfString-1));
        if (decNum <= 9)
          massNumber += 10*decNum;
        if (intNum <= 9)
          massNumber += intNum;
      } else {
        int hunNum = -firstNumberCode + (int)products.at(i).at(lengthOfString-3);//std::atoi(products.at(i).at(lengthOfString-3));
        int decNum = -firstNumberCode + (int)products.at(i).at(lengthOfString-2);//std::atoi(products.at(i).at(lengthOfString-2));
        int intNum = -firstNumberCode + (int)products.at(i).at(lengthOfString-1);//std::atoi(products.at(i).at(lengthOfString-1));
        if (hunNum <= 9)
          massNumber += 100*hunNum;
        if (decNum <= 9)
          massNumber += 10*decNum;
        if (intNum <= 9)
          massNumber += intNum;
      }
    } else
      std::cout << "Unknown part: " << products.at(i) << std::endl;
  }
  return massNumber;
}

std::vector<std::string> SplitString(std::string& s, const std::string& delimiter)
{
  std::vector<std::string> tokens;
  size_t pos = 0;
  std::string token;
  while ((pos = s.find(delimiter)) != std::string::npos) {
    token = s.substr(0, pos);
    tokens.push_back(token);
    s.erase(0, pos + delimiter.length());
  }
  tokens.push_back(s);

  return tokens;
}

double SmearEnergy(double energy)
{
  return gRandom->Gaus(0,1)*(a + b*sqrt(energy + c*pow(energy, 2)))/(2.35482004503);
}

bool FileCheck(const std::string& NameOfFile)
{
    struct stat buffer;
    return (stat(NameOfFile.c_str(), &buffer) == 0);
}
