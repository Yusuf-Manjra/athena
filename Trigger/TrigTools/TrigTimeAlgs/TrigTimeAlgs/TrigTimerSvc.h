// this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigTimerSvc.h,v 1.10 2009-04-15 14:00:22 tbold Exp $
// $Name: not supported by cvs2svn $
#ifndef TrigTIMERSVC_H_
#define TrigTIMERSVC_H_
#include <vector>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IInterface.h"
#include "AthenaBaseComps/AthService.h"
#include "TrigTimeAlgs/ITrigTimerSvc.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "boost/regex.hpp"

/**
 * @brief This is class implementing interface defined by ITrigTimerSvc
 *
 * In addition to basic functionality it has some configuration options.
 * They are: IncludeName and ExcludeName which are deciding which timers 
 * given with addTimer are in dummy state. This properties are plain regular expressions.
 */

class TrigTimerSvc : public AthService, virtual public ITrigTimerSvc {
public:

  TrigTimerSvc( const std::string& name, ISvcLocator* svcloc );
  ~TrigTimerSvc();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
  virtual StatusCode initialize ( );
  virtual StatusCode finalize ( );

  /**
   * \brief adds new timer of name given as arg
   * \param name TrigTimer instance name
   */
  virtual TrigTimer* addItem(const std::string &name);

  /**
   * \brief begin iterator of vector of all active timers 
   * inactive are inaccessible outside
   */
  virtual std::vector < TrigTimer* >::iterator begin( void );

  /**
   * \brief next-to-end iterator of vector of all active timers 
   * inactive are inaccessible outside
   */
  virtual std::vector < TrigTimer* >::iterator end( void );

  /**
   * \brief pretty print routine
   */
  virtual void print( );

  //  inline TrigTimer* TrigTimerSvc::findItem(const std::string& name);
  
private:
  std::vector< TrigTimer* > m_itemList;   //!< list of active timers
  std::vector< TrigTimer* > m_allTimers;  //!< this list contains ptrs to all timers including inactive

  std::string m_includeName;     //!<  selection of allowed timers
  std::string m_excludeName;     //!< selection of disallowed timers (just to ease usage)
  boost::regex m_includeRegex;   //!< compiled BOOST regex structures for those accepted
  boost::regex m_excludeRegex;   //!< and those rejected
  
  bool m_dumpFinalStatistics;

};


#endif // TrigTIMER_H

