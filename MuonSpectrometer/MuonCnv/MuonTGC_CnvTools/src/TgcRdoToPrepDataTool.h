/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOL_H
#define MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOL_H
      
#include "CxxUtils/checker_macros.h"

#include "TgcRdoToPrepDataToolCore.h"

namespace Muon 
{
  /** @class TgcRdoToPrepDataTool 
   *  This is the algorithm that convert TGCRdo To TGCPrepdata as a tool.
   * 
   * @author Susumu Oda <Susumu.Oda@cern.ch> 
   * @author  Edward Moyse 
   * 
   * This class was developed by Takashi Kubota. 
   */  
  /// This class is only used in a single-thread mode
  class ATLAS_NOT_THREAD_SAFE TgcRdoToPrepDataTool : virtual public TgcRdoToPrepDataToolCore
  {
    public:
      /** Constructor */
      TgcRdoToPrepDataTool(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Destructor */
      virtual ~TgcRdoToPrepDataTool()=default;
      
      /** Standard AthAlgTool initialize method */
      virtual StatusCode initialize() override;
    
      using TgcRdoToPrepDataToolCore::decode; // To prevent the decode below from hiding the superclass decode methods
      virtual StatusCode decode(std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& idWithDataVect) override;

      virtual void printPrepData() override;

    private:
      /** Vector of RDO collections already decoded in the event */
      std::vector<const TgcRdo*> m_decodedRdoCollVec;

      /** Flag to check the ROB with an onlineId is already decoded */  
      std::vector<bool> m_decodedOnlineId;  

      /** Containers being recorded */
      State m_state;

      /** Switch for keeping Track of FullEvent Decoding */
      bool m_fullEventDone[NBC+1] = {false};
   }; 
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOL_H
