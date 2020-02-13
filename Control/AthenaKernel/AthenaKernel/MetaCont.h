/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVSVC_METACONT_H
#define IOVSVC_METACONT_H 1

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <typeinfo>
#include <mutex>
#include <typeinfo>

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/DataObjID.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/SourceID.h"
#include "AthenaKernel/DataBucketTraitFwd.h"
#include "AthenaKernel/MetaContDataBucket.h"

namespace SG {
  class DataProxy;
  template<class T>
  class ReadMetaHandle;
}

class MetaContBase {
//typedef Guid SourceID;
public:
  typedef SG::SourceID SourceID;
  MetaContBase() {};
  virtual ~MetaContBase(){};

  //  virtual void list() const { std::cout << "CCBase" << std::endl; }
  virtual void list(std::ostringstream&) const = 0;
  virtual int entries() const { return 0; }
  virtual bool valid( const SourceID& t) const = 0;
  virtual const DataObjID& id() const = 0;
  virtual bool insert(const SourceID& sid, DataObject* obj) = 0;
  virtual bool insert(const SourceID& sid, void* obj) = 0;

  virtual SG::DataProxy* proxy() = 0;
  virtual void setProxy(SG::DataProxy*) = 0;

  virtual std::vector<SourceID> sources() const = 0;

private:
};

///////////////////////////////////////////////////////////////////////////

template <typename T>
class MetaCont: public MetaContBase {
public:

  typedef T Payload_t;

  friend SG::ReadMetaHandle<T>;

  MetaCont(const DataObjID& id, SG::DataProxy* prx=nullptr):
    m_id(id), m_proxy(prx) {};
  ~MetaCont();

  bool insert(const SourceID& r, DataObject* t);
  bool insert(const SourceID& r, void* t);
  bool insert(const SourceID& r, T* t);

  bool find(const SourceID& t, T*&) const;

  virtual void list(std::ostringstream&) const;
  virtual int entries() const;

  virtual bool valid( const SourceID& t ) const;

  virtual const DataObjID& id() const { return m_id; }

  virtual SG::DataProxy* proxy() { return m_proxy; }
  virtual void setProxy(SG::DataProxy* prx) { m_proxy = prx; }

  virtual std::vector<SourceID> sources() const;

private:

  mutable std::mutex m_mut;

  typedef std::map<SourceID,T*> MetaContSet;
  MetaContSet m_metaSet;

  DataObjID m_id;

  SG::DataProxy* m_proxy;

};


namespace SG {


/**
 * @brief Metafunction to find the proper @c DataBucket class for @c T.
 *
 * Specialize this for @c MetaCont.
 * See AthenaKernel/StorableConversions.h for an explanation.
 */
template <class T, class U>
struct DataBucketTrait<MetaCont<T>, U>
{
  typedef MetaContDataBucket<U> type;
  static void init() {}
};


} // namespace SG


//
///////////////////////////////////////////////////////////////////////////
//

template <typename T>
MetaCont<T>::~MetaCont<T>() {
  for (auto itr=m_metaSet.begin(); itr != m_metaSet.end(); ++itr) {
    delete itr->second;
  }
  m_metaSet.clear();
}    

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool MetaCont<T>::insert(const SourceID& r, void* obj) {
  T* t = static_cast<T*>(obj);

  return insert(r, t);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool MetaCont<T>::insert(const SourceID& r, DataObject* obj) {
  T* t = dynamic_cast<T*>( obj );

  if (t == 0) {
    std::cerr << "MetaCont<>T unable to dcast from DataObject to " 
              << typeid(T).name()
              << std::endl;
    return false;
  }

  return insert(r, t);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool MetaCont<T>::insert(const SourceID& r, T* t) {

  std::lock_guard<std::mutex> lock(m_mut);

  return m_metaSet.insert(std::make_pair(r,t)).second;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool MetaCont<T>::valid(const SourceID& it) const {
  typename MetaContSet::const_iterator itr;
  
  std::lock_guard<std::mutex> lock(m_mut);

  return m_metaSet.find(it)!=m_metaSet.end();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool MetaCont<T>::find(const SourceID& it, T*& t) const {
  typename MetaContSet::const_iterator itr;
  
  std::lock_guard<std::mutex> lock(m_mut);

  //std::cerr << "MetaCont find SID " << it << std::endl;

  itr = m_metaSet.find(it);
  if (itr != m_metaSet.end()) {
    t=itr->second;
    return true;
  }

  return false;
}

    
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
void MetaCont<T>::list(std::ostringstream& ost) const {
  std::lock_guard<std::mutex> lock(m_mut);

  ost << "id: " << m_id << "  proxy: " << m_proxy << std::endl;

  ost << "Meta: [" << m_metaSet.size() << "]" << std::endl;
  //auto itr = m_metaSet.begin();
  //for (; itr != m_metaSet.end(); ++itr) {
  //  ost << *(itr->second) << std::endl;
  //}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
int MetaCont<T>::entries() const {
  std::lock_guard<std::mutex> lock(m_mut);
  return m_metaSet.size();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
std::vector<MetaContBase::SourceID> 
MetaCont<T>::sources() const {
  std::lock_guard<std::mutex> lock(m_mut);

  std::vector<MetaContBase::SourceID> r;
  for (auto ent : m_metaSet) {
    r.push_back(ent.first);
  }

  return r;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( MetaContBase , 34480469 , 1 )


/// Declare a metadata container along with its CLID:
//    METACONT_DEF(TYPE, CLID);
//
#define METACONT_DEF(T, CLID) \
  CLASS_DEF( MetaCont<T>, CLID, 1 )             \
  SG_BASE( MetaCont<T>, MetaContBase )

#endif

