/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVDBDATAMODEL_IOVMETADATACONTAINER_H
#define IOVDBDATAMODEL_IOVMETADATACONTAINER_H

/**
 * @file IOVMetaDataContainer.h
 *
 * @brief This class is a container for conditions data. It is
 * intended to be used to store conditions data from COOL as file meta
 * data.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Antoine Perus <perus@lal.in2p3.fr>
 *
 * $Id: IOVMetaDataContainer.h,v 1.4 2007-07-23 12:40:37 schaffer Exp $
 */

#include "GaudiKernel/ClassID.h"
#include "IOVDbDataModel/IOVPayloadContainer.h"

#include <string>
#include <vector>
#include <sstream>

class  CondAttrListCollection;

/**
 * @class IOVMetaDataContainer
 *
 * @brief This class is a container for conditions data. It is
 * intended to be used to store conditions data from COOL as file metadata.
 */

class IOVMetaDataContainer {
 public:

  /// \name structors
  //@{
  IOVMetaDataContainer();
  IOVMetaDataContainer(const std::string& folderName
		       , const std::string& folderDescription);
  IOVMetaDataContainer(const IOVMetaDataContainer& cont);
  IOVMetaDataContainer& operator=(const IOVMetaDataContainer& cont);
  ~IOVMetaDataContainer();
  //@}

  /// \name Metadata information accessors
  //@{

  /// Folder name
  const std::string&          folderName        () const;

  /// Folder description
  const std::string&          folderDescription () const;

  /// Access to payload container
  const IOVPayloadContainer*  payloadContainer  () const;

  /// \name Metadata information setting
  //@{
  /// Set folder name
  void setFolderName                            (const std::string& folderName);

  /// Set description
  void setFolderDescription                     (const std::string& description);

  /// Add in new payload. Note that duplicates are NOT added.
  /// @return value is true is payload was merged, false if it is a duplicate
  bool merge                                    (CondAttrListCollection* payload);

  //@}

  /// Dump the content
  void dump(std::ostringstream& stream) const;

 private:
  friend class IOVMetaDataContainerCnv_p1;
  friend class IOVMetaDataContainerPTCnv_p1;

  std::string           m_folderName;
  std::string           m_folderDescription;
  IOVPayloadContainer*  m_payload;
};


inline 
IOVMetaDataContainer::IOVMetaDataContainer()
		     : m_payload(new IOVPayloadContainer())
{
}

inline 
IOVMetaDataContainer::IOVMetaDataContainer(const std::string& folderName
					   , const std::string& folderDescription)
		     : m_folderName(folderName)
		     , m_folderDescription(folderDescription)
		     , m_payload(new IOVPayloadContainer())
{
}

inline
IOVMetaDataContainer::IOVMetaDataContainer(const IOVMetaDataContainer& cont)
		     : m_folderName(cont.m_folderName)
 		     , m_folderDescription(cont.m_folderDescription)
 		     , m_payload(new IOVPayloadContainer(*(cont.m_payload)))
{
}

inline
IOVMetaDataContainer&
IOVMetaDataContainer::operator=(const IOVMetaDataContainer& cont)
{
  if (this != &cont) {
    delete m_payload;
    m_payload = new IOVPayloadContainer(*(cont.m_payload));
    m_folderName = cont.m_folderName;
    m_folderDescription= cont.m_folderDescription;
  }
  return *this;
}

inline 
const std::string&
IOVMetaDataContainer::folderName        () const
{
  return (m_folderName);
}
 
inline 
const std::string&  
IOVMetaDataContainer::folderDescription () const
{
  return (m_folderDescription);
}

inline 
const IOVPayloadContainer*
IOVMetaDataContainer::payloadContainer    () const
{
  return (m_payload);
}

inline 
void 
IOVMetaDataContainer::setFolderName       (const std::string& folderName)
{
  m_folderName = folderName;
}
    
inline 
void
IOVMetaDataContainer::setFolderDescription(const std::string& description)
{
    m_folderDescription = description;
}

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( IOVMetaDataContainer , 1316383046 , 1 )
#include "AthenaKernel/MetaCont.h"
CLASS_DEF( MetaCont<IOVMetaDataContainer> , 1158158193 , 1 )
#include "AthenaKernel/BaseInfo.h"
SG_BASE( MetaCont<IOVMetaDataContainer>, MetaContBase );

#endif  // 
