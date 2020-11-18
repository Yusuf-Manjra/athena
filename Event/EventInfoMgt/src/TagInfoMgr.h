//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFOMGT_TAGINFOMGR_H
# define EVENTINFOMGT_TAGINFOMGR_H
/**
 * @file TagInfoMgr.h
 *
 * @brief This is a Athena service which manages detector description
 *  tag information. It maintains a TagInfo object in the Detector
 *  Store with current tag values. 
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "EventInfoMgt/ITagInfoMgr.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaBaseComps/AthCnvSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/IOVRange.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "EventInfo/TagInfo.h"
#include "IOVDbMetaDataTools/IIOVDbMetaDataTool.h"

#include <map>

//<<<<<< PUBLIC TYPES                                                   >>>>>>

class StoreGateSvc;
class CondAttrListCollection;

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 * @class TagInfoMgr
 *
 * @brief This is a Athena service which manages detector description
 *  tag information. It maintains a TagInfo object in the Detector
 *  Store with current tag values. 
 *
 *  The tags to fill the TagInfo object may come from a variety of
 *  sources: i.e. the tags specified by the GeoModelSvc and IOVDbSvc,
 *  or the tags stored in the incoming EventInfo object, or the
 *  TagInfo object itself may be in the IOV DB. The TagInfoMgr itself
 *  only adds in tags from the incoming event. Others are added by
 *  external clients.
 *
 *  The interface provides a 'input' method to allow client to add in
 *  their own tags:
 *     
 *  virtual StatusCode addTag(const std::string& tagName, 
 *                            const std::string& tagValue) = 0;
 *
 *  The full set of tags in TagInfo are copied each event into
 *  EventInfo (specifically EventType) to be written out in the event
 *  stream. Currently, the tags are NOT written to the IOV DB via the
 *  TagInfoMgr. 
 *
 *  Some clients need to know when detector description tags change
 *  so that they may update their descriptions, such as the geometry
 *  description. These clients need to set up a callback with the
 *  IOVSvc, as is done below for this class. 
 *
 */
class TagInfoMgr : public ::AthCnvSvc,
                   virtual public ITagInfoMgr,
                   virtual public IIncidentListener,
		   virtual public IAddressProvider
{
public:

    /// Constructor with parameters:
    TagInfoMgr(const std::string &name,ISvcLocator *pSvcLocator);
    ~TagInfoMgr();

    /// @name Basic service methods
    //@{
    virtual StatusCode   initialize() override;
    virtual StatusCode   start() override;
    virtual StatusCode   finalize() override;
  
    /// Query the interfaces.
    virtual StatusCode   queryInterface( const InterfaceID& riid, void** ppvInterface ) override;
    //@}

    /// @name Interface methods
    //@{
    /// @name Method to allow clients add in tags as: tag name, tag value
    virtual StatusCode   addTag(const std::string& tagName, 
                              const std::string& tagValue) override;

    /// Method to allow clients to remove a tag which may have come in
    /// on the input
    virtual StatusCode   removeTagFromInput(const std::string& tagName) override;

    /// Method to allow clients to access the TagInfo object key.
    virtual std::string& tagInfoKey() override;

    /// callback from IOVSvc - only used as test of callback
    virtual StatusCode           checkTagInfo(IOVSVC_CALLBACK_ARGS) override;
    //@}

    /// Find tag by name, return by value
    virtual std::string findTag(const std::string & name) const override final;

    /// Return a vector with all current input tags
    virtual NameTagPairVec getInputTags() const override final;

    /// Dump the content of the current TagInfo to std::string for debug
    virtual std::string dumpTagInfoToStr() const override final;
   
    /// @name TagInfo management methods:
    //@{
    /// Callback at BeginRun and BeginEvent
    ///   - BeginRun:   fill and regsister TagInfo
    ///   - BeginEvent: fill EventInfo from TagInfo
    virtual void handle(const Incident& incident) override;

    typedef IAddressProvider::tadList tadList;
    typedef IAddressProvider::tadListIterator tadListIterator;
  
    /// preload the detector store with the transient address for
    /// TagInfo 
    virtual StatusCode preLoadAddresses( StoreID::type storeID,
					 tadList& tlist ) override;
      
    ///  Create a TagInfo object and record in storegate
    virtual StatusCode updateAddress(StoreID::type storeID, SG::TransientAddress* tad,
                                     const EventContext& ctx) override;


    /// Implementation of IConverter: Create the transient representation of an object from persistent state.
    /// @param pAddress [IN] pointer to IOpaqueAddress of the representation.
    /// @param refpObject [OUT] pointer to DataObject to be created.
    virtual StatusCode createObj(IOpaqueAddress* pAddress, DataObject*& refpObject) override;

   /// Create a Generic address using explicit arguments to identify a single object.
   /// @param svcType [IN] service type of the address.
   /// @param clid [IN] class id for the address.
   /// @param par [IN] string containing the database name.
   /// @param ip [IN] object identifier.
   /// @param refpAddress [OUT] converted address.
   virtual StatusCode createAddress(long svcType,
		   const CLID& clid,
		   const std::string* par,
		   const unsigned long* ip,
		   IOpaqueAddress*& refpAddress) override;

   /// Convert address to string form
   /// @param pAddress [IN] address to be converted.
   /// @param refAddress [OUT] converted string form.
   virtual StatusCode convertAddress(const IOpaqueAddress* pAddress, std::string& refAddress) override;

    /// Create address from string form
    /// @param svcType [IN] service type of the address.
    /// @param clid [IN] class id for the address.
    /// @param refAddress [IN] string form to be converted.
    /// @param refpAddress [OUT] converted address.
    virtual StatusCode createAddress(long svcType,
                                     const CLID& clid,
                                     const std::string& refAddress,
                                     IOpaqueAddress*& refpAddress) override;
    //@}

    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
private:
    /// Allow the factory class access to the constructor
    template <class TYPE> class SvcFactory;  /// forward declaration
    friend class SvcFactory<TagInfoMgr>;

    TagInfoMgr();
    TagInfoMgr(const TagInfoMgr&);
    TagInfoMgr &operator=(const TagInfoMgr&);
    StatusCode getRunNumber (unsigned int& runNumber);
  
    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:


    /// @name TagInfo management methods:
    //@{
    StatusCode fillTagInfo    (const CondAttrListCollection* tagInfoCond, TagInfo* tagInfo) const;
    StatusCode fillMetaData   (const TagInfo* tagInfo, const CondAttrListCollection* tagInfoCond);
    //@}

    /// Flag to add override the tags from EventInfo from other
    /// sources 
    Gaudi::Property<bool>   m_overrideEventInfoTags { this, "OverrideEventInfoTags", true, "Override tags yes/no" };

    /// Extra tags/values pairs added in my jobOptions
    Gaudi::Property<std::map<std::string,std::string> >
        m_extraTagValuePairs { this, "ExtraTagValuePairs", {}, "key/value pairs to be added", "mapMergeNoReplace<T,T>" };

    /// Extra tags/values pairs added in via interface
    std::map<std::string,std::string> m_extraTagValuePairsViaInterface;

    /// Extra tags to be removed
    std::set<std::string>          m_tagsToBeRemoved;

    /// The StoreGate key for the TagInfo
    Gaudi::Property<std::string>   m_tagInfoKey{ this, "TagInfoKey", "ProcessingTags", "SG key for TagInfo" };

    std::string                    m_tagInfoKeyValue;

    /// The event store
    ServiceHandle<StoreGateSvc>    m_storeGate { this, "StoreGateSvc", "StoreGateSvc" };

    /// The detector store
    ServiceHandle<StoreGateSvc>    m_detStore { this, "DetectorStore", "DetectorStore" };

    /// Access to IOVDbSvc interface - used to register callback
    ServiceHandle<IIOVDbSvc>       m_iovDbSvc { this, "IOVDbSvc", "IOVDbSvc" };

    /// Access to iov meta data tool
    ToolHandle<IIOVDbMetaDataTool> m_metaDataTool { this, "IOVDbMetaDataTool", "IOVDbMetaDataTool" };

    /// Flag to identify the first BeginRun incident
    bool                           m_isFirstBeginRun { true };

    /// conditionsRun from the first BeginRun incident (HLT)
    EventIDBase::number_type       m_conditionsRun { EventIDBase::UNDEFNUM };

    /// current run number from BeginRun incident
    EventIDBase::number_type       m_currentRun { EventIDBase::UNDEFNUM };

    /// IOVRange of last TagInfo added to the file meta data
    IOVRange                       m_lastIOVRange { IOVRange(IOVTime(), IOVTime()) };

    /// Last TagInfo added to the detector store
    TagInfo                        m_lastTagInfo;

};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // EVENTINFOMGT_TAGINFOMGR_H
