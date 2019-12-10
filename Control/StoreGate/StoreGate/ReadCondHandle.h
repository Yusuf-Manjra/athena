/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_READCONDHANDLE_H
#define STOREGATE_READCONDHANDLE_H 1

#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/IOVEntryT.h"
#include "AthenaKernel/ExtendedEventContext.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "CxxUtils/AthUnlikelyMacros.h"

#include "GaudiKernel/DataHandle.h"
#include "GaudiKernel/DataObjID.h"
#include "GaudiKernel/EventIDBase.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include <string>
#include <stdexcept>


namespace SG {

  template <typename T>
  class ReadCondHandle {

  public: 
    typedef T*               pointer_type; // FIXME: better handling of
    typedef const T*   const_pointer_type; //        qualified T type ?
    typedef T&             reference_type;
    typedef const T& const_reference_type;

  public:
    ReadCondHandle(const SG::ReadCondHandleKey<T>& key);
    ReadCondHandle(const SG::ReadCondHandleKey<T>& key, 
                   const EventContext& ctx);
    
    ~ReadCondHandle() {};
    
    const std::string& key() const { return m_hkey.key(); }
    const DataObjID& fullKey() const { return m_hkey.fullKey(); }

    const_pointer_type retrieve();
    const_pointer_type retrieve( const EventIDBase& t);

    const_pointer_type  operator->()  { return  retrieve(); }
    const_pointer_type  operator*()   { return  retrieve(); }
    const_pointer_type  cptr()        { return  retrieve(); }

    
    bool isValid();
    bool isValid(const EventIDBase& t) const ;

    bool range(EventIDRange& r);
    bool range(const EventIDBase& t, EventIDRange& r) const;
    const EventIDRange& getRange();
    
  private:

    bool initCondHandle();
        
    EventIDBase m_eid;
    CondCont<T>*  m_cc {nullptr};
    const T* m_obj { nullptr };
    const EventIDRange* m_range { nullptr };
    
    const SG::ReadCondHandleKey<T>& m_hkey;
  };


  //---------------------------------------------------------------------------

  template <typename T>
  ReadCondHandle<T>::ReadCondHandle(const SG::ReadCondHandleKey<T>& key):
  ReadCondHandle(key, Gaudi::Hive::currentContext())
  { 
  }

  //---------------------------------------------------------------------------

  template <typename T>
  ReadCondHandle<T>::ReadCondHandle(const SG::ReadCondHandleKey<T>& key,
                                    const EventContext& ctx):
    m_eid( ctx.eventID() ),
    m_cc( key.getCC() ),
    m_hkey(key)
  {
    try {
      EventIDBase::number_type conditionsRun =
        Atlas::getExtendedEventContext(ctx).conditionsRun();
      if (conditionsRun != EventIDBase::UNDEFNUM) {
        m_eid.set_run_number (conditionsRun);
      }
    }
    catch (const std::bad_any_cast& e) {
      MsgStream msg(Athena::getMessageSvc(), "ReadCondHandle");
      msg << MSG::ERROR
          << "EventContext extension is not "
          << (ctx.hasExtension() ? "of type Atlas::ExtendedEventContext" : "set")
          << endmsg;
      throw;
    }

    if (ATH_UNLIKELY(!key.isInit())) {
      MsgStream msg(Athena::getMessageSvc(), "ReadCondHandle");
      msg << MSG::ERROR 
          << "ReadCondHandleKey " << key.objKey() << " was not initialized"
          << endmsg;
      throw std::runtime_error("ReadCondHandle: ReadCondHandleKey was not initialized");
    }

    if (ATH_UNLIKELY(m_cc == 0)) {
      // try to retrieve it
      StoreGateSvc* cs = m_hkey.getCS();
      CondContBase *cb(nullptr);
      if (cs->retrieve(cb, m_hkey.key()).isFailure()) {
        MsgStream msg(Athena::getMessageSvc(), "ReadCondHandle");
        msg << MSG::ERROR
            << "can't retrieve " << m_hkey.fullKey() 
            << " via base class" << endmsg;
        throw std::runtime_error("ReadCondHandle: ptr to CondCont<T> is zero");
      } else {
        m_cc = dynamic_cast< CondCont<T>* > (cb);
        if (m_cc == 0) {
          MsgStream msg(Athena::getMessageSvc(), "ReadCondHandle");
          msg << MSG::ERROR
              << "can't dcast CondContBase to " << m_hkey.fullKey() 
              << endmsg;
          throw std::runtime_error("ReadCondHandle: ptr to CondCont<T> is zero");
        }
      }
          



      // if ( m_cs->retrieve(m_cc, SG::VarHandleKey::key()).isFailure() ) {
      //   MsgStream msg(Athena::getMessageSvc(), "ReadCondHandle");
      //   msg << MSG::ERROR
      //       << "ReadCondHandle : unable to retrieve ReadCondHandle of type"
      //       << Gaudi::DataHandle::fullKey() << " from "
      //       << StoreID::storeName(StoreID::CONDITION_STORE)
      //       << " with key " << SG::VarHandleKey::key()
      //       << endmsg;
      //   throw std::runtime_error("ReadCondHandle: ptr to CondCont<T> is zero");
      // }
    }
  
  }

  //---------------------------------------------------------------------------

  template <typename T>
  bool
  ReadCondHandle<T>::initCondHandle() {

    if (m_obj != 0) return true;

    if ( ATH_UNLIKELY(!m_cc->find(m_eid, m_obj, &m_range)) ) {
      std::ostringstream ost;
      m_cc->list(ost);
      MsgStream msg(Athena::getMessageSvc(), "ReadCondHandle");
      msg << MSG::ERROR 
          << "ReadCondHandle: could not find current EventTime " 
          << m_eid  << " for key " << m_hkey.objKey() << "\n"
          << ost.str()
          << endmsg;
      m_obj = nullptr;
      return false;
    }

    return true;
  }

  //---------------------------------------------------------------------------

  template <typename T>
  const T*
  ReadCondHandle<T>::retrieve() {
    
    if (m_obj == 0) {
      if (!initCondHandle()) {
      // std::ostringstream ost;
      // m_cc->list(ost);
      // MsgStream msg(Athena::getMessageSvc(), "ReadCondHandle");
      // msg << MSG::ERROR 
      //     << "ReadCondHandle::retrieve() could not find EventTime " 
      //     << m_eid  << " for key " << objKey() << "\n"
      //     << ost.str()
      //     << endmsg;
        return nullptr;
      }
    }

    return m_obj;
  }

  //---------------------------------------------------------------------------

  template <typename T>
  const T*
  ReadCondHandle<T>::retrieve(const EventIDBase& eid) {
    if (eid == m_eid) {
      return retrieve();
    }

    //    pointer_type obj(0);
    const_pointer_type cobj(0);
    if (! (m_cc->find(eid, cobj) ) ) {
      std::ostringstream ost;
      m_cc->list(ost);
      MsgStream msg(Athena::getMessageSvc(), "ReadCondHandle");
      msg << MSG::ERROR 
          << "ReadCondHandle::retrieve() could not find EventTime " 
          << eid  << " for key " << m_hkey.objKey() << "\n"
          << ost.str()
          << endmsg;
      return nullptr;
    }
  
    //    const_pointer_type cobj = const_cast<const_pointer_type>( obj );

    return cobj;
  }

  //---------------------------------------------------------------------------

  template <typename T>
  bool 
  ReadCondHandle<T>::isValid()  {

    return initCondHandle();
  }

  //---------------------------------------------------------------------------

  template <typename T>
  bool 
  ReadCondHandle<T>::isValid(const EventIDBase& t) const {

    return (m_cc->valid(t));
  }

  //---------------------------------------------------------------------------
  
  template <typename T>
  bool 
  ReadCondHandle<T>::range(EventIDRange& r) {

    if (m_obj == 0) {
      if (!initCondHandle()) {
        return false;
      }
    }

    if (m_range) {
      r = *m_range;
      return true;
    }
      
    return false;
  }

  template <typename T>
  const EventIDRange&
  ReadCondHandle<T>::getRange() {

    if (m_obj == 0) {
      if (!initCondHandle()) {
        throw std::runtime_error("ReadCondHandle: handle not initialized when doing getRange()");
      }
    }

    if (!m_range) {
        throw std::runtime_error("ReadCondHandle: range obj not set when doing getRange()");
    }
    return *m_range;

  }

  
  //---------------------------------------------------------------------------
  
  template <typename T>
  bool 
  ReadCondHandle<T>::range(const EventIDBase& eid, EventIDRange& r) const {
    
    return ( m_cc->range(eid, r) );
  }

}

#endif
  
