/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/


#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/tools/error.h"
#include "AthContainers/exceptions.h"
#include "AthContainersInterfaces/AuxDataOption.h"

#include "RNTupleAuxDynStore.h"
#include "RNTupleAuxDynReader.h"

#include <ROOT/RNTuple.hxx>
#include <ROOT/REntry.hxx>
#include <ROOT/RFieldValue.hxx>
using RFieldValue   = ROOT::Experimental::Detail::RFieldValue;
using RNTupleReader = ROOT::Experimental::RNTupleReader;
using REntry = ROOT::Experimental::REntry;

using namespace RootAuxDynIO;

RNTupleAuxDynStore::RNTupleAuxDynStore( RNTupleAuxDynReader& aux_reader,
                                        RNTupleReader* rnt_reader,
                                        const std::string& branchName,
                                        long long entry, bool standalone,
                                        std::recursive_mutex* iomtx)
   : RootAuxDynStore( aux_reader, entry, standalone, iomtx ),
     m_reader( aux_reader ),
     m_ntupleReader( rnt_reader ),
     m_baseBranchName( branchName ) 
{
}


bool RNTupleAuxDynStore::readData(SG::auxid_t auxid)
{
   try {
      auto& fieldInfo = m_reader.getFieldInfo(auxid, *this);
      if( fieldInfo.status == RNTupleAuxDynReader::FieldInfo::NotFound ) return false; 

      // Make a 1-element vector in the underlying AuxStore for the given auxid
      SG::AuxStoreInternal::getDataInternal(auxid, 1, 1, true);
      if( fieldInfo.isPackedContainer ) {
         setOption (auxid, SG::AuxDataOption ("nbits", 32));
      }
  
      // get memory location where to write data from the branch entry
      // const_cast because Field::CaptureValue() requires void*
      void* data ATLAS_THREAD_SAFE = const_cast<void*>(SG::AuxStoreInternal::getIOData(auxid));
      
      // if have mutex, lock to prevent potential concurrent I/O from elsewhere
      auto io_lock = m_iomutex? std::unique_lock<std::recursive_mutex>(*m_iomutex)
         : std::unique_lock<std::recursive_mutex>();

      REntry *entry = m_ntupleReader->GetModel()->GetDefaultEntry();
      for( auto iValue = entry->begin(); iValue != entry->end(); ++iValue ) {
         std::string field_name = iValue->GetField()->GetName();
         if( field_name == fieldInfo.fieldName ) {
            RFieldValue rfv = iValue->GetField()->CaptureValue( data );
            iValue->GetField()->Read(m_entry, &rfv);
            break;
         }
      }

      int  nbytes = 1;   // MN: TODO how to get this?
      if( nbytes <= 0 ) {
         throw std::string("Error reading field ") + fieldInfo.fieldName;
      }
      // read OK
      m_reader.addBytes(nbytes);
   }
   catch(const std::string& e_str) {
      ATHCONTAINERS_ERROR("RNTupleAuxDynStore::getData", e_str);
      return false;
   }
   return true;
}
