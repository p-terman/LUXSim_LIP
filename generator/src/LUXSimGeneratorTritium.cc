////////////////////////////////////////////////////////////////////////////////
/*   LUXSimGeneratorTritium.cc
*
*	This is the code file for the tritium generator.
*
*	This generator simply uses the standard beta spectrum formula to generate
*	the energy of the beta particle. We assume the neutrino mass is negligible.
*	The "standard beta spectrum" combines the basic kinematic curve with a
*	simplified Fermi function.
*
********************************************************************************
* Change log
*    23-Jul-2013 - Initial submission (Kareem)
*
*/
////////////////////////////////////////////////////////////////////////////////

//
//   GEANT4 includes
//
#include "globals.hh"
#include "G4Electron.hh"

//
//   LUXSim includes
//
#include "LUXSimGeneratorTritium.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//               LUXSimGeneratorTritium()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimGeneratorTritium::LUXSimGeneratorTritium()
{
	name = "Tritium";
	activityMultiplier = 1;
	
	//	The Q value comes from Sz. Nagy et al., "On the Q-value of the tritium
	//	beta-decay", Europhys. Lett., 74 (3), p. 404 (2006).
	Q = 18.5898;
	m_e = 511.;
	pi = 3.14159265358979312;
	a = 1./137;
	Z = 2.;
	
	xmax = Q;
	ymax = 1.1e7;
	electronDef = G4Electron::Definition();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//               ~LUXSimGeneratorTritium()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimGeneratorTritium::~LUXSimGeneratorTritium() {}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                    GenerateEventList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimGeneratorTritium::GenerateEventList( G4ThreeVector position,
                G4int sourceByVolumeID, G4int sourcesID, G4double time)
{
    G4int a=-1; G4int z=-1;
    G4double hl=-1;
    Isotope *currentIso = new Isotope(name, z, a, hl);
    luxManager->RecordTreeInsert( currentIso, time, position, 
                  sourceByVolumeID, sourcesID );    
}


//------++++++------++++++------++++++------++++++------++++++------++++++------
//                    GenerateFromEventList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimGeneratorTritium::GenerateFromEventList( G4GeneralParticleSource
       *particleGun, G4Event *event, decayNode *firstNode  )
{

     //stored in seconds, time in ns
     G4double time = (firstNode->timeOfEvent)/ns;

     G4ThreeVector pos = G4ThreeVector(firstNode->pos);

     //   First generate the neutron
     particleGun->SetParticleDefinition( electronDef );
     particleGun->GetCurrentSource()->GetPosDist()->SetCentreCoords(pos);
     particleGun->GetCurrentSource()->SetParticleTime( time*ns );
     particleGun->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(
           GetRandomDirection() );
     G4double singleElectronEnergy = GetElectronEnergy();
     particleGun->GetCurrentSource()->GetEneDist()->SetMonoEnergy(
           singleElectronEnergy*keV );

     particleGun->GeneratePrimaryVertex( event );
     luxManager->AddPrimaryParticle( GetParticleInfo(particleGun) );
     
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//               GetElectronEnergy()
//------++++++------++++++------++++++------++++++------++++++------++++++------
G4double LUXSimGeneratorTritium::GetElectronEnergy()
{
	G4double xTry = xmax*G4UniformRand();
	G4double yTry = ymax*G4UniformRand();
        G4double B = sqrt(xTry*xTry + 2*xTry*m_e) / (xTry + m_e);
        G4double x = (2*pi*Z*a)*(xTry + m_e)/sqrt(xTry*xTry + 2*xTry*m_e);

	while( yTry > (sqrt(2*xTry*m_e) *
                      (xTry + m_e) *
		      (Q-xTry) * (Q-xTry) *
                      x*(1./(1-exp(-x)))*(1.002037-0.001427*(B))) ){
		xTry = xmax*G4UniformRand();
		yTry = ymax*G4UniformRand();
                B = sqrt(xTry*xTry + 2*xTry*m_e) / (xTry + m_e);
                x = (2*pi*Z*a)*(xTry + m_e)/sqrt(xTry*xTry + 2*xTry*m_e);
	}
	return( xTry );
}
