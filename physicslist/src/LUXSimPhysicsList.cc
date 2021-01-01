////////////////////////////////////////////////////////////////////////////////
/*	LUXSimPhysicsList.cc
*
* This code file is part of the LUXSim physics list. It uses the QGSP_BIC_HP
* physics list that's included as part of the hadronic physics lists in GEANT4.
* It was also ripped off wholesale from the extended/field/field04 list, with
* some additional input from Peter Gumplinger. It also uses the
* G4RadioactiveDecayPhysics, G4EmExtraPhysics, and the Livermore physics list
* from the extended electromagnetic examples (see, e.g., TestEm1).
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*	14-Aug-09 - Major rewrite to use modern physics lists instead of the line-
*				by-line approach used previously. (Kareem)
*	01-Oct-09 - Reversed the order in which the "physicsVector" instantiated
*				lists and the "factory" instantiated lists are created. This
*				ensures the low-energy electromagnetics models are used rather
*				than the standard EM models. (Kareem)
*	02-Oct-09 - Added a flag to not invoke the standard EM models at all. They
*				were preventing the low-energy EM models from being instantiated
*				(Kareem)
*	05-Oct-09 - Fixed a bug where the physics constructors were declared after
*				they were needed instead of before (Kareem)
*	25-Jan-10 - Removed the in-place Livermore list with the EmLivermore
*				list distributed with GEANT4. This prevents a long list of
*				warnings starting with GEANT4.9.3 (Kareem)
*	26-Jan-10 - Put the in-place Livermore list back for backwards-compatability
*				with GEANT4 versions prior to 4.9.3, and added some version-
*				checking and a suggestion to upgrade to GEANT4.9.3. (Kareem)
*	13-Sep-11 - Lowered shortCutValue for enhanced accuracy (Matthew)
*	09-Nov-12 - Simplified the physics list, removing compatibility with
*				anything earlier than Geant4.9.4 (Kareem)
*   20-May-14 - Changed the long cut lengths from 0.1 mm to 0.01 mm, and made
*               the long cuts the default (Kareem)
*   18-Dec-2015 - Added support for building Shielding physics list for use
*               with the muon generator (David W)
*
*/
////////////////////////////////////////////////////////////////////////////////

//
//	C/C++ includes
//
#include <iostream>
#include <iomanip>
#include <ctime>

//
//	GEANT4 includes
//
#include "G4VModularPhysicsList.hh"

#include "G4Version.hh"
#include "G4LossTableManager.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessTable.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

#include "G4PhysListFactory.hh"

#include "G4EmExtraPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4EmLivermorePhysics.hh"

//
// Particles
//
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

//
//	LUXSim includes
//
#include "LUXSimPhysicsList.hh"
#include "LUXSimPhysicsStepMax.hh"
#include "LUXSimPhysicsOpticalPhysics.hh"
#include "LUXSimPhysicsMuonNuclear.hh"

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimPhysicsList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimPhysicsList::LUXSimPhysicsList(G4bool useShielding) : G4VModularPhysicsList()
{
	luxManager = LUXSimManager::GetManager();
	luxManager->Register( this );
	if (G4VERSION_NUMBER < 951)
	{
		cout << " If you wish to use the shielding physics list, please upgrade to Geant4.9.5" << endl;
		cout << " ***** Shielding physics has been disabled ***** " << endl;
		useShielding = false;
	}
	//	I don't know what this next line does, but it was part of the field04
	//	physics list, so here it is.
	G4LossTableManager::Instance();

	//	Set default cut values

	defaultCutValue = 1*nm;
	longCutValue = 1*nm;
	shortCutValue = 1*nm;

	if (useShielding) defaultCutValue = 1 * mm;

	cutForProton = defaultCutValue;
	cutForAlpha = defaultCutValue;
	cutForGenericIon = defaultCutValue;

	SetCutsLong();

	VerboseLevel = 1;
	OpVerbLevel = 0;
	
	SetVerboseLevel( VerboseLevel );
	
	G4RadioactiveDecayPhysics *radPhys = new G4RadioactiveDecayPhysics();
	G4EmExtraPhysics *extraPhys = new G4EmExtraPhysics();
	G4EmLivermorePhysics *lowEPhys = new G4EmLivermorePhysics();
	LUXSimPhysicsOpticalPhysics *optPhys = new LUXSimPhysicsOpticalPhysics();
	
	G4cout << "RegisterPhysics: " << radPhys->GetPhysicsName() << G4endl;
	RegisterPhysics( radPhys );
	G4cout << "RegisterPhysics: " << extraPhys->GetPhysicsName() << G4endl;
	RegisterPhysics( extraPhys );
	if (!useShielding) {
		G4cout << "RegisterPhysics: " << lowEPhys->GetPhysicsName() << G4endl;
		RegisterPhysics(lowEPhys);
	}
	G4cout << "RegisterPhysics: " << optPhys->GetPhysicsName() << G4endl;
	RegisterPhysics( optPhys );
	if (useShielding) {
		LUXSimPhysicsMuonNuclear *muonNuclearPhys = new LUXSimPhysicsMuonNuclear("muNucl");
		G4cout << "RegisterPhysics: " << muonNuclearPhys->GetPhysicsName() << G4endl;
		RegisterPhysics(muonNuclearPhys);
	}

	G4PhysListFactory factory;
//	G4VModularPhysicsList *phys = factory.GetReferencePhysList( "QGSP_BERT_HP");
	G4VModularPhysicsList *phys;
	if (!useShielding) phys = factory.GetReferencePhysList("QGSP_BIC_HP");
	else phys = factory.GetReferencePhysList("Shielding");
	for( G4int i=0; ; ++i ) {
		G4VPhysicsConstructor *elem =
				const_cast<G4VPhysicsConstructor*> (phys->GetPhysics(i));
		if( elem == NULL ) break;
		if( elem->GetPhysicsName() != "G4EmStandard" || useShielding) {
			G4cout << "RegisterPhysics: " << elem->GetPhysicsName() << G4endl;
			RegisterPhysics( elem );
		}
	}
	
	stepMaxProcess = new LUXSimPhysicsStepMax();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					~LUXSimPhysicsList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimPhysicsList::~LUXSimPhysicsList()
{
	delete stepMaxProcess;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetCuts()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimPhysicsList::SetCuts()
{
	SetCutValue( cutForGamma, "gamma" );
	SetCutValue( cutForElectron, "e-" );
	SetCutValue( cutForPositron, "e+" );
	SetCutValue( cutForProton, "proton" );
	SetCutValue( cutForAlpha, "alpha" );
	SetCutValue( cutForGenericIon, "GenericIon" );

	G4int nParticles = 5;
	DumpCutValuesTable( nParticles );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetCutsLong()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimPhysicsList::SetCutsLong()
{
	cutForGamma = longCutValue;
	cutForElectron = longCutValue;
	cutForPositron = longCutValue;

	SetCuts();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetCutsShort()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimPhysicsList::SetCutsShort()
{
	cutForGamma = shortCutValue;
	cutForElectron = shortCutValue;
	cutForPositron = shortCutValue;

	SetCuts();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					ClearPhysics()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimPhysicsList::ClearPhysics()
{
	for( G4PhysConstVector::iterator p = physicsVector->begin();
			p != physicsVector->end(); ++p )
		delete( *p );
	
	physicsVector->clear();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					RemoveFromPhysicsList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimPhysicsList::RemoveFromPhysicsList( const G4String& name )
{
	G4bool success = false;
	for( G4PhysConstVector::iterator p = physicsVector->begin();
			p != physicsVector->end(); ++p ) {
		G4VPhysicsConstructor *e = (*p);
		if( e->GetPhysicsName() == name ) {
			physicsVector->erase(p);
			success = true;
			break;
		}
	}
	
	if( !success ) {
		std::ostringstream message;
		message << "PhysicsList::RemoveFromEMPhysicsList " << name
				<< " not found";
#if G4VERSION_NUMBER<950
		G4Exception( message.str().c_str() );
#endif
	}
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					ConstructParticle()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimPhysicsList::ConstructParticle()
{
	G4VModularPhysicsList::ConstructParticle();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					ConstructProcess()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimPhysicsList::ConstructProcess()
{
	G4VModularPhysicsList::ConstructProcess();
	AddStepMax();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetStepMax()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimPhysicsList::SetStepMax( G4double step )
{
	MaxChargedStep = step;
	stepMaxProcess->SetStepMax( MaxChargedStep );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					AddStepMax()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimPhysicsList::AddStepMax()
{
	theParticleIterator->reset();
	while( (*theParticleIterator)() ) {
		G4ParticleDefinition *particle = theParticleIterator->value();
		G4ProcessManager *pmanager = particle->GetProcessManager();
		
		if( stepMaxProcess->IsApplicable(*particle)
				&& !particle->IsShortLived() ) {
			if( pmanager )
				pmanager->AddDiscreteProcess( stepMaxProcess );
		}
	}
}
