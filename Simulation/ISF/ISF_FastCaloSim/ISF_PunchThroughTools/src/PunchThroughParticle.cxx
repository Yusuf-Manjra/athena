/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PunchThroughParticle.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "PunchThroughParticle.h"

// ROOT
#include "TH2F.h"

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
ISF::PunchThroughParticle::PunchThroughParticle(int pdg, bool doAnti):
  m_pdgId(pdg),
  m_doAnti(doAnti),
  m_minEnergy(0.),
  m_maxNum(-1),
  m_numParticlesFactor(1.),
  m_energyFactor(1.),
  m_posAngleFactor(1.),
  m_momAngleFactor(1.),
  m_corrPdg(0),
  m_corrMinEnergy(0),
  m_corrFullEnergy(0),
  m_histCorrLowE(nullptr),
  m_histCorrHighE(nullptr),
  m_corrHistDomains(nullptr),
  m_pdfNumParticles(nullptr),	//does this number ever change?
  m_pdf_pca0(nullptr),
  m_pdf_pca1(nullptr),
  m_pdf_pca2(nullptr),
  m_pdf_pca3(nullptr),
  m_pdf_pca4(nullptr)
{ }

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
ISF::PunchThroughParticle::~PunchThroughParticle()
{
  delete[] m_corrHistDomains;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

void ISF::PunchThroughParticle::setMinEnergy(double minEnergy)
{
  m_minEnergy = minEnergy;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

void ISF::PunchThroughParticle::setMaxNumParticles(int maxNum)
{
  m_maxNum = maxNum;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

void ISF::PunchThroughParticle::setNumParticlesFactor(double numFactor)
{
  m_numParticlesFactor = numFactor;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

void ISF::PunchThroughParticle::setEnergyFactor(double energyFactor)
{
  m_energyFactor = energyFactor;
}


/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void ISF::PunchThroughParticle::setPosAngleFactor(double posAngleFactor)
{
  m_posAngleFactor = posAngleFactor;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void ISF::PunchThroughParticle::setMomAngleFactor(double momAngleFactor)
{
  m_momAngleFactor = momAngleFactor;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

void ISF::PunchThroughParticle::setNumParticlesPDF(std::unique_ptr<PDFcreator> pdf)
{
  m_pdfNumParticles = std::move(pdf);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

void ISF::PunchThroughParticle::setCorrelation(int corrPdg,
                                               TH2F *histLowE, TH2F *histHighE,
                                               double minCorrelationEnergy,
                                               double fullCorrelationEnergy,
                                               double lowE,
                                               double midE,
                                               double upperE)
{
  m_corrPdg = corrPdg;
  m_histCorrLowE = histLowE;
  m_histCorrHighE = histHighE;
  m_corrMinEnergy = minCorrelationEnergy;
  m_corrFullEnergy = fullCorrelationEnergy;

  m_corrHistDomains = new double [3];
  m_corrHistDomains[0] = lowE;
  m_corrHistDomains[1] = midE;
  m_corrHistDomains[2] = upperE;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:

 *  ==> see headerfile
 *=======================================================================*/

void ISF::PunchThroughParticle::setPCA0PDF(std::unique_ptr<PDFcreator> pdf)
{
  m_pdf_pca0 = std::move(pdf);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

void ISF::PunchThroughParticle::setPCA1PDF(std::unique_ptr<PDFcreator> pdf)
{
  m_pdf_pca1 = std::move(pdf);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

void ISF::PunchThroughParticle::setPCA2PDF(std::unique_ptr<PDFcreator> pdf)
{
  m_pdf_pca2 = std::move(pdf);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void ISF::PunchThroughParticle::setPCA3PDF(std::unique_ptr<PDFcreator> pdf)
{
  m_pdf_pca3 = std::move(pdf);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void ISF::PunchThroughParticle::setPCA4PDF(std::unique_ptr<PDFcreator> pdf)
{
  m_pdf_pca4 = std::move(pdf);
}
