/// emacs: this is -*- c++ -*-
///
///   @class RegSelTool RegSelTool.h
/// 
///          This is the Region Selector tool for the ID And muon spectrometer 
///          tables
///     
///   @author Mark Sutton
///
///   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
///

#ifndef REGIONSELECTOR_REGSELTOOL_H
#define REGIONSELECTOR_REGSELTOOL_H

// interface includes
#include "IRegionSelector/IRegSelTool.h"
#include "IRegionSelector/IRoiDescriptor.h"

// spam
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include <string>
#include <iostream>
#include <vector>
#include <cstdint>

#include "IRegionSelector/RegSelLUTCondData.h"

class RegSelModule;
class RegSelSiLUT;
class IInterface;


class RegSelTool : public extends<AthAlgTool, IRegSelTool> {

public:

  /** @c Standard constructor for tool (obviously).
   */
  RegSelTool( const std::string& type, const std::string& name, const IInterface* parent );

  //! Destructor.
  virtual ~RegSelTool() override;


  //! @method initialize, loads lookup tables for retrieve %Identifier %Hash and ROBID 
  virtual StatusCode initialize() override;

  //! @method finalize, deletes lookup table from memory
  virtual StatusCode finalize() override;
  
  /// IRegSlTool interface ...

  // Interface inherited from IRegSelTool service

  virtual
  void HashIDList( const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) const override;

  virtual
  void HashIDList( long layer, const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist) const override;

  virtual
  void ROBIDList( const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const override;

  virtual
  void ROBIDList( long layer, const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const override;

   
protected:

  // full scan
  virtual 
  void HashIDList( std::vector<IdentifierHash>& idlist ) const;  

  // full scan for a specific layer
  virtual 
  void HashIDList( long layer, std::vector<IdentifierHash>& idlist ) const;
     
  // Methods to obtain the rob id list

  // full scan
  void ROBIDList( std::vector<uint32_t>& roblist ) const;

  // full scan by layer
  void ROBIDList( long layer, std::vector<uint32_t>& roblist ) const;

  // get list of modules  
  void getRoIData( const IRoiDescriptor& roi, std::vector<const RegSelModule*>& modulelist ) const;

  //! @method lookup, actually retrieve the lookup table as conditions data - might combine with handle()
  const RegSelSiLUT* lookup() const;

protected:

  void cleanup( std::vector<IdentifierHash>& idvec ) const;
  
private:

  //! Flag to determine whether it has yet been initialised
  bool              m_initialised; 

  std::string       m_tableName;

  //! Flag to dump loaded table in data file.
  BooleanProperty  m_dumpTable;

  SG::ReadCondHandleKey<RegSelLUTCondData> m_tableKey{ this, "RegSelLUT", "Tool_Not_Initalised", "Region Selector lookup table" };

  /// flag to avoid the need for a separate rpc lookup table class
  /// FixMe: this flag may be replaced by custom derived RegSelTool 
  ///        class for the RPC. The tools are retrieved only via  
  ///        the IRegSelTool interface so this would also be 
  ///        transaprent to the user

  bool m_rpcflag; 
  
};

#endif // REGIONSELECTOR_REGSELTOOL_H
