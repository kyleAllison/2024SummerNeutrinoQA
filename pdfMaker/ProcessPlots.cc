#include <iostream>
#include <set>
#include <vector>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TObject.h>
#include <TKey.h>
#include <TString.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TFitResult.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <algorithm>
#include <TLine.h>
#include "TMath.h"

using namespace std;

void DrawWithUncertaintyEnvelope(TObject* object) {
  //This must be a TGraphErrors*!
  if (string(object->ClassName()) != string("TGraphErrors")) {
    cout << "[ERROR] Tried to fit for Max ADC on a non-TGraphErrors object! Which? " 
	 <<  object->GetName() << endl;
    return;
  }

  cout << "Making nice envelope plot for object " << object->GetName() << endl;

  TGraphErrors* graph = static_cast<TGraphErrors*>(object);

  const int color = graph->GetMarkerColor();

  graph->SetLineColor(color);
  graph->SetLineWidth(2);
  graph->SetFillColor(color);
  graph->SetFillColorAlpha(color,0.5);
  graph->Draw("a3");
  graph->Draw("LX SAME");
}

void IncreaseTH1DPlotMax(TObject* object, const double factor = 1.2) {
  //This must be a TH1D*!
  if (string(object->ClassName()) != string("TH1D")) {
    cout << "[ERROR] Tried to increase plot max of a non-TH1D object! Which? "
	 << object->GetName() << endl;
    return;
  }

  TH1D* histogram = static_cast<TH1D*>(object);  
  
  histogram->SetMaximum(factor*histogram->GetMaximum());
}

void OverlayTH1Ds(TObject* object1,TObject* object2) {
  //This must be a TH1D*!
  if (string(object1->ClassName()) != string("TH1D") ||
      string(object2->ClassName()) != string("TH1D") ) {
    cout << "[ERROR] Tried to overlay non-TH1D object!"<< endl;
    return;
  }

  TH1D* histogram1 = static_cast<TH1D*>(object1);  
  TH1D* histogram2 = static_cast<TH1D*>(object2);  
  
  const double max = (histogram1->GetMaximum() > histogram2->GetMaximum()) ?
    histogram1->GetMaximum() : histogram2->GetMaximum();

  histogram1->SetLineColor(kBlue-2);
  histogram2->SetLineColor(kOrange+2);
  histogram1->SetLineWidth(2);
  histogram2->SetLineWidth(2);

  
  histogram1->SetMaximum(max);
  histogram2->SetMaximum(max);

  histogram1->Draw();
  histogram2->Draw("SAME");
}

void NormalizeTH1D(TObject* object) {
  //This must be a TH1D*!
  if (string(object->ClassName()) != string("TH1D")) {
    cout << "[ERROR] Tried to normalize a non-TH1D object! Which? "
	 << object->GetName() << endl;
    return;
  }

  TH1D* histogram = static_cast<TH1D*>(object);  

  if (histogram->GetEntries() == 0)
    return;

  histogram->Scale(100./histogram->GetEntries());
}

void IncreaseTH1DMarkerSize(TObject* object, const double size) {
  //This must be a TH1D*!
  if (string(object->ClassName()) != string("TH1D")) {
    cout << "[ERROR] Tried to increase marker size of a non-TH1D object! Which?"
	 << object->GetName() << endl;
    return;
  }

  TH1D* histogram = static_cast<TH1D*>(object);  

  histogram->SetMarkerSize(size);
}

void FitForMaxADC(TObject* object) {
  TLatex label; //for MaxADC
  label.SetTextSize(0.1);
  
  //This must be a TH1D*!
  if (string(object->ClassName()) != string("TH1D")) {
    cout << "[ERROR] Tried to fit for Max ADC on a non-TH1D object!" << endl;
  }
  
  const double minMaxADCAllowed = 20;

  TH1D* histogram = static_cast<TH1D*>(object);

  // Get the max value to fit around. If it is less than 20, try again but cut off the region
  double maxVal = histogram->GetXaxis()->GetBinCenter(histogram->GetMaximumBin());
  if (maxVal < minMaxADCAllowed) {
    int maxBin = histogram->GetMaximumBin();
    histogram->GetXaxis()->SetRange(maxBin + 10, histogram->GetNbinsX());
    
    cout << "maxValu: " << maxVal << endl;
    cout << "maxBin ++ 10: " << maxBin + 10 << endl;
    cout << "histogram->GetNbinsX(): " << histogram->GetNbinsX() << endl;
    maxVal = histogram->GetXaxis()->GetBinCenter(histogram->GetMaximumBin());
    cout << "maxVal: " << maxVal << endl;
  }

  //Get max value to fit around. Fit near max with Landau.

  TFitResultPtr r = histogram->Fit("landau", "SIQ", "", maxVal - 20, maxVal + 40);

  //Draw result.  
  int fitStatus = r;
  if (fitStatus >= 0) {
    double maxMaxADC = r->Parameter(1);
    label.DrawLatexNDC(0.38, 0.75, Form("MaxADC_{max}#approx%0.1f", 
					maxMaxADC));
    histogram->GetXaxis()->SetRange(0, 130);
  }
}

// Draw a vertical red line at the max value
// TODO: Why doesn't this work?
void 
DrawLineAtMax(TObject* obj, TCanvas* c, const string type)
{
  if (type != "TH1D") {
    cout << "Cannot call DrawLineAtMax on type: " << type << endl;
    return;
  }

  TH1D* h = static_cast<TH1D*>(obj);
  const double maxBin = h->GetMaximumBin();
  const double maxValue = h->GetXaxis()->GetBinCenter(maxBin);
  const double x1 = maxValue;
  const double x2 = maxValue;
  const double y1 = 0.0;
  const double y2 = h->GetBinContent(maxBin);

  c->Update();
  c->Range(-700.0, 0.0, -300.0, 50000.0);
  TLine line(x1, y1, x2, y2);
  line.SetLineColor(kRed);
  line.SetLineWidth(2);
  line.Draw();
}

void
SetLabelSize(TObject* obj, const double axisLabelSize, const string type, 
	     const bool setXNumberSizeOnly, const bool setYNumberSizeOnly)
{
  if (setXNumberSizeOnly && setYNumberSizeOnly) {
    cout << "Wrong use of the SetLabelSize() function." << endl;
    return;
  }

  // TH1D
  if (type == "TH1D") {
    TH1D* h = static_cast<TH1D*>(obj);
    if (setXNumberSizeOnly)
      h->GetXaxis()->SetLabelSize(axisLabelSize);
    else if (setYNumberSizeOnly)
      h->GetYaxis()->SetLabelSize(axisLabelSize);
    else {
      h->GetYaxis()->SetLabelSize(axisLabelSize);
      h->GetXaxis()->SetLabelSize(axisLabelSize);
      h->GetXaxis()->SetTitleSize(axisLabelSize);
      h->GetYaxis()->SetTitleSize(axisLabelSize);
    }
  }
    
  // TH2D
  if (type == "TH2D") {
    TH2D* h = static_cast<TH2D*>(obj);
    if (setXNumberSizeOnly)
      h->GetXaxis()->SetLabelSize(axisLabelSize);
    else if (setYNumberSizeOnly)
      h->GetYaxis()->SetLabelSize(axisLabelSize);
    else {
      h->GetYaxis()->SetLabelSize(axisLabelSize);
      h->GetXaxis()->SetLabelSize(axisLabelSize);
      h->GetXaxis()->SetTitleSize(axisLabelSize);
      h->GetYaxis()->SetTitleSize(axisLabelSize);
    }
  }

  // TGraph
  if (type == "TGraph") {
    TGraph* h = static_cast<TGraph*>(obj);
    if (setXNumberSizeOnly)
      h->GetXaxis()->SetLabelSize(axisLabelSize);
    else if (setYNumberSizeOnly)
      h->GetYaxis()->SetLabelSize(axisLabelSize);
    else {
      h->GetYaxis()->SetLabelSize(axisLabelSize);
      h->GetXaxis()->SetLabelSize(axisLabelSize);
      h->GetXaxis()->SetTitleSize(axisLabelSize);
      h->GetYaxis()->SetTitleSize(axisLabelSize);
    }
  }
  
  // TGraphErrors
  if (type == "TGraphErrors") {
    TGraphErrors* h = static_cast<TGraphErrors*>(obj);
    if (setXNumberSizeOnly)
      h->GetXaxis()->SetLabelSize(axisLabelSize);
    else if (setYNumberSizeOnly)
      h->GetYaxis()->SetLabelSize(axisLabelSize);
    else {
      h->GetYaxis()->SetLabelSize(axisLabelSize);
      h->GetXaxis()->SetLabelSize(axisLabelSize);
      h->GetXaxis()->SetTitleSize(axisLabelSize);
      h->GetYaxis()->SetTitleSize(axisLabelSize);
    }
  }

  // TH1F
  if (type == "TH1F") {
    TH1F* h = static_cast<TH1F*>(obj);
    if (setXNumberSizeOnly)
      h->GetXaxis()->SetLabelSize(axisLabelSize);
    else if (setYNumberSizeOnly)
      h->GetYaxis()->SetLabelSize(axisLabelSize);
    else {
      h->GetYaxis()->SetLabelSize(axisLabelSize);
      h->GetXaxis()->SetLabelSize(axisLabelSize);
      h->GetXaxis()->SetTitleSize(axisLabelSize);
      h->GetYaxis()->SetTitleSize(axisLabelSize);
    }
  }

  
  // TH2F
  if (type == "TH2F") {
    TH2F* h = static_cast<TH2F*>(obj);
    if (setXNumberSizeOnly)
      h->GetXaxis()->SetLabelSize(axisLabelSize);
    else if (setYNumberSizeOnly)
      h->GetYaxis()->SetLabelSize(axisLabelSize);
    else {
      h->GetYaxis()->SetLabelSize(axisLabelSize);
      h->GetXaxis()->SetLabelSize(axisLabelSize);
      h->GetXaxis()->SetTitleSize(axisLabelSize);
      h->GetYaxis()->SetTitleSize(axisLabelSize);
    }
  }
  
  // TProfile2D
  if (type == "TProfile2D") {
    TProfile2D* h = static_cast<TProfile2D*>(obj);
    if (setXNumberSizeOnly)
      h->GetXaxis()->SetLabelSize(axisLabelSize);
    else if (setYNumberSizeOnly)
      h->GetYaxis()->SetLabelSize(axisLabelSize);
    else {
      h->GetYaxis()->SetLabelSize(axisLabelSize);
      h->GetXaxis()->SetLabelSize(axisLabelSize);
      h->GetXaxis()->SetTitleSize(axisLabelSize);
      h->GetYaxis()->SetTitleSize(axisLabelSize);
    }
  }

  // TProfile
  if (type == "TH1Profile") {
    TProfile* h = static_cast<TProfile*>(obj);
    if (setXNumberSizeOnly)
      h->GetXaxis()->SetLabelSize(axisLabelSize);
    else if (setYNumberSizeOnly)
      h->GetYaxis()->SetLabelSize(axisLabelSize);
    else {
      h->GetYaxis()->SetLabelSize(axisLabelSize);
      h->GetXaxis()->SetLabelSize(axisLabelSize);
      h->GetXaxis()->SetTitleSize(axisLabelSize);
      h->GetYaxis()->SetTitleSize(axisLabelSize);
    }
  }
} // end set label size

// Change the Y-axis to be +/- bound around the min, max values. Only for TGraph, TGraphErrors
void 
TightenRange(TObject* obj, const string type, const double upperBound, const double lowerBound)
{
  
  // TGraph
  if (type == "TGraph") {
    TGraph* h = static_cast<TGraph*>(obj);
    const int n = h->GetN();
    const double minY = TMath::MaxElement(n, h->GetY());
    const double maxY = TMath::MinElement(n, h->GetY());
    h->GetHistogram()->SetMinimum(minY - lowerBound);
    h->GetHistogram()->SetMaximum(maxY + upperBound);    
  }

  // TGraphErrors
  if (type == "TGraphErrors") {
    TGraphErrors* h = static_cast<TGraphErrors*>(obj);
    const int n = h->GetN();
    const double minY = TMath::MaxElement(n, h->GetY());
    const double maxY = TMath::MinElement(n, h->GetY());
    h->GetHistogram()->SetMinimum(minY - lowerBound);
    h->GetHistogram()->SetMaximum(maxY + upperBound);    
  }


} // end TightenRange

void
CutOffMaxBin(TObject* obj, const string type, const double boundDecrease)
{
  // Get the max bin. If positive, set range to -10000 to maxBinXValue - boundDecrease
  // If negative, set range to maxBinXValue + boundDecrease to 100000
  // Only for TH1Ds
  if (type != "TH1D") {
    cout << "Cannot call CutOffMaxBin on type: " << type << endl;
    return;
  }
  TH1D* h = static_cast<TH1D*>(obj);
  const double maxBin = h->GetMaximumBin();
  const double maxValue = h->GetXaxis()->GetBinCenter(maxBin);
  

  cout << "\nh->GetName(): " << h->GetName() << endl;
  cout << "maxBin: " << maxBin << endl;
  cout << "maxValue: " << maxValue << endl;
  
  if (maxValue > 0)
    h->GetXaxis()->SetRangeUser(-10000, maxValue - boundDecrease);
  else
    h->GetXaxis()->SetRangeUser(maxValue + boundDecrease, 10000);
}

int
main(int argc, char* argv[])
{
  if (argc < 2) {
    cerr << "usage: " << argv[0] << " <filename>" << endl;
    return 1;
  }

  const string filename(argv[1]);

  set<string> objectsToNotDraw {"TTree","TNtuple"}; 

  set<string> objectsToNormalize {
    "S1Counts",
    "T1Counts",
    "T2Counts",
    "T3Counts",
    "T4Counts",
    "EventComposition"
  }; 

  //Objects for which to draw uncertainty envelope. For scalers
  //graphs.
  set<string> uncertaintyEnvelopeObjects {
    "T2T1Ratio",
    "T4T3Ratio",
    "T1T3Ratio",
    "T1S1Ratio",
    "T3S1Ratio",
    "S1Intensity"
  };

  // Add object names with special draw options here. Key: Histogram
  // name. Value: Draw options.
  map<string,string> specialDrawOptionsMap {
    {"runNumberGraph","AL"},
    {"eventIdGraph","AL"},
    {"S1Counts","hist TEXT"},
    {"T1Counts","hist TEXT"},
    {"T2Counts","hist TEXT"},
    {"T3Counts","hist TEXT"},
    {"T4Counts","hist TEXT"},
    {"EventComposition","hist TEXT"}
  };

  //Add object names with text option here to increase text font size.
  map<string,double> textSizeMap {
    {"S1Counts",2.2},
    {"T1Counts",2.2},
    {"T2Counts",2.2},
    {"T3Counts",2.2},
    {"T4Counts",2.2},
    {"EventComposition",2.2}
  };
  
  //Default special draw options for plot types.
  map<string,string> drawOptionsMap {
    {"TGraph","AP"},
    {"TGraphErrors","AP"},
    {"TGraphAsymmErrors","AP"},
    {"TH2D","COLZ"},
    {"TH2F","COLZ"},
    {"TProfile2D","COLZ"}
  };
  //Add histogram names to be plotted with log X-scale here.
  vector<string> objectsWithLogX { 
    "dummy","dummy2"
  };
  //Add histogram names to be plotted with log Y-scale here.
  vector<string> objectsWithLogY {
    "S1MHTDC","T1MHTDC","T2MHTDC","T3MHTDC","T4MHTDC",
    "S1Counts","T1Counts","T2Counts","T3Counts","T4Counts",
    "Cl_Y_VTPC1", "Cl_Y_VTPC2", "Cl_Y_MTPCL", "Cl_Y_MTPCR", 
    "Cl_Y_FTPC1", "Cl_Y_FTPC2", "Cl_Y_FTPC3", "Cl_Y_GTPC",
    "SecondaryVertexCount", "SecondaryTrackCount"
  };
  //Add histogram names to be plotted with log Z-scale here.
  vector<string> objectsWithLogZ {
    "SecondaryVerticesXZ","SecondaryVerticesZY","SecondaryVerticesZX",
    "Cl_XY","Cl_ZX","Cl_ZY","hitXY","pmtAmplitudeCorrelationScintillator",
    "ClustersStation","PixelsStation","psdModSectScopeT1","psdFrontViewT1","dYVsY","BPD", 
    "VTPC1vsVTPC2ClustersT1", "VTPC1vsVTPC2ClustersT2", "VTPC2vsMTPCClustersT1", 
    "VTPC2vsMTPCClustersT2", "SecondaryVerticesXY", "BeamVTPC1TrackDifferenceAtTargetZ",
    "trackXY", "S1BeamCounters2D", "S2BeamCounters2D", "S3BeamCounters2D", "S3pBeamCounters2D", "S4BeamCounters2D",
    "S5BeamCounters2D", "V0BeamCounters2D", "V1BeamCounters2D"
  };
  //Add histogram names to be plotted with log Z-scale here in addition to not log z
  vector<string> objectsWithLogZNewPlot {
    "BeamXZ", "BeamYZ"
  };
  vector<string> objectsWithLogYNewPlot { 
    "SecondaryVerticesZVTPC1TargetIn","SecondaryVerticesZVTPC1TargetOut" 
  };
  map<string,double> graphMins {
    {"T2T1Ratio",0},
    {"T4T3Ratio",0},
    {"T1T3Ratio",0},
    {"T1S1Ratio",0},
    {"T3S1Ratio",0},
    {"S1Intensity",0}
  };
  map<string,double> graphMaxes {
    {"T2T1Ratio",1.1},
    {"T4T3Ratio",1.1},
    {"T1T3Ratio",1.1},
    {"T1S1Ratio",1.1},
    {"T3S1Ratio",1.1}
  };

  // Large numbers/date strings, etc
  vector<string> objectsWithSmallXAxisNumberFont {
    "S1Intensity", "T2T1Ratio", "T1T3Ratio", "T4T3Ratio", "runNumberGraph", "eventIdGraph", 
    "T1S1Ratio", "psdEnergyCorrT1", "psdEnergyCorrT2", "VTPC1vsVTPC2ClustersT1", 
    "VTPC1vsVTPC2ClustersT2", "VTPC2vsMTPCClustersT1", "VTPC2vsMTPCClustersT2", 
    "vdriftReco_VTPC1", "vdriftReco_VTPC2", "vdriftReco_MTPCL", "vdriftReco_MTPCR", 
    "BeamXZ", "BeamYZ"
  };

  // Large numbers/date strings, etc
  vector<string> objectsWithSmallYAxisNumberFont {
    "TPCBusyTime", "runNumberGraph", "psdEnergyCorrT2", "VTPC1vsVTPC2ClustersT1", 
    "VTPC1vsVTPC2ClustersT2", "VTPC2vsMTPCClustersT1", "VTPC2vsMTPCClustersT2",
    "psdEnergyCorrT1", "BeamVTPC1TrackYDifferenceAtTargetZ", "BeamVTPC1TrackXDifferenceAtTargetZ",
    "SecondaryVerticesZVTPC1"
  };

  // Tighten the y bounds around max here, if desired
  vector<string> tightenYBounds {
    "T2T1Ratio", "T1T3Ratio", "T4T3Ratio", "T1S1Ratio"
  };

  // In addition to being draw normally, these plots will be drawn with the max bin area cut off
  // and no log scale
  vector<string> cutOffMax {
    "Cl_Y_VTPC1", "Cl_Y_VTPC2", "Cl_Y_MTPCL", "Cl_Y_MTPCR"
  };

  // Draw a line at the max value
  vector<string> drawLineAtMax {
    "SecondaryVerticesZVTPC1"
  };
  
  //No stats.
  gStyle->SetOptStat(0);

  //One sig fig in text draw options.
  gStyle->SetPaintTextFormat("3.2f");

  //Make sure file is open.  
  TFile* file = new TFile(filename.data()) ;
  if (!file->IsOpen()) {
    exit(1) ;
  }

  //Get keys.
  TIter fileIter(file->GetListOfKeys());
  TKey* key ;

  //Loop through keys.
  while ((key = (TKey*)fileIter())) {
    TObject* obj = key->ReadObj();

    const string name = obj->GetName();
    const string type = obj->ClassName();

    // Based on the type, get the object and change axis label size
    const double axisLabelSize = 0.07;
    const double axisLabelSizeSmall = 0.035;

    // With increased font size, need to increase canvas margin to not cut off text
    const double marginBuffer = 0.15;

    // Range to extend below min/max after tightenting bounds for trigger ratio plots
    const double rangeExtension = 0.1;

    // Amount to decrease the range when cutting off the max bins
    const double boundDecrease = 20;

    // Set every plots label font size
    SetLabelSize(obj, axisLabelSize, type, false, false);
        
    bool setLogX = false;
    bool setLogY = false;
    bool setLogZ = false;

    for (auto it = objectsWithLogX.begin(), itEnd = objectsWithLogX.end(); it != itEnd; ++it)
      if (name.find(*it) != string::npos)
    	setLogX = true;
    for (auto it = objectsWithLogY.begin(), itEnd = objectsWithLogY.end(); it != itEnd; ++it)
      if (name.find(*it) != string::npos)
    	setLogY = true;
    for (auto it = objectsWithLogZ.begin(), itEnd = objectsWithLogZ.end(); it != itEnd; ++it)
      if (name.find(*it) != string::npos)
    	setLogZ = true;
    
    for (auto it = objectsWithSmallXAxisNumberFont.begin(), 
	   itEnd = objectsWithSmallXAxisNumberFont.end(); it != itEnd; ++it) {
      if (name.find(*it) != string::npos){
	SetLabelSize(obj, axisLabelSizeSmall, type, true, false);
      }
    }
    for (auto it = objectsWithSmallYAxisNumberFont.begin(), 
	   itEnd = objectsWithSmallYAxisNumberFont.end(); it != itEnd; ++it) {
      if (name.find(*it) != string::npos){
	SetLabelSize(obj, axisLabelSizeSmall, type, false, true);
      }
    }    

    //Determine draw options.
    string drawOptions = "";
    if (specialDrawOptionsMap.find(name) != specialDrawOptionsMap.end()) {
      drawOptions = specialDrawOptionsMap[name];
    }
    else if (drawOptionsMap.find(type) != drawOptionsMap.end()) {
      drawOptions = drawOptionsMap[type];
    }
    
    //Make sure we should draw this type of object.
    if (objectsToNotDraw.find(type) != objectsToNotDraw.end())
      continue;

    //Normalize Histograms.
    if (objectsToNormalize.find(name) != objectsToNormalize.end())
      NormalizeTH1D(obj);
    
    TCanvas c("QACanvas", "", 900, 600);

    // Since we increased the size of the text, need to increase the margins
    c.SetLeftMargin(marginBuffer);
    c.SetBottomMargin(marginBuffer);

    //Increase text size,if desired. Increase plot max at the same time.
    if (textSizeMap.find(name) != textSizeMap.end()) {
      IncreaseTH1DMarkerSize(obj,textSizeMap.at(name));
      if (setLogY)
	IncreaseTH1DPlotMax(obj,10.);
      else 
	IncreaseTH1DPlotMax(obj,1.2);
    }
    //Draw with options. Include special options, if desired.
    if (uncertaintyEnvelopeObjects.find(name) != uncertaintyEnvelopeObjects.end())
      DrawWithUncertaintyEnvelope(obj);
    else
      obj->Draw(drawOptions.c_str());

    if (setLogX)
      c.SetLogx();
    if (setLogY)
      c.SetLogy();
    if (setLogZ)
      c.SetLogz();
    
    //Set limits for TGraphs, if defined.
    if (graphMins.find(name) != graphMins.end()) {
      TGraph* graph = static_cast<TGraph*>(obj);
      graph->SetMinimum(graphMins.at(name));
    }
    if (graphMaxes.find(name) != graphMaxes.end()) {
      TGraph* graph = static_cast<TGraph*>(obj);
      graph->SetMaximum(graphMaxes.at(name));
    }
    
    
    //Fit for maxADC.
    if (name.find("MaxADC") != string::npos) {
      cout << "Found MaxADC in name " << name << ". Position: " << name.find("MaxADC") << endl;
      FitForMaxADC(obj);
    }

    /*
    // For some plots, cut off the max and save again
    for (auto it = drawLineAtMax.begin(), itEnd = drawLineAtMax.end(); it != itEnd; ++it) 
    if (name.find(*it) != string::npos) 
    DrawLineAtMax(obj, &c, type);
    */

    // Save plot
    const TString saveName = "plots/" + string(obj->GetName()) + ".png";
    c.SaveAs(saveName.Data());

    // For some plots, cut off the max and save again
    for (auto it = cutOffMax.begin(), itEnd = cutOffMax.end(); it != itEnd; ++it) {
      if (name.find(*it) != string::npos) {
	
	// Turn off log scale
	c.SetLogy(0);

	// Cut off the max bin area
	CutOffMaxBin(obj, type, boundDecrease);
	
	// Now save the image with a different name
	const TString saveName = "plots/" + string(obj->GetName()) + "_CutOffMax.png";
	c.SaveAs(saveName.Data());
      }
    }

    // Tighten ranges and make new plots
    for (auto it = tightenYBounds.begin(), 
	   itEnd = tightenYBounds.end(); it != itEnd; ++it) {
      if (name.find(*it) != string::npos){
	TightenRange(obj, type, rangeExtension, rangeExtension);
	const TString saveName = "plots/" + string(obj->GetName()) + "_Tightened.png";
	c.SaveAs(saveName.Data());
      }
    }    

    // Tighten ranges and make new plots
    for (auto it = objectsWithLogZNewPlot.begin(), 
	   itEnd = objectsWithLogZNewPlot.end(); it != itEnd; ++it) {
      if (name.find(*it) != string::npos){
	c.SetLogz();
	const TString saveName = "plots/" + string(obj->GetName()) + "_LogZ.png";
	c.SaveAs(saveName.Data());
      }
    }    
    for (auto it = objectsWithLogYNewPlot.begin(), 
	   itEnd = objectsWithLogYNewPlot.end(); it != itEnd; ++it) {    
      if (name.find(*it) != string::npos){    
	c.SetLogy();   
	const TString saveName = "plots/" + string(obj->GetName()) + "_LogY.png";
	c.SaveAs(saveName.Data());          
      } 
    } 
    // CLose original canvas
    c.Close();
    


  }
} 
