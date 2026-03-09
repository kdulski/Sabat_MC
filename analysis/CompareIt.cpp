#include <TGraph2DAsymmErrors.h>
#include <TGraphAsymmErrors.h>
#include <TGraph2DErrors.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TGraph.h>
#include <TFile.h>
#include <TH3D.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TF12.h>
#include <TF1.h>
#include <TF2.h>

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
#include <cmath>

std::string NumberToChar(double number, int precision);

int main(int argc, char* argv[])
{
  std::vector<std::string> NameOfTheFiles;
  std::vector<std::string> NameOfTheBackgroundFiles;
  std::vector<double> CorrelatingArguments;
  int numberOfFiles;

  if (argc < 2) {
    std::cout << "No arguments given. Turning on interactive input mode." << std::endl;
    std::cout << "How many files do you want to provide (number of degrade thicknesses)? :" << std::endl;
    std::cin >> numberOfFiles;

    std::string tempStr, tempStrBG;
    double tempArg;

    for (unsigned i=0; i<numberOfFiles; i++) {
      std::cout << "Give me the " << i+1 << " File with the path : " << std::endl;
      std::cin >> tempStr;
      std::cout << "Give me the " << i+1 << " argument : " << std::endl;
      std::cin >> tempArg;
      std::cout << "Give me the " << i+1 << " Background File with the path : " << std::endl;
      std::cin >> tempStrBG;

      NameOfTheFiles.push_back(tempStr);
      CorrelatingArguments.push_back(tempArg);
      NameOfTheBackgroundFiles.push_back(tempStrBG);
    }
  } else {
    std::cout << "Assuming that you first gave the names and then the arguments as numbers and then the names for background" << std::endl;
    numberOfFiles = std::floor((argc-1)/3);
    for (unsigned i=1; i<=numberOfFiles; i++) {
      std::cout << "File with name " << argv[i] << " and argument equal to " << argv[i+numberOfFiles] << " and background file with name " << argv[i+2*numberOfFiles] << std::endl;
      NameOfTheFiles.push_back(argv[i]);
      CorrelatingArguments.push_back(std::atof(argv[i+numberOfFiles]));
      NameOfTheBackgroundFiles.push_back(argv[i+2*numberOfFiles]);
    }
  }

  std::vector<std::string> NameOfTheHistogramsToGet;
  NameOfTheHistogramsToGet.push_back("Raw");
  std::vector<TH2D*> Histograms2DForDifferentArg;
  std::vector<TH2D*> Histograms2DForDifferentArgBG;

  std::vector<std::string> NameOfTheGraphsToGet;
  NameOfTheGraphsToGet.push_back("GaussIntensByH");
  NameOfTheGraphsToGet.push_back("GaussIntensByEE");
  NameOfTheGraphsToGet.push_back("GaussIntensByO");
  NameOfTheGraphsToGet.push_back("GaussIntensByCl");
  std::vector<TGraph2DAsymmErrors*> Graphs2DForDifferentArg;
  std::vector<TGraph2DAsymmErrors*> Graphs2DForDifferentArgBG;

  double binMin, binMax, binSize;
  double argMin = CorrelatingArguments.at(0), argMax = CorrelatingArguments.at(CorrelatingArguments.size() - 1);
  int numberOfArgBins = (argMax-argMin)/10;
  double ArgBinSize = (argMax - argMin)/(double)numberOfArgBins;
  TH2D *tempHisto2D;
  int numberOfPointsInGraph;
  TGraph2DAsymmErrors *tempGraph2D;
  TGraph2DAsymmErrors *g1, *g2, *g3, *g4;

  int namesSizes = NameOfTheHistogramsToGet.size();

  std::vector<double> SignalsEnergies = {0.44, 0.472, 0.511, 0.625, 0.692, 0.785, 0.845, 0.92, 1.16, 1.63,
                                        1.73, 1.83, 1.965, 2.12, 2.23, 2.31, 2.44, 2.74, 3.06, 3.84,
                                        4.44, 5.05, 5.74, 6.12, 6.65, 6.92, 7.11, 7.4, 7.65, 7.82,
                                        8.24, 8.58, 10.8};
  std::vector<double> SignalsEnergiesAccRange = {0.015, 0.015, 0.01, 0.025, 0.03, 0.025, 0.03, 0.03, 0.04, 0.045,
                                                0.045, 0.045, 0.045, 0.05, 0.04, 0.045, 0.05, 0.05, 0.1, 0.1,
                                                0.1, 0.2, 0.12, 0.12, 0.1, 0.08, 0.1, 0.1, 0.1, 0.1,
                                                0.15, 0.15, 0.8};


  for (unsigned i=0; i<NameOfTheFiles.size(); i++) {
    std::cout << "Reading file : " << NameOfTheFiles.at(i) << std::endl;
    TFile* fileIn = new TFile(NameOfTheFiles.at(i).c_str(), "READ");

    if (!fileIn) {
      std::cout << "No such file" << std::endl;
      break;
    }

    fileIn->cd();

    for (unsigned j=0; j<NameOfTheHistogramsToGet.size(); j++) {
      std::cout << "Taking histo : " << NameOfTheHistogramsToGet.at(j) << std::endl;
      TH1D *tempHisto = dynamic_cast<TH1D*>(fileIn->Get((NameOfTheHistogramsToGet.at(j)).c_str()));

      if (!tempHisto) {
        std::cout << "No such histogram" << std::endl;
      } else {
        if (i == 0) {
          binMin = tempHisto->GetBinCenter(1);
          binMax = tempHisto->GetBinCenter(tempHisto->GetNbinsX() - 1);
          binSize = tempHisto->GetBinCenter(2) - binMin;
          tempHisto2D = new TH2D(("Histo2D_ForCorrelation_" + NameOfTheHistogramsToGet.at(j)).c_str(),
                                 ("Argument for different histo -> " + NameOfTheHistogramsToGet.at(j)).c_str(),
                                 numberOfArgBins + 3, argMin - 1.5*ArgBinSize, argMax + 1.5*ArgBinSize,
                                 tempHisto->GetNbinsX(), binMin - 0.5*binSize, binMax + 0.5*binSize);
          tempHisto2D->SetDirectory(0);
          Histograms2DForDifferentArg.push_back(tempHisto2D);
        }

        for (unsigned k=1; k<tempHisto->GetNbinsX(); k++) {
          Int_t xBin = Histograms2DForDifferentArg.at(j)->GetXaxis()->FindBin(CorrelatingArguments.at(i));
          Histograms2DForDifferentArg.at(j)->AddBinContent(xBin, k, tempHisto->GetBinContent(k));
        }
      }
    }

    for (unsigned j=0; j<NameOfTheGraphsToGet.size(); j++) {
      std::cout << "Taking graph : " << NameOfTheGraphsToGet.at(j) << std::endl;
      TGraphAsymmErrors *tempGraph = dynamic_cast<TGraphAsymmErrors*>(fileIn->Get((NameOfTheGraphsToGet.at(j)).c_str()));

      if (!tempGraph) {
        std::cout << "No such graph" << std::endl;
      } else {
        numberOfPointsInGraph = tempGraph->GetN();
        if (i == 0) {
          if (j == 0) {
            g1 = new TGraph2DAsymmErrors((Int_t)(numberOfPointsInGraph * NameOfTheGraphsToGet.size()));
            Graphs2DForDifferentArg.push_back(g1);
            g1->SetDirectory(0);
          } else if (j == 1) {
            g2 = new TGraph2DAsymmErrors((Int_t)(numberOfPointsInGraph * NameOfTheGraphsToGet.size()));
            Graphs2DForDifferentArg.push_back(g2);
            g2->SetDirectory(0);
          } else if (j == 2) {
            g3 = new TGraph2DAsymmErrors((Int_t)(numberOfPointsInGraph * NameOfTheGraphsToGet.size()));
            Graphs2DForDifferentArg.push_back(g3);
            g3->SetDirectory(0);
          } else {
            g4 = new TGraph2DAsymmErrors((Int_t)(numberOfPointsInGraph * NameOfTheGraphsToGet.size()));
            Graphs2DForDifferentArg.push_back(g4);
            g4->SetDirectory(0);
          }
          tempHisto2D = new TH2D(("Histo2D_ForCorrelationFromGraph_" + NameOfTheGraphsToGet.at(j)).c_str(),
                                 ("Argument for different histo -> " + NameOfTheGraphsToGet.at(j)).c_str(),
                                 numberOfArgBins + 3, argMin - 1.5*ArgBinSize, argMax + 1.5*ArgBinSize,
                                 numberOfPointsInGraph, 0.5, numberOfPointsInGraph+0.5);
          tempHisto2D->SetDirectory(0);
          Histograms2DForDifferentArg.push_back(tempHisto2D);
        }

        for (unsigned k=0; k<numberOfPointsInGraph; k++) {
          Graphs2DForDifferentArg.at(j)->AddPointError(tempGraph->GetPointX(k), CorrelatingArguments.at(i), tempGraph->GetPointY(k),
                                                       tempGraph->GetErrorXlow(k), tempGraph->GetErrorXhigh(k), 0., 0.,
                                                       tempGraph->GetErrorYlow(k), tempGraph->GetErrorYhigh(k));
          Int_t xBin = Histograms2DForDifferentArg.at(j+namesSizes)->GetXaxis()->FindBin(CorrelatingArguments.at(i));
          int yBin = SignalsEnergies.size()+1;
          for (unsigned l=0; l<SignalsEnergies.size(); l++) {
            if (fabs(tempGraph->GetPointX(k) - SignalsEnergies.at(l)) <= SignalsEnergiesAccRange.at(l))
              yBin = l+1;
          }
          double weight = 0, val = 0, err = 0;
          if (tempGraph->GetErrorYlow(k) > 0) {
            weight += 1/std::pow(tempGraph->GetErrorYlow(k), 2);
            val += tempGraph->GetPointY(k)/std::pow(tempGraph->GetErrorYlow(k), 2);
          }
          if (Histograms2DForDifferentArg.at(j+namesSizes)->GetBinError(xBin, yBin) > 0) {
            weight += 1/std::pow(Histograms2DForDifferentArg.at(j+namesSizes)->GetBinError(xBin, yBin), 2);
            val += Histograms2DForDifferentArg.at(j+namesSizes)->GetBinContent(xBin, yBin)/std::pow(Histograms2DForDifferentArg.at(j+namesSizes)->GetBinError(xBin, yBin), 2);
          }
          if (weight > 0) {
            val = val/weight;
            err = 1/std::sqrt(weight);
          }
          Histograms2DForDifferentArg.at(j+namesSizes)->AddBinContent(xBin, yBin, val);
          Histograms2DForDifferentArg.at(j+namesSizes)->SetBinError(xBin, yBin, err);
        }
      }
    }

    fileIn->Close();
  }

  for (unsigned i=0; i<NameOfTheBackgroundFiles.size(); i++) {
    std::cout << "Reading file : " << NameOfTheBackgroundFiles.at(i) << std::endl;
    TFile* fileIn = new TFile(NameOfTheBackgroundFiles.at(i).c_str(), "READ");

    if (!fileIn) {
      std::cout << "No such file" << std::endl;
      break;
    }

    fileIn->cd();

    for (unsigned j=0; j<NameOfTheHistogramsToGet.size(); j++) {
      std::cout << "Taking histo : " << NameOfTheHistogramsToGet.at(j) << std::endl;
      TH1D *tempHisto = dynamic_cast<TH1D*>(fileIn->Get((NameOfTheHistogramsToGet.at(j)).c_str()));

      if (!tempHisto) {
        std::cout << "No such histogram" << std::endl;
      } else {
        if (i == 0) {
          binMin = tempHisto->GetBinCenter(1);
          binMax = tempHisto->GetBinCenter(tempHisto->GetNbinsX() - 1);
          binSize = tempHisto->GetBinCenter(2) - binMin;
          tempHisto2D = new TH2D(("Histo2D_ForCorrelation_BG_" + NameOfTheHistogramsToGet.at(j)).c_str(),
                                 ("Argument for different histo -> " + NameOfTheHistogramsToGet.at(j)).c_str(),
                                 numberOfArgBins + 3, argMin - 1.5*ArgBinSize, argMax + 1.5*ArgBinSize,
                                 tempHisto->GetNbinsX(), binMin - 0.5*binSize, binMax + 0.5*binSize);
          tempHisto2D->SetDirectory(0);
          Histograms2DForDifferentArgBG.push_back(tempHisto2D);
        }

        for (unsigned k=1; k<tempHisto->GetNbinsX(); k++) {
          Int_t xBin = Histograms2DForDifferentArgBG.at(j)->GetXaxis()->FindBin(CorrelatingArguments.at(i));
          Histograms2DForDifferentArgBG.at(j)->AddBinContent(xBin, k, tempHisto->GetBinContent(k));
        }
      }
    }

    for (unsigned j=0; j<NameOfTheGraphsToGet.size(); j++) {
      std::cout << "Taking graph : " << NameOfTheGraphsToGet.at(j) << std::endl;
      TGraphAsymmErrors *tempGraph = dynamic_cast<TGraphAsymmErrors*>(fileIn->Get((NameOfTheGraphsToGet.at(j)).c_str()));

      if (!tempGraph) {
        std::cout << "No such graph" << std::endl;
      } else {
        numberOfPointsInGraph = tempGraph->GetN();
        if (i == 0) {
          if (j == 0) {
            g1 = new TGraph2DAsymmErrors((Int_t)(numberOfPointsInGraph * NameOfTheGraphsToGet.size()));
            Graphs2DForDifferentArgBG.push_back(g1);
            g1->SetDirectory(0);
          } else if (j == 1) {
            g2 = new TGraph2DAsymmErrors((Int_t)(numberOfPointsInGraph * NameOfTheGraphsToGet.size()));
            Graphs2DForDifferentArgBG.push_back(g2);
            g2->SetDirectory(0);
          } else if (j == 2) {
            g3 = new TGraph2DAsymmErrors((Int_t)(numberOfPointsInGraph * NameOfTheGraphsToGet.size()));
            Graphs2DForDifferentArgBG.push_back(g3);
            g3->SetDirectory(0);
          } else {
            g4 = new TGraph2DAsymmErrors((Int_t)(numberOfPointsInGraph * NameOfTheGraphsToGet.size()));
            Graphs2DForDifferentArgBG.push_back(g4);
            g4->SetDirectory(0);
          }
          tempHisto2D = new TH2D(("Histo2D_ForCorrelationFromGraph_BG_" + NameOfTheGraphsToGet.at(j)).c_str(),
                                 ("Argument for different histo -> " + NameOfTheGraphsToGet.at(j)).c_str(),
                                 numberOfArgBins + 3, argMin - 1.5*ArgBinSize, argMax + 1.5*ArgBinSize,
                                 numberOfPointsInGraph, 0.5, numberOfPointsInGraph+0.5);
          tempHisto2D->SetDirectory(0);
          Histograms2DForDifferentArgBG.push_back(tempHisto2D);
        }

        for (unsigned k=0; k<numberOfPointsInGraph; k++) {
          Graphs2DForDifferentArgBG.at(j)->AddPointError(tempGraph->GetPointX(k), CorrelatingArguments.at(i), tempGraph->GetPointY(k),
                                                       tempGraph->GetErrorXlow(k), tempGraph->GetErrorXhigh(k), 0., 0.,
                                                       tempGraph->GetErrorYlow(k), tempGraph->GetErrorYhigh(k));
          Int_t xBin = Histograms2DForDifferentArgBG.at(j+namesSizes)->GetXaxis()->FindBin(CorrelatingArguments.at(i));
          int yBin = SignalsEnergies.size()+1;
          for (unsigned l=0; l<SignalsEnergies.size(); l++) {
            if (fabs(tempGraph->GetPointX(k) - SignalsEnergies.at(l)) <= SignalsEnergiesAccRange.at(l))
              yBin = l+1;
          }
          double weight = 0, val = 0, err = 0;
          if (tempGraph->GetErrorYlow(k) > 0) {
            weight += 1/std::pow(tempGraph->GetErrorYlow(k), 2);
            val += tempGraph->GetPointY(k)/std::pow(tempGraph->GetErrorYlow(k), 2);
          }
          if (Histograms2DForDifferentArgBG.at(j+namesSizes)->GetBinError(xBin, yBin) > 0) {
            weight += 1/std::pow(Histograms2DForDifferentArgBG.at(j+namesSizes)->GetBinError(xBin, yBin), 2);
            val += Histograms2DForDifferentArgBG.at(j+namesSizes)->GetBinContent(xBin, yBin)/std::pow(Histograms2DForDifferentArgBG.at(j+namesSizes)->GetBinError(xBin, yBin), 2);
          }
          if (weight > 0) {
            val = val/weight;
            err = 1/std::sqrt(weight);
          }
          Histograms2DForDifferentArgBG.at(j+namesSizes)->AddBinContent(xBin, yBin, val);
          Histograms2DForDifferentArgBG.at(j+namesSizes)->SetBinError(xBin, yBin, err);
        }
      }
    }

    fileIn->Close();
  }

  if (Histograms2DForDifferentArg.size()*Graphs2DForDifferentArg.size() == 0) {
    std::cout << "No histograms or graphs to save" << std::endl;
    return 0;
  }

  TString outputName = "Corr_FirstFile_" + NameOfTheFiles.at(0);
  TFile* outputFile = new TFile(outputName, "RECREATE");

  std::vector<int> chosenLines = {3, 6, 7, 9, 13, 14, 15, 16, 18, 20, 21, 22, 24, 30, 32};

  for (unsigned i=0; i<NameOfTheHistogramsToGet.size(); i++) {
    Histograms2DForDifferentArg.at(i)->Write((NameOfTheHistogramsToGet.at(i) + "_corr").c_str());
    Histograms2DForDifferentArgBG.at(i)->Write((NameOfTheHistogramsToGet.at(i) + "_corr_BG").c_str());
  }
  for (unsigned i=0; i<NameOfTheGraphsToGet.size(); i++) {
    std::cout << NameOfTheGraphsToGet.at(i) << std::endl;
    Graphs2DForDifferentArg.at(i)->Write((NameOfTheGraphsToGet.at(i) + "_corr").c_str());
    Histograms2DForDifferentArg.at(i+namesSizes)->Write((NameOfTheGraphsToGet.at(i) + "_Histcorr").c_str());
    Graphs2DForDifferentArgBG.at(i)->Write((NameOfTheGraphsToGet.at(i) + "_corr_BG").c_str());
    Histograms2DForDifferentArgBG.at(i+namesSizes)->Write((NameOfTheGraphsToGet.at(i) + "_Histcorr_BG").c_str());

 /*   for (unsigned j=1; j<=SignalsEnergies.size(); j++) {
      TH1D* histoToFit = (TH1D*)Histograms2DForDifferentArg.at(i+namesSizes)->ProjectionX("projX",j,j);
      double firstArg = CorrelatingArguments.at(0);
      double secondLastArg = CorrelatingArguments.at(CorrelatingArguments.size()-2);
      double lastArg = CorrelatingArguments.at(CorrelatingArguments.size()-1);
      double firstSecondLastDiff = fabs(histoToFit->GetBinContent(histoToFit->FindBin(firstArg) - histoToFit->GetBinContent(histoToFit->FindBin(secondLastArg))));
      double lastSecondLastDiff = fabs(histoToFit->GetBinContent(histoToFit->FindBin(lastArg) - histoToFit->GetBinContent(histoToFit->FindBin(secondLastArg))));
      double asymptTest = (lastSecondLastDiff <= firstSecondLastDiff) ? 1 : -1;
      double offsetInitArg = (asymptTest > 0) ? lastArg : firstArg;
      double slopeInit = (asymptTest > 0) ? -1 : 1;
      double ampInit = (histoToFit->GetBinContent(histoToFit->FindBin(firstArg)) >= histoToFit->GetBinContent(histoToFit->FindBin(lastArg))) ? asymptTest : -asymptTest;
      TF1* fitFunc = new TF1("fitFunc", "[0] + [1]*exp([2]*x)", firstArg, lastArg);
      fitFunc->SetParameters(histoToFit->GetBinContent(histoToFit->FindBin(offsetInitArg)), ampInit, slopeInit);

      double largeN = 1E8;
      if (histoToFit->GetBinContent(histoToFit->FindBin(0)) > histoToFit->GetBinContent(histoToFit->FindBin(60))) {
        if (histoToFit->GetMean() > 30.) {
          fitFunc->SetParameters(histoToFit->GetBinContent(histoToFit->FindBin(60)), -1, 1);
        } else {
          fitFunc->SetParameters(histoToFit->GetBinContent(histoToFit->FindBin(0)), 1, -1);
        }
      } else {
        if (histoToFit->GetMean() > 30.) {
          fitFunc->SetParameters(histoToFit->GetBinContent(histoToFit->FindBin(60)), -1, -1);
        } else {
          fitFunc->SetParameters(histoToFit->GetBinContent(histoToFit->FindBin(0)), 1, 1);
        }
      }
      histoToFit->Fit("fitFunc", "RW");
      histoToFit->Write((NameOfTheGraphsToGet.at(i) + "_corr_Proj" + NumberToChar(j,0)).c_str());
      delete fitFunc;
      delete histoToFit;
    }*/

    std::ofstream output;
    std::string addition = NameOfTheFiles.at(0);
    std::string outName = "ChosenLine_" + addition.erase(addition.length()-5,5) + "BG.dat";
    output.open(outName.c_str(), std::ofstream::out | std::ofstream::app);
    for (unsigned j=0; j<chosenLines.size(); j++) {
      TH1D* histoToFit = (TH1D*)Histograms2DForDifferentArgBG.at(i+namesSizes)->ProjectionX("projX",chosenLines.at(j),chosenLines.at(j));
      output << SignalsEnergies.at(chosenLines.at(j)-1);
      for (unsigned k=0; k<CorrelatingArguments.size(); k++) {
        output  << "\t" << histoToFit->GetBinContent(histoToFit->FindBin(CorrelatingArguments.at(k))) << "\t" << histoToFit->GetBinError(histoToFit->FindBin(CorrelatingArguments.at(k)));
      }
      output << std::endl;
    }
    output.close();

 /*   for (unsigned j=1; j<=SignalsEnergies.size(); j++) {
      TH1D* histoToFit = (TH1D*)Histograms2DForDifferentArgBG.at(i+namesSizes)->ProjectionX("projX",j,j);
      double firstArg = CorrelatingArguments.at(0);
      double secondLastArg = CorrelatingArguments.at(CorrelatingArguments.size()-2);
      double lastArg = CorrelatingArguments.at(CorrelatingArguments.size()-1);
      double firstSecondLastDiff = fabs(histoToFit->GetBinContent(histoToFit->FindBin(firstArg) - histoToFit->GetBinContent(histoToFit->FindBin(secondLastArg))));
      double lastSecondLastDiff = fabs(histoToFit->GetBinContent(histoToFit->FindBin(lastArg) - histoToFit->GetBinContent(histoToFit->FindBin(secondLastArg))));
      double asymptTest = (lastSecondLastDiff <= firstSecondLastDiff) ? 1 : -1;
      double offsetInitArg = (asymptTest > 0) ? lastArg : firstArg;
      double slopeInit = (asymptTest > 0) ? -1 : 1;
      double ampInit = (histoToFit->GetBinContent(histoToFit->FindBin(firstArg)) >= histoToFit->GetBinContent(histoToFit->FindBin(lastArg))) ? asymptTest : -asymptTest;
      TF1* fitFunc = new TF1("fitFunc", "[0] + [1]*exp([2]*x)", firstArg, lastArg);
      //fitFunc->SetParameters(histoToFit->GetBinContent(histoToFit->FindBin(offsetInitArg)), ampInit, slopeInit);
      double largeN = 1E8;
      if (histoToFit->GetBinContent(histoToFit->FindBin(0)) > histoToFit->GetBinContent(histoToFit->FindBin(60))) {
        if (histoToFit->GetMean() > 30.) {
          fitFunc->SetParameters(histoToFit->GetBinContent(histoToFit->FindBin(60)), -1, 1);
        } else {
          fitFunc->SetParameters(histoToFit->GetBinContent(histoToFit->FindBin(0)), 1, -1);
        }
      } else {
        if (histoToFit->GetMean() > 30.) {
          fitFunc->SetParameters(histoToFit->GetBinContent(histoToFit->FindBin(60)), -1, -1);
        } else {
          fitFunc->SetParameters(histoToFit->GetBinContent(histoToFit->FindBin(0)), 1, 1);
        }
      }
      histoToFit->Fit("fitFunc", "RW");
      histoToFit->Write((NameOfTheGraphsToGet.at(i) + "_corrBG_Proj" + NumberToChar(j,0)).c_str());
      delete fitFunc;
      delete histoToFit;
    }*/

    tempHisto2D = (TH2D*)Histograms2DForDifferentArg.at(i+namesSizes)->Clone("histoDiff");
    tempHisto2D->Add(Histograms2DForDifferentArgBG.at(i+namesSizes), -1.);
    tempHisto2D->Write((NameOfTheGraphsToGet.at(i) + "_DiffFromBG").c_str());
  }

  outputFile->Close();;

  return 0;
}

std::string NumberToChar(double number, int precision)
{
  std::ostringstream conv;
  conv << std::fixed << std::setprecision(precision);
  conv << number;
  return conv.str();
}
