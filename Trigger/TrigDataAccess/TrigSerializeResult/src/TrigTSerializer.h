// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @class  TrigTSerializer
 * @author Jiri Masik <Jiri.Masik@cern.ch>
 */


#ifndef TRIGTSERIALIZER_H
#define TRIGTSERIALIZER_H

#include "GaudiKernel/AlgTool.h"
#include "TrigSerializeResult/ITrigSerializerToolBase.h"
#include "RVersion.h"
#include "Rtypes.h"
#include "TError.h"
#include <string>
#include <map>

//forward decl
class TBuffer;
class MsgStream;
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,19,00)
  namespace Reflex {
    class Type;
    class Object;
  }
#define REFLEX_NS Reflex
#else
namespace ROOT {
   namespace Reflex {
      class Type;
      class Object;
   }
}
#define REFLEX_NS ROOT::Reflex
#endif



class TrigTSerializer : virtual public ITrigSerializerToolBase, public AlgTool {
public:
  TrigTSerializer(const std::string& name, const std::string& type,
		  const IInterface* parent);
  virtual ~TrigTSerializer();

  StatusCode initialize();
  StatusCode finalize();

  /*
  StatusCode serialize(uint32_t *serialized, void *instance,
		       const std::string nameOfClass, int &veclen, const int maxsize);
  */
  std::vector<uint32_t> serialize(const std::string &nameOfClass, void* instance);

  void serialize(const std::string &nameOfClass, void* instance, std::vector<uint32_t> &v);

  void* deserialize(const std::string &nameOfClass, std::vector<uint32_t>& v);
  /*
    StatusCode deserialize(const uint32_t *serialized,  const std::string nameOfClass);
  */

  StatusCode initClass(const std::string &nameOfClass) const;

  //reset Serializer internal data between running serialize methods with unrelated data
  void reset();

  StatusCode peekCLID(const std::vector<uint32_t>& v, uint32_t *guid) const;
  
  void setCLID(const uint32_t *guid);

private:
  void do_persistify(const std::string nameOfClass, void* instance);
  void do_persistify_obj(const std::string nameOfClass, void* instance);
  void do_stl_workaround(const REFLEX_NS::Type *mytype,
                         const REFLEX_NS::Object *myobject);
  void do_follow_ptr(const std::string nameOfClass, void* instance);
  void add_previous_streamerinfos();
  
  static void streamerErrorHandler(Int_t level, Bool_t abort_bool,
				   const char* location, const char *msg);
  void prepareForTBuffer(const std::string &nameOfClass);
  void restoreAfterTBuffer(const std::string &nameOfClass);

  //  StatusCode persistifyEL(const TClass *cl, void* instance);
  MsgStream *m_log;
  size_t    m_outputlevel;
  bool      m_onlineMode;
  uint32_t  m_guid[4];  

  // for error handling
  ErrorHandlerFunc_t  m_defaultHandler;
  static   bool       s_decodingError;
  //static   bool       m_reportError;
  uint32_t  m_IgnoreErrLvl;
  std::map<std::string, uint32_t>   m_errCount;
  
};

#undef REFLEX_NS


#endif
