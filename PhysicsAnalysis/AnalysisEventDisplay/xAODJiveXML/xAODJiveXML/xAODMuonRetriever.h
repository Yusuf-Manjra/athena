/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_xAODMUONRETRIEVER_H
#define JIVEXML_xAODMUONRETRIEVER_H

#include <string>
#include <vector>
#include <map>

#include "JiveXML/IDataRetriever.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "xAODMuon/MuonContainer.h" 

namespace JiveXML{
  
  /**
   * @class xAODMuonRetriever
   * @brief Retrieves all @c Muon @c objects (MuonAODCollection etc.)
   *
   *  - @b Properties
   *    - StoreGateKey: First collection to be retrieved, displayed
   *      in Atlantis without switching. All other collections are 
   *      also retrieved.
   *
   *  - @b Retrieved @b Data
   *    - Usual four-vectors: phi, eta, et etc.
   *    - Associations for clusters and tracks via ElementLink: key/index scheme
   */
  class xAODMuonRetriever : virtual public IDataRetriever,
                                   public AthAlgTool {
    
    public:
      
      /// Standard Constructor
      xAODMuonRetriever(const std::string& type,const std::string& name,const IInterface* parent);
      
      /// Retrieve all the data
      virtual StatusCode retrieve(ToolHandle<IFormatTool> &FormatTool); 
      const DataMap getData(const xAOD::MuonContainer*);
    
      /// Return the name of the data type
      virtual std::string dataTypeName() const { return m_typeName; };

    private:
      ///The data type that is generated by this retriever
      const std::string m_typeName;

      std::string m_sgKey;
      std::vector<std::string> m_otherKeys;
  };
}
#endif
