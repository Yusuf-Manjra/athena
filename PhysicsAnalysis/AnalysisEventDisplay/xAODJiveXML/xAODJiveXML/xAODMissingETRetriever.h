/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJIVEXML_xAODMISSINGETRETRIEVER_H
#define XAODJIVEXML_xAODMISSINGETRETRIEVER_H

#include <string>
#include <vector>
#include <map>

#include "JiveXML/IDataRetriever.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "xAODMissingET/MissingETContainer.h" 

namespace JiveXML{
  
  /**
   * @class xAODMissingETRetriever
   * @brief Retrieves all @c MissingET @c objects (MissingETAODCollection etc.)
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
  class xAODMissingETRetriever : virtual public IDataRetriever,
                                   public AthAlgTool {
    
    public:
      
      /// Standard Constructor
      xAODMissingETRetriever(const std::string& type,const std::string& name,const IInterface* parent);
      
      /// Retrieve all the data
      virtual StatusCode retrieve(ToolHandle<IFormatTool> &FormatTool); 
      const DataMap getData(const xAOD::MissingETContainer*);
    
      /// Return the name of the data type
      virtual std::string dataTypeName() const { return m_typeName; };

    private:
      ///The data type that is generated by this retriever
      const std::string m_typeName;

      std::string m_sgKeyFavourite;
      std::vector<std::string> m_otherKeys;
  };
}
#endif // not XAODJIVEXML_xAODMISSINGETRETRIEVER_H
