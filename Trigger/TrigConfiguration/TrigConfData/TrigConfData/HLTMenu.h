/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_HLTMENU_H
#define TRIGCONFDATA_HLTMENU_H

#include "TrigConfData/ConstIter.h"
#include "TrigConfData/DataStructure.h"
#include "TrigConfData/HLTChain.h"

namespace TrigConf {

   /** 
    * @brief HLT menu configuration
    *
    * Provides access to menu attributes like its name and to the trigger chains
    */
   class HLTMenu final : public DataStructure {
   public:

      /** Constructor */
      HLTMenu();

      /** Constructor initialized with configuration data 
       * @param data The data containing the HLT menu 
       */
      HLTMenu(const ptree & data);

      /** Destructor */
      ~HLTMenu();

      /** Accessor to the menu name */
      std::string name() const;

      /** Accessor to the number of HLT chains */
      std::size_t size() const;

      /** Iterator over the HLT chains */
      using const_iterator = ConstIter<ptree, Chain>;

      /** Begin of the HLT chains list
       *
       * Needed for range-based loops
       */
      const_iterator begin() const;

      /** End of the HLT chains list
       *
       * Needed for range-based loops
       */
      const_iterator end() const;
   };
}

#ifndef TRIGCONF_STANDALONE

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( TrigConf::HLTMenu , 24176960 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( TrigConf::HLTMenu , 155284098 );

#endif

#endif
