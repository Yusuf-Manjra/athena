/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "StorageSvc/DbHeap.h"
#include "StorageSvc/DbObject.h"
#include "StorageSvc/DbContainer.h"
#include "StorageSvc/DbObjectGuard.h"

#include <memory>
#include <cassert>

using namespace pool;

static const size_t GUARDSIZE = 72;

inline static DbObjectGuard* GUARD(void* ptr)  {
  char* cptr = static_cast<char*>(ptr);
  return reinterpret_cast<DbObjectGuard*> (cptr-GUARDSIZE);
}

inline static const DbObjectGuard* GUARD(const void* ptr)  {
  const char* cptr = static_cast<const char*>(ptr);
  return reinterpret_cast<const DbObjectGuard*> (cptr-GUARDSIZE);
}

/// Standard Constructor
DbHeap::DbHeap() {
}

/// Standard Destructor
DbHeap::~DbHeap() {
}

/// Access the object guard size
size_t DbHeap::guardSize() {
  return GUARDSIZE;
}

/// Number of chunks currently allocated from the heap
Token::OID_t& DbHeap::oid(DbObject* pObj)  {
  return GUARD(pObj)->oid();
}

/// Number of chunks currently allocated from the heap
const Token::OID_t& DbHeap::oid(const DbObject* pObj)  {
  return GUARD(pObj)->oid();
}

/// Number of chunks currently allocated from the heap
DbContainer& DbHeap::container(DbObject* pObj)  {
  return GUARD(pObj)->container();
}

/// Number of chunks currently allocated from the heap
const DbContainer& DbHeap::container(const DbObject* pObj)  {
  return GUARD(pObj)->container();
}

/// Free an object, which was initially allocated from the heap
DbStatus DbHeap::free(void* ptr, DbContainer* /* cntH */ ) {
  if ( ptr )  {
    DbObjectGuard* g = GUARD(ptr);
    g->~DbObjectGuard();
    ::operator delete(g);
    return Success;
  }
  return Error;
}

/// Access the container of an object
DbObject* DbHeap::allocate( size_t                    size, 
                            DbContainer*              cntH,
                            const Token::OID_t*       lnkH,
                            DbObjectHandle<DbObject>* objH)
{
  assert(sizeof(DbObjectGuard)<=GUARDSIZE);
  char* ptr = (char*)::operator new(size+GUARDSIZE);
  DbObjectGuard* g = ::new(ptr) DbObjectGuard(0, size+GUARDSIZE);
  ptr += GUARDSIZE;
  DbObject* pObj = (DbObject*)ptr;
  if ( lnkH ) g->oid()       = *lnkH;
  if ( cntH ) g->container() = *cntH;
  if ( objH ) objH->_setObject(pObj);
  return pObj;
}
