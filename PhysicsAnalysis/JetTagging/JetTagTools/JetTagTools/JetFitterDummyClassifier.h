/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGTOOL_JETFITTERDUMMYCLASSIFIER_C
#define BTAGTOOL_JETFITTERDUMMYCLASSIFIER_C

/******************************************************
    @class JetFitterDummyClassifier
     Package : JetTagTools
     Created : March 2007

     DESCRIPTION: filler for JetFitter classifier, does nothing

********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "JetTagTools/IJetFitterClassifierTool.h"

#include "xAODBTagging/ftagfloat_t.h"

namespace Analysis {


  class IJetFitterTagInfo;

  class JetFitterDummyClassifier : public extends<AthAlgTool, IJetFitterClassifierTool>
  {
  public:
    using base_class::base_class;

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    // IJetFitterClassifier interface
    virtual
    StatusCode fillLikelihoodValues(xAOD::BTagging* BTag,
                                    const std::string & jetauthor,
                                    const std::string& inputbasename,
                                    const std::string& outputbasename,
                                    ftagfloat_t jetpT,
                                    ftagfloat_t jeteta,
                                    ftagfloat_t IP3dlike=-5000) const override;
  };

}//end Analysis namespace

#endif
