/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTAINERS_INTERNALOFFLINE_H
#define EVENTCONTAINERS_INTERNALOFFLINE_H
#include "EventContainers/I_InternalIDC.h"

namespace EventContainers{
/*
This class implements the IdentifiableContainer code for the offline case.
To avoid excess memory usage just the key-value pair is stored and can be directly
iterated upon.

Faster consecutive iteration, slower random access.
Prefer iteration over FindIndexPtr and indexFind
*/
class InternalOffline final : public I_InternalIDC {
public:
    InternalOffline(size_t max);
    virtual ~InternalOffline()=default;
    virtual InternalConstItr cbegin() const override;
    virtual InternalConstItr cend() const override;
    virtual InternalConstItr indexFind( IdentifierHash hashId ) const override;
    virtual const std::vector < hashPair >& GetAllHashPtrPair() const override;
    std::vector<std::pair<IdentifierHash::value_type, const void*>> m_map;
    size_t m_maximumSize;
    virtual bool tryAddFromCache(IdentifierHash hashId, EventContainers::IDC_WriteHandleBase &lock) override;
    virtual bool tryAddFromCache(IdentifierHash hashId) override;
    virtual void Wait() const override;
    virtual std::vector<IdentifierHash> GetAllCurrentHashes() const override;
    virtual size_t numberOfCollections() const override;
    void cleanUp(deleter_f* deleter) noexcept;
    virtual size_t fullSize() const noexcept override {return m_maximumSize;}
    virtual StatusCode fetchOrCreate(IdentifierHash hashId) override;
    virtual StatusCode fetchOrCreate(const std::vector<IdentifierHash> &hashIds) override;
    virtual bool insert(IdentifierHash hashId, const void* ptr) override;
    virtual const void* FindIndexPtr(IdentifierHash hashId) const noexcept override;
    virtual StatusCode addLock(IdentifierHash hashId, const void* ptr) override;
    virtual void* removeCollection( IdentifierHash hashId ) override;

};

}
#endif
