////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorMUSUN.hh
*
* This is the header file for the MUSUN generator.
*
********************************************************************************
* Change log
*    22-Oct-2015 - Initial submission (David)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorMUSUN_HH
#define LUXSimGeneratorMUSUN_HH 1

//
//    GEANT4 includes
//
#include "globals.hh"

//
//    LUXSim includes
//
#include "LUXSimSource.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimGeneratorMUSUN : public LUXSimSource
{
    public:
        LUXSimGeneratorMUSUN();
        ~LUXSimGeneratorMUSUN();

    public:
        using LUXSimSource::GenerateEventList;
        void GenerateEventList(G4double); 
        using LUXSimSource::GenerateFromEventList;
        void GenerateFromEventList(G4GeneralParticleSource*,G4Event*,decayNode*);


    private:
        void CalculateMuonVariables();
        void Initialization( double theta1, double theta2, double phi1, double phi2, int igflag, double s_hor, double s_ver1, double s_ver2, double *FI );
	void Sampling( double *E, double *theta, double *phi, double *dep );

    private:
        void SetMuonX0( double tmp ) {m_x0=tmp;};
        double GetMuonX0()           {return m_x0;};
        void SetMuonY0( double tmp ) {m_y0=tmp;};
        double GetMuonY0()           {return m_y0;};
        void SetMuonZ0( double tmp ) {m_z0=tmp;};
        double GetMuonZ0()           {return m_z0;};
        void SetMuonE( double tmp )  {m_E=tmp;};
        double GetMuonE()            {return m_E;};
        void SetMuonSign( int tmp )  {m_signid=tmp;};
        int GetMuonSign()            {return m_signid;};
        void SetMuonCx( double tmp ) {m_cx=tmp;};
        double GetMuonCx()           {return m_cx;};
        void SetMuonCy(double tmp )  {m_cy=tmp;};
        double GetMuonCy()           {return m_cy;};
        void SetMuonCz( double tmp ) {m_cz=tmp;};
        double GetMuonCz() {return m_cz;};
  
        double m_x0;
        double m_y0;
        double m_z0;
        double m_E;
        double m_signid;
        double m_cx;
        double m_cy;
        double m_cz;
  
        //--- Global variables
        double spmu[121][62][51], fnmu[32401], depth[360][91], fmu[360][91],
        e1, e2, the1, the2, ph1, ph2;

        G4ParticleDefinition *ionDef;
        G4ParticleDefinition *neutronDef;
        G4ParticleDefinition *muonplusDef;
        G4ParticleDefinition *muonminusDef;
        G4ParticleDefinition *gammaDef;
        
};

#endif
