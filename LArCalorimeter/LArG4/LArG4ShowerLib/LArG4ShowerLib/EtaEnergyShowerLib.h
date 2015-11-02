/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef SHOWER_LIB_ETA_ENERGY_SHOWER_LIB_H
#define SHOWER_LIB_ETA_ENERGY_SHOWER_LIB_H

// STL includes
#include <map>
#include <vector>

// local includes
#include "LArG4ShowerLib/IShowerLib.h"

// G4 forward declarations
class G4Track;

// Namespace for the ShowerLib related classes
namespace ShowerLib {

  /**
   *
   *   @short Class for shower library shower lib
   *
   *          Store complete shower library (collection of energy bins)
   *
   *  @author Wolfgang Ehrenfeld, University of Hamburg, Germany
   *  @author Sasha Glazov, DESY Hamburg, Germany
   *
   * @version \$Id: EtaEnergyShowerLib.h 576158 2013-12-18 16:58:58Z gsedov $
   *
   */

  class EtaEnergyShowerLib : public IShowerLib {

  public:

    //! factory method. create a library from root file. returns NULL if file is invalid.
	static IShowerLib* readFromROOTFile(TFile* source);

	//! factory method. create empty library with the given structure. returns NULL if file is invalid.
	static IShowerLib* createEmptyLib(const std::string& inputFile);

    //! default destructor
    virtual ~EtaEnergyShowerLib() {}

    //! get shower for given G4 track
    virtual std::vector<EnergySpot>* getShower(const G4Track* track, ShowerLibStatistics* stats, int randomShift) const;
    //! get average length of showers for the given energy
    virtual double getContainmentZ(const G4Track* track) const;
    //! get average lateral spread of the showers for the given energy
    virtual double getContainmentR(const G4Track* track) const;
    //! store shower in the library
    virtual bool storeShower(const HepMC::GenParticle* genParticle,const Shower* shower);
    //! write library to ROOT file
    virtual bool writeToROOT(TFile* dest);

    virtual ShowerLibStatistics* createStatistics() const;

    inline virtual const std::string getName() const { return "Eta Energy ShowerLib"; }

  protected:

    virtual const std::string printParameters() const {return "";}

  private:

    EtaEnergyShowerLib(): mineta(0), maxeta(0), onlyPositive(false) {}

    //! read library from given TTree
    bool read(TTree* source);
    //! write library to given TTree
    bool write(TTree* dest) const;
    //!
    bool readStructure(std::vector<float>& structure);

    bool checkEtaAndStuff(const G4Track* track) const;

    float mineta, maxeta;

    typedef std::map<float,Shower> etabin;
    typedef std::map<float,etabin> library;

    library libData;
    bool onlyPositive;

  };

} // namespace ShowerLib

#endif // SHOWER_LIB_ETA_SHOWER_LIB_H
