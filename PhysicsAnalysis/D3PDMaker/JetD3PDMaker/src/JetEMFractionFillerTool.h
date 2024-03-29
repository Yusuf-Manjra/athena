// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetD3PDMaker/src/JetEMFractionFillerTool.h
 * @author Pier-Olivier DeViveiros < viveiros AT physics.utoronto.ca >
 * @date September, 2009
 * @brief Block filler tool for Jet EM Fraction
 */

#ifndef JETD3PDMAKER_JETEMFRACTIONFILLERTOOL_H
#define JETD3PDMAKER_JETEMFRACTIONFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
class Jet;


namespace D3PD {


/**
 * @brief Block filler tool for Jet EM Fraction.
 */
class JetEMFractionFillerTool
  : public BlockFillerTool<Jet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetEMFractionFillerTool (const std::string& type,
			   const std::string& name,
			   const IInterface* parent);
  

  /// Book variables for this block.
  virtual StatusCode book() final;


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const Jet& p) override;
  
  
private:
  /// Variable: Jet EM Fraction.
  float* m_emfrac;

  /// Pathological jet checking.
  bool m_checked;
  bool m_goodjets;
};


} // namespace D3PD


#endif // not JETD3PDMAKER_JETEMFRACTIONFILLERTOOL_H
