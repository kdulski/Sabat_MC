#ifndef Sabat_Analysis_h
#define Sabat_Analysis_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class Sabat_Analysis {
public :
  TTree          *fChain;
  Int_t           fCurrent;

  Double_t        a, b, c, EnergyDepositFinal=0, EnergyDepositFinal_smr=0, EngSmr=0, DistanceBtAlphaGamma;

  Double_t        Energy_Deposit;
  Double_t        Neutron_Theta, Neutron_Phi;
  Double_t        Time;
  Double_t        Hit_X;
  Double_t        Hit_Y;
  Double_t        Hit_Z;
  Double_t        Veto_Energy_Deposit;
  Double_t        Veto_Time;
  Double_t        Veto_Hit_X;
  Double_t        Veto_Hit_Y;
  Double_t        Veto_Hit_Z;
  Int_t           Event=0;
  Int_t           Parent_ID;
  Char_t          Particles[6];
  Char_t          Veto_Particles[6];
  Char_t          Process[22];
  Int_t           Volume;
  Char_t          Volume2[13];
  Int_t           Event_ID;

   // Branches - 
  TBranch        *b_Energy_Deposit;
  TBranch        *b_Time;
  TBranch        *b_Hit_X;
  TBranch        *b_Hit_Y;
  TBranch        *b_Hit_Z;
  TBranch        *b_Parent_ID;
  TBranch        *b_Particles;
  TBranch        *b_Process;
  TBranch        *b_Volume;
  TBranch        *b_Volume2;
  TBranch        *b_Event_ID;
  TBranch        *b_Neutron_Theta;
  TBranch        *b_Neutron_Phi;
  TBranch        *b_Veto_Energy_Deposit;
  TBranch        *b_Veto_Time;
  TBranch        *b_Veto_Hit_X;
  TBranch        *b_Veto_Hit_Y;
  TBranch        *b_Veto_Hit_Z;
  TBranch        *b_Veto_Particles;

  Sabat_Analysis(TTree *tree=0);
  Sabat_Analysis(TString input);
  virtual ~Sabat_Analysis();
  virtual Int_t    Cut(Long64_t entry);
  virtual Int_t    GetEntry(Long64_t entry);
  virtual Long64_t LoadTree(Long64_t entry);
  virtual void     Init(TTree *tree);
  virtual void     Loop();
  virtual Bool_t   Notify();
  virtual void     Show(Long64_t entry = -1);

private:
  TString inputName;
};

#endif

#ifdef Sabat_Analysis_cxx
Sabat_Analysis::Sabat_Analysis(TTree *tree) : fChain(0) 
{
  if (tree == 0) {
    TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("neutron.root");
    if (!f || !f->IsOpen()) {
      f = new TFile("test.root");
    }
  f->GetObject("EventTree", tree);
  }
  Init(tree);
}

Sabat_Analysis::Sabat_Analysis(TString input) : fChain(0)
{
  inputName = input;
  TTree *tree = 0;
  TFile *f = new TFile(input);
  if (f != 0) {
    f->GetObject("EventTree", tree);
    if (tree) {
      std::cout << input << std::endl;
      Init(tree);
      Loop();
    }
  }
}

Sabat_Analysis::~Sabat_Analysis()
{
  if (!fChain) return;
  delete fChain->GetCurrentFile();
}

Int_t Sabat_Analysis::GetEntry(Long64_t entry)
{
  if (!fChain) return 0;
  return fChain->GetEntry(entry);
}
Long64_t Sabat_Analysis::LoadTree(Long64_t entry)
{
  if (!fChain)
    return -5;
  Long64_t centry = fChain->LoadTree(entry);
  if (centry < 0)
    return centry;
  if (fChain->GetTreeNumber() != fCurrent) {
    fCurrent = fChain->GetTreeNumber();
    Notify();
  }
  return centry;
}

void Sabat_Analysis::Init(TTree *tree)
{
  if (!tree) return;
  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("Energy_Deposition", &Energy_Deposit, &b_Energy_Deposit);
  fChain->SetBranchAddress("Time", &Time, &b_Time);
  fChain->SetBranchAddress("Hit_X", &Hit_X, &b_Hit_X);
  fChain->SetBranchAddress("Hit_Y", &Hit_Y, &b_Hit_Y);
  fChain->SetBranchAddress("Hit_Z", &Hit_Z, &b_Hit_Z);
  fChain->SetBranchAddress("Parent_ID", &Parent_ID, &b_Parent_ID);
  fChain->SetBranchAddress("Particles", Particles, &b_Particles);
  fChain->SetBranchAddress("Process", Process, &b_Process);
  fChain->SetBranchAddress("Volume", &Volume, &b_Volume);
  fChain->SetBranchAddress("Volume2", Volume2, &b_Volume2);
  fChain->SetBranchAddress("Event_ID", &Event_ID, &b_Event_ID);
  fChain->SetBranchAddress("Neutron_Theta", &Neutron_Theta, &b_Neutron_Theta);
  fChain->SetBranchAddress("Neutron_Phi", &Neutron_Phi, &b_Neutron_Phi);
  fChain->SetBranchAddress("Veto_Energy_Deposition", &Veto_Energy_Deposit, &b_Veto_Energy_Deposit);
  fChain->SetBranchAddress("Veto_Time", &Veto_Time, &b_Veto_Time);
  fChain->SetBranchAddress("Veto_Hit_X", &Veto_Hit_X, &b_Veto_Hit_X);
  fChain->SetBranchAddress("Veto_Hit_Y", &Veto_Hit_Y, &b_Veto_Hit_Y);
  fChain->SetBranchAddress("Veto_Hit_Z", &Veto_Hit_Z, &b_Veto_Hit_Z);
  fChain->SetBranchAddress("Veto_Particles", Veto_Particles, &b_Veto_Particles);
  Notify();
}

Bool_t Sabat_Analysis::Notify()
{
  return kTRUE;
}

void Sabat_Analysis::Show(Long64_t entry)
{
// Print contents of entry.
  if (!fChain)
    return;
  fChain->Show(entry);
}

Int_t Sabat_Analysis::Cut(Long64_t entry)
{
  return 1;
}

#endif // #ifdef Sabat_Analysis_cxx
