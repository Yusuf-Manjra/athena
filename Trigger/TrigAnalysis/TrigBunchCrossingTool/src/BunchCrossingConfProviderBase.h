// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchCrossingConfProviderBase.h 618331 2014-09-24 11:55:26Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_BUNCHCROSSINGCONFPROVIDERBASE_H
#define TRIGBUNCHCROSSINGTOOL_BUNCHCROSSINGCONFPROVIDERBASE_H

// Interface include(s):
#include "TrigAnalysisInterfaces/IBunchCrossingConfProvider.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

namespace Trig {

   /**
    *  @short Common implementation of the IBunchCrossingConfProvider interface
    *
    *         Implementing the IBunchCrossingConfProvider interface is quite
    *         simple actually, and it can be done in a common way. So it doesn't
    *         make sense to do it separately for all the tool implementations.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 618331 $
    * $Date: 2014-09-24 13:55:26 +0200 (Wed, 24 Sep 2014) $
    */
   class BunchCrossingConfProviderBase :
      public virtual IBunchCrossingConfProvider,
      public virtual IBunchCrossingTool {

   public:
      /// Function providing the BCIDs of the configured colliding bunches
      virtual std::vector< int > configuredBCIDs() const;
      /// Function providing the intensities of the configured colliding bunches
      virtual std::vector< float > configuredIntensitiesBeam1() const;
      /// Function providing the intensities of the configured colliding bunches
      virtual std::vector< float > configuredIntensitiesBeam2() const;

      /// Function providing the BCIDs of the configured unpaired bunches
      virtual std::vector< int > configuredUnpairedBCIDsBeam1() const;
      /// Function providing the BCIDs of the configured unpaired bunches
      virtual std::vector< int > configuredUnpairedBCIDsBeam2() const;
      /// Function providing the intensities of the configured unpaired bunches
      virtual std::vector< float > configuredUnpairedIntensitiesBeam1() const;
      /// Function providing the intensities of the configured unpaired bunches
      virtual std::vector< float > configuredUnpairedIntensitiesBeam2() const;

   protected:
      /// Type name for the incident that such tools should emit
      static const std::string BUNCH_CONFIG_INCIDENT_NAME;

   }; // class BunchCrossingConfProviderBase

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_BUNCHCROSSINGCONFPROVIDERBASE_H
