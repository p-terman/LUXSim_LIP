////////////////////////////////////////////////////////////////////////////////
/*	LUXSimPhysicsOpticalPhysics.cc
*
* This code file comes from Peter Gumplinger, and is intended to be a part of
* the extended example field04 (which should be part of GEANT4.9.3). It turns
* on physics for optical models.
*
********************************************************************************
* Change log
*	26-Aug-09 - Initial submission (Kareem)
*	28-Aug-09 - Added registration to the LUXSimManager singleton (Kareem)
*       13-Sep-11 - Swapped out G4Scintillation for G4S1Light (Matthew)
*       14-Feb-12 - Added S2 process and made S1,S2 RestDiscrete (Matthew)
*       21-Feb-12 - Added QE process (Matthew)
*       02-Nov-12 - Improved Cerenkov options (Matthew, Henrique)
*       08-Jun-15 - The G4S2Light class now gets invoked via a different call,
*                   it now includes a pointer to the G4S1Light class (Kareem)
*/
////////////////////////////////////////////////////////////////////////////////

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4ProcessManager.hh"

#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpWLS.hh"

#include "LUXSimQuantumEfficiency.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4OpticalPhoton.hh"
#include "G4ThermalElectron.hh"
#include "G4S2Light.hh"

//
//	LUXSim includes
//
#include "LUXSimPhysicsOpticalPhysics.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimPhysicsOpticalPhysics()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimPhysicsOpticalPhysics::LUXSimPhysicsOpticalPhysics()
    : G4VPhysicsConstructor("Optical")
{
	luxManager = LUXSimManager::GetManager();
	luxManager->Register( this );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					~LUXSimPhysicsOpticalPhysics()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimPhysicsOpticalPhysics::~LUXSimPhysicsOpticalPhysics() { }

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					ConstructParticle()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimPhysicsOpticalPhysics::ConstructParticle()
{
	G4OpticalPhoton::OpticalPhotonDefinition();
	G4ThermalElectron::ThermalElectronDefinition();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					ConstructProcess()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimPhysicsOpticalPhysics::ConstructProcess()
{
	G4cout << "LUXSimPhysicsOpticalPhysics:: Add Optical Physics Processes"
	<< G4endl;
	
	theScintProcess = new G4S1Light();
    G4S2Light *theLuminProcess = new G4S2Light( "S2", fElectromagnetic,
            theScintProcess );
	theCerenkovProcess = new G4Cerenkov();
	G4int MaxNumPhotons = 300;
	theCerenkovProcess->SetTrackSecondariesFirst(true);
	theCerenkovProcess->SetMaxNumPhotonsPerStep(MaxNumPhotons);
	
	LUXSimQuantumEfficiency *PHE = new LUXSimQuantumEfficiency();
	
	G4OpAbsorption* theAbsorptionProcess = new G4OpAbsorption();
	G4OpRayleigh* theRayleighScattering = new G4OpRayleigh();
	G4OpBoundaryProcess* theBoundaryProcess = new G4OpBoundaryProcess();
	G4OpWLS* theWLSProcess = new G4OpWLS();

	G4ProcessManager* pManager = 
	G4OpticalPhoton::OpticalPhoton()->GetProcessManager();

	if( !pManager ) {
		std::ostringstream o;
		o << "Optical Photon without a Process Manager";
		G4Exception("LUXSimPhysicsOpticalPhysics::ConstructProcess()","",
		FatalException,o.str().c_str());
	}


	pManager->AddDiscreteProcess(theAbsorptionProcess);
	pManager->AddDiscreteProcess(theRayleighScattering);

	theBoundaryProcess->SetModel(unified);
	//  theBoundaryProcess->SetModel(glisur);

	pManager->AddDiscreteProcess(theBoundaryProcess);

	theWLSProcess->UseTimeProfile("delta");
	//  theWLSProcess->UseTimeProfile("exponential");

	pManager->AddDiscreteProcess(theWLSProcess);

	//theScintProcess->SetScintillationYieldFactor(1.);
	//theScintProcess->SetScintillationExcitationRatio(0.06);
	theScintProcess->SetTrackSecondariesFirst(true);

	theParticleIterator->reset();

	while( (*theParticleIterator)() ) {
		G4ParticleDefinition* particle = theParticleIterator->value();
		G4String particleName = particle->GetParticleName();

		pManager = particle->GetProcessManager();
		if (!pManager) {
			std::ostringstream o;
			o << "Particle " << particleName << "without a Process Manager";
			G4Exception("LUXSimPhysicsOpticalPhysics::ConstructProcess()","",
			FatalException,o.str().c_str());
		}

		if( theCerenkovProcess->IsApplicable(*particle) ) {
			pManager->AddProcess(theCerenkovProcess);
			pManager->SetProcessOrdering(theCerenkovProcess,idxPostStep);
		}
		if( theScintProcess->IsApplicable(*particle) )
		  pManager->AddProcess(theScintProcess,ordDefault,ordInActive,ordDefault);
		if( theLuminProcess->IsApplicable(*particle) )
		  pManager->AddProcess(theLuminProcess,ordDefault,ordInActive,ordDefault);
		if( PHE->IsApplicable(*particle) )
		  pManager->AddProcess(PHE,ordDefault,ordInActive,ordDefault);
	}
}
