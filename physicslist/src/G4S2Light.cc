//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The NEST program is intended for use with the Geant4 software,   *
// * which is copyright of the Copyright Holders of the Geant4        *
// * Collaboration. This additional software is copyright of the NEST *
// * development team. As such, it is subject to the terms and        *
// * conditions of both the Geant4 License, included with your copy   *
// * of Geant4 and available at http://cern.ch/geant4/license, as     *
// * well as the NEST License included with the download of NEST and  *
// * available at http://nest.physics.ucdavis.edu/                    *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutions, nor the agencies providing financial support for   *
// * this work make any representation or warranty, express or        *
// * implied, regarding this software system, or assume any liability *
// * for its use. Please read the pdf license or view it online       *
// * before download for the full disclaimer and lack of liability.   *
// *                                                                  *
// * This code implementation is based on work by Peter Gumplinger    *
// * and his fellow collaborators on Geant4 and is distributed with   *
// * the express written consent of the Geant4 collaboration. By      *
// * using, copying, modifying, or sharing the software (or any work  *
// * based on the software) you agree to acknowledge use of both NEST *
// * and Geant4 in resulting scientific publications, and you         *
// * indicate your acceptance of all the terms and conditions of the  *
// * licenses, which must always be included with this code.          *
// ********************************************************************
//
//
// Noble Element Simulation Technique "G4S2Light" physics process
//
////////////////////////////////////////////////////////////////////////
// S2 Scintillation Light Class Implementation
////////////////////////////////////////////////////////////////////////
//
// File:        G4S2Light.cc (companion of G4S1Light.cc)
// Description: RestDiscrete Process - Generation of S2 Photons in GXe
// Version:     0.99 beta 1a LUX specific
// Created:     Friday, September 13, 2013
// Authors:     Matthew Szydagis, Dustin Stolp, and Michael Woods
//
// mail:        mmszydagis@ucdavis.edu
//
////////////////////////////////////////////////////////////////////////

#include "G4ParticleTypes.hh" //lets you refer to G4OpticalPhoton, etc.
#include "G4EmProcessSubType.hh" //lets you call this process Scintillation
#include "G4S2Light.hh"
#include "G4S1Light.hh"

#define GRID_DENSITY 8.03*(g/cm3) //density of your grid material

G4double thr[100]; //offset in linear light yield formula for S2 ph/e-
G4double E_eV[100]; //energy of single photon in gas, eV
G4double tau1[100], tau3[100], ConvertEff[100];

G4S2Light::G4S2Light( G4String processName,
                     G4ProcessType type, G4S1Light *s1Light)
: G4VRestDiscreteProcess(processName, type), theScintProcess(s1Light)
{
  thr[18] = 0.190; E_eV[18] = 9.7;
  tau1[18] = 6*ns; tau3[18] = 1600*ns; ConvertEff[18]=0.7857;
  thr[54] = 0.474; E_eV[54] = 7.143;
  ConvertEff[54] = 1.01; //50% when LXe TPC is dirty
        luxManager = LUXSimManager::GetManager();
        SetProcessSubType(fScintillation);
        fTrackSecondariesFirst = true;
        if (verboseLevel>0) {
	  G4cout << GetProcessName() << " is created " << G4endl;
        }
}

G4S2Light::~G4S2Light(){} //destructor

G4VParticleChange*
G4S2Light::AtRestDoIt(const G4Track& aTrack, const G4Step& aStep)
{
  return G4S2Light::PostStepDoIt(aTrack, aStep);
}

G4VParticleChange*
G4S2Light::PostStepDoIt(const G4Track& aTrack, const G4Step& aStep)
{
// The function where all the action happens! Not yet well commented, as this
// is but an alpha version of the electroluminescence code
        YieldFactor = 1.000;
        aParticleChange.Initialize(aTrack);
	const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
        const G4Material* aMaterial = aTrack.GetMaterial();
	if ( !aMaterial ) {
	  aParticleChange.ProposeTrackStatus(fStopAndKill);
          return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
	}
	G4MaterialPropertiesTable* aMaterialPropertiesTable = 
	  aMaterial->GetMaterialPropertiesTable();
	if ( !aMaterialPropertiesTable ) {
	  aParticleChange.ProposeTrackStatus(fStopAndKill);
          return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
	}
	const G4ElementVector* theElementVector =
	  aMaterial->GetElementVector();
	G4Element* Element = (*theElementVector)[0]; G4int z1;
	if ( Element ) z1 = (G4int)(Element->GetZ()); else z1 = -1;
    G4double z_anode = theScintProcess->BORDER + theScintProcess->GASGAP;
    G4double z_start = theScintProcess->BORDER;
    if(luxManager->GetGasRun()) z_start=theScintProcess->GAT;

	tau1[54] = G4RandGauss::shoot(5.18*ns,1.55*ns);
        tau3[54] = G4RandGauss::shoot(100.1*ns,7.9*ns);
	if ( aMaterial->GetTemperature() < 200 * kelvin )
	  E_eV[54] = 6.93;
	
	G4StepPoint* pPreStepPoint = aStep.GetPreStepPoint();
	G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();
	G4ThreeVector x0 = pPreStepPoint->GetPosition();
	G4ThreeVector x1 = pPostStepPoint->GetPosition();
	G4double      t1 = pPostStepPoint->GetGlobalTime();
	G4double Density = aMaterial->GetDensity()/(g/cm3);
	G4double nDensity = aMaterial->GetElectronDensity()*cm3/G4double(z1);
	G4int Phase = aMaterial->GetState();
	
    if ( Phase == kStateLiquid && x0[2] > (theScintProcess->GAT-5*mm) &&
            theScintProcess->GAT != 0 ) {
        if(luxManager->GetEFieldFromFile()) {
            //G4String driftTimeFile = luxManager->GetDriftTimeFile();
           // luxManager->LoadXYZDependentDriftTime(driftTimeFile);
            G4double calculatedDriftTime =
                    luxManager->GetXYZDependentDriftTime(x0);
            aParticleChange.ProposeEnergy(
                    theScintProcess->GetLiquidElectronDriftSpeed(
                    aMaterial->GetTemperature(),
                    fabs(aMaterialPropertiesTable->GetConstProperty(
                    "ELECTRICFIELDSURFACE")/(volt/cm)),
                    MillerDriftSpeed,z1,luxManager->GetEFieldFromFile(),
                    x0[2],calculatedDriftTime));
        } else {
            aParticleChange.ProposeEnergy(
                    theScintProcess->GetLiquidElectronDriftSpeed(
                    aMaterial->GetTemperature(),
                    fabs(aMaterialPropertiesTable->
                    GetConstProperty("ELECTRICFIELDSURFACE")/(volt/cm)),
                    MillerDriftSpeed,z1,luxManager->GetEFieldFromFile(),
                    x0[2], 0.));
              }
	  aParticleChange.ProposeWeight(2.0);
	  return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
	}
	
	// absorb the electron if you are in liquid and there is non-perfect
	// purity, you turned S2 off with YieldFactor, the e- is above the
	// anode in the gas already, or it's not present in a noble element
	if ( Phase == kStateLiquid || !YieldFactor || x0[2] > z_anode ||
	     x1[2] > z_anode || fabs(x1[2]-x0[2]) < 1e-7*nm ||
	     (z1!=2 && z1!=10 && z1!=18 && z1!=36 && z1!=54) ) {
	  G4double KE = aParticle->GetKineticEnergy();
	  aParticleChange.ProposeTrackStatus(fStopAndKill);
	  aParticleChange.ProposeEnergy(0.);
	  aParticleChange.ProposeLocalEnergyDeposit(KE);
	  return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
        }
	if ( x0[2] <= z_start && x1[2] <= z_start )
	  return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
	
	G4double ElectricField;
    if(theScintProcess->WIN>0 && theScintProcess->TOP>0 &&
            theScintProcess->ANE>0 && theScintProcess->SRF>0 &&
            theScintProcess->GAT>0 && theScintProcess->CTH>0 &&
            theScintProcess->BOT>0 && theScintProcess->PMT>0)
	  ElectricField = aMaterialPropertiesTable->
	    GetConstProperty("ELECTRICFIELD");
	else
	  ElectricField = aMaterialPropertiesTable->
	    GetConstProperty("ELECTRICFIELDANODE");
        ElectricField = 
	  fabs(ElectricField/(kilovolt/cm));
	
	if ( Density > 0. && Phase == kStateGas && (x0[2] > z_start ||
	     x1[2] > z_start) && aParticleChange.GetWeight() >= 1.0 ) {
	  aParticleChange.ProposeWeight(0);
	  LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();
        G4double ExtractionField;
        if( theScintProcess->UsingLZ() ) {
            ExtractionField = ElectricField/1.96;
        } else {
            ExtractionField = fabs(luxMaterials->LiquidXe()->
                    GetMaterialPropertiesTable()->
                    GetConstProperty("ELECTRICFIELDSURFACE")/(kilovolt/cm));
        }
        
        G4double ExtractEff;
	  G4double ExtractEffA = -0.0012052 + //Aprile 2004 IEEE No.5
	    0.1638*ElectricField-0.0063782*pow(ElectricField,2.);
	  G4double ExtractEffB = 1 - 1.3558*exp(-0.074312* //Gushchin
				 pow(ExtractionField,2.4259));
        if( theScintProcess->UsingLZ() )
            ExtractEff = ExtractEffB;
        else
            ExtractEff = 0.5 * ( ExtractEffA + ExtractEffB );//average
	  if ( ElectricField >= 12. ) ExtractEff = 1.00;
	  if ( ElectricField <= 0.0 || ExtractionField < 0. ) ExtractEff=0.00;
	  if ( ExtractEff < 0 ) ExtractEff = 0;
	  if ( ExtractEff > 1 ) ExtractEff = 1;
	  if ( luxManager->GetGasRun() ) ExtractEff = 1;
        G4bool passEFieldTest;
        if( theScintProcess->UsingLZ() )
            passEFieldTest = !ElectricField || G4UniformRand() >= ExtractEff ||
            (1/E_eV[z1])*1e17*((ElectricField*1e3)/nDensity)<=thr[z1];
        else
            passEFieldTest = !ElectricField ||
            (1/E_eV[z1])*1e17*((ElectricField*1e3)/nDensity)<=thr[z1];
        if ( passEFieldTest ) {
	    aParticleChange.ProposeTrackStatus(fStopAndKill);
	    return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
	  }
	  else {
	    G4double eDrift =
          theScintProcess->GetGasElectronDriftSpeed(1000.*ElectricField,
                  nDensity);
	    aParticleChange.ProposeEnergy(eDrift);
	  }
    if ( !theScintProcess->UsingLZ() && G4UniformRand() >= ExtractEff ) {
	    G4double trainProb = G4UniformRand();
	    if ( trainProb < (0.4889*ElectricField - 2.8533) ) //e-train
	      aParticleChange.ProposeWeight(200e3*ns*log(G4UniformRand()));
	    else //super e-train
	      aParticleChange.ProposeWeight(20.e6*ns*log(G4UniformRand()));
	  } //delay "unextracted" electrons to make "e-trains"
	}
	
	if ( G4UniformRand () >= luxManager->GetS1Gain() )
          return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
	
	aParticleChange.SetNumberOfSecondaries(G4int(floor(YieldFactor)));
	if ( verboseLevel > 0 ) {
	  G4cout << "\n Exiting from G4S2Light::DoIt -- "
		 << "NumberOfSecondaries = "
		 << aParticleChange.GetNumberOfSecondaries() << G4endl;
	}
	
	// start particle creation
	G4double sampledEnergy;
	G4DynamicParticle* aQuantum;
	
	// Generate random direction
	G4double cost = 1. - 2.*G4UniformRand();
	G4double sint = std::sqrt((1.-cost)*(1.+cost));
	G4double phi = twopi*G4UniformRand();
	G4double sinp = std::sin(phi);
	G4double cosp = std::cos(phi);
	G4double px = sint*cosp; G4double py = sint*sinp;
	G4double pz = cost;
	
	// Create momentum direction vector
	G4ParticleMomentum photonMomentum(px, py, pz);
	
	// Determine polarization of new photon
	G4double sx = cost*cosp; G4double sy = cost*sinp;
	G4double sz = -sint;
	G4ThreeVector photonPolarization(sx, sy, sz);
	G4ThreeVector perp = photonMomentum.cross(photonPolarization);
	phi = twopi*G4UniformRand();
	sinp = std::sin(phi); cosp = std::cos(phi);
	photonPolarization = cosp * photonPolarization + sinp * perp;
	photonPolarization = photonPolarization.unit();
	
	// Generate a new photon:
	sampledEnergy = G4RandGauss::shoot(E_eV[z1]*eV,0.2*eV);
	if ( z1==54 && sampledEnergy>8.5*eV ) sampledEnergy = 8.5*eV;
	aQuantum = 
	  new G4DynamicParticle(G4OpticalPhoton::OpticalPhoton(),
				photonMomentum);
	aQuantum->SetPolarization(photonPolarization.x(),
				  photonPolarization.y(),
				  photonPolarization.z());
	aQuantum->SetCharge(.001); //kludge for tagging S2 photons
	if ( aParticle->GetPolarization()[2] == 1 )
	  aQuantum->SetCharge(.003);
	
	//assign energy to make particle real
	aQuantum->SetKineticEnergy(sampledEnergy);
	if (verboseLevel>1)
	  G4cout << "sampledEnergy = " << sampledEnergy << G4endl;
	
	// electroluminesence emission time distribution
	G4double aSecondaryTime = t1,
	  SingTripRatio=.1; //guess: revisit
	if(G4UniformRand()<SingTripRatio/(1+SingTripRatio))
	  aSecondaryTime -= tau1[z1]*log(G4UniformRand());
	else aSecondaryTime -= 
	       tau3[z1]*log(G4UniformRand());
	G4double OriginalEnergy = aParticle->GetPolarization()[1];
	G4double tail = 4e-4*OriginalEnergy*1e3;
	if ( G4UniformRand() <0.1 && !luxManager->GetLUXSurfaceGeometry() &&
        aParticleChange.GetWeight() >= 0 && theScintProcess->BORDER < 55*cm )
	  aSecondaryTime -= tail*ns*log(G4UniformRand());
	
	//the position of the new secondary particle
    if ( x1[2] < theScintProcess->BORDER + theScintProcess->GASGAP / 2. )
	  x1[2] += 0.001*mm;
    if ( x1[2] > theScintProcess->BORDER + theScintProcess->GASGAP / 2. )
	  x1[2] -= 0.001*mm;
	G4ThreeVector aSecondaryPosition = x1;
	
	if ( aParticleChange.GetWeight() < 0 && //e-train times
	     aParticle->GetPolarization()[2] != 1 ) {
	  aSecondaryTime -= aParticleChange.GetWeight();
	  aQuantum->SetCharge(.002); //separate tag for answer key
	}
	
	// GEANT4 business: stuff you need to make a new track
	G4Track* aSecondaryTrack = 
	  new G4Track(aQuantum,aSecondaryTime,aSecondaryPosition);
	aParticleChange.AddSecondary(aSecondaryTrack);
	
	return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
}

// GetMeanFreePath
// ---------------
G4double G4S2Light::GetMeanFreePath(const G4Track& aTrack,
                                          G4double ,
                                          G4ForceCondition* condition)
{
  const G4Material* aMaterial = aTrack.GetMaterial();
  if ( !aMaterial ) return DBL_MIN;
  const G4ElementVector* theElementVector = aMaterial->GetElementVector();
  G4Element* Element = (*theElementVector)[0]; G4int z1;
  if ( Element ) z1 = (G4int)(Element->GetZ()); else z1 = -1;
  if ( (z1==2 || z1==10 || z1==18 || z1==36 || z1==54) &&
       aMaterial->GetState() == kStateLiquid ) {
      if( theScintProcess->UsingLZ() )
          return luxManager->GetDriftElecAttenuation();
      else if ( aTrack.GetPosition()[2] < (theScintProcess->GAT-5*mm) ||
              aTrack.GetWeight() == 2 )
          return luxManager->GetDriftElecAttenuation();
      else return 0.000*mm;
  }
  else if ((z1==2 || z1==10 || z1==18 || z1==36 || z1==54) &&
   aMaterial->GetState()== kStateGas && aMaterial->GetDensity()>0.0*(g/cm3)) {
    G4MaterialPropertiesTable* aMaterialPropertiesTable =
      aMaterial->GetMaterialPropertiesTable();
    if(!aMaterialPropertiesTable) return DBL_MIN;
    if ( aTrack.GetPosition()[2] < (theScintProcess->GAT-5*mm) ) return DBL_MAX;
   G4double ElectricField = fabs(aMaterialPropertiesTable->
				  GetConstProperty("ELECTRICFIELDANODE"));
    ElectricField = ElectricField/(volt/cm);
    G4double mfp, nDensity = 
      aMaterial->GetElectronDensity()*cm3/G4double(z1);
    if ( (1/E_eV[z1])*(ElectricField/nDensity)*1e17 == thr[z1] ) mfp = 0*cm;
    else //denominator of formula OK
      mfp = 1 / 
	(nDensity*1e-17*((1/E_eV[z1])*(ElectricField/nDensity)*1e17-thr[z1]));
    //equation 8 within Monteiro's paper JINST 2007 (where 140=1/7.14.. eV)
      mfp*=cm/ConvertEff[z1];
      if(mfp<0)mfp=0;
      if(mfp>theScintProcess->GASGAP/2.7)mfp=theScintProcess->GASGAP/exp(1);
    return mfp; //mean free path for 1 photon
  }
  else {
    *condition = NotForced;
    const G4Material* aMaterial = aTrack.GetMaterial();
    if ( aMaterial->GetDensity() == GRID_DENSITY )
      return DBL_MAX; //pass electrons through grid wires
    return 0*nm; //kill elsewhere
  }
}

// GetMeanLifeTime
// ---------------
G4double G4S2Light::GetMeanLifeTime(const G4Track&,
                                          G4ForceCondition* condition)
{
  *condition = InActivated;
  return DBL_MAX;
}
