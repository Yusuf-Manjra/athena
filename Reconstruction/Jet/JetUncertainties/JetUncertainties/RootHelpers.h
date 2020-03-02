/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_ROOTHELPERS_H
#define JETUNCERTAINTIES_ROOTHELPERS_H


#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "TString.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TFile.h"


class TH1;

class TAxis;

class RootHelpers 
{

// Histogram reading helpers
	//        double readHisto(const double var1, const double var2=0, const double var3=0) const;
	//        double checkBoundaries(const TAxis* axis, const int numBins, const double valInput) const;

        // RootHelper to have a const method for interpolation (why is there not a const version in ROOT???)
public:
        double Interpolate(const TH1* histo, const double x) const;
        double Interpolate(const TH1* histo, const double x, const double y) const;
        double Interpolate(const TH1* histo, const double x, const double y, const double z) const;
        Int_t FindBin(const TAxis* axis, const double x) const;
        
        double Interpolate2D(const TH1* histo, const double x, const double y, const int xAxis=1, const int yAxis=2, const int otherDimBin=-1) const;

	Int_t test();

private:

};





#endif

