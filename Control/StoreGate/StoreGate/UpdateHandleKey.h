// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file StoreGate/UpdateHandleKey.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Property holding a SG store/key/clid from which an UpdateHandle is made.
 */


#ifndef STOREGATE_UPDATEHANDLEKEY_H
#define STOREGATE_UPDATEHANDLEKEY_H


#include "StoreGate/VarHandleKey.h"
#include "AthenaKernel/CLASS_DEF.h"


namespace SG {


/**
 * @brief Property holding a SG store/key/clid from which an UpdateHandle is made.
 *
 * This class holds the key part of an UpdateHandle.  For a reentrant algorithm,
 * you would use this as the algorithm property and construct the actual
 * ReadHandle on the stack from this key object (and optionally the event
 * context).
 *
 * See VarHandleKey for more details.
 */
template <class T>
class UpdateHandleKey
  : public VarHandleKey
{
public:
  /**
   * @brief Constructor.
   * @param key The StoreGate key for the object.
   * @param storeName Name to use for the store, if it's not encoded in sgkey.
   *
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present
   * the store named by @c storeName is used.
   */
  UpdateHandleKey (const std::string& key = "",
                   const std::string& storeName = StoreID::storeName(StoreID::EVENT_STORE));

  /**
   * @brief auto-declaring Property Constructor.
   * @param owner Owning component.
   * @param name name of the Property
   * @param key  default StoreGate key for the object.
   * @param doc Documentation string.
   *
   * will associate the named Property with this WHK via declareProperty
   *
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present
   * the store named by @c storeName is used.
   */
  template <class OWNER,
            typename = typename std::enable_if<std::is_base_of<IProperty, OWNER>::value>::type>
  inline UpdateHandleKey (OWNER* owner,
                         std::string name,
                         const std::string& key={},
                         std::string doc="");
  /**
   * @brief Change the key of the object to which we're referring.
   * @param sgkey The StoreGate key for the object.
   * 
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present,
   * the store is not changed.
   */
  UpdateHandleKey& operator= (const std::string& sgkey);
};


} // namespace SG


#include "StoreGate/UpdateHandleKey.icc"


#endif // not STOREGATE_UPDATEHANDLEKEY_H
