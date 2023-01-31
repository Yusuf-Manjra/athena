/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class HistValBase                       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "HistValBase.h"

//____________________________________________________________________
HistValBase::HistValBase()
  : m_compareBinContentsOnEachFill(true),
    m_triggeredConversionToROOTHist(false)
{
}

//____________________________________________________________________
HistValBase::~HistValBase()
{
}

//____________________________________________________________________
void HistValBase::compareAllContents()
{
  compareBinContents();
  compareMetaData();
}

//____________________________________________________________________
void HistValBase::compareAll()
{
  compareAllContents();
  compareTH1s();
  compareFastLoop();
}

//____________________________________________________________________
void HistValBase::binContentsChanged()
{
  if (m_compareBinContentsOnEachFill)
    compareBinContents();
}

//____________________________________________________________________
void HistValBase::titleOrNameWasChanged()
{
  compareMetaData();
}

//____________________________________________________________________
void HistValBase::setCompareBinContentsOnEachFill(bool b)
{
  m_compareBinContentsOnEachFill = b;
  if (b)
    compareBinContents();
}
