/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaROOTAccess/src/RootAuxVectorFactory.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Dynamic implementation of @c IAuxVectorFactory,
 *        relying on root vector proxy.
 */


#include "RootAuxVectorFactory.h"
#include "AthContainers/tools/error.h"
#include "TClass.h"
#include "TVirtualCollectionProxy.h"
#include "TROOT.h"
#include <iostream>
#include <stdexcept>


namespace AthenaROOTAccess {


/**
 * @brief Find the vector type associated with @c CL by looking
 *        up the @c vector_type typedef.
 * @param cl The class for which to find the associated vector type.
 *
 * Given type CL, looks for a typedef CL::vector_type and returns the TClass
 * for that if found.  Works for, eg, SG::PackedContainer.
 */
TClass* lookupVectorType (TClass *cl)
{
  std::string tname = cl->GetName();
  tname += "::vector_type";
  TDataType* typ = gROOT->GetType (tname.c_str());
  if (typ)
    return TClass::GetClass (typ->GetFullTypeName());
  return nullptr;
}


/**
 * @brief Constructor.  Makes a new vector.
 * @param factory The factory object for this type.
 * @param size Initial size of the new vector.
 * @param capacity Initial capacity of the new vector.
 */
RootAuxVector::RootAuxVector (const RootAuxVectorFactory* factory,
                              size_t size, size_t /*capacity*/)
  : m_factory (factory)
{
  TClass* vecClass = factory->vecClass();
  m_proxy = vecClass->GetCollectionProxy();
  m_obj = factory->objClass()->New ();
  m_vec = reinterpret_cast<char*> (m_obj) + factory->offset();
  this->resize (size);
}


/**
 * @brief Copy constructor.
 * @param other The vector to copy.
 */
RootAuxVector::RootAuxVector (const RootAuxVector& other)
  : m_factory (other.m_factory),
    m_proxy (other.m_proxy)
{
  m_obj = m_factory->objClass()->New ();
  m_vec = reinterpret_cast<char*> (m_obj) + m_factory->offset();
  size_t sz = other.size();
  this->resize (sz);

  if (sz > 0) {
    const RootUtils::Type& rootType = m_factory->rootType();

    const void* otherPtr = 0;
    {
      TVirtualCollectionProxy::TPushPop bind (m_proxy, other.m_vec);
      otherPtr = m_proxy->At(0);
    }
    
    rootType.copyRange (this->toPtr(), otherPtr, sz);
  }
}


/**
 * @brief Destructor.
 *
 * This will free the vector data.
 */
RootAuxVector::~RootAuxVector()
{
  m_factory->objClass()->Destructor (m_obj);
}


/**
 * @brief Make a copy of this vector.
 */
SG::IAuxTypeVector* RootAuxVector::clone() const
{
  return new RootAuxVector (*this);
}


/**
 * @brief Return a pointer to the start of the vector's data.
 */
void* RootAuxVector::toPtr ()
{
  TVirtualCollectionProxy::TPushPop bind (m_proxy, m_vec);
  if (m_proxy->Size() == 0)
    return 0;
  return m_proxy->At(0);
}


/**
 * @brief Return a pointer to the overall object.
 */
void* RootAuxVector::toVector ()
{
  return m_obj;
}


/**
 * @brief Return the size of the vector.
 */
size_t RootAuxVector::size() const
{
  TVirtualCollectionProxy::TPushPop bind (m_proxy, m_vec);
  return m_proxy->Size();
}


/**
 * @brief Change the size of the vector.
 * @param sz The new vector size.
 */
void RootAuxVector::resize (size_t sz)
{
  TVirtualCollectionProxy::TPushPop bind (m_proxy, m_vec);
  m_proxy->Allocate(sz, false);
}


/**
 * @brief Change the capacity of the vector.
 * @param sz The new vector capacity.
 */
void RootAuxVector::reserve (size_t /*sz*/)
{
}


/**
 * @brief Shift the elements of the vector.
 * @param pos The starting index for the shift.
 * @param offs The (signed) amount of the shift.
 *
 * This operation shifts the elements in the vectors for all
 * aux data items, to implement an insertion or deletion.
 * @c offs may be either positive or negative.
 *
 * If @c offs is positive, then the container is growing.
 * The container size should be increased by @c offs,
 * the element at @c pos moved to @c pos + @c offs,
 * and similarly for following elements.
 * The elements between @c pos and @c pos + @c offs should
 * be default-initialized.
 *
 * If @c offs is negative, then the container is shrinking.
 * The element at @c pos should be moved to @c pos + @c offs,
 * and similarly for following elements.
 * The container should then be shrunk by @c -offs elements
 * (running destructors as appropriate).
 */
void RootAuxVector::shift (size_t pos, ptrdiff_t offs)
{
  TVirtualCollectionProxy::TPushPop bind (m_proxy, m_vec);
  size_t eltsz = m_proxy->GetIncrement();

  const RootUtils::Type& rootType = m_factory->rootType();

  if (offs < 0) {
    if (-offs > static_cast<ptrdiff_t>(pos)) offs = -pos;
    char* beg = reinterpret_cast<char*>(m_proxy->At(0));
    rootType.copyRange (beg + eltsz*(pos+offs),
                        beg + eltsz*pos,
                        m_proxy->Size() - pos);
    m_proxy->Allocate (m_proxy->Size() + offs, false);
  }
  else if (offs > 0) {
    size_t oldsz = m_proxy->Size();
    m_proxy->Allocate (oldsz + offs, false);
    char* beg = reinterpret_cast<char*>(m_proxy->At(0));
    rootType.copyRange (beg + eltsz*(pos+offs),
                        beg + eltsz*pos,
                        m_proxy->Size() - pos - offs);
    rootType.clearRange (beg + eltsz*pos, offs);
  }
}


/**
 * @brief Return the type of the complete object to be saved.
 *
 * For example, if the object is a @c std::vector, then we return
 * the @c type_info of the vector.  But if we're holding
 * a @c PackedContainer, then we return the @c type_info of the
 * @c PackedContainer.
 *
 * Can return null if the operation is not supported.  In that case,
 * I/O will use the type found from the variable registry.
 */
const std::type_info* RootAuxVector::objType() const
{
  return m_factory->objClass()->GetTypeInfo();
}


//==================================================================


/**
 * @brief Constructor.
 * @param vecClass The @c TClass for the vector object.
 */
RootAuxVectorFactory::RootAuxVectorFactory (TClass* objClass)
  : m_objClass (objClass),
    m_vecClass (objClass),
    m_offset (0)
{
  TVirtualCollectionProxy* proxy = m_vecClass->GetCollectionProxy();

  if (!proxy) {
    TClass* vecClass = lookupVectorType (objClass);
    if (vecClass) {
      m_vecClass = vecClass;
      Int_t offs = objClass->GetBaseClassOffset (vecClass);
      if (offs >= 0) {
        m_offset = offs;
        proxy = vecClass->GetCollectionProxy();
      }
      else {
        ATHCONTAINERS_ERROR("RootAuxVectorFactory::RootAuxVectorFactory",
                            std::string("Can't find vector base class in ") +
                            objClass->GetName());
      }
    }
  }

  if (!proxy) {
    std::string err = "Can't find collection proxy for ";
    err += m_vecClass->GetName();
    throw std::runtime_error (err.c_str());
  }

  if (m_vecClass->GetTypeInfo() == 0) {
    ATHCONTAINERS_ERROR("RootAuxVectorFactory::RootAuxVectorFactory",
                        std::string("No type_info available for class ") +
                        m_vecClass->GetName() +
                        std::string(".  There is probably a missing dictionary.  We will likely crash further on."));
  }

  TClass* eltClass = proxy->GetValueClass();
  if (eltClass)
    m_type.init (eltClass);
  else
    m_type.init (proxy->GetType());
}


/**
 * @brief Destructor.
 */
RootAuxVectorFactory::~RootAuxVectorFactory()
{
}


/**
 * @brief Create a vector object of this type.
 * @param size Initial size of the new vector.
 * @param capacity Initial capacity of the new vector.
 */
SG::IAuxTypeVector*
RootAuxVectorFactory::create (size_t size, size_t capacity) const
{
  return new RootAuxVector (this, size, capacity);
}


/**
 * @brief Copy an element between vectors.
 * @param dst Pointer to the start of the destination vector's data.
 * @param dst_index Index of destination element in the vector.
 * @param src Pointer to the start of the source vector's data.
 * @param src_index Index of source element in the vector.
 *
 * @c dst and @ src can be either the same or different.
 */
void RootAuxVectorFactory::copy (void* dst,        size_t dst_index,
                                 const void* src,  size_t src_index) const
{
  m_type.assign (dst, dst_index, src, src_index);
}


/**
 * @brief Swap an element between vectors.
 * @param a Pointer to the start of the first vector's data.
 * @param aindex Index of the element in the first vector.
 * @param b Pointer to the start of the second vector's data.
 * @param bindex Index of the element in the second vector.
 *
 * @c a and @ b can be either the same or different.
 */
void RootAuxVectorFactory::swap (void* a, size_t aindex,
                                 void* b, size_t bindex) const
{
  m_type.swap (a, aindex, b, bindex);
}


/**
 * @brief Clear an element within a vector (static method).
 * @param dst Pointer to the start of the vector's data.
 * @param dst_index Index of the element in the vector.
 */
void RootAuxVectorFactory::clear (void* dst, size_t dst_index) const
{
  m_type.clear (dst, dst_index);
}


/**
 * @brief Return the size of an element of this vector type.
 */
size_t RootAuxVectorFactory::getEltSize() const
{
  return m_type.getSize();
}


/**
 * @brief Return the @c type_info of the overall object.
 */
const std::type_info* RootAuxVectorFactory::tiVec() const
{
  return m_objClass->GetTypeInfo();
}


/**
 * @brief True if the vectors created by this factory work by dynamic
 *        emulation (via @c TVirtualCollectionProxy or similar); false
 *        if the std::vector code is used directly.
 */
bool RootAuxVectorFactory::isDynamic() const
{
  return true;
}


} // namespace AthenaROOTAccess
