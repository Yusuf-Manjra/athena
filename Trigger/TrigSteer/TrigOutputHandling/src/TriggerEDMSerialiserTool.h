/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_TRIGGEREDMSERIALISERTOOL_H
#define TRIGOUTPUTHANDLING_TRIGGEREDMSERIALISERTOOL_H

#include <string>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "xAODCore/AuxSelection.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IAthenaSerializeSvc.h"
#include "AthenaKernel/IDictLoaderSvc.h"
#include "TrigOutputHandling/HLTResultMTMakerTool.h"
#include "TrigSerializeTP/TrigSerTPTool.h"
#include "Gaudi/Parsers/Factory.h" // Needed to declare less common Property types


/**
 * @class TriggerEDMSerialiserTool is tool responsible for creation of HLT Result filled with streamed EDM collections
 *
 * Its configuration is a list of EDM classes + keys + designation of dynamic varaibles.
 * Accordfing to this list objects are looked up in the main store and if found the
 * serialisation service is used to convert the collections into arrays of bytes.
 * Serialised data is wrapped info a vector of 32 bit integers and prefixed wiht header containing description of the payload (details may change).
 * Such fragments for subsequent collections are concatentated together to form EDM payload in the HLTResult.
 * The HLTResultMT has option to maintain several modules (designated to end up in separate ROBS) that can be filled.
 * That is also specified in the configuration as documented below.
 *
 * For unpacking, the @see TriggerEDMDeserialiserAlg is used
 **/

class DataObject; //!< Forward declaration

class TriggerEDMSerialiserTool: public extends<AthAlgTool, HLTResultMTMakerTool>
{

 public:

  TriggerEDMSerialiserTool( const std::string& type,
	     const std::string& name,
	     const IInterface* parent );

  virtual ~TriggerEDMSerialiserTool();
  virtual StatusCode fill( HLT::HLTResultMT& resultToFill ) const override;

  virtual StatusCode  initialize() override;

 private:
  friend StatusCode tester( TriggerEDMSerialiserTool* );
  Gaudi::Property< std::vector< std::string > > m_collectionsToSerialize {
    this, "CollectionsToSerialize", {},
    "EDM streaming configuration \'collectionKeyType;module1,module2,module3\' where collectionKeyType is a string formatted like for "
    "AthenaOutputStream, e.g. TYPE#SG.aux1.aux2..etc. The type has to be an exact type, i.e. with _vN not the alias "
    "type. moduleIdVec is the vector of HLTResult ROB module IDs to which the collection should be written. ID=0 is "
    "the main result, other IDs are used for data scouting."
  };
  Gaudi::Property<bool> m_saveDynamic { this, "SaveDynamic", true, "If false skips serialising of dynamic varaibles. Use for test purpose only." };


  /// @class Address
  /// Internal structure to keep configuration organised conveniently
  ///

  struct Address {
    enum Category { xAODInterface, xAODAux, OldTP, xAODDecoration, None };
    Address( const std::string& transType_,
	     const std::string& persType_,
	     const CLID clid_,
	     const std::string& key_,
	     const std::vector<uint16_t> module_={},
	     const Category category_ = None,
	     const xAOD::AuxSelection& sel_ = {} )
    : transType(transType_),
      persType(persType_),
      clid(clid_),
      key(key_),
      moduleIdVec(module_),
      category(category_),
      sel(sel_){}

    std::string transType;
    std::string persType; // actuall versioned type
    CLID clid;
    std::string key;
    std::vector<uint16_t> moduleIdVec;

    Category category;
    xAOD::AuxSelection sel = {}; //!< xAOD dynamic varaibles selection, relevant only for xAODAux category

  };

  std::vector< Address > m_toSerialise; // postprocessed configuration info

  ServiceHandle<IClassIDSvc> m_clidSvc{ this, "ClassIDSvc", "ClassIDSvc",
      "Service to translate class name to CLID" };
  ServiceHandle<IAthenaSerializeSvc> m_serializerSvc{ this, "Serializer", "AthenaRootSerializeSvc",
      "Service that translates transient to persistent respresenation" };

  ToolHandle<TrigSerTPTool> m_tpTool{ this, "TPTool", "TrigSerTPTool/TrigSerTPTool",
      "Tool to do Transient/Persistent conversion (Old EDM)"};



  /**
   * Given the ID if the collection (in address arg) insert basic streaming info into the buffer.
   */
  StatusCode makeHeader( const TriggerEDMSerialiserTool::Address& address, std::vector<uint32_t>& buffer  ) const;

  /**
   * For copy bytest from the memory into the buffer converting from char[] to uint32_t[]
   * This function is candidate to be made global function at some point
   * and we will need also readPayload function
   */
  StatusCode fillPayload( const void* data, size_t sz, std::vector<uint32_t>& buffer ) const;


  /**
   * Place inside the buffer the serialised container (can be either TP, xAOD)
   * invloves simple involcation of serialiser
   */
  StatusCode serialiseContainer( void* data, const Address& address, std::vector<uint32_t>& buffer ) const;
  /**
   * Place inside the buffer serialised the xOAD Aux container
   * invloves selection and recording of dynamic variables
   */
  StatusCode serialisexAODAuxContainer( void* data, const Address& address, std::vector<uint32_t>& buffer ) const;

  /**
   * Place inside the buffer the serialised old type of container
   * invloves T/P conversion
   */
  StatusCode serialiseTPContainer( void* data, const Address& address, std::vector<uint32_t>& buffer ) const;



  /**
   * Adds dynamic variables to the payload
   */
  StatusCode serialiseDynAux( DataObject* dObject, const Address& address, std::vector<uint32_t>& buffer, size_t& nDynWritten ) const;


  /**
   * Obtain version from the actaul type name
   */
  std::string version( const std::string& name ) const;

};


#endif //> !TRIGOUTPUTHANDLING_TRIGGEREDMSERIALISERTOOL_H
