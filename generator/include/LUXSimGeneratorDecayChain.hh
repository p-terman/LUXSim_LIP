////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorDecayChain.hh
*
* This is the header file.
*
********************************************************************************
* Change log
*    21 Jul 2011 - Initial submission (modified from Kareem's stand-alone code)
*                 (Nick)
*    24 Aug 2012 - Add time unit specification to variables (Nick)
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorDecayChain_HH
#define LUXSimGeneratorDecayChain_HH 1

//
//    GEANT4 includes
//
#include "G4ParticleDefinition.hh"
#include "globals.hh"

//
//    LUXSim includes
//
#include "LUXSimBST.hh"
#include "LUXSimIsotope.hh"
#include "LUXSimSource.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimGeneratorDecayChain : public LUXSimSource
{
    public:
        LUXSimGeneratorDecayChain();
        ~LUXSimGeneratorDecayChain();

    public:
        using LUXSimSource::GenerateEventList;
        void GenerateEventList( G4ThreeVector, G4int, G4int, G4String );
        using LUXSimSource::CalculatePopulationsInEventList;
        void CalculatePopulationsInEventList( G4double, G4double, G4String );
        using LUXSimSource::GenerateFromEventList;
        void GenerateFromEventList( G4GeneralParticleSource*, G4Event*, 
                                decayNode* );
        G4double GetParentDecayTime() {return originalTime_s;};
   
    private:
        //Th232
        G4double halflivesTh[11];
        G4double lambdaTh232[11];
        //U238
        G4double halflivesU238[19];
        G4double lambdaU238[19];

    private:
        //Th232 with 11 isotopes (10 levels)
        G4int CalculatePopulationsTh232( G4double[11], G4double, G4double);
        G4double CalculateRatesTh232(    G4double[11], G4double[11] );
        G4double ReducePopulationsTh232( G4double[11], G4double[11], G4double);
        // U238 with 19 isotopes (14 levels)
        G4int CalculatePopulationsU238( G4double[19], G4double, G4double);
        G4double CalculateRatesU238(    G4double[19], G4double[19] );
        G4double ReducePopulationsU238( G4double[19], G4double[19], G4double);

    private:
        G4double No; //the starting population after the specified time
        //Th232
        G4double populationTh[11], ratesTh[11];
        Isotope *isoArrayTh[11];
        //U238
        G4double populationU[19], ratesU[19];
        Isotope *isoArrayU[19];

    private:
        G4ParticleDefinition *ion;
        G4double originalRate;
        G4double originalTime_s;
};

    
#endif
