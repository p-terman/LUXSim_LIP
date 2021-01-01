////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorXe131m.hh
*
* This is the header file for the Xe131m generator.
*
********************************************************************************
* Change log
*	2013-03-12 DCM - Original version
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorXe131m_HH
#define LUXSimGeneratorXe131m_HH 1

//
//    GEANT4 includes
//
#include "G4ParticleDefinition.hh"
#include "globals.hh"

//
//    LUXSim includes
//
#include "LUXSimSource.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimGeneratorXe131m : public LUXSimSource
{
  public:
    LUXSimGeneratorXe131m();
    ~LUXSimGeneratorXe131m();

  public:
    using LUXSimSource::GenerateEventList;
    void GenerateEventList( G4ThreeVector, G4int, G4int, G4double );
    using LUXSimSource::GenerateFromEventList;
    void GenerateFromEventList(G4GeneralParticleSource*,G4Event*,decayNode*);
    //using LUXSimSource::GenerateEvent;
    //void GenerateEvent( G4GeneralParticleSource*, G4Event* );

  private:
    G4ParticleDefinition *ion;
    G4ParticleDefinition *gammaDef;

};

#endif
