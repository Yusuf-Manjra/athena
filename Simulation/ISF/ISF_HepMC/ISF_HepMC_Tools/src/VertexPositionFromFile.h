/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VertexPositionFromFile.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_HEPMC_VERTEXPOSITIONFROMFILE_H
#define ISF_HEPMC_VERTEXPOSITIONFROMFILE_H 1

// STL includes
#include <string>
#include <utility>
#include <vector>
#include <map>
// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
// ISF includes
#include "ISF_HepMC_Interfaces/ILorentzVectorGenerator.h"

namespace ISF {

    /** using typedefs to make code more readable for (#run,#event) -> (x,y,z) mapping */
    typedef std::pair< int, int >                    RunEventPair;
    typedef std::vector< double >                    XYZCoordinates;
    typedef std::map< RunEventPair, XYZCoordinates>  EventCoordinatesMap;

  /** @class VertexPositionFromFile
  
      Returns a VertexShift based on what is given in the input file.
      
      based on:
      https://svnweb.cern.ch/trac/atlasoff/browser/Simulation/G4Atlas/G4AtlasUtilities/trunk/src/VertexPositioner.cxx
      
      @author Elmar.Ritsch -at- cern.ch
     */
  class VertexPositionFromFile : public AthAlgTool,
                                 virtual public ILorentzVectorGenerator { 
      
    public: 
      /** Constructor with parameters */
      VertexPositionFromFile( const std::string& t, const std::string& n, const IInterface* p );

      /** Destructor */
      ~VertexPositionFromFile();

      /** Athena algtool's Hooks */
      StatusCode  initialize();
      StatusCode  finalize();
	  
      /** returns current shift */
      CLHEP::HepLorentzVector *generate();
	  
	private:
      /** read-in and cache vertex positions from file */
      StatusCode  readVertexPosFile();
      /** read-in and cache run/event number overrides locally for vertex positioning */
      StatusCode  readRunEventNumFile();

      /** set vertex position by file */
      std::string                     m_vertexPositionFile;        //!< Name of file containing vertex position overrides
      EventCoordinatesMap             m_vertexPositionMap;         //!< vertex position for (#run,#event) pairs
      /** run and event number overrides according to file
          (to be used optionally in combination with 'set vertex by file') */
      std::string                     m_runEventNumbersFile;       //!< Name of file containing event info overrides for pos overrides
      unsigned int                    m_runEventNumbersIndex;      //!< current index in EventNum/RunNum vectors
      std::vector<int>                m_vertexPositionRunNum;      //!< run number override vector
      std::vector<int>                m_vertexPositionEventNum;    //!< event number override vector

  }; 
  
}

#endif //> !ISF_HEPMC_VERTEXPOSITIONFROMFILE_H
