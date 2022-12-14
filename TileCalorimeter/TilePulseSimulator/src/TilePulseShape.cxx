/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "TilePulseSimulator/TilePulseShape.h"

#include "TMath.h"
#include "TGraph.h"
#include "TSpline.h"
#include "TF1.h"

#include <iostream>

#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/GaudiException.h"

//_________________________________________________________________________
TilePulseShape::TilePulseShape(IMessageSvc* msgSvc, const std::string& name)
  : AthMessaging(msgSvc, name)
{
}

//_________________________________________________________________________
TilePulseShape::TilePulseShape(IMessageSvc* msgSvc, const std::string& name, const TString& fileName)
  : AthMessaging(msgSvc, name)
{
  loadPulseShape(fileName);
} 

//________________________________________________________________________
TilePulseShape::TilePulseShape(IMessageSvc* msgSvc, const std::string& name, const std::vector<double>& shapevec)
  : AthMessaging(msgSvc, name)
{
  setPulseShape(shapevec);
}

//_________________________________________________________________________
TilePulseShape::~TilePulseShape()
{
  resetDeformation();
  if(m_pulseShape) delete m_pulseShape;
  if(m_deformedSpline) delete m_deformedSpline;
}

//_________________________________________________________________________
void TilePulseShape::loadPulseShape(const TString& fileName)
{
  resetDeformation();
  if(m_pulseShape) delete m_pulseShape;
  m_pulseShape = new TGraph(fileName.Data());
  if(m_pulseShape->IsZombie()) {
    throw GaudiException(std::string("could not load pulseshape from file: ") + fileName.Data(),
                         "TilePulseShape", StatusCode::FAILURE);
  } else ATH_MSG_INFO("Loaded pulseshape from file: " << fileName);
  resetDeformation();
}

//_________________________________________________________________________
void TilePulseShape::setPulseShape(const std::vector<double>& shapevec)
{
  resetDeformation();
  if(m_pulseShape) delete m_pulseShape;
  m_pulseShape = new TGraph(shapevec.size());
  for(std::vector<double>::size_type i = 0; i != shapevec.size(); i++) {
    m_pulseShape->SetPoint(i, -75.5+i*0.5, shapevec.at(i));
  }
  resetDeformation();
}
    

//
//_________________________________________________________________________
// This function gives a pulse shaper for the 3-in-1 FEB
double TilePulseShape::eval(double x, bool useSpline, bool useUndershoot)
{

  //=== make sure pulseshape is available
  if(!m_deformedShape){
    ATH_MSG_ERROR("No pulseshape loaded!");
    return 0.;
  }

  //=== interpolate pulseShape value, TMath::BinarySearch returns 
  //=== index of value smaller or equal to the search value. 
  //=== -> Need to catch boundary values
  double y(0.);
  int n(m_deformedShape->GetN());
  int idx = TMath::BinarySearch(n,m_deformedShape->GetX(),x);
  if(idx<0){
    //=== left out of bounds, return leftmost value
//    y = (_deformedShape->GetY())[0]; 
    y = 0;
    ATH_MSG_DEBUG("Left out of bounds. Replacing y = " << (m_deformedShape->GetY())[0] << " with y = 0. (idx = " << idx << ", x = " << x << ")");
  }
  else if(idx>=n-1){
    //=== right out of bounds, return rightmost value
//    y = (_deformedShape->GetY())[n-1]; 
    if(useUndershoot){
      y = 0.00196 * (1 - exp(- x / 20664.59) ) - 0.00217;
    }
    else{
      y = 0;
      ATH_MSG_DEBUG("Right out of bounds. Replacing y = " << (m_deformedShape->GetY())[0] << " with y = 0. (idx = " << idx << ", x = " << x << ")");
    }
  }
  else{
    //=== linear interpolation
    double xLo = (m_deformedShape->GetX())[idx  ];
    double xHi = (m_deformedShape->GetX())[idx+1];
    double yLo = (m_deformedShape->GetY())[idx  ];
    double yHi = (m_deformedShape->GetY())[idx+1];
    double yLinear = yLo + (yHi-yLo)/(xHi-xLo) * (x-xLo);
    //=== spline interpolation
    double ySpline = m_deformedSpline->Eval(x);

    if(useSpline) y = ySpline;
    else          y = yLinear;
  }

  return y;
}

//  
//_______________________________________________________________________
void TilePulseShape::resetDeformation()
{
  if(m_deformedShape==m_pulseShape) return;
  
  delete m_deformedShape;  
  delete m_deformedSpline; 
  m_deformedShape = m_pulseShape;
  m_deformedSpline = new TSpline3("deformedSpline",m_deformedShape);
}

//
//_______________________________________________________________________
int TilePulseShape::scalePulse(double leftSF, double rightSF)
{

  resetDeformation();
  if(!m_pulseShape) {
    ATH_MSG_WARNING("Attempted pulse shape scaling before loading pulse shape");
    return 1;
   } else {
    m_deformedShape = (TGraph*) m_pulseShape->Clone();
  
    for(int i=0; i<m_deformedShape->GetN(); ++i){
      double x,y;
      m_deformedShape->GetPoint(i,x,y);
      if(x<0.)      x*= leftSF;
      else if(x>0.) x*=rightSF;
      m_deformedShape->SetPoint(i,x,y);
    }

    delete m_deformedSpline;
    m_deformedSpline = new TSpline3("deformedSpline",m_deformedShape);
    return 0;
  }   

}

//
//_______________________________________________________________________
TGraph* TilePulseShape::getGraph(double t0, double ped, double amp)
{
  TGraph* gr = (TGraph*) m_deformedShape->Clone();
  for(int i=0; i<gr->GetN(); i++){
    double x,y;
    gr->GetPoint(i,x,y);
    y*=amp;
    y+=ped;
    x+=t0;
    gr->SetPoint(i,x,y);
  }
  return gr;
}
