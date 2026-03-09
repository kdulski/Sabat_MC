#include <sys/stat.h>
#include <functional>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <utility>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <string>
#include <cstdio>
#include <chrono>
#include <cmath>
#include <map>
#include <time.h>
#include <unistd.h>

#include <TGraphAsymmErrors.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TMinuit.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TFile.h>
#include <TMath.h>
#include <TROOT.h>
#include <TKey.h>
#include <TF12.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3F.h>
#include <TF1.h>
#include <TF2.h>

int typeOfBackground = 1; // 1 -> exponential, 2 -> quadratic

int sizeOfHalfWindow = 5;
int orderOfPolynomialToInterpolate = 2;

TF1* initialBG = nullptr;
TGraph* initialPeaks = nullptr;

bool FileCheck(const std::string& NameOfFile);
std::string NumberToChar(double number, int precision);

void PlotDerivatives(TH1D* histo, std::string nameOfInput);
TGraph* FindDecreasingSeries(TH1D* derivHisto);
TGraphAsymmErrors* FindHistogramBGpoints(TH1D* derivHisto);
void FitSimSpectrum(TH1D* histoToFit, std::string nameOfInput);
void FitExpSpectrum(TH1D* histoToFit, std::string nameOfInput);
std::string FitLine(TH1D* histoToFit, double energy);

Double_t eneLineFunction(Double_t *A, Double_t *P);
Double_t bgFunction(Double_t *A, Double_t *P);
double GaussDistr(double x, double mean, double sigma);
double GaussDistrToDraw(Double_t *A, Double_t *P);
double GramPolynomial(int i, int m, int k, int s);
double GenFactor(int a, int b);
double CalcWeight(int i, int t, int m, int n, int s);
std::vector<double> ComputeWeights(int m, int t, int n, int s);

int main(int argc, char* argv[])
{
  if (argc < 2) {
    std::cout << "Not enough arguments. Provide the name of the input root file" << std::endl;
    return 0;
  }
  std::string nameOfInput = argv[1];

  int checkOfExistence = FileCheck(nameOfInput);

  if (!checkOfExistence || nameOfInput.size() < 6) {
    std::cout << "No such file or name without proper extension (.root, .dat or .txt): " << nameOfInput << std::endl;
    return 0;
  }

  TString fileToAnalyze = nameOfInput;
  TString ext(fileToAnalyze(fileToAnalyze.Length()-5,fileToAnalyze.Length()));
  TH1D* histoToFit, *histoToFitM;

  std::cout << " Reading file " << fileToAnalyze << std::endl;

  bool RootOrNot = true;

  if (ext == ".root") {
    TFile* inputFile = new TFile(fileToAnalyze, "READ");

    TIter next(inputFile->GetListOfKeys());
    TKey *key;
    TH1D* temp1D;
    TH2D* temp2D;
    std::vector<TH1D*> HistogramsToPotentialFit1D;
    std::vector<TH2D*> HistogramsToPotentialFit2D;

    while ((key = (TKey*)next())) {
      TClass *cl = gROOT->GetClass(key->GetClassName());

      if (cl->InheritsFrom("TH1D")) {
        temp1D = (TH1D*) key->ReadObj()->Clone();
        temp1D->SetDirectory(0);
        std::cout << "1D Histogram with name: " << key->GetName() << " have ID equal to " << HistogramsToPotentialFit1D.size() << std::endl;
        HistogramsToPotentialFit1D.push_back(temp1D);
      } else if (cl->InheritsFrom("TH2D")) {
        temp2D = (TH2D*) key->ReadObj()->Clone();
        temp2D->SetDirectory(0);
        std::cout << "2D Histogram with name: " << key->GetName() << " have ID equal to " << HistogramsToPotentialFit2D.size() << std::endl;
        HistogramsToPotentialFit2D.push_back(temp2D);
      }
    }

    TString histoName;

    if (HistogramsToPotentialFit1D.size() + HistogramsToPotentialFit2D.size() == 0) {
      std::cout << "No histogram to fit" << std::endl;
      return 0;
    } else {
      int IDofHistogram;
      TString histoDimension;
      std::cout << "Provide the ID of the histogram to fit:" << std::endl;
      std::cin >> IDofHistogram;
      std::cout << "Do you want to fit 1D or 2D histogram? Write 1D or 2D. If you want 2D but substract projections type M" << std::endl;
      std::cin >> histoDimension;

      if (histoDimension == "1D" || histoDimension == "1") {
        histoToFit = HistogramsToPotentialFit1D.at(IDofHistogram);
        histoName = histoToFit->GetName();
      } else if (histoDimension == "2D" || histoDimension == "2") {
        int yBinMin, yBinMax;
        std::cout << "From which bin for projection of 2D histogram you want to fit. Starting from 1" << std::endl;
        std::cin >> yBinMin;
        std::cout << "Until which bin for projection of 2D histogram you want to fit. Less than " << HistogramsToPotentialFit2D.at(IDofHistogram)->GetNbinsY() << std::endl;
        std::cin >> yBinMax;

        if (yBinMin < 1 || yBinMin > HistogramsToPotentialFit2D.at(IDofHistogram)->GetNbinsY()) {
          std::cout << "Provded bin " << yBinMin << " is less than 1 or greater than number of bins equal to ";
          std::cout << HistogramsToPotentialFit2D.at(IDofHistogram)->GetNbinsY() << std::endl;
          return 0;
        } else if (yBinMax < 1 || yBinMax > HistogramsToPotentialFit2D.at(IDofHistogram)->GetNbinsY()) {
          std::cout << "Provded bin " << yBinMax << " is less than 1 or greater than number of bins equal to ";
          std::cout << HistogramsToPotentialFit2D.at(IDofHistogram)->GetNbinsY() << std::endl;
          return 0;
        }

        std::cout << "Getting projection for Y min: " << HistogramsToPotentialFit2D.at(IDofHistogram)->GetYaxis()->GetBinCenter(yBinMin) << std::endl;
        std::cout << "Getting projection for Y max: " << HistogramsToPotentialFit2D.at(IDofHistogram)->GetYaxis()->GetBinCenter(yBinMax) << std::endl;
        histoToFit = (TH1D*)HistogramsToPotentialFit2D.at(IDofHistogram)->ProjectionX("projX",yBinMin,yBinMax);
        histoToFit->SetDirectory(0);
        histoName = HistogramsToPotentialFit2D.at(IDofHistogram)->GetName();
        TString tempAdd = "for_bin_";
        TString binAdd = NumberToChar(yBinMin, 0) + " " + NumberToChar(yBinMax, 0);
        histoName = histoName + tempAdd + binAdd;
        if (histoToFit->GetEntries() == 0) {
          std::cout << "Histogram " << HistogramsToPotentialFit2D.at(IDofHistogram)->GetName() << " for bins " << yBinMin << " " << yBinMax << " have no entries" << std::endl;
          return 0;
        }
        nameOfInput = "ProjPos_" + nameOfInput;
      } else if (histoDimension == "2DM" || histoDimension == "2M" || histoDimension == "2Dm" || histoDimension == "2m" || histoDimension == "M" || histoDimension == "m") {
        int yBinMin, yBinMax;
        std::cout << "From which bin for projection of 2D histogram you want to substract. Starting from 1" << std::endl;
        std::cin >> yBinMin;
        std::cout << "To which bin for projection of 2D histogram you want to substract. Less than " << HistogramsToPotentialFit2D.at(IDofHistogram)->GetNbinsY() << std::endl;
        std::cin >> yBinMax;

        if (yBinMin < 1 || yBinMin > HistogramsToPotentialFit2D.at(IDofHistogram)->GetNbinsY()) {
          std::cout << "Provded bin " << yBinMin << " is less than 1 or greater than number of bins equal to ";
          std::cout << HistogramsToPotentialFit2D.at(IDofHistogram)->GetNbinsY() << std::endl;
          return 0;
        } else if (yBinMax < 1 || yBinMax > HistogramsToPotentialFit2D.at(IDofHistogram)->GetNbinsY()) {
          std::cout << "Provded bin " << yBinMax << " is less than 1 or greater than number of bins equal to ";
          std::cout << HistogramsToPotentialFit2D.at(IDofHistogram)->GetNbinsY() << std::endl;
          return 0;
        }

        std::cout << "Getting projection for Y plus: " << HistogramsToPotentialFit2D.at(IDofHistogram)->GetYaxis()->GetBinCenter(yBinMin) << std::endl;
        std::cout << "Getting projection for Y minus: " << HistogramsToPotentialFit2D.at(IDofHistogram)->GetYaxis()->GetBinCenter(yBinMax) << std::endl;
        histoToFit = (TH1D*)HistogramsToPotentialFit2D.at(IDofHistogram)->ProjectionX("projX",yBinMin,yBinMin);
        histoToFitM = (TH1D*)HistogramsToPotentialFit2D.at(IDofHistogram)->ProjectionX("projXM",yBinMax,yBinMax);
        histoToFit->SetDirectory(0);
        histoToFitM->SetDirectory(0);
        histoToFit->Add(histoToFitM,-1);
        histoName = HistogramsToPotentialFit2D.at(IDofHistogram)->GetName();
        TString tempAdd = "for_bin_";
        TString binAdd = NumberToChar(yBinMin, 0) + " minus " + NumberToChar(yBinMax, 0);
        histoName = histoName + tempAdd + binAdd;
        if (histoToFit->GetEntries() == 0) {
          std::cout << "Histogram " << HistogramsToPotentialFit2D.at(IDofHistogram)->GetName() << " for bins " << yBinMin << " or " << yBinMax << " have no entries" << std::endl;
          return 0;
        }
        nameOfInput = "ProjNeg_" + nameOfInput;
      } else {
        std::cout << "Wrong option given. Type 1D or 2D " << std::endl;
        return 0;
      }
    }
    inputFile->Close();
  } else {
    RootOrNot = false;
    std::ifstream data;
    std::string energy, count;
    data.open(fileToAnalyze);
    std::vector<double> Counts;

    int nmbOfBins = 0;
    double energyDiscriminator = 3E-1;
    double firstEnergy = energyDiscriminator, lastEnergy;
    while (data >> energy >> count) {
      if (firstEnergy == energyDiscriminator) {
        if (energy != "inf" && energy != "nan")
          firstEnergy = std::stod(energy)/1000;
      }
      if (energy != "inf" && energy != "nan")
        lastEnergy = std::stod(energy)/1000;

      if (count != "inf" && count != "nan") {
        Counts.push_back(stod(count));
        nmbOfBins++;
      }
    }
    data.close();
    double binWidth = (lastEnergy-firstEnergy)/nmbOfBins;

    histoToFit = new TH1D("histoToFit", "Histogram from the data; Energy [MeV]; Counts", nmbOfBins, firstEnergy, lastEnergy);
    for (unsigned i=1; i<=nmbOfBins; i++) {
      histoToFit->SetBinContent(i, Counts[i]);
    }
    histoToFit->Rebin(4);
  }

  if (histoToFit) {
    if (RootOrNot)
      FitSimSpectrum(histoToFit, nameOfInput);
    else {
      PlotDerivatives(histoToFit, nameOfInput);
      FitExpSpectrum(histoToFit, nameOfInput);
    }

//  PlotDerivatives(histoToFit, nameOfInput);
/*  double desiredEnergy = 1.;
  std::string test = "y";
  while (test == "y") {
    std::cout << "What energy line to estimate in [MeV]: " << std::endl;
    std::cin >> desiredEnergy;
    test = FitLine(histoToFit, desiredEnergy);
  }*/
  }

  return 0;
}

void PlotDerivatives(TH1D* histoToFit, std::string nameOfInput)
{
  int controlNumber = histoToFit->GetBinContent(10), controlNumber2 = histoToFit->GetBinContent(15);

  std::vector<double> weightsForSmooth = ComputeWeights(sizeOfHalfWindow, 0, orderOfPolynomialToInterpolate, 0);
  // 2nd arg - central point, 4th - order of interpolation funtion, where 0 is smooth and 1 is derivative
  std::vector<double> weightsForDerivative = ComputeWeights(sizeOfHalfWindow, 0, orderOfPolynomialToInterpolate, 1);
  std::vector<double> weightsFor2ndDerivative = ComputeWeights(sizeOfHalfWindow, 0, orderOfPolynomialToInterpolate, 2);

  TH1D* smoothHisto = (TH1D*)histoToFit->Clone("smoothHisto");
  TH1D* derivHisto = (TH1D*)histoToFit->Clone("derivHisto");
  TH1D* secDerivHisto = (TH1D*)histoToFit->Clone("secDerivHisto");
  int endBin = histoToFit->GetNbinsX();
  double sizeOfBin = 1.;
  //  if (endBin > 3)
  //    sizeOfBin = derivHisto->GetXaxis()->GetBinCenter(2) - derivHisto->GetXaxis()->GetBinCenter(1);

  for (unsigned i=sizeOfHalfWindow+2; i<endBin-sizeOfHalfWindow-2; i++) {
    double weight = 0;
    for (int j=-sizeOfHalfWindow; j<=sizeOfHalfWindow; j++) {
      weight += weightsForDerivative.at(j+sizeOfHalfWindow)*histoToFit->GetBinContent(i+j);
    }
    derivHisto->SetBinContent(i, weight/sizeOfBin);

    weight = 0;
    for (int j=-sizeOfHalfWindow; j<=sizeOfHalfWindow; j++) {
      weight += weightsFor2ndDerivative.at(j+sizeOfHalfWindow)*histoToFit->GetBinContent(i+j);
    }
    secDerivHisto->SetBinContent(i, weight/sizeOfBin);

    weight = 0;
    for (int j=-sizeOfHalfWindow; j<=sizeOfHalfWindow; j++) {
      weight += weightsForSmooth.at(j+sizeOfHalfWindow)*histoToFit->GetBinContent(i+j);
    }
    smoothHisto->SetBinContent(i, weight/sizeOfBin);
  }
  for (unsigned i=0; i<sizeOfHalfWindow+1+1; i++) {
    derivHisto->SetBinContent(1+i, 0);
    derivHisto->SetBinContent(endBin-1-i, 0);
    secDerivHisto->SetBinContent(1+i, 0);
    secDerivHisto->SetBinContent(endBin-1-i, 0);
  }

//  TString outputName = "Derivatives_" + nameOfInput;
  TString outputName = "Derivatives_" + NumberToChar(controlNumber, 0) + "_" + NumberToChar(controlNumber2, 0) + ".root";
  TFile* outputFile = new TFile(outputName, "RECREATE");

  histoToFit->Write("RawEnergy");
  smoothHisto->Write("SmoothEnergy");
  derivHisto->Write("Derivative");
  secDerivHisto->Write("2ndDerivative");

  TGraph* locMaxCand = FindDecreasingSeries(derivHisto);
  locMaxCand->Write("LocalMaxCand");

  TGraphAsymmErrors* locBgCand = FindHistogramBGpoints(histoToFit);
  std::vector<double> backgroundGaussesOff = {0.5, 1.26, 1.76, 2.19, 2.36, 4.4, 5.6, 6.55};
  TF1* fitFunc = new TF1("fitFunc", bgFunction, 0.4, 11, 4 + 3*backgroundGaussesOff.size());
  fitFunc->SetParameter(0, 0.5*histoToFit->GetMinimum());
  fitFunc->SetParLimits(0, 0, 10*histoToFit->GetMinimum());
  fitFunc->SetParameter(1, 4*histoToFit->GetBinContent((histoToFit->FindBin(0.4))));
  fitFunc->SetParLimits(1, 0.1*histoToFit->GetBinContent((histoToFit->FindBin(0.4))), 10*histoToFit->GetBinContent((histoToFit->FindBin(0.4))));
  fitFunc->SetParameter(2, 0.4*histoToFit->GetMean());
  fitFunc->SetParLimits(2, 0.15*histoToFit->GetMean(), 0.75*histoToFit->GetMean());
  fitFunc->FixParameter(3, backgroundGaussesOff.size());
  for (unsigned i=0; i<backgroundGaussesOff.size(); i++) {
    Double_t offset = backgroundGaussesOff.at(i);
    Double_t intens = 0.1*histoToFit->GetBinContent(histoToFit->FindBin(backgroundGaussesOff.at(i)));
    Double_t sigma = 0.1*offset;

    fitFunc->SetParameter(4+3*i, intens);
    fitFunc->SetParName(4+3*i, "Intensity");
    fitFunc->SetParLimits(4+3*i, 0, 100*histoToFit->GetMaximum());

    fitFunc->SetParameter(5+3*i, sigma);
    fitFunc->SetParName(5+3*i, "Sigma");
    fitFunc->SetParLimits(5+3*i, 0.01, 5);

    fitFunc->SetParameter(6+3*i, offset);
    fitFunc->SetParName(6+3*i, "Offset");
    fitFunc->SetParLimits(6+3*i, 0.4, 10.8);
  }
  locBgCand->Fit(fitFunc,"RM");
  locBgCand->Write("LocalBGpoints");

  initialBG = fitFunc;
  initialPeaks = locMaxCand;

  outputFile->Close();
}

TGraph* FindDecreasingSeries(TH1D* derivHisto)
{
// importance of maximum will be defined by the length and the amplitude of the constantly decreasing series
  int startBin = 0, stopBin = 0;
  int minLength = 10;
  int halfLengthToCheckExtremum = 11;
  bool testMax, testMin;

  std::vector<std::pair<int,int>> localMaxima; // start + stop bins

  for (unsigned i=halfLengthToCheckExtremum+1; i<derivHisto->GetNbinsX() - halfLengthToCheckExtremum; i++) {
    if (startBin != 0) {
      if (derivHisto->GetBinContent(startBin) < derivHisto->GetBinContent(i))
        startBin = 0;
    }

    if (startBin == 0) {
      testMax = true;
      int j=1;
      while (testMax && j<=halfLengthToCheckExtremum) {
        if ((derivHisto->GetBinContent(i) < derivHisto->GetBinContent(i+j) || derivHisto->GetBinContent(i) < derivHisto->GetBinContent(i-j)) && testMax)
          testMax = false;
        j++;
      }
      if (testMax)
        startBin = i;
    } else {
      testMin = true;
      int j=1;
      while (testMin && j<=halfLengthToCheckExtremum) {
        if ((derivHisto->GetBinContent(i) > derivHisto->GetBinContent(i+j) || derivHisto->GetBinContent(i) > derivHisto->GetBinContent(i-j)) && testMin)
          testMin = false;
        j++;
      }
      if (testMin) {
        if (i - startBin > minLength)
          localMaxima.push_back(std::make_pair(startBin, i));
        startBin = 0;
      }
    }
  }
  const Int_t n = localMaxima.size();
  Double_t x[n], y[n];
  for (unsigned i=0; i<localMaxima.size(); i++) {
    int centralBin  = (localMaxima.at(i).second + localMaxima.at(i).first)/2;
// y = ax + b
    double a = (derivHisto->GetBinContent(localMaxima.at(i).first) - derivHisto->GetBinContent(localMaxima.at(i).second)) /
               (derivHisto->GetBinCenter(localMaxima.at(i).first) - derivHisto->GetBinCenter(localMaxima.at(i).second));
    double b = derivHisto->GetBinContent(localMaxima.at(i).first) - a*derivHisto->GetBinCenter(localMaxima.at(i).first);
    x[i] = -b/a;
    y[i] = derivHisto->GetBinContent(localMaxima.at(i).first) - derivHisto->GetBinContent(localMaxima.at(i).second);
  }

  return new TGraph(n,x,y);
}

TGraphAsymmErrors* FindHistogramBGpoints(TH1D* derivHisto)
{
  // importance of maximum will be defined by the length and the amplitude of the constantly decreasing series
  int startBin = 0, stopBin = 0;
  int halfLengthToCheckExtremum = 10;
  bool testMax, testMin;

  std::vector<int> BGpoints; // start + stop bins

  for (unsigned i=halfLengthToCheckExtremum+1; i<derivHisto->GetNbinsX() - halfLengthToCheckExtremum; i++) {
    testMin = true;
    int j=1;
    while (testMin && j<=halfLengthToCheckExtremum) {
      if ((derivHisto->GetBinContent(i) > derivHisto->GetBinContent(i+j) || derivHisto->GetBinContent(i) > derivHisto->GetBinContent(i-j)) && testMin)
        testMin = false;
      j++;
    }
    if (testMin) {
      BGpoints.push_back(i);
    }
  }
  const Int_t n = BGpoints.size();
  Double_t x[n], y[n];
  Double_t ex[n], ey[n];
  for (unsigned i=0; i<BGpoints.size(); i++) {
    x[i] = derivHisto->GetBinCenter(BGpoints.at(i));
    y[i] = derivHisto->GetBinContent(BGpoints.at(i));
    ex[i] = 0;
    ey[i] = 0;
  }

  return new TGraphAsymmErrors(n,x,y,ex,ex,ey,ey);
}

void FitSimSpectrum(TH1D* histoToFit, std::string nameOfInput)
{
  std::cout << "Fitting background separately " << (typeOfBackground == 1 ? "exponential type " : "quadratic type ") << std::endl;
  std::vector<double> backgroundGaussesOff = {0.5, 1.26, 1.76, 2.19, 2.36, 4.4, 5.6, 6.55};
  std::vector<double> backgroundGaussesOffFreddom = {0.06, 0.09, 0.15, 0.2, 0.4, 0.85, 0.6, 0.3};
  double FracFromHisto = 1;
  std::vector<double> backgroundGaussesOffIntensMaxFrac = {3, 225, 120, 250, 15, 15, 20, 20}; //{3, 225, 120, 250, 15, 15, 20, 20};(1B counts)
  std::vector<double> predefGaussOff = {0.44, 0.472, 0.511, 0.625, 0.692, 0.785, 0.845, 0.88, 0.91, 0.95, 1.01, 1.16, 1.25, 1.31, 1.41, 1.48, 1.625, 1.73, 1.825, 1.965, 2.12, 2.23, 2.31, 2.44, 2.74, 2.86, 3.06, 3.84, 4.44, 4.98, 5.74, 6.02, 6.13, 6.36, 6.65, 6.92, 7.12, 7.41, 7.64, 7.79, 8.22, 8.58, 10.8};
  int hydrogenGauss = 21, eeAnniGauss = 2, oxygenGauss = 32, chlorineGauss = 19; // When adding/removing components from above adjust this parameters
  int noOfPredefinedGauss = predefGaussOff.size(), noOfRandomGauss = backgroundGaussesOff.size();
  unsigned noOfParameters = 3 + 1 + 3*(noOfRandomGauss + noOfPredefinedGauss);
  Double_t startFit = 0.428, endFit = 11;
  int numberOfPointsForDrawing = (endFit - startFit)*500;
/*
Parameters
0 -2 -> BG as exp/poly2 + const
3 -> noOfRandomGausses
the rest are intensity + sigma + offset for each gaussian
*/
  TF1* fitFunc = new TF1("fitFunc", bgFunction, startFit, endFit, noOfParameters);
  fitFunc -> SetNpx(numberOfPointsForDrawing);

  double min = 0.000003*histoToFit->GetMaximum();
  if (typeOfBackground == 1) {
    fitFunc->SetParameter(0, 0.5*min);
    fitFunc->SetParName(0, "Background constant");
    fitFunc->SetParLimits(0, 0, min);
    fitFunc->SetParameter(1, 4*histoToFit->GetBinContent((histoToFit->FindBin(startFit))));
    fitFunc->SetParName(1, "Background amplitude");
//    fitFunc->SetParLimits(1, 0.25*histoToFit->GetBinContent((histoToFit->FindBin(startFit))), 10*histoToFit->GetBinContent((histoToFit->FindBin(startFit))));
    fitFunc->SetParLimits(1, 0.25*histoToFit->GetBinContent((histoToFit->FindBin(startFit))), 10*histoToFit->GetBinContent((histoToFit->FindBin(startFit)))); // for labrcut
//    fitFunc->SetParameter(2, 0.6*histoToFit->GetMean());
//    fitFunc->SetParLimits(2, 0.2*histoToFit->GetMean(), 0.75*histoToFit->GetMean());
    fitFunc->SetParameter(2, 0.4*histoToFit->GetMean());
    fitFunc->SetParLimits(2, 0.2*histoToFit->GetMean(), 0.5*histoToFit->GetMean());
    fitFunc->SetParName(2, "Background mean");

    FracFromHisto = 0.001*histoToFit->GetBinContent((histoToFit->FindBin(startFit)));
  } else {
    double amplitudeLimit = (histoToFit->GetMaximum() - min)/4E8;
    fitFunc->SetParameter(0, 0.5*amplitudeLimit);
    fitFunc->SetParName(0, "Background amplitude");
    fitFunc->SetParLimits(0, 0, amplitudeLimit);
    fitFunc->SetParameter(1, 0.5*(endFit - startFit));
    fitFunc->SetParName(1, "Background minimum horizontal shift");
    fitFunc->SetParLimits(1, 0, endFit);
    double minLimit = histoToFit->GetBinContent((histoToFit->FindBin(endFit)));
    fitFunc->SetParameter(2, minLimit);
    fitFunc->SetParName(2, "Background minimum vertical shift");
    fitFunc->SetParLimits(2, 0, 1.2*minLimit);
  }
  fitFunc->FixParameter(3, noOfRandomGauss + noOfPredefinedGauss);
  fitFunc->SetParName(3, "Number of Gaussians");

  double veryLargeIntensity = histoToFit->GetMaximum()*10;
  double freedomFraction = 0.01;
  for (unsigned i=0; i<noOfPredefinedGauss; i++) {
    Double_t offset = predefGaussOff.at(i);
    Double_t intens = 0.001*veryLargeIntensity/std::pow(offset, 2);
    Double_t sigma = 0.02*offset;

    fitFunc->SetParameter(4+3*i, intens);
    fitFunc->SetParName(4+3*i, "Intensity");
    fitFunc->SetParLimits(4+3*i, 0, veryLargeIntensity);

    fitFunc->SetParameter(5+3*i, sigma);
    fitFunc->SetParName(5+3*i, "Sigma");
    fitFunc->SetParLimits(5+3*i, 0.5*sigma, 1.1*sigma);

    fitFunc->SetParameter(6+3*i, offset);
    fitFunc->SetParName(6+3*i, "Offset");
    fitFunc->SetParLimits(6+3*i, (1-0.5*freedomFraction)*offset, (1+0.5*freedomFraction)*offset);
  }
  for (unsigned i=noOfPredefinedGauss; i<noOfRandomGauss + noOfPredefinedGauss; i++) {
    Double_t offset = backgroundGaussesOff.at(i-noOfPredefinedGauss);//startFit + (i+1 - noOfPredefinedGauss)*(endFit-startFit)/noOfRandomGauss;

    fitFunc->SetParameter(4+3*i, 0.5*FracFromHisto*backgroundGaussesOffIntensMaxFrac.at(i-noOfPredefinedGauss));
    fitFunc->SetParName(4+3*i, "Intensity");
    fitFunc->SetParLimits(4+3*i, FracFromHisto, FracFromHisto*backgroundGaussesOffIntensMaxFrac.at(i-noOfPredefinedGauss));

    fitFunc->SetParameter(5+3*i, 0.5);
    fitFunc->SetParName(5+3*i, "Sigma");
    fitFunc->SetParLimits(5+3*i, 0.2, 5);

    fitFunc->SetParameter(6+3*i, offset);
    fitFunc->SetParName(6+3*i, "Offset");
   // fitFunc->SetParLimits(6+3*i, (1-10*freedomFraction)*offset, (1+12.5*freedomFraction)*offset);
    fitFunc->SetParLimits(6+3*i, offset-backgroundGaussesOffFreddom.at(i-noOfPredefinedGauss), offset+backgroundGaussesOffFreddom.at(i-noOfPredefinedGauss));
  }

  TGraphAsymmErrors *graphToFit = new TGraphAsymmErrors(histoToFit);
  Int_t np = graphToFit->GetN();
  for (Int_t i=0; i<np; i++) {
    graphToFit->SetPointEXhigh(i,0.);
    graphToFit->SetPointEXlow(i,0.);
  }
  graphToFit->Fit(fitFunc,"RM");

  TCanvas *c1 = new TCanvas("c1", "", 710, 500);
  c1->SetHighLightColor(2);
  c1->SetFillColor(0);
  c1->SetFrameBorderMode(0);
  c1->SetBorderSize(2);
  c1->SetFrameLineWidth(2);
  c1->SetLeftMargin(0.1162571);
  c1->SetRightMargin(0.0831758);
  graphToFit->Draw();
  fitFunc->Draw("same");

  TString outputName = "Fitted_" + nameOfInput;
  TFile* outputFile = new TFile(outputName, "RECREATE");

  histoToFit->Write("Raw");
  graphToFit->Write("Fitted");

  std::cout << "Fitted background parameters " << (typeOfBackground == 1 ? "exponential type " : "quadratic type ");
  std::cout << "and parameters y0, Amplitude and Mean [MeV] ";
  std::cout << fitFunc->GetParameter(0) << " " << fitFunc->GetParameter(1) << " " << fitFunc->GetParameter(2) << std::endl;

  const Int_t n = predefGaussOff.size(); // 32
  Double_t x[n], y[n], ex[n], ey[n];
  Double_t yH[n], eyH[n];
  Double_t yEE[n], eyEE[n];
  Double_t yO[n], eyO[n];
  Double_t yCl[n], eyCl[n];

  graphToFit->Draw();
  for (unsigned i=0; i<noOfPredefinedGauss; i++) {
    TString nameTemp = "gaussFunc" + NumberToChar(i,0);
    TF1* gaussToDraw = new TF1(nameTemp, GaussDistrToDraw, startFit, endFit, 3);
    gaussToDraw -> SetNpx(numberOfPointsForDrawing);
    gaussToDraw->SetParameters(fitFunc->GetParameter(4+3*i), fitFunc->GetParameter(6+3*i), fitFunc->GetParameter(5+3*i));
    gaussToDraw->SetLineColor(kGreen);
    gaussToDraw->Draw("same");
    std::cout << fitFunc->GetParameter(6+3*i) << " " << fitFunc->GetParameter(4+3*i) << " " << fitFunc->GetParError(4+3*i) << std::endl;
    x[i] = fitFunc->GetParameter(6+3*i);
    y[i] = fitFunc->GetParameter(4+3*i);
    ex[i] = 0.;
    ey[i] = fitFunc->GetParError(4+3*i);
    yH[i] = fitFunc->GetParameter(4+3*i)/fitFunc->GetParameter(4+3*hydrogenGauss);
    eyH[i] = sqrt(std::pow(fitFunc->GetParError(4+3*i)/fitFunc->GetParameter(4+3*hydrogenGauss),2) + std::pow(fitFunc->GetParError(4+3*hydrogenGauss)*fitFunc->GetParameter(4+3*i)/std::pow(fitFunc->GetParameter(4+3*hydrogenGauss), 2),2));
    yEE[i] = fitFunc->GetParameter(4+3*i)/fitFunc->GetParameter(4+3*eeAnniGauss);
    eyEE[i] = sqrt(std::pow(fitFunc->GetParError(4+3*i)/fitFunc->GetParameter(4+3*eeAnniGauss),2) + std::pow(fitFunc->GetParError(4+3*eeAnniGauss)*fitFunc->GetParameter(4+3*i)/std::pow(fitFunc->GetParameter(4+3*eeAnniGauss), 2),2));
    yO[i] = fitFunc->GetParameter(4+3*i)/fitFunc->GetParameter(4+3*oxygenGauss);
    eyO[i] = sqrt(std::pow(fitFunc->GetParError(4+3*i)/fitFunc->GetParameter(4+3*oxygenGauss),2) + std::pow(fitFunc->GetParError(4+3*oxygenGauss)*fitFunc->GetParameter(4+3*i)/std::pow(fitFunc->GetParameter(4+3*oxygenGauss), 2),2));
    yCl[i] = fitFunc->GetParameter(4+3*i)/fitFunc->GetParameter(4+3*chlorineGauss);
    eyCl[i] = sqrt(std::pow(fitFunc->GetParError(4+3*i)/fitFunc->GetParameter(4+3*chlorineGauss),2) + std::pow(fitFunc->GetParError(4+3*chlorineGauss)*fitFunc->GetParameter(4+3*i)/std::pow(fitFunc->GetParameter(4+3*chlorineGauss), 2),2));
  }
  c1->Write("FittedByComponents");

  TGraphAsymmErrors *graphWithResults = new TGraphAsymmErrors(n,x,y,ex,ex,ey,ey);
  graphWithResults->Write("GaussIntens");
  TGraphAsymmErrors *graphWithResultsByH = new TGraphAsymmErrors(n,x,yH,ex,ex,eyH,eyH);
  graphWithResultsByH->Write("GaussIntensByH");
  TGraphAsymmErrors *graphWithResultsByEE = new TGraphAsymmErrors(n,x,yEE,ex,ex,eyEE,eyEE);
  graphWithResultsByEE->Write("GaussIntensByEE");
  TGraphAsymmErrors *graphWithResultsByO = new TGraphAsymmErrors(n,x,yO,ex,ex,eyO,eyO);
  graphWithResultsByO->Write("GaussIntensByO");
  TGraphAsymmErrors *graphWithResultsByCl = new TGraphAsymmErrors(n,x,yCl,ex,ex,eyCl,eyCl);
  graphWithResultsByCl->Write("GaussIntensByCl");

  graphToFit->Draw();
  TF1* bgFunc = new TF1("bgFunc", bgFunction, startFit, endFit, 3 + 1 + 3*noOfPredefinedGauss);
  bgFunc -> SetNpx(numberOfPointsForDrawing);
  bgFunc->SetParameter(0, fitFunc->GetParameter(0));
  bgFunc->SetParameter(1, fitFunc->GetParameter(1));
  bgFunc->SetParameter(2, fitFunc->GetParameter(2));
  bgFunc->SetParameter(3, noOfRandomGauss);
  for (unsigned i=noOfPredefinedGauss; i<noOfRandomGauss + noOfPredefinedGauss; i++) {
    Double_t offset = backgroundGaussesOff.at(i-noOfPredefinedGauss);
    bgFunc->SetParameter(4+3*(i-noOfPredefinedGauss), fitFunc->GetParameter(4+3*i));
    bgFunc->SetParameter(5+3*(i-noOfPredefinedGauss), fitFunc->GetParameter(5+3*i));
    bgFunc->SetParameter(6+3*(i-noOfPredefinedGauss), fitFunc->GetParameter(6+3*i));
    std::cout << "BG " << fitFunc->GetParameter(6+3*i) << " " << fitFunc->GetParameter(4+3*i) << " " << fitFunc->GetParError(4+3*i) << std::endl;
  }
  bgFunc->SetLineColor(kBlue);
  bgFunc->Draw("same");
  c1->Write("FittedWithBackground");

  outputFile->Close();
}

void FitExpSpectrum(TH1D* histoToFit, std::string nameOfInput)
{
  int controlNumber = histoToFit->GetBinContent(10), controlNumber2 = histoToFit->GetBinContent(15);

  std::cout << "Fitting background separately " << (typeOfBackground == 1 ? "exponential type " : "quadratic type ") << std::endl;
  std::vector<double> backgroundGaussesOff = {0.8, 1.31, 1.45, 1.5, 2.35, 3.3, 3.7, 3.9, 5, 5.5, 6.1, 6.5, 7.0, 7.9};
 // std::vector<double> backgroundGaussesOff = {0.4, 0.44, 1.15, 1.4, 1.57, 2.65, 4.5, 4.9, 5.1, 5.2, 5.3, 5.8, 6.2, 6.6, 8.0, 8.4};
  std::vector<double> backgroundGaussesOffFreddom = {1.06, 1.09, 1.15, 1.2, 1.4, 1.85, 1.6, 1.3};
  double FracFromHisto = 1;
  std::vector<double> backgroundGaussesOffIntensMaxFrac = {3, 225, 120, 250, 15, 15, 20, 20}; //{3, 225, 120, 250, 15, 15, 20, 20};(1B counts)
  std::vector<double> predefGaussOff = {0.44, 0.472, 0.511, 0.58, 0.692, 0.785, 0.88, 0.95, 1.01, 1.13, 1.19, 1.25, 1.32, 1.46, 1.625, 1.73, 1.825, 1.965, 2.12, 2.23, 2.31, 2.54, 2.64, 2.74, 2.86, 3.06, 3.2, 3.4, 3.5, 3.72, 3.9, 4.00, 4.2, 4.47, 4.92, 5.11, 5.35, 5.66, 5.81, 6.17, 6.44, 6.67, 6.92, 7.15, 7.79, 8.58, 10.8};
  Double_t startFit = 0.428, endFit = 11;

  /*predefGaussOff.clear();
  std::vector<double> predefGaussInt;
  for (unsigned i=1; i<initialPeaks->GetN(); i++) {
    if (initialPeaks->GetPointX(i) > startFit && initialPeaks->GetPointX(i) < endFit) {
      predefGaussOff.push_back(initialPeaks->GetPointX(i));
      predefGaussInt.push_back(initialPeaks->GetPointY(i));
    }
  }*/

  backgroundGaussesOff.clear();
  std::vector<double> backgroundGaussesSig;
  std::vector<double> backgroundGaussesInt;
  for (unsigned i=0; i<(initialBG->GetNumberFreeParameters() - 3)/3; i++) {
    backgroundGaussesOff.push_back(initialBG->GetParameter(6+3*i));
    backgroundGaussesSig.push_back(initialBG->GetParameter(5+3*i));
    backgroundGaussesInt.push_back(initialBG->GetParameter(4+3*i));
//    std::cout << initialBG->GetParameter(6+3*i) << std::endl;
  }

  int hydrogenGauss = 21, eeAnniGauss = 2, oxygenGauss = 32, chlorineGauss = 19; // When adding/removing components from above adjust this parameters
  int noOfPredefinedGauss = predefGaussOff.size(), noOfRandomGauss = backgroundGaussesOff.size();
  unsigned noOfParameters = 3 + 1 + 3*(noOfRandomGauss + noOfPredefinedGauss);
  int numberOfPointsForDrawing = (endFit - startFit)*500;

  /*
   P *arameters
   0 -2 -> BG as exp/poly2 + const
   3 -> noOfRandomGausses
   the rest are intensity + sigma + offset for each gaussian
   */
  TF1* fitFunc = new TF1("fitFunc", bgFunction, startFit, endFit, noOfParameters);
  fitFunc -> SetNpx(numberOfPointsForDrawing);

  double min = 0.000003*histoToFit->GetMaximum();

    fitFunc->SetParameter(0, initialBG->GetParameter(0));
    fitFunc->SetParName(0, "Background constant");
    fitFunc->SetParLimits(0, 0, 5*min);
    fitFunc->SetParameter(1, initialBG->GetParameter(1));
    fitFunc->SetParName(1, "Background amplitude");
    fitFunc->SetParLimits(1, 0.05*histoToFit->GetBinContent((histoToFit->FindBin(startFit))), 20*histoToFit->GetBinContent((histoToFit->FindBin(startFit))));
    fitFunc->SetParameter(2, initialBG->GetParameter(2));
    fitFunc->SetParLimits(2, 0.1*histoToFit->GetMean(), 5*histoToFit->GetMean());
    fitFunc->SetParName(2, "Background mean");

    FracFromHisto = 0.001*histoToFit->GetBinContent((histoToFit->FindBin(startFit)));

  fitFunc->FixParameter(3, noOfRandomGauss + noOfPredefinedGauss);
  fitFunc->SetParName(3, "Number of Gaussians");

  double veryLargeIntensity = histoToFit->GetMaximum()*10;
  double freedomFraction = 0.05;
  for (unsigned i=0; i<noOfPredefinedGauss; i++) {
    Double_t offset = predefGaussOff.at(i);
    Double_t intens = 0.001*veryLargeIntensity/std::pow(offset, 2); //predefGaussInt.at(i);
    Double_t sigma = 0.02*offset;

    fitFunc->SetParameter(4+3*i, 0.2*intens);
    fitFunc->SetParName(4+3*i, "Intensity");
    fitFunc->SetParLimits(4+3*i, 0, veryLargeIntensity);

    fitFunc->SetParameter(5+3*i, sigma);
    fitFunc->SetParName(5+3*i, "Sigma");
    fitFunc->SetParLimits(5+3*i, 0.5*sigma, 1.15*sigma);

    fitFunc->SetParameter(6+3*i, offset);
    fitFunc->SetParName(6+3*i, "Offset");
    fitFunc->SetParLimits(6+3*i, (1-0.5*freedomFraction)*offset, (1+0.5*freedomFraction)*offset);
  }
  for (unsigned i=noOfPredefinedGauss; i<noOfRandomGauss + noOfPredefinedGauss; i++) {
    Double_t offset = backgroundGaussesOff.at(i-noOfPredefinedGauss);//startFit + (i+1 - noOfPredefinedGauss)*(endFit-startFit)/noOfRandomGauss;
    Double_t intens = backgroundGaussesInt.at(i-noOfPredefinedGauss);
    Double_t sigma = backgroundGaussesSig.at(i-noOfPredefinedGauss);

    fitFunc->SetParameter(4+3*i, intens);
    fitFunc->SetParName(4+3*i, "Intensity");
    fitFunc->SetParLimits(4+3*i, 0.0001*FracFromHisto, 100000*veryLargeIntensity);

    fitFunc->SetParameter(5+3*i, sigma);
    fitFunc->SetParName(5+3*i, "Sigma");
    fitFunc->SetParLimits(5+3*i, 0.1*sigma, 5*sigma);

    fitFunc->SetParameter(6+3*i, offset);
    fitFunc->SetParName(6+3*i, "Offset");
   // fitFunc->SetParLimits(6+3*i, (1-10*freedomFraction)*offset, (1+12.5*freedomFraction)*offset);
    fitFunc->SetParLimits(6+3*i, 0.4, 8.5);
  }

  TGraphAsymmErrors *graphToFit = new TGraphAsymmErrors(histoToFit);
  Int_t np = graphToFit->GetN();
  for (Int_t i=0; i<np; i++) {
    graphToFit->SetPointEXhigh(i,0.);
    graphToFit->SetPointEXlow(i,0.);
  }
  graphToFit->Fit(fitFunc,"RM");

  TCanvas *c1 = new TCanvas("c1", "", 710, 500);
  c1->SetHighLightColor(2);
  c1->SetFillColor(0);
  c1->SetFrameBorderMode(0);
  c1->SetBorderSize(2);
  c1->SetFrameLineWidth(2);
  c1->SetLeftMargin(0.1162571);
  c1->SetRightMargin(0.0831758);
  graphToFit->Draw();
  fitFunc->Draw("same");

  TString outputName = "Fitted_" + nameOfInput.substr(0,3) + "_" + NumberToChar(controlNumber, 0) + "_" + NumberToChar(controlNumber2, 0) + ".root";
  TFile* outputFile = new TFile(outputName, "RECREATE");

  histoToFit->Write("Raw");
  graphToFit->Write("Fitted");

  std::cout << "Fitted background parameters " << (typeOfBackground == 1 ? "exponential type " : "quadratic type ");
  std::cout << "and parameters y0, Amplitude and Mean [MeV] ";
  std::cout << fitFunc->GetParameter(0) << " " << fitFunc->GetParameter(1) << " " << fitFunc->GetParameter(2) << std::endl;

  const Int_t n = predefGaussOff.size(); // 32
  Double_t x[n], y[n], ex[n], ey[n];
  Double_t yH[n], eyH[n];
  Double_t yEE[n], eyEE[n];
  Double_t yO[n], eyO[n];
  Double_t yCl[n], eyCl[n];

  graphToFit->Draw();
  for (unsigned i=0; i<noOfPredefinedGauss; i++) {
    TString nameTemp = "gaussFunc" + NumberToChar(i,0);
    TF1* gaussToDraw = new TF1(nameTemp, GaussDistrToDraw, startFit, endFit, 3);
    gaussToDraw -> SetNpx(numberOfPointsForDrawing);
    gaussToDraw->SetParameters(fitFunc->GetParameter(4+3*i), fitFunc->GetParameter(6+3*i), fitFunc->GetParameter(5+3*i));
    gaussToDraw->SetLineColor(kGreen);
    gaussToDraw->Draw("same");
    std::cout << fitFunc->GetParameter(6+3*i) << " " << fitFunc->GetParameter(4+3*i) << " " << fitFunc->GetParError(4+3*i) << std::endl;
    x[i] = fitFunc->GetParameter(6+3*i);
    y[i] = fitFunc->GetParameter(4+3*i);
    ex[i] = 0.;
    ey[i] = fitFunc->GetParError(4+3*i);
    yH[i] = fitFunc->GetParameter(4+3*i)/fitFunc->GetParameter(4+3*hydrogenGauss);
    eyH[i] = sqrt(std::pow(fitFunc->GetParError(4+3*i)/fitFunc->GetParameter(4+3*hydrogenGauss),2) + std::pow(fitFunc->GetParError(4+3*hydrogenGauss)*fitFunc->GetParameter(4+3*i)/std::pow(fitFunc->GetParameter(4+3*hydrogenGauss), 2),2));
    yEE[i] = fitFunc->GetParameter(4+3*i)/fitFunc->GetParameter(4+3*eeAnniGauss);
    eyEE[i] = sqrt(std::pow(fitFunc->GetParError(4+3*i)/fitFunc->GetParameter(4+3*eeAnniGauss),2) + std::pow(fitFunc->GetParError(4+3*eeAnniGauss)*fitFunc->GetParameter(4+3*i)/std::pow(fitFunc->GetParameter(4+3*eeAnniGauss), 2),2));
    yO[i] = fitFunc->GetParameter(4+3*i)/fitFunc->GetParameter(4+3*oxygenGauss);
    eyO[i] = sqrt(std::pow(fitFunc->GetParError(4+3*i)/fitFunc->GetParameter(4+3*oxygenGauss),2) + std::pow(fitFunc->GetParError(4+3*oxygenGauss)*fitFunc->GetParameter(4+3*i)/std::pow(fitFunc->GetParameter(4+3*oxygenGauss), 2),2));
    yCl[i] = fitFunc->GetParameter(4+3*i)/fitFunc->GetParameter(4+3*chlorineGauss);
    eyCl[i] = sqrt(std::pow(fitFunc->GetParError(4+3*i)/fitFunc->GetParameter(4+3*chlorineGauss),2) + std::pow(fitFunc->GetParError(4+3*chlorineGauss)*fitFunc->GetParameter(4+3*i)/std::pow(fitFunc->GetParameter(4+3*chlorineGauss), 2),2));
  }
  c1->Write("FittedByComponents");

  TGraphAsymmErrors *graphWithResults = new TGraphAsymmErrors(n,x,y,ex,ex,ey,ey);
  graphWithResults->Write("GaussIntens");
  TGraphAsymmErrors *graphWithResultsByH = new TGraphAsymmErrors(n,x,yH,ex,ex,eyH,eyH);
  graphWithResultsByH->Write("GaussIntensByH");
  TGraphAsymmErrors *graphWithResultsByEE = new TGraphAsymmErrors(n,x,yEE,ex,ex,eyEE,eyEE);
  graphWithResultsByEE->Write("GaussIntensByEE");
  TGraphAsymmErrors *graphWithResultsByO = new TGraphAsymmErrors(n,x,yO,ex,ex,eyO,eyO);
  graphWithResultsByO->Write("GaussIntensByO");
  TGraphAsymmErrors *graphWithResultsByCl = new TGraphAsymmErrors(n,x,yCl,ex,ex,eyCl,eyCl);
  graphWithResultsByCl->Write("GaussIntensByCl");

  graphToFit->Draw();
  TF1* bgFunc = new TF1("bgFunc", bgFunction, startFit, endFit, 3 + 1 + 3*noOfPredefinedGauss);
  bgFunc -> SetNpx(numberOfPointsForDrawing);
  bgFunc->SetParameter(0, fitFunc->GetParameter(0));
  bgFunc->SetParameter(1, fitFunc->GetParameter(1));
  bgFunc->SetParameter(2, fitFunc->GetParameter(2));
  bgFunc->SetParameter(3, noOfRandomGauss);
  for (unsigned i=noOfPredefinedGauss; i<noOfRandomGauss + noOfPredefinedGauss; i++) {
    Double_t offset = backgroundGaussesOff.at(i-noOfPredefinedGauss);
    bgFunc->SetParameter(4+3*(i-noOfPredefinedGauss), fitFunc->GetParameter(4+3*i));
    bgFunc->SetParameter(5+3*(i-noOfPredefinedGauss), fitFunc->GetParameter(5+3*i));
    bgFunc->SetParameter(6+3*(i-noOfPredefinedGauss), fitFunc->GetParameter(6+3*i));
    std::cout << "BG " << fitFunc->GetParameter(6+3*i) << " " << fitFunc->GetParameter(4+3*i) << " " << fitFunc->GetParError(4+3*i) << std::endl;
  }
  bgFunc->SetLineColor(kBlue);
  bgFunc->Draw("same");
  c1->Write("FittedWithBackground");

  outputFile->Close();
}

std::string FitLine(TH1D* histoToFit, double energy)
{
  double a = 2.0*pow(10, -4); // in MeV
  double b = 2.22*pow(10, -2);
  double c = 0.5;
  double sigma = (a + b*sqrt(energy + c*pow(energy, 2)))/(2.35482004503);
  double sigmaFracExtend = 5;
  Double_t startFit = energy - sigmaFracExtend*sigma, endFit = energy + sigmaFracExtend*sigma;
  int numberOfPointsForDrawing = (endFit - startFit)*500;
  TF1* fitFunc = new TF1("fitFunc", eneLineFunction, startFit, endFit, 6);
  fitFunc -> SetNpx(numberOfPointsForDrawing);
  double min = histoToFit->GetMinimum();
  if (typeOfBackground == 1) {
    fitFunc->SetParameter(0, 10);
    fitFunc->SetParName(0, "Background constant");
    fitFunc->SetParLimits(0, 0, min);
    fitFunc->SetParameter(1, histoToFit->GetBinContent((histoToFit->FindBin(startFit))));
    fitFunc->SetParName(1, "Background amplitude");
    fitFunc->SetParLimits(1, 0, 10*histoToFit->GetBinContent((histoToFit->FindBin(startFit))));
    fitFunc->SetParameter(2, histoToFit->GetMean());
    fitFunc->SetParLimits(2, 0, 5*histoToFit->GetMean());
    fitFunc->SetParName(2, "Background mean");
  } else {
    double amplitudeLimit = (histoToFit->GetMaximum() - min)/4E8;
    fitFunc->SetParameter(0, 0.5*amplitudeLimit);
    fitFunc->SetParName(0, "Background amplitude");
    fitFunc->SetParLimits(0, 0, amplitudeLimit);
    fitFunc->SetParameter(1, 0.5*(endFit - startFit));
    fitFunc->SetParName(1, "Background minimum horizontal shift");
    fitFunc->SetParLimits(1, 0, endFit);
    double minLimit = histoToFit->GetBinContent((histoToFit->FindBin(endFit)));
    fitFunc->SetParameter(2, minLimit);
    fitFunc->SetParName(2, "Background minimum vertical shift");
    fitFunc->SetParLimits(2, 0, 1.2*minLimit);
  }
  double veryLargeIntensity = histoToFit->GetMaximum()*10;
  double freedomFraction = 0.05;

  Double_t offset = energy;
  Double_t intens = 0.001*veryLargeIntensity/std::pow(offset, 2);

  fitFunc->SetParameter(3, intens);
  fitFunc->SetParName(3, "Intensity");
  fitFunc->SetParLimits(3, 0, veryLargeIntensity);

  fitFunc->SetParameter(4, sigma);
  fitFunc->SetParName(4, "Sigma");
  fitFunc->SetParLimits(4, 0.5*sigma, 5*sigma);

  fitFunc->SetParameter(5, offset);
  fitFunc->SetParName(5, "Offset");
  fitFunc->SetParLimits(5, (1-freedomFraction)*offset, (1+freedomFraction)*offset);

  TGraphAsymmErrors *graphToFit = new TGraphAsymmErrors(histoToFit);
  Int_t np = graphToFit->GetN();
  for (Int_t i=0; i<np; i++) {
    graphToFit->SetPointEXhigh(i,0.);
    graphToFit->SetPointEXlow(i,0.);
  }
  graphToFit->Fit(fitFunc,"RM");

  TCanvas *c1 = new TCanvas("c1", "", 710, 500);
  c1->SetHighLightColor(2);
  c1->SetFillColor(0);
  c1->SetFrameBorderMode(0);
  c1->SetBorderSize(2);
  c1->SetFrameLineWidth(2);
  c1->SetLeftMargin(0.1162571);
  c1->SetRightMargin(0.0831758);
  graphToFit->Draw();
  fitFunc->Draw("same");

  std::cout << "Fitted background parameters " << (typeOfBackground == 1 ? "exponential type " : "quadratic type ");
  std::cout << "and parameters y0, Amplitude and Mean [MeV] " << std::endl;
  std::cout << fitFunc->GetParameter(0) << " " << fitFunc->GetParameter(1) << " " << fitFunc->GetParameter(2) << std::endl;
  std::cout << "Gauss parameters [Intensity] [Sigma] [Offset]" << std::endl;
  std::cout << fitFunc->GetParameter(4) << " " << fitFunc->GetParameter(5) << " " << fitFunc->GetParameter(6) << std::endl;

  Int_t minBin = histoToFit->FindBin(startFit);
  Int_t maxBin = histoToFit->FindBin(endFit);

  Double_t binSize = histoToFit->GetBinCenter(minBin+1) - histoToFit->GetBinCenter(minBin);
  std::cout << "Integral: " << binSize*histoToFit->Integral(minBin, maxBin) << std::endl;
  Double_t startCenter = histoToFit->GetBinCenter(minBin), endCenter = histoToFit->GetBinCenter(maxBin);
  Double_t startCount = histoToFit->GetBinContent(minBin), endCount = histoToFit->GetBinContent(maxBin);
  double aParam = (endCount - startCount)/(endCenter - startCenter);
  double bParam = startCount - startCenter*aParam;
  double BGsum = 0.;
  for (unsigned i=minBin; i<=maxBin; i++) {
    BGsum += binSize*(bParam+aParam*histoToFit->GetBinCenter(i));
  }
  std::cout << std::endl;
  std::cout << "Lin Background: " << BGsum << std::endl;
  std::cout << "Diff: " << binSize*histoToFit->Integral(minBin, maxBin) - BGsum << std::endl;

  std::string test;
  std::cout << "Next line? [y/n]" << std::endl;
  std::cin >> test;

  return test;
}

Double_t eneLineFunction(Double_t *A, Double_t *P)
{
  Double_t result = 0;
  if (typeOfBackground == 1)
    result += P[0] + P[1]*exp(-A[0]/P[2]);
  else
    result += P[0]*std::pow(A[0] - P[1], 2) + P[2];

  result += P[3]*GaussDistr(A[0], P[5], P[4]);

  return result;
}

Double_t bgFunction(Double_t *A, Double_t *P)
{
  Double_t result = 0;
  if (typeOfBackground == 1)
    result += P[0] + P[1]*exp(-A[0]/P[2]);
  else
    result += P[0]*std::pow(A[0] - P[1], 2) + P[2];

  Int_t noOfRandomGauss = P[3];
  for (unsigned i=0; i<noOfRandomGauss; i++) {
    result += P[4+3*i]*GaussDistr(A[0], P[6+3*i], P[5+3*i]);
  }

  return result;
}

double GaussDistr(double x, double mean, double sigma)
{
  return 1/sqrt(2*M_PI)/sigma*exp(-0.5*std::pow((x-mean)/sigma, 2));
}

double GaussDistrToDraw(Double_t *A, Double_t *P)
{
  return P[0]/sqrt(2*M_PI)/P[2]*exp(-0.5*std::pow((A[0]-P[1])/P[2], 2));
}

double GramPolynomial(int i, int m, int k, int s)
{
  if (k>0) {
    return (4.*k - 2.)/(k*(2.*m - k + 1.))*(i*GramPolynomial(i, m, k-1, s) + s*GramPolynomial(i, m, k-1, s-1)) - ((k-1.)*(2.*m + k))/(k*(2.*m - k + 1.))*GramPolynomial(i, m, k-2, s);
  } else {
    if (k==0 && s==0)
      return 1.;
    else
      return 0.;
  }
}

double GenFactor(int a, int b)
{
  double gf = 1.;
  for (int j=(a-b)+1; j<=a; j++) {
    gf*=j;
  }
  return gf;
}

double CalcWeight(int i, int t, int m, int n, int s)
{
  double w = 0;
  for (int k=0; k<=n; ++k) {
    w = w + (2*k + 1)*(GenFactor(2*m, k)/GenFactor(2*m + k + 1, k + 1))*GramPolynomial(i, m, k, 0)*GramPolynomial(t, m, k, s);
  }
  return w;
}

// m -> size of half window - how many points from the central are taken
// t -> number of data point (give 0)
// n -> order of interpolation polynomial -> 1 - line, 2 - quadratic ...
// s -> order of function, 0 - smooth, 1 - derivative, 2 - second derivative, ...
std::vector<double> ComputeWeights(int m, int t, int n, int s)
{
  std::vector<double> weights(2*m + 1);
  for (int i=0; i<2*m+1; ++i) {
    weights.at(i) = CalcWeight(i-m, t, m, n, s);
  }

  std::cout << "Calculated weights ";
  for (unsigned i=0; i<weights.size(); i++)
    std::cout << weights.at(i) << " ";
  std::cout << std::endl;

  return weights;
}

bool FileCheck(const std::string& NameOfFile)
{
    struct stat buffer;
    return (stat(NameOfFile.c_str(), &buffer) == 0);
}

std::string NumberToChar(double number, int precision)
{
    std::ostringstream conv;
    conv << std::fixed << std::setprecision(precision);
    conv << number;
    return conv.str();
}
