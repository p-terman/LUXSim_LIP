////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorTritium.hh
*
* This is the header file for the tritium generator.
*
********************************************************************************
* Change log
*    23-Jul-2013 - Initial submission (Kareem)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorTritium_HH
#define LUXSimGeneratorTritium_HH 1

//
//    GEANT4 includes
//
#include "globals.hh"

//
//    LUXSim includes
//
#include "LUXSimSource.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimGeneratorTritium : public LUXSimSource
{
    public:
        LUXSimGeneratorTritium();
        ~LUXSimGeneratorTritium();

    public:
        using LUXSimSource::GenerateEventList;
        void GenerateEventList( G4ThreeVector, G4int, G4int, G4double );
        using LUXSimSource::GenerateFromEventList;
        void GenerateFromEventList(G4GeneralParticleSource*,G4Event*,decayNode*);

    private:
        G4double GetElectronEnergy();

    private:
		G4double Q;
		G4double m_e;
		G4double pi;
		G4double a;
		G4double Z;
		G4double xmax;
		G4double ymax;
        G4ParticleDefinition *electronDef;        
};

#endif
