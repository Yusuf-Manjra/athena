/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
			LargestWeightsMultiStateMerger.h  -  description
			------------------------------------------------
begin                : Wednesday 23rd February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Class for merging components of a multi-state based on 
                       retention of those with largest weights
*********************************************************************************/

#ifndef TrkLargestWeightsMultiStateMerger_H
#define TrkLargestWeightsMultiStateMerger_H

#include "TrkGaussianSumFilter/IMultiComponentStateAssembler.h"
#include "TrkGaussianSumFilter/IMultiComponentStateCombiner.h"
#include "TrkGaussianSumFilter/IMultiComponentStateMerger.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk{


class LargestWeightsMultiStateMerger : public AthAlgTool, virtual public IMultiComponentStateMerger {

 public:
  /** Constructor with parameters to be passed to AlgTool */
  LargestWeightsMultiStateMerger(const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~LargestWeightsMultiStateMerger();

  /** AlgTool initialise method */
  StatusCode initialize();

  /** AlgTool finalise method */
  StatusCode finalize();

  virtual const MultiComponentState* merge(const MultiComponentState&) const;

 private:  
  int                           m_outputlevel;                      //!< to cache current output level

  unsigned int                  m_maximumNumberOfComponents;
  bool                          m_doSmallComponentMerging;
  
  PublicToolHandle<IMultiComponentStateCombiner>  m_stateCombiner
     {this,"MultiComponentStateCombiner","Trk::MultiComponentStateCombiner/LargestWeightsStateCombiner",""};
  PublicToolHandle<IMultiComponentStateAssembler> m_stateAssembler
     {this,"MultiComponentStateAssembler","Trk::MultiComponentStateAssembler/LargestWeightsStateAssembler",""};

};


} // end Trk namespace

#endif
