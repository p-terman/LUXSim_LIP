////////////////////////////////////////////////////////////////////////////////
/*	LUXSimPhysicsMuonNuclear.cc
*
********************************************************************************
* Change log
*	20-Oct-14 - Initial submission (David W)
*/
////////////////////////////////////////////////////////////////////////////////

//
//	GEANT4 includes
#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

#include "G4ParticleDefinition.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"

#include "G4ProcessManager.hh"


#if G4VERSION_NUMBER >= 951
#include "G4MuonNuclearProcess.hh"
#include "G4VDMuonNuclearModel.hh"
#endif

//
//	LUXSim includes
//
#include "LUXSimPhysicsMuonNuclear.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimPhysicsMuonNuclearPhysics()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimPhysicsMuonNuclear::LUXSimPhysicsMuonNuclear(const G4String& name)
    : G4VPhysicsConstructor(name)
{
	luxManager = LUXSimManager::GetManager();
	luxManager->Register( this );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					~LUXSimPhysicsMuonNuclearPhysics()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimPhysicsMuonNuclear::~LUXSimPhysicsMuonNuclear() { }

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					ConstructParticle()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimPhysicsMuonNuclear::ConstructParticle()
{
  G4BosonConstructor  pBosonConstructor;
  pBosonConstructor.ConstructParticle();

  G4LeptonConstructor pLeptonConstructor;
  pLeptonConstructor.ConstructParticle();

  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();

  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();

  G4IonConstructor pIonConstructor;
  pIonConstructor.ConstructParticle();

  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();

}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					ConstructProcess()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimPhysicsMuonNuclear::ConstructProcess()
{
	G4cout << "LUXSimPhysicsMuonNuclear:: Add Muon-Nuclear Interaction"
	<< G4endl;

	G4ProcessManager * pManager = 0;
#if G4VERSION_NUMBER >= 951
	G4MuonNuclearProcess* muNucProcess = new G4MuonNuclearProcess();
	G4VDMuonNuclearModel* muNucModel = new G4VDMuonNuclearModel();
	muNucProcess->RegisterMe(muNucModel);


	pManager = G4MuonPlus::MuonPlus()->GetProcessManager();
	pManager->AddDiscreteProcess(muNucProcess);
	
	pManager = G4MuonMinus::MuonMinus()->GetProcessManager();
	pManager->AddDiscreteProcess(muNucProcess);
#endif

}
