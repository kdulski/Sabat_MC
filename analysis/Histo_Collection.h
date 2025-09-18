#ifndef Histo_Collection_h
#define Histo_Collection_h

#include "TLatex.h"
#include "TAxis.h"
#include "TFile.h"
#include "TLine.h"
#include "TF1.h"
#include "TF2.h"
#include "TF3.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

//TODO: include enumerate with labels for histogram and a general fill method
//TODO: design a safe method to store the histograms in a container and extract them with labels or histogram names

class HistCollection
{
public:
  HistCollection() {;};
  ~HistCollection() {;};

  void CreateEnergyHistos(double minEnergy, double maxEnergy, double binSize)
  {
    EnergyDeposition = new TH1D("EnergyDeposition","Energy Deposition; Energy [MeV]; Counts", (maxEnergy - minEnergy)/binSize,
                                minEnergy - 0.5*binSize, maxEnergy - 0.5*binSize);
    EnergyDepositionSmeared = new TH1D("EnergyDepositionSmeared","Energy Deposition Smeared; Energy [MeV]; Counts", (maxEnergy - minEnergy)/binSize,
                                       minEnergy - 0.5*binSize, maxEnergy - 0.5*binSize);
    EnergyDepositionVeto = new TH1D("EnergyDepositionVeto","Energy Deposition Veto; Energy [MeV]; Counts", (5*maxEnergy - minEnergy)/binSize/5,
                                minEnergy - 0.5*binSize*5, 5*maxEnergy - 0.5*binSize*5);

    EnergyDepositionWithVeto = new TH1D("EnergyDepositionWithVeto","Energy Deposition; Energy [MeV]; Counts", (maxEnergy - minEnergy)/binSize,
                                minEnergy - 0.5*binSize, maxEnergy - 0.5*binSize);
    EnergyDepositionWithVetoSmeared = new TH1D("EnergyDepositionWithVetoSmeared","Energy Deposition Smeared; Energy [MeV]; Counts", (maxEnergy - minEnergy)/binSize,
                                       minEnergy - 0.5*binSize, maxEnergy - 0.5*binSize);

    EnergyDepositionCapture = new TH1D("EnergyDepositionCapture","Energy Deposition; Energy [MeV]; Counts", (maxEnergy - minEnergy)/binSize,
                                minEnergy - 0.5*binSize, maxEnergy - 0.5*binSize);
    EnergyDepositionInelastic = new TH1D("EnergyDepositionInelastic","Energy Deposition; Energy [MeV]; Counts", (maxEnergy - minEnergy)/binSize,
                                       minEnergy - 0.5*binSize, maxEnergy - 0.5*binSize);
    EnergyDepositionOther = new TH1D("EnergyDepositionOther","Energy Deposition; Energy [MeV]; Counts", (maxEnergy - minEnergy)/binSize,
                                         minEnergy - 0.5*binSize, maxEnergy - 0.5*binSize);

    EnergyDepositionVsMassNumber = new TH2D("EnergyDepositionVsMassNumber","Energy Deposition; Energy [MeV]; Counts", (maxEnergy - minEnergy)/binSize,
                                minEnergy - 0.5*binSize, maxEnergy - 0.5*binSize, 300, -0.5, 299.5);
  }

  void CreateTimingHistos(double minTime, double timeSeparator, double maxTime, double binSizeSmall, double binSizeLarge)
  {
    TimeLaBr = new TH1D("TimeLaBr","Time; Time [us]; Counts", (timeSeparator - minTime)/binSizeSmall,
                                   minTime - 0.5*binSizeSmall, timeSeparator - 0.5*binSizeSmall);
    TimeVeto = new TH1D("TimeVeto","Time; Time [us]; Counts", (timeSeparator - minTime)/binSizeSmall,
                        minTime - 0.5*binSizeSmall, timeSeparator - 0.5*binSizeSmall);
    TimeDifferenceSmall = new TH1D("TimeDifferenceSmall","Time Difference; Time [us]; Counts", (timeSeparator - minTime)/binSizeSmall,
                                minTime - 0.5*binSizeSmall, timeSeparator - 0.5*binSizeSmall);
    TimeDifferenceLarge = new TH1D("TimeDifferenceLarge","Time Difference; Time [us]; Counts", (maxTime - timeSeparator)/binSizeLarge,
                                       timeSeparator - 0.5*binSizeLarge, maxTime - 0.5*binSizeLarge);
    timeSep = timeSeparator;

    TimeLaBrCaptureSmall = new TH1D("TimeLaBrCaptureSmall","Time; Time [us]; Counts", (timeSeparator - minTime)/binSizeSmall,
                                    minTime - 0.5*binSizeSmall, timeSeparator - 0.5*binSizeSmall);
    TimeLaBrInelasticSmall = new TH1D("TimeLaBrInelasticSmall","Time; Time [us]; Counts", (timeSeparator - minTime)/binSizeSmall,
                                      minTime - 0.5*binSizeSmall, timeSeparator - 0.5*binSizeSmall);
    TimeLaBrOtherSmall = new TH1D("TimeLaBrOtherSmall","Time; Time [us]; Counts", (timeSeparator - minTime)/binSizeSmall,
                                  minTime - 0.5*binSizeSmall, timeSeparator - 0.5*binSizeSmall);
    TimeLaBrCaptureLarge = new TH1D("TimeLaBrCaptureLarge","Time; Time [us]; Counts", (maxTime - timeSeparator)/binSizeLarge,
                                    timeSeparator - 0.5*binSizeLarge, maxTime - 0.5*binSizeLarge);
    TimeLaBrInelasticLarge = new TH1D("TimeLaBrInelasticLarge","Time; Time [us]; Counts", (maxTime - timeSeparator)/binSizeLarge,
                                      timeSeparator - 0.5*binSizeLarge, maxTime - 0.5*binSizeLarge);
    TimeLaBrOtherLarge = new TH1D("TimeLaBrOtherLarge","Time; Time [us]; Counts", (maxTime - timeSeparator)/binSizeLarge,
                                  timeSeparator - 0.5*binSizeLarge, maxTime - 0.5*binSizeLarge);
  }

  void CreateEnergyVsTimingHistos(double minEnergy, double maxEnergy, double binSize, double minTime, double maxTime, double binSizeLarge)
  {
    double eneBinSize = 2*binSize;
    double timeBinSize = 5*binSizeLarge;
    EnergyDepositionVsTimeDiff = new TH2D("EnergyDepositionVsTimeDiff","Energy Deposition; Energy [MeV]; Time [us]", (maxEnergy - minEnergy)/eneBinSize,
                                          minEnergy - 0.5*eneBinSize, maxEnergy - 0.5*eneBinSize, (maxTime - minTime)/timeBinSize,
                                          minTime - 0.5*timeBinSize, maxTime - 0.5*timeBinSize);
    EnergyDepositionVsTimeDiffSmeared = new TH2D("EnergyDepositionWithVetoSmearedVsTimeDiff","Energy Deposition Smeared; Energy [MeV]; Time [us]",
                                                 (maxEnergy - minEnergy)/eneBinSize, minEnergy - 0.5*eneBinSize, maxEnergy - 0.5*eneBinSize,
                                                 (maxTime - minTime)/timeBinSize, minTime - 0.5*timeBinSize, maxTime - 0.5*timeBinSize);
    double minTimeCut = 0.002, binSizeForTimeCut = 0.002;
    int noOfBinsForTimeCut = 50;
    EnergyDepositionVsTimeDiffCut = new TH2D("EnergyDepositionVsTimeDiffCut","Energy Deposition; Energy [MeV]; Time [us]", (maxEnergy - minEnergy)/eneBinSize,
                                             minEnergy - 0.5*eneBinSize, maxEnergy - 0.5*eneBinSize, noOfBinsForTimeCut,
                                             minTimeCut - 0.5*binSizeForTimeCut, minTimeCut + (noOfBinsForTimeCut-0.5)*binSizeForTimeCut);
    EnergyDepositionVsTimeDiffCutSmeared = new TH2D("EnergyDepositionWithVetoSmearedVsTimeDiffCut","Energy Deposition Smeared; Energy [MeV]; Time [us]",
                                                    (maxEnergy - minEnergy)/eneBinSize, minEnergy - 0.5*eneBinSize, maxEnergy - 0.5*eneBinSize,
                                                    noOfBinsForTimeCut, minTimeCut - 0.5*binSizeForTimeCut, minTimeCut + (noOfBinsForTimeCut-0.5)*binSizeForTimeCut);
  }

// If more histos create table/vector/container and add references to the common functions
  void FillEnergyDeposition(double energy) {EnergyDeposition->Fill(energy);}
  void FillEnergyDepositionSmeared(double energy) {EnergyDepositionSmeared->Fill(energy);}
  void FillEnergyDepositionVeto(double energy) {EnergyDepositionVeto->Fill(energy);}
  void FillEnergyDepositionWithVeto(double energy) {EnergyDepositionWithVeto->Fill(energy);}
  void FillEnergyDepositionWithVetoSmeared(double energy) {EnergyDepositionWithVetoSmeared->Fill(energy);}
  void FillEnergyDepositionVsTimeDiff(double energy, double time) {EnergyDepositionVsTimeDiff->Fill(energy, time);}
  void FillEnergyDepositionVsTimeDiffSmeared(double energy, double time) {EnergyDepositionVsTimeDiffSmeared->Fill(energy, time);}
  void FillEnergyDepositionVsTimeDiffCut(double energy, double time) {
    for (unsigned i=1; i<EnergyDepositionVsTimeDiffCut->GetYaxis()->GetNbins(); i++) {
      double binCent = EnergyDepositionVsTimeDiffCut->GetYaxis()->GetBinCenter(i);
      if (time > binCent)
        EnergyDepositionVsTimeDiffCut->Fill(energy, binCent);
    }
  }
  void FillEnergyDepositionVsTimeDiffCutSmeared(double energy, double time) {
    for (unsigned i=1; i<EnergyDepositionVsTimeDiffCutSmeared->GetYaxis()->GetNbins(); i++) {
      double binCent = EnergyDepositionVsTimeDiffCutSmeared->GetYaxis()->GetBinCenter(i);
      if (time > binCent)
        EnergyDepositionVsTimeDiffCutSmeared->Fill(energy, binCent);
    }
  }
  void FillEnergyDepositionForAProcess(double energy, char label) {
    if (label == 'C')
      EnergyDepositionCapture->Fill(energy);
    else if (label == 'I')
      EnergyDepositionInelastic->Fill(energy);
    else
      EnergyDepositionOther->Fill(energy);
  }
  void FillEnergyDepositionVsMassNumber(double energy, int massNo) {
    EnergyDepositionVsMassNumber->Fill(energy, massNo);
  }
  void FillTimeLaBr(double time) {TimeLaBr->Fill(time);}
  void FillTimeVeto(double time) {TimeVeto->Fill(time);}
  void FillTimeDifference(double time)
  {
    if (time < timeSep)
      TimeDifferenceSmall->Fill(time);
    else
      TimeDifferenceLarge->Fill(time);
  }
  void FillTimeLaBrForAProcess(double time, char label) {
    if (label == 'C') {
      if (time < timeSep)
        TimeLaBrCaptureSmall->Fill(time);
      else
        TimeLaBrCaptureLarge->Fill(time);
    } else if (label == 'I') {
      if (time < timeSep)
        TimeLaBrInelasticSmall->Fill(time);
      else
        TimeLaBrInelasticLarge->Fill(time);
    } else {
      if (time < timeSep)
        TimeLaBrOtherSmall->Fill(time);
      else
        TimeLaBrOtherLarge->Fill(time);
    }
  }

  void SaveHistos(TString output)
  {
    std::cout << "Saving histos to " << output << std::endl;
    TFile* outfile = new TFile(output, "RECREATE");
    outfile->cd();
    EnergyDeposition->Write("EnergyDeposition");
    EnergyDepositionSmeared->Write("EnergyDepositionSmeared");
    EnergyDepositionVeto->Write("EnergyDepositionVeto");
    EnergyDepositionWithVeto->Write("EnergyDepositionWithVeto");
    EnergyDepositionWithVetoSmeared->Write("EnergyDepositionWithVetoSmeared");
    EnergyDepositionVsTimeDiff->Write("EnergyDepositionVsTimeDiff");
    EnergyDepositionVsTimeDiffSmeared->Write("EnergyDepositionVsTimeDiffSmeared");
    EnergyDepositionVsTimeDiffCut->Write("EnergyDepositionVsTimeDiffCut");
    EnergyDepositionVsTimeDiffCutSmeared->Write("EnergyDepositionVsTimeDiffCutSmeared");
    EnergyDepositionCapture->Write("EnergyDepositionCapture");
    EnergyDepositionInelastic->Write("EnergyDepositionInelastic");
    EnergyDepositionOther->Write("EnergyDepositionOther");
    EnergyDepositionVsMassNumber->Write("EnergyDepositionVsMassNumber");
    TimeLaBr->Write("TimeLaBr");
    TimeVeto->Write("TimeVeto");
    TimeDifferenceSmall->Write("TimeDifferenceSmall");
    TimeDifferenceLarge->Write("TimeDifferenceLarge");
    TimeLaBrCaptureSmall->Write("TimeLaBrCaptureSmall");
    TimeLaBrInelasticSmall->Write("TimeLaBrInelasticSmall");
    TimeLaBrOtherSmall->Write("TimeLaBrOtherSmall");
    TimeLaBrCaptureLarge->Write("TimeLaBrCaptureLarge");
    TimeLaBrInelasticLarge->Write("TimeLaBrInelasticLarge");
    TimeLaBrOtherLarge->Write("TimeLaBrOtherLarge");
    outfile->Close();
  }

private:
  double timeSep;

  TH1D *EnergyDeposition;
  TH1D *EnergyDepositionSmeared;
  TH1D *EnergyDepositionVeto;
  TH1D *EnergyDepositionWithVeto;
  TH1D *EnergyDepositionWithVetoSmeared;
  TH2D *EnergyDepositionVsTimeDiff;
  TH2D *EnergyDepositionVsTimeDiffSmeared;
  TH2D *EnergyDepositionVsTimeDiffCut;
  TH2D *EnergyDepositionVsTimeDiffCutSmeared;

  TH1D *EnergyDepositionCapture;
  TH1D *EnergyDepositionInelastic;
  TH1D *EnergyDepositionOther;
  TH2D* EnergyDepositionVsMassNumber;

  TH1D *TimeLaBr;
  TH1D *TimeVeto;
  TH1D *TimeDifferenceSmall;
  TH1D *TimeDifferenceLarge;
  TH1D *TimeLaBrCaptureSmall;
  TH1D *TimeLaBrInelasticSmall;
  TH1D *TimeLaBrOtherSmall;
  TH1D *TimeLaBrCaptureLarge;
  TH1D *TimeLaBrInelasticLarge;
  TH1D *TimeLaBrOtherLarge;
};

#endif
