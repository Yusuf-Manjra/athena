/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_ISCTRAWDATAPROVIDERTOOL_H
#define SCT_RAWDATABYTESTREAMCNV_ISCTRAWDATAPROVIDERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "ByteStreamData/RawEvent.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/InDetTimeCollection.h"
#include "InDetByteStreamErrors/InDetBSErrContainer.h"
#include "SCT_ConditionsData/SCT_ByteStreamFractionContainer.h"

#include <set>
#include <string>


class InterfaceID;

class ISCTRawDataProviderTool : virtual public IAlgTool
{

 public:
   
  //! AlgTool InterfaceID
  static const InterfaceID& interfaceID( ) ;
  

  //! destructor 
  virtual ~ISCTRawDataProviderTool() {};

  //! this is the main decoding method
  virtual StatusCode convert( std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>&,
                              ISCT_RDO_Container&,
                              InDetBSErrContainer* errs,
                              SCT_ByteStreamFractionContainer* bsFracCont) = 0;

  //Replace the incident calls with private calls, more MT friendly
  virtual void BeginNewEvent() = 0;
};


inline const InterfaceID& ISCTRawDataProviderTool::interfaceID( )
{ 
  static const InterfaceID ISCTRawDataProviderToolIID("ISCTRawDataProviderTool", 1, 0);
  return ISCTRawDataProviderToolIID; 
}


#endif



