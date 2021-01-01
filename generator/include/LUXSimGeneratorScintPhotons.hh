////////////////////////////////////////////////////////////////////////////////
/*   LUXSimGeneratorScintPhotons.hh
*
* This is the header file for the ScintPhoton generator.
*
********************************************************************************
* Change log
*   19-Aug-11 - Initial submission (Nick)
*   14-Jul-12 - GeneateEvent changed to use binary search tree (Nick)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorScintPhoton_HH
#define LUXSimGeneratorScintPhoton_HH 1

//
//   GEANT4 includes
//
#include "G4ParticleDefinition.hh"
#include "globals.hh"

//
//   LUXSim includes
//
#include "LUXSimSource.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimGeneratorScintPhotons : public LUXSimSource
{
  public:
    LUXSimGeneratorScintPhotons();
    ~LUXSimGeneratorScintPhotons();

  public:
    using LUXSimSource::GenerateEventList;
    void GenerateEventList( G4ThreeVector, G4int, G4int, G4double);
    using LUXSimSource::GenerateFromEventList;
    void GenerateFromEventList(G4GeneralParticleSource*,G4Event*, decayNode*);
    //using LUXSimSource::GenerateEvent;
    //void GenerateEvent( G4GeneralParticleSource*, G4Event*);

  private:
    G4ParticleDefinition *photon;
    G4ThreeVector direction, polarization, perp;
    G4double theta, xpol, ypol, zpol, phi, sinp, cosp;

};

#endif
