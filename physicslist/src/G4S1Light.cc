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
// Noble Element Simulation Technique "G4S1Light" physics process
//
////////////////////////////////////////////////////////////////////////
// S1 Scintillation Light Class Implementation
////////////////////////////////////////////////////////////////////////
//
// File:        G4S1Light.cc (lives in physicslist/src)
// Description: (Rest)Discrete Process - Generation of S1 Photons
// Version:     1.01 alpha 1a LUX edition
// Created:     Thursday, November 6, 2014 11:33 P.M.
// Author:      Matthew Szydagis, SUNY Albany
//              Kevin O'Sullivan, Yale University
//
// mail:        mszydagis@albany.edu, matthew.szydagis@gmail.com
//              kevin.osullivan@yale.edu
////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "G4ParticleTypes.hh" //lets you refer to G4OpticalPhoton, etc.
#include "G4EmProcessSubType.hh" //lets you call this process Scintillation
#include "G4Version.hh" //tells you what Geant4 version you are running
#include "G4S1Light.hh"

#include "LUXSimDetectorComponent.hh"
#include "LUXSimFastSim.hh"

#define MIN_ENE -1*eV //lets you turn NEST off BELOW a certain energy
#define MAX_ENE 1.*TeV //lets you turn NEST off ABOVE a certain energy
#define HIENLIM 5*MeV //energy at which Doke model used exclusively

#define R_TOL 0.2*mm //tolerance (for edge events)
G4bool diffusion = true; G4bool FastSimBool = false;

FastSim fastSim("physicslist/src/fastSimLibrary_fromKr_V04.dat",
        "physicslist/src/fastSimConnections_fromKr_V04.dat");
G4String ConvertNumberToString ( G4int pmtCall );

G4bool SinglePhase=false, ThomasImelTail=true, OutElectrons=true;
G4double GetGasElectronDriftSpeed(G4double efieldinput,G4double density);
G4double CalculateElectronLET ( G4double E, G4int Z );

G4double biExc = 0.77; //for alpha particles (bi-excitonic collisions)
G4double UnivScreenFunc ( G4double E, G4double Z, G4double A );

G4int BinomFluct(G4int N0, G4double prob); //function for doing fluctuations
int modPoisRnd ( double poisMean, double preFactor );
void InitMatPropValues ( G4MaterialPropertiesTable* nobleElementMat );

#define Density_LXe 2.888 //reference density for density-dep. effects
#define Density_LAr 1.393
#define Density_LNe 1.207
#define Density_LKr 2.413

#define DAQWIN_MS 1.*ms //plus and minus 0.5 ms before + after S2 (DAQ window)

using namespace std;

G4S1Light::G4S1Light(const G4String& processName,
		           G4ProcessType type)
      : G4VRestDiscreteProcess(processName, type)
{
        luxManager = LUXSimManager::GetManager();
        SetProcessSubType(fScintillation);
	
        fTrackSecondariesFirst = true;
	//particles die first, then scintillation is generated
	
        if (verboseLevel>0) {
	  G4cout << GetProcessName() << " is created " << G4endl;
        }
    
    SetLUXGeoValues();
}

G4S1Light::~G4S1Light(){} //destructor needed to avoid linker error

G4VParticleChange*
G4S1Light::AtRestDoIt(const G4Track& aTrack, const G4Step& aStep)

// This routine simply calls the equivalent PostStepDoIt since all the
// necessary information resides in aStep.GetTotalEnergyDeposit()
{
  return G4S1Light::PostStepDoIt(aTrack, aStep);
}

G4VParticleChange*
G4S1Light::PostStepDoIt(const G4Track& aTrack, const G4Step& aStep)
// this is the most important function, where all light & charge yields happen!
{
        aParticleChange.Initialize(aTrack);
	
	if ( !YieldFactor ) //set YF=0 when you want S1Light off in your sim
          return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
	
	if( aTrack.GetParentID() == 0 && aTrack.GetCurrentStepNumber() == 1 ) {
	  fExcitedNucleus = false; //an initialization or reset
	  fVeryHighEnergy = false; //initializes or (later) resets this
	  fAlpha = false; //ditto
	  fMultipleScattering = false;
	}

        const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
	G4ParticleDefinition *pDef = aParticle->GetDefinition();
	G4String particleName = pDef->GetParticleName();
        const G4Material* aMaterial = aStep.GetPreStepPoint()->GetMaterial();
	const G4Material* bMaterial = aStep.GetPostStepPoint()->GetMaterial();
	
	if((particleName == "neutron" || particleName == "antineutron") &&
	   aStep.GetTotalEnergyDeposit() <= 0)
	  return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);

       	// Load table of double phe probabilities in case the user wants them.
       	LoadDoublePHEProb("physicslist/src/DoublePHEperDPH.txt");
	
	// code for determining whether the present/next material is noble
	// element, or, in other words, for checking if either is a valid NEST
	// scintillating material, and save Z for later L calculation, or
	// return if no valid scintillators are found on this step, which is
	// protection against G4Exception or seg. fault/violation
	G4Element *ElementA = NULL, *ElementB = NULL;
	if (aMaterial) {
	  const G4ElementVector* theElementVector1 =
	    aMaterial->GetElementVector();
	  ElementA = (*theElementVector1)[0];
	}
	if (bMaterial) {
	  const G4ElementVector* theElementVector2 =
	    bMaterial->GetElementVector();
	  ElementB = (*theElementVector2)[0];
	}
	G4int z1,z2,j=1; G4bool NobleNow=false,NobleLater=false;
	if (ElementA) z1 = (G4int)(ElementA->GetZ()); else z1 = -1;
	if (ElementB) z2 = (G4int)(ElementB->GetZ()); else z2 = -1;
	if ( z1==2 || z1==10 || z1==18 || z1==36 || z1==54 ) {
	  NobleNow = true;
	  j = (G4int)aMaterial->GetMaterialPropertiesTable()->
	    GetConstProperty("TOTALNUM_INT_SITES"); //get current number
	  if ( j < 0 ) {
	    InitMatPropValues(aMaterial->GetMaterialPropertiesTable());
	    j = 0; //no sites yet
	  } //material properties initialized
	} //end of atomic number check
	if ( z2==2 || z2==10 || z2==18 || z2==36 || z2==54 ) {
	  NobleLater = true;
	  j = (G4int)bMaterial->GetMaterialPropertiesTable()->
	    GetConstProperty("TOTALNUM_INT_SITES");
	  if ( j < 0 ) {
	    InitMatPropValues(bMaterial->GetMaterialPropertiesTable());
	    j = 0; //no sites yet
	  } //material properties initialized
	} //end of atomic number check
	
	if ( !NobleNow && !NobleLater )
	  return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
	
	// retrieval of the particle's position, time, attributes at both the 
	// beginning and the end of the current step along its track
        G4StepPoint* pPreStepPoint  = aStep.GetPreStepPoint();
	G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();
	G4ThreeVector x1 = pPostStepPoint->GetPosition();
        G4ThreeVector x0 = pPreStepPoint->GetPosition();
	G4double evtStrt = pPreStepPoint->GetGlobalTime();
        G4double      t0 = pPreStepPoint->GetLocalTime();
	G4double      t1 = pPostStepPoint->GetLocalTime();
	G4double driftTime = DBL_MIN; //just an initialization of variable
	
	// now check if we're entering a scintillating material (inside) or
        // leaving one (outside), in order to determine (later on in the code,
        // based on the booleans inside & outside) whether to add/subtract
        // energy that can potentially be deposited from the system
	G4bool outside = false, inside = false, InsAndOuts = false;
        G4MaterialPropertiesTable* aMaterialPropertiesTable =
	  aMaterial->GetMaterialPropertiesTable();
	if ( NobleNow && !NobleLater ) outside = true;
	if ( !NobleNow && NobleLater ) {
	  aMaterial = bMaterial; inside = true; z1 = z2;
	  aMaterialPropertiesTable = bMaterial->GetMaterialPropertiesTable();
	}
	if ( NobleNow && NobleLater && 
	     aMaterial->GetDensity() != bMaterial->GetDensity() )
	  InsAndOuts = true;
	
	//      Get the LUXSimMaterials pointer
        LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();
	
	// retrieve scintillation-related material properties
	G4double Density = aMaterial->GetDensity()/(g/cm3);
	G4double nDensity = aMaterial->GetElectronDensity()*cm3/G4double(z1);
	G4int Phase = aMaterial->GetState(); //solid, liquid, or gas?
	G4double ElectricField=0., FieldSign; //for field quenching of S1
	G4bool GlobalFields = false;
        LUXSimManager* luxManager = LUXSimManager::GetManager();
        G4bool EFieldFromFile = luxManager->GetEFieldFromFile();
        G4bool DriftTimeFromFile = luxManager->GetDriftTimeFromFile();
        G4bool RadialDriftFromFile = luxManager->GetRadialDriftFromFile();
        if (EFieldFromFile) {
          G4String eFieldFile  = luxManager->GetEFieldFile();
          luxManager->LoadXYZDependentEField(eFieldFile);
        }
        if (DriftTimeFromFile) {
          G4String driftTimeFile  = luxManager->GetDriftTimeFile();
          luxManager->LoadXYZDependentDriftTime(driftTimeFile);
        }
        if (RadialDriftFromFile) {
          G4String radialDriftFile  = luxManager->GetRadialDriftFile();
          luxManager->LoadXYZDependentRadialDrift(radialDriftFile);
        }
	if ( WIN>0 && TOP>0 && ANE>0 && SRF>0 && GAT>0 && CTH>0 && BOT>0 && PMT>0 ) {
          ElectricField = aMaterialPropertiesTable->GetConstProperty("ELECTRICFIELD");
        }
	else {
	  if ( x1[2] < WIN && x1[2] > TOP && Phase == kStateGas ) {
            ElectricField = aMaterialPropertiesTable->GetConstProperty("ELECTRICFIELDWINDOW");
          }
	  else if ( x1[2] < TOP && x1[2] > ANE && Phase == kStateGas ) {
            ElectricField = aMaterialPropertiesTable->GetConstProperty("ELECTRICFIELDTOP");
          }
	  else if ( x1[2] < ANE && x1[2] > SRF && Phase == kStateGas ) {
            ElectricField = aMaterialPropertiesTable->GetConstProperty("ELECTRICFIELDANODE");
          }
	  else if ( Phase == kStateLiquid ) {
            if (EFieldFromFile) {
  	      if ( x1[2] < TOP && x1[2] > PMT ) {
                ElectricField = luxManager->GetXYZDependentElectricField (x1);
              }
              else {
                ElectricField = 0;
              }
            }
	    else if ( x1[2] < SRF && x1[2] > GAT ) {
              ElectricField = aMaterialPropertiesTable->GetConstProperty("ELECTRICFIELDSURFACE");
            }
	    else if ( x1[2] < GAT && x1[2] > CTH ) {
              ElectricField = aMaterialPropertiesTable->GetConstProperty("ELECTRICFIELDGATE");
            }
  	    else if ( x1[2] < CTH && x1[2] > BOT ) {
              ElectricField = aMaterialPropertiesTable->GetConstProperty("ELECTRICFIELDCATHODE");
            }
	    else if ( x1[2] < BOT && x1[2] > PMT ) {
              ElectricField = aMaterialPropertiesTable->GetConstProperty("ELECTRICFIELDBOTTOM");
            }
          }
	  else if ( x1[2] < SRF && x1[2] > GAT && luxManager->GetGasRun() ) {
            ElectricField=luxMaterials->LiquidXe()->GetMaterialPropertiesTable()->GetConstProperty("ELECTRICFIELDSURFACE");
          }
	  else if ( x1[2] < GAT && x1[2] > CTH && luxManager->GetGasRun() ) {
            ElectricField=luxMaterials->LiquidXe()->GetMaterialPropertiesTable()->GetConstProperty("ELECTRICFIELDGATE");
          }
	  else if ( x1[2] < CTH && x1[2] > BOT && luxManager->GetGasRun() ) {
            ElectricField=luxMaterials->LiquidXe()->GetMaterialPropertiesTable()->GetConstProperty("ELECTRICFIELDCATHODE");
          }
	  else if ( x1[2] < BOT && x1[2] > PMT && luxManager->GetGasRun() ) {
            ElectricField=luxMaterials->LiquidXe()->GetMaterialPropertiesTable()->GetConstProperty("ELECTRICFIELDBOTTOM");
          }
	  else {
            ElectricField = aMaterialPropertiesTable->GetConstProperty("ELECTRICFIELD");
          }
	}
        if ( ElectricField >= 0 ) FieldSign = 1; else FieldSign = -1;
        if (luxManager->GetDriftTimeFromFile() &&
            luxManager->GetXYZDependentDriftTime(x1) < 0) { // if drifttime is negative, produce no S2
            FieldSign = 1;
        }
	ElectricField = fabs((1e3*ElectricField)/(kilovolt/cm));
	G4double Temperature = aMaterial->GetTemperature();
	G4double ScintillationYield, ResolutionScale, R0 = 1.0*um,
	  DokeBirks[3], ThomasImel = 0.00, delta = 1*mm;
	DokeBirks[0] = 0.00; DokeBirks[2] = 1.00;
	G4double PhotMean = 7*eV, PhotWidth = 1.0*eV; //photon properties
	G4double SingTripRatioR, SingTripRatioX, tau1, tau3, tauR = 0*ns;
	switch ( z1 ) { //sort prop. by noble element atomic#
	case 2: //helium
	  ScintillationYield = 1 / (41.3*eV); //all W's from noble gas book
	  ExcitationRatio = 0.00; //nominal (true value unknown)
	  ResolutionScale = 0.2; //Aprile, Bolotnikov, Bolozdynya, Doke
	  PhotMean = 15.9*eV;
	  tau1 = G4RandGauss::shoot(10.0*ns,0.0*ns);
	  tau3 = 1.6e3*ns;
          tauR = G4RandGauss::shoot(13.00*s,2.00*s); //McKinsey et al. 2003
	  break;
	case 10: //neon
	  ScintillationYield = 1 / (29.2*eV);
	  ExcitationRatio = 0.00; //nominal (true value unknown)
	  ResolutionScale = 0.13; //Aprile et. al book
	  PhotMean = 15.5*eV; PhotWidth = 0.26*eV;
	  tau1 = G4RandGauss::shoot(10.0*ns,10.*ns);
          tau3 = G4RandGauss::shoot(15.4e3*ns,200*ns); //Nikkel et al. 2008
	  break;
	case 18: //argon
	  ScintillationYield = 1 / (19.5*eV);
	  ExcitationRatio = 0.21; //Aprile et. al book
	  ResolutionScale = 0.107; //Doke 1976
	  R0 = 1.568*um; //Mozumder 1995
	  if(ElectricField) {
	    ThomasImel = 0.156977*pow(ElectricField,-0.1);
	    DokeBirks[0] = 0.07*pow((ElectricField/1.0e3),-0.85);
	    DokeBirks[2] = 0.00;
	  }
	  else {
	    ThomasImel = 0.099;
	    DokeBirks[0] = 0.0003;
	    DokeBirks[2] = 0.75;
	  }
	  PhotMean = 9.69*eV; PhotWidth = 0.22*eV;
	  tau1 = G4RandGauss::shoot(6.5*ns,0.8*ns); //err from wgted avg.
	  tau3 = G4RandGauss::shoot(1300*ns,50*ns); //ibid.
	  tauR = G4RandGauss::shoot(0.8*ns,0.2*ns); //Kubota 1979
	  biExc = 0.6; break;
	case 36: //krypton
	  if ( Phase == kStateGas ) ScintillationYield = 1 / (30.0*eV);
	  else ScintillationYield = 1 / (15.0*eV);
	  ExcitationRatio = 0.08; //Aprile et. al book
	  ResolutionScale = 0.05; //Doke 1976
	  PhotMean = 8.43*eV;
	  tau1 = G4RandGauss::shoot(2.8*ns,.04*ns);
	  tau3 = G4RandGauss::shoot(93.*ns,1.1*ns);
	  tauR = G4RandGauss::shoot(12.*ns,.76*ns);
	  break;
	case 54: //xenon
	default:
	  ScintillationYield = 1. / (13.7*eV);
	  ExcitationRatio = (0.059813+0.031228*Density)*(1. - exp(-3.8288 * aStep.GetTotalEnergyDeposit()/keV + 0.73515));
	  if ( ExcitationRatio < 0. ) ExcitationRatio = 0.;
	  ResolutionScale = 1.00 * //Fano factor <<1
	    (0.12724-0.032152*Density-0.0013492*pow(Density,2.));
	  //~0.1 for GXe w/ formula from Bolotnikov et al. 1995
	  if ( Phase == kStateLiquid ) {
	    ResolutionScale *= 1.5; //to get it to be ~0.03 for LXe
	    R0 = 16.6*um; //for zero electric field
	    //length scale above which Doke model used instead of Thomas-Imel
	    if(ElectricField) //change it with field (see NEST paper)
	      R0 = 69.492*pow(ElectricField,-0.50422)*um;
	    if(ElectricField) { //formulae & values all from NEST paper
	      DokeBirks[0]= 19.171*pow(ElectricField+25.552,-0.83057)+0.026772;
	      DokeBirks[2] = 0.00; //only volume recombination (above)
	    }
	    else { //zero electric field magnitude
	      DokeBirks[0] = 0.18; //volume/columnar recombination factor (A)
	      DokeBirks[2] = 0.58; //geminate/Onsager recombination (C)
	    }
	    //"ThomasImel" is alpha/(a^2*v), the recomb. coeff.
	    ThomasImel = 0.05; //aka xi/(4*N_i) from the NEST paper
	    //distance used to determine when one is at a new interaction site
	    delta = 0.4*mm; //distance ~30 keV x-ray travels in LXe
	    PhotMean = 6.97*eV; PhotWidth = 0.23*eV;
	    // 178+/-14nmFWHM, taken from Jortner JchPh 42 '65.
	    //these singlet and triplet times may not be the ones you're
	    //used to, but are the world average: Kubota 79, Hitachi 83 (2
	    //data sets), Teymourian 11, Morikawa 89, and Akimov '02
	    tau1 = G4RandGauss::shoot(3.1*ns,.7*ns); //err from wgted avg.
	    tau3 = G4RandGauss::shoot(24.*ns,1.*ns); //ibid.
	  } //end liquid
	  else if ( Phase == kStateGas ) {
	    if(!fAlpha) ExcitationRatio=0.07; //Nygren NIM A 603 (2009) p. 340
	    else { biExc = 1.00;
	      ScintillationYield = 1 / (12.98*eV); } //Saito 2003
	    R0 = 0.0*um; //all Doke/Birks interactions (except for alphas)
	    G4double Townsend = (ElectricField/nDensity)*1e17;
	    DokeBirks[0] = 0.0000; //all geminate (except at zero, low fields)
	    DokeBirks[2] = 0.1933*pow(Density,2.6199)+0.29754 - 
	      (0.045439*pow(Density,2.4689)+0.066034)*log10(ElectricField);
	    if ( ElectricField>6990 ) DokeBirks[2]=0.0;
	    if ( ElectricField<1000 ) DokeBirks[2]=0.2;
	    if ( ElectricField<100. ) { DokeBirks[0]=0.18; DokeBirks[2]=0.58; }
	    if( Density < 0.061 ) ThomasImel = 0.041973*pow(Density,1.8105);
	    else if( Density >= 0.061 && Density <= 0.167 )
	      ThomasImel=5.9583e-5+0.0048523*Density-0.023109*pow(Density,2.);
	    else ThomasImel = 6.2552e-6*pow(Density,-1.9963);
	  if(ElectricField)ThomasImel=1.2733e-5*pow(Townsend/Density,-0.68426);
	    // field\density dependence from Kobayashi 2004 and Saito 2003
	    PhotMean = 7.1*eV; PhotWidth = 0.2*eV;
	    tau1 = G4RandGauss::shoot(5.18*ns,1.55*ns);
	    tau3 = G4RandGauss::shoot(100.1*ns,7.9*ns);
	  } //end gas information (preliminary guesses)
	  else {
	    tau1 = 3.5*ns; tau3 = 20.*ns; tauR = 40.*ns;
	  } //solid Xe
	}
	
	// log present and running tally of energy deposition in this section
	G4double anExcitationEnergy = ((const G4Ions*)(pDef))->
	  GetExcitationEnergy(); //grab nuclear energy level
        G4double TotalEnergyDeposit = //total energy deposited so far
          aMaterialPropertiesTable->GetConstProperty( "ENERGY_DEPOSIT_TOT" );
	G4bool convert = false, annihil = false;
	//set up special cases for pair production and positron annihilation
	if(pPreStepPoint->GetKineticEnergy()>=(2*electron_mass_c2) && 
	   !pPostStepPoint->GetKineticEnergy() && 
	   !aStep.GetTotalEnergyDeposit() && aParticle->GetPDGcode()==22) {
	  convert = true; TotalEnergyDeposit = electron_mass_c2;
	}
	if(pPreStepPoint->GetKineticEnergy() && 
	   !pPostStepPoint->GetKineticEnergy() && 
	   aParticle->GetPDGcode()==-11) {
	  annihil = true; TotalEnergyDeposit += aStep.GetTotalEnergyDeposit();
	}
	G4bool either = false;
        if(inside || outside || convert || annihil || InsAndOuts) either=true;
        //conditions for returning when energy deposits too low
        if( anExcitationEnergy<100*eV && aStep.GetTotalEnergyDeposit()<1*eV &&
	   !either && !fExcitedNucleus )
          return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
	//add current deposit to total energy budget
        if ( !annihil ) TotalEnergyDeposit += aStep.GetTotalEnergyDeposit();
        if ( !convert ) aMaterialPropertiesTable->
	  AddConstProperty( "ENERGY_DEPOSIT_TOT", TotalEnergyDeposit );
	//save current deposit for determining number of quanta produced now
        TotalEnergyDeposit = aStep.GetTotalEnergyDeposit();
	
	// check what the current "goal" E is for dumping scintillation,
	// often the initial kinetic energy of the parent particle, and deal
	// with all other energy-related matters in this block of code
	G4double InitialKinetEnergy = aMaterialPropertiesTable->
	  GetConstProperty( "ENERGY_DEPOSIT_GOL" );
	//if zero, add up initial potential and kinetic energies now
        if ( InitialKinetEnergy == 0 ) {
	  G4double tE = pPreStepPoint->GetKineticEnergy()+anExcitationEnergy;
	  if ( (fabs(tE-1.8*keV) < 1e-10*eV || fabs(tE-9.4*keV) < 1e-10*eV) &&
	       Phase == kStateLiquid && z1 == 54 ) {
	    tE = 9.4*keV; t1 = aParticle->GetPolarization()[0]*ns; fKr83m=0;
	  }
	  if ( fKr83m && ElectricField != 0 && !luxManager->GetGasRun() )
	    DokeBirks[2] = 0.20;
          aMaterialPropertiesTable->
	    AddConstProperty ( "ENERGY_DEPOSIT_GOL", tE );
	  //excited nucleus is special case where accuracy reduced for total
          //energy deposition because of G4 inaccuracies and scintillation is
	  //forced-dumped when that nucleus is fully de-excited
          if ( anExcitationEnergy ) fExcitedNucleus = true;
	}
	//if a particle is leaving, remove its kinetic energy from the goal
	//energy, as this will never get deposited (if depositable)
	if(outside){ aMaterialPropertiesTable->
	    AddConstProperty("ENERGY_DEPOSIT_GOL",
	    InitialKinetEnergy-pPostStepPoint->GetKineticEnergy());
	  if(aMaterialPropertiesTable->
	     GetConstProperty("ENERGY_DEPOSIT_GOL")<0)
            aMaterialPropertiesTable->AddConstProperty("ENERGY_DEPOSIT_GOL",0);
	}
	//if a particle is coming back into your scintillator, then add its
	//energy to the goal energy
	if(inside) { aMaterialPropertiesTable->
	    AddConstProperty("ENERGY_DEPOSIT_GOL",
	    InitialKinetEnergy+pPreStepPoint->GetKineticEnergy());
	  if ( TotalEnergyDeposit > 0 && InitialKinetEnergy == 0 ) {
            aMaterialPropertiesTable->AddConstProperty("ENERGY_DEPOSIT_GOL",0);
            TotalEnergyDeposit = .000000;
          }
        }
	if ( InsAndOuts ) {
	  aMaterialPropertiesTable->
	    AddConstProperty("ENERGY_DEPOSIT_GOL",(-0.1*keV)+
	    InitialKinetEnergy-pPostStepPoint->GetKineticEnergy());
	  InitialKinetEnergy = bMaterial->GetMaterialPropertiesTable()->
	    GetConstProperty("ENERGY_DEPOSIT_GOL");
	  bMaterial->GetMaterialPropertiesTable()->
	    AddConstProperty("ENERGY_DEPOSIT_GOL",(-0.1*keV)+
	    InitialKinetEnergy+pPreStepPoint->GetKineticEnergy());
	  if(aMaterialPropertiesTable->
	     GetConstProperty("ENERGY_DEPOSIT_GOL")<0)
	    aMaterialPropertiesTable->AddConstProperty("ENERGY_DEPOSIT_GOL",0);
	  if ( bMaterial->GetMaterialPropertiesTable()->
	     GetConstProperty("ENERGY_DEPOSIT_GOL") < 0 )
	    bMaterial->GetMaterialPropertiesTable()->
	     AddConstProperty ( "ENERGY_DEPOSIT_GOL", 0 );
        }
	InitialKinetEnergy = aMaterialPropertiesTable->
	  GetConstProperty("ENERGY_DEPOSIT_GOL"); //grab current goal E
	if ( annihil ) //if an annihilation occurred, add energy of two gammas
	  InitialKinetEnergy += 2*electron_mass_c2;
	//if pair production occurs, then subtract energy to cancel with the
	//energy that will be added in the line above when the e+ dies
	if ( convert )
	  InitialKinetEnergy -= 2*electron_mass_c2;
	//update the relevant material property (goal energy)
	aMaterialPropertiesTable->
	  AddConstProperty("ENERGY_DEPOSIT_GOL",InitialKinetEnergy);
	if (anExcitationEnergy < 1e-100 && aStep.GetTotalEnergyDeposit()==0 &&
	aMaterialPropertiesTable->GetConstProperty("ENERGY_DEPOSIT_GOL")==0 &&
	aMaterialPropertiesTable->GetConstProperty("ENERGY_DEPOSIT_TOT")==0)
	  return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
	
	G4String procName;
        if ( aTrack.GetCreatorProcess() )
          procName = aTrack.GetCreatorProcess()->GetProcessName();
        else
          procName = "NULL";
        if ( procName == "eBrem" && outside && !OutElectrons )
          fMultipleScattering = true;
	
	// next 2 codeblocks deal with position-related things
	if ( fAlpha || InitialKinetEnergy == 9.4*keV ) delta = 1000.*km;
	G4int i, k, counter = 0; G4double pos[3];
	if ( outside ) { //leaving
	  if ( aParticle->GetPDGcode() == 11 && !OutElectrons )
	    fMultipleScattering = true;
	  x1 = x0; //prevents generation of quanta outside active volume
	} //no scint. for e-'s that leave
	
	char xCoord[80]; char yCoord[80]; char zCoord[80];
	G4bool exists = false; //for querying whether set-up of new site needed
	for(i=0;i<j;i++) { //loop over all saved interaction sites
	  counter = i; //save site# for later use in storing properties
	  sprintf(xCoord,"POS_X_%d",i); sprintf(yCoord,"POS_Y_%d",i);
	  sprintf(zCoord,"POS_Z_%d",i);
	  pos[0] = aMaterialPropertiesTable->GetConstProperty(xCoord);
	  pos[1] = aMaterialPropertiesTable->GetConstProperty(yCoord);
	  pos[2] = aMaterialPropertiesTable->GetConstProperty(zCoord);
	  if ( sqrt(pow(x1[0]-pos[0],2.)+pow(x1[1]-pos[1],2.)+
		    pow(x1[2]-pos[2],2.)) < delta ) {
	    exists = true; break; //we find interaction is close to an old one
	  }
	}
	if(!exists && TotalEnergyDeposit) { //current interaction too far away
	  counter = j;
	  sprintf(xCoord,"POS_X_%i",j); sprintf(yCoord,"POS_Y_%i",j); 
	  sprintf(zCoord,"POS_Z_%i",j);
	  //save 3-space coordinates of the new interaction site
	  aMaterialPropertiesTable->AddConstProperty( xCoord, x1[0] );
	  aMaterialPropertiesTable->AddConstProperty( yCoord, x1[1] );
	  aMaterialPropertiesTable->AddConstProperty( zCoord, x1[2] );
	  j++; //increment number of sites
          aMaterialPropertiesTable-> //save
            AddConstProperty( "TOTALNUM_INT_SITES", j );
	}
	
	// this is where nuclear recoil "L" factor is handled: total yield is
	// reduced for nuclear recoil as per Lindhard theory
	
	//we assume you have a mono-elemental scintillator only
	//now, grab A's and Z's of current particle and of material (avg)
	G4double a1 = ElementA->GetA();
	z2 = pDef->GetAtomicNumber(); 
	G4double a2 = (G4double)(pDef->GetAtomicMass());
	if ( particleName == "alpha" || (z2 == 2 && a2 == 4) )
          fAlpha = true; //used later to get S1 pulse shape correct for alpha
	if ( fAlpha || abs(aParticle->GetPDGcode()) == 2112 )
          a2 = a1; //get average A for element at hand
	G4double epsilon = 11.5*(TotalEnergyDeposit/keV)*pow(z1,(-7./3.));
	G4double gamma = 3.*pow(epsilon,0.15)+0.7*pow(epsilon,0.6)+epsilon;
	G4double kappa = 0.1394*sqrt(a2/131.293);//0.133*pow(z1,(2./3.))*pow(a2,(-1./2.))*(2./3.);
        if ( (z1 == z2 && z1 == 54) )
            kappa = 0.1735; // 2015-11-27 - Brian L.
	//check if we are dealing with nuclear recoil (Z same as material)
	if ( (z1 == z2 && pDef->GetParticleType() == "nucleus" &&
	      !fExcitedNucleus) ||
	     particleName == "neutron" || particleName == "antineutron" ) {
          //YieldFactor=UnivScreenFunc(TotalEnergyDeposit/keV, z1, a1);
          YieldFactor=( kappa * gamma ) / ( 1 + kappa * gamma ); // 2015-11-23 - Brian L.
	  if ( z1 == 18 && Phase == kStateLiquid )
	    YieldFactor=0.23*(1+exp(-5*epsilon)); //liquid argon L_eff
	  //just a few safety checks, like for recombProb below
	  if ( YieldFactor > 1 ) YieldFactor = 1;
	  if ( YieldFactor < 0 ) YieldFactor = 0;
	  if ( ElectricField == 0 && Phase == kStateLiquid ) {
	    if ( z1 == 54 ) ElectricField = 1.03;
	    if ( z1 == 18 ) ThomasImel = 0.25;
	  } //special TIB parameters for nuclear recoil only, in LXe and LAr
          //ExcitationRatio = 0.439;
          ExcitationRatio = 0.482; // 2015-11-27 - Brian L.
	}
	else YieldFactor = 1.000; //default
	
	// determine ultimate #quanta from current E-deposition (ph+e-)
	//G4double MeanNumberOfQuanta = //total mean number of exc/ions
	//ScintillationYield*TotalEnergyDeposit;
	//the total number of either quanta produced is equal to product of the
	//work function, the energy deposited, and yield reduction, for NR
	//G4double sigma = sqrt(ResolutionScale*MeanNumberOfQuanta); //Fano
	G4int NumQuanta = //stochastic variation in NumQuanta
	  int(floor(ScintillationYield*TotalEnergyDeposit+0.5));
	
	//if E below work function, can't make any quanta, and if NumQuanta
	//less than zero because Gaussian fluctuated low, update to zero
	if(TotalEnergyDeposit < 1/ScintillationYield || NumQuanta < 0)
	  NumQuanta = 0;

	// next section binomially assigns quanta to excitons and ions
	G4int NumExcitons = 
	  BinomFluct(NumQuanta,ExcitationRatio/(1+ExcitationRatio));
	G4int NumIons = NumQuanta - NumExcitons;
	if ( YieldFactor < 1 ) {
	  NumIons = modPoisRnd(ScintillationYield*TotalEnergyDeposit*YieldFactor*(1/(1+ExcitationRatio)),1.);
	  NumExcitons = modPoisRnd(ScintillationYield*TotalEnergyDeposit*YieldFactor*(ExcitationRatio/(1+ExcitationRatio)),1.);
	  NumQuanta = NumExcitons + NumIons;
	}
	
	// this section calculates recombination following the modified Birks'
	// Law of Doke, deposition by deposition, and may be overridden later
	// in code if a low enough energy necessitates switching to the 
	// Thomas-Imel box model for recombination instead (determined by site)
	G4double dE, dx=0, LET=0, recombProb;
	dE = TotalEnergyDeposit/MeV;
	if ( particleName != "e-" && particleName != "e+" && z1 != z2 &&
	     particleName != "mu-" && particleName != "mu+" ) {
	  //in other words, if it's a gamma,ion,proton,alpha,pion,et al. do not
	  //use the step length provided by Geant4 because it's not relevant,
	  //instead calculate an estimated LET and range of the electrons that
	  //would have been produced if Geant4 could track them
	  LET = CalculateElectronLET( 1000*dE, z1 );
	  if(LET) dx = dE/(Density*LET); //find the range based on the LET
	  if(abs(aParticle->GetPDGcode())==2112) dx=0;
        }
        else { //normal case of an e-/+ energy deposition recorded by Geant
	  dx = aStep.GetStepLength()/cm;
	  if(dx) LET = (dE/dx)*(1/Density); //lin. energy xfer (prop. to dE/dx)
	  if ( LET > 0 && dE > 0 && dx > 0 ) {
	    G4double ratio = CalculateElectronLET(dE*1e3,z1)/LET;
	    if ( j == 1 && ratio < 0.7 && !ThomasImelTail && 
		 particleName == "e-" ) {
	      dx /= ratio; LET *= ratio; }}
	}
	DokeBirks[1] = DokeBirks[0]/(1-DokeBirks[2]); //B=A/(1-C) (see paper)
	//Doke/Birks' Law as spelled out in the NEST paper
	recombProb = (DokeBirks[0]*LET)/(1+DokeBirks[1]*LET)+DokeBirks[2];
	if ( Phase == kStateLiquid ) {
//	  if ( z1 == 54 ) recombProb *= (Density/Density_LXe);
	  if ( z1 == 18 ) recombProb *= (Density/Density_LAr);
	}
	//check against unphysicality resulting from rounding errors
	if(recombProb<0) recombProb=0;
	if(recombProb>1) recombProb=1;
	//use binomial distribution to assign photons, electrons, where photons
	//are excitons plus recombined ionization electrons, while final
	//collected electrons are the "escape" (non-recombined) electrons
	G4int NumPhotons = NumExcitons + BinomFluct(NumIons,recombProb);
	G4int NumElectrons = NumQuanta - NumPhotons;
	
	// next section increments the numbers of excitons, ions, photons, and
	// electrons for the appropriate interaction site; it only appears to
	// be redundant by saving seemingly no longer needed exciton and ion
	// counts, these having been already used to calculate the number of ph
	// and e- above, whereas it does need this later for Thomas-Imel model
	char numExc[80]; char numIon[80]; char numPho[80]; char numEle[80];
	sprintf(numExc,"N_EXC_%i",counter); sprintf(numIon,"N_ION_%i",counter);
	NumExcitons += (G4int)aMaterialPropertiesTable->
	  GetConstProperty( numExc );
        NumIons     += (G4int)aMaterialPropertiesTable->
	  GetConstProperty( numIon );
        aMaterialPropertiesTable->AddConstProperty( numExc, NumExcitons );
        aMaterialPropertiesTable->AddConstProperty( numIon, NumIons     );
        sprintf(numPho,"N_PHO_%i",counter); sprintf(numEle,"N_ELE_%i",counter);
	NumPhotons   +=(G4int)aMaterialPropertiesTable->
	  GetConstProperty( numPho );
	NumElectrons +=(G4int)aMaterialPropertiesTable->
	  GetConstProperty( numEle );
	aMaterialPropertiesTable->AddConstProperty( numPho, NumPhotons   );
	aMaterialPropertiesTable->AddConstProperty( numEle, NumElectrons );
	
	// increment and save the total track length, and save interaction
	// times for later, when generating the scintillation quanta
	char trackL[80]; char time00[80]; char time01[80]; char energy[80];
	sprintf(trackL,"TRACK_%i",counter); sprintf(energy,"ENRGY_%i",counter);
	sprintf(time00,"TIME0_%i",counter); sprintf(time01,"TIME1_%i",counter);
	delta = aMaterialPropertiesTable->GetConstProperty( trackL );
	G4double energ = aMaterialPropertiesTable->GetConstProperty( energy );
	delta += dx*cm; energ += dE*MeV;
	aMaterialPropertiesTable->AddConstProperty( trackL, delta );
	aMaterialPropertiesTable->AddConstProperty( energy, energ );
	if ( TotalEnergyDeposit > 0 ) {
	  G4double deltaTime = aMaterialPropertiesTable->
	    GetConstProperty( time00 );
	  //for charged particles, which continuously lose energy, use initial
	  //interaction time as the minimum time, otherwise use only the final
	  if( aParticle->GetCharge() != 0 || InitialKinetEnergy == 9.4*keV ) {
	    if (t0 < deltaTime)
	      aMaterialPropertiesTable->AddConstProperty( time00, t0 );
	  }
	  else {
	    if (t1 < deltaTime)
	      aMaterialPropertiesTable->AddConstProperty( time00, t1 );
	  }
	  deltaTime = aMaterialPropertiesTable->GetConstProperty( time01 );
	  //find the maximum possible scintillation "birth" time
	  if (t1 > deltaTime)
	    aMaterialPropertiesTable->AddConstProperty( time01, t1 );
	}
	
	// begin the process of setting up creation of scint./ionization
	TotalEnergyDeposit=aMaterialPropertiesTable->
	  GetConstProperty("ENERGY_DEPOSIT_TOT"); //get the total E deposited
	InitialKinetEnergy=aMaterialPropertiesTable->
	  GetConstProperty("ENERGY_DEPOSIT_GOL"); //E that should have been
	if(InitialKinetEnergy > HIENLIM && 
	   abs(aParticle->GetPDGcode()) != 2112) fVeryHighEnergy=true;
	G4double safety; //margin of error for TotalE.. - InitialKinetEnergy
	if (fVeryHighEnergy && !fExcitedNucleus) safety = 0.2*keV;
	else safety = 2.*eV;
	
	//force a scintillation dump for NR and for full nuclear de-excitation
	if( !anExcitationEnergy && pDef->GetParticleType() == "nucleus" && 
	    aTrack.GetTrackStatus() != fAlive && !fAlpha )
	  InitialKinetEnergy = TotalEnergyDeposit;
	if ( particleName == "neutron" || particleName == "antineutron" )
	  InitialKinetEnergy = TotalEnergyDeposit;
	
	//force a dump of all saved scintillation under the following
	//conditions: energy goal reached, and current particle dead, or an 
	//error has occurred and total has exceeded goal (shouldn't happen)
	//if( fabs(TotalEnergyDeposit-InitialKinetEnergy)<safety || 
	//  TotalEnergyDeposit>=InitialKinetEnergy ){
	if ( 1 ) {
	  dx = 0; dE = 0;
	  //calculate the total number of quanta from all sites and all
	  //interactions so that the number of secondaries gets set correctly
	  NumPhotons = 0; NumElectrons = 0;
	  for(i=0;i<j;i++) {
	    sprintf(numPho,"N_PHO_%d",i); sprintf(numEle,"N_ELE_%d",i);
	    NumPhotons  +=(G4int)aMaterialPropertiesTable->
	      GetConstProperty( numPho );
            NumElectrons+=(G4int)aMaterialPropertiesTable->
	      GetConstProperty( numEle );
	    sprintf(trackL,"TRACK_%d",i); sprintf(energy,"ENRGY_%d",i);
	    //add up track lengths of all sites, for a total LET calc (later)
            dx += aMaterialPropertiesTable->GetConstProperty(trackL);
	    dE += aMaterialPropertiesTable->GetConstProperty(energy);
	  }
	  if ( luxManager->GetS1Gain() < 1. && luxManager->GetS2Gain() < 1. )
	    FastSimBool = true;
	  G4int buffer = 100; if(fVeryHighEnergy && !FastSimBool) buffer=1;
	  aParticleChange.SetNumberOfSecondaries(
	    buffer*(NumPhotons+NumElectrons)); G4int TotElec = NumElectrons;
	  if (fTrackSecondariesFirst) {
	    if (aTrack.GetTrackStatus() == fAlive )
	      aParticleChange.ProposeTrackStatus(fSuspend);
	  }
	  
	  // begin the loop over all sites which generates all the quanta
	  for(i=0;i<j;i++) {
	    // get the position X,Y,Z, exciton and ion numbers, total track 
	    // length of the site, and interaction times
	    sprintf(xCoord,"POS_X_%d",i); sprintf(yCoord,"POS_Y_%d",i);
	    sprintf(zCoord,"POS_Z_%d",i);
	    sprintf(numExc,"N_EXC_%d",i); sprintf(numIon,"N_ION_%d",i);
	    sprintf(numPho,"N_PHO_%d",i); sprintf(numEle,"N_ELE_%d",i);
	    NumExcitons = (G4int)aMaterialPropertiesTable->
	      GetConstProperty( numExc );
	    NumIons     = (G4int)aMaterialPropertiesTable->
	      GetConstProperty( numIon );
	    sprintf(trackL,"TRACK_%d",i); sprintf(energy,"ENRGY_%d",i);
	    sprintf(time00,"TIME0_%d",i); sprintf(time01,"TIME1_%d",i);
	    delta = aMaterialPropertiesTable->GetConstProperty( trackL );
	    energ = aMaterialPropertiesTable->GetConstProperty( energy );
	    t0 = aMaterialPropertiesTable->GetConstProperty( time00 );
	    t1 = aMaterialPropertiesTable->GetConstProperty( time01 );
	    
	    //if site is small enough, override the Doke/Birks' model with
	    //Thomas-Imel, but not if we're dealing with super-high energy 
	    //particles, and if it's NR force Thomas-Imel (though NR should be
	    //already short enough in track even up to O(100) keV)
            G4double tibCurlZ;
	    if ( 0 ) {
	      if( z1 == 54 && ElectricField && //see NEST paper for ER formula
		  Phase == kStateLiquid ) {
		if ( abs ( z1 - z2 ) && //electron recoil
		     abs ( aParticle->GetPDGcode() ) != 2112 ) {
                  G4double tibMain = 0.96478;
                  G4double tibEdep = 1.31180;
                  G4double tibCurlA = 16.357;
                  G4double tibCurlB = .93927;
                  tibCurlZ = 1.4635;
		  ThomasImel = tibMain*pow(dE/keV,-tibEdep)*
		    (1.-exp(-pow((dE/keV-tibCurlZ)/tibCurlA,tibCurlB)));
		  if ( fAlpha ) //technically ER, but special
		    ThomasImel=0.057675*pow(ElectricField,-0.49362);
		} //end electron recoil (ER)
		else { //nuclear recoil
		  // spline of NR data of C.E. Dahl PhD Thesis Princeton '09
		  // functions found using zunzun.com
                  //ThomasImel = 0.0541; 
                  ThomasImel = 0.0671; // 2015-11-27 - Brian L.
		} //end NR information
		// Never let LY exceed 0-field yield!
		if (ThomasImel > 0.19) ThomasImel = 0.19;
		//if (ThomasImel < 2e-2 && (dE/keV) < 5. ) ThomasImel = 2e-2;
	      } //end non-zero E-field segment
	      if ( Phase == kStateLiquid ) {
//		if ( z1 == 54 ) ThomasImel *= pow((Density/Density_LXe),0.3);
		if ( z1 == 18 ) ThomasImel *= pow((Density/Density_LAr),0.3);
	      }
	      //calculate the Thomas-Imel recombination probability, which
	      //depends on energy via NumIons, but not on dE/dx, and protect
	      //against seg fault by ensuring a positive number of ions
	      if (NumIons > 0) {
		G4double xi;
		xi = (G4double(NumIons)/4.)*ThomasImel;
		if ( InitialKinetEnergy == 9.4*keV ) {
		  G4double NumIonsEff = 4445.3 - 8.1306e7/pow((t1+t0)/ns,2.) +
		    6.04e13/pow((t1+t0)/ns,4.);
		  if ( (t1+t0) < 150*ns ) NumIonsEff = G4double(NumIons);
		  if ( NumIonsEff > 1e6 ) NumIonsEff = 1e6; t1 = t0;
		  xi = (G4double(NumIonsEff)/4.)*ThomasImel;
		}
		if ( fKr83m && ElectricField==0 )
		  xi = (G4double(1.6*NumIons)/4.)*ThomasImel;
		recombProb = 1-log(1+xi)/xi;
		if ( InitialKinetEnergy == 9.4*keV )
		  recombProb *= 1 - 6e-3*sqrt(ElectricField);
		if(recombProb<0 || InitialKinetEnergy/keV <= tibCurlZ) {
                  recombProb=0;
                }
		if(recombProb>1) {
                  recombProb=1;
                }
	      }
	      //just like Doke: simple binomial distribution
              G4double FanoFactor;
              if (YieldFactor < 1) { //NR
                FanoFactor = 0.007 * NumIons;
              }
              else { //ER
                FanoFactor = 0.0075 * NumIons;
              }
	      if ( FanoFactor < 0. ) FanoFactor = 0.;
              if(std::isnan(recombProb)) {
                recombProb = 0.;
              }
	      NumElectrons = modPoisRnd(NumIons * (1. - recombProb) * 1.00, FanoFactor);
	      if(NumElectrons > NumIons) {
                NumElectrons = NumIons;
              }
              if (NumElectrons < 0) {
                NumElectrons = 0;
              }
	      NumPhotons = (NumExcitons + NumIons) - NumElectrons;
              if (NumPhotons < 0) {
                NumPhotons = 0;
              }
              if (NumPhotons < NumExcitons) {
                NumElectrons = NumExcitons;
              }
              if (NumPhotons > NumQuanta) {
                NumPhotons = NumQuanta;
              }
	      //override Doke NumPhotons and NumElectrons
	      aMaterialPropertiesTable->
		AddConstProperty( numPho, NumPhotons   );
	      aMaterialPropertiesTable->
		AddConstProperty( numEle, NumElectrons );
	    }
            if(!recombProb ||
               InitialKinetEnergy/keV <= tibCurlZ) {
              NumPhotons = NumExcitons;
              NumElectrons = NumIons;
            }
	    // grab NumPhotons/NumElectrons, which come from Birks if
	    // the Thomas-Imel block of code above was not executed
            NumPhotons  = (G4int)aMaterialPropertiesTable->
	      GetConstProperty( numPho );
            NumElectrons =(G4int)aMaterialPropertiesTable->
	      GetConstProperty( numEle );
	    
            int NumQuenched;
	    if ( 0 ) { ;}
	    else { //other effects
	      if ( fAlpha || z1 == z2 ) { //bi-excitonic quenching due to high dE/dx
		//if ( z2 == z1 ) biExc =1./(1.+15.3*0.166*pow(epsilon,.5));
                if ( z2 == z1 ) biExc = 1./(1. + 13.2*0.166*pow(epsilon,0.5)); // 2015-11-27 - Brian L.
		NumQuenched = BinomFluct(NumPhotons,biExc);
                NumElectrons = NumElectrons + int(floor(0.111*(double(NumPhotons)-double(NumQuenched))+0.5)); // 2015-11-27 - Brian L.
		NumPhotons = NumQuenched;
	      }
	      if ( ! FastSimBool ) NumPhotons =
		BinomFluct(NumPhotons,luxManager->GetS1Gain());
	    } if (FastSimBool) NumElectrons = BinomFluct(NumElectrons,
	      exp(-(BORDER-aMaterialPropertiesTable->GetConstProperty
		    (zCoord))/luxManager->GetDriftElecAttenuation()));
	    if ( luxManager->GetS2Gain() < 1.0 ) NumElectrons =
		BinomFluct(NumElectrons,luxManager->GetS2Gain());
	    else NumElectrons = 
	      G4int(floor(NumElectrons*luxManager->GetS2Gain()+0.5));
	    //printf("%f\t%f\t%f\t%e\t%f\t%e\t%e\t%i\t%i\n",
	    //x0[0]/cm,x0[1]/cm,x0[2]/cm,recombProb,energ/keV,
	    //ExcitationRatio,ThomasImel,NumPhotons,NumElectrons);
	    
	    // new stuff to make Kr-83m work properly
	    if(fKr83m || InitialKinetEnergy==9.4*keV) fKr83m += dE/keV;
	    if(fKr83m > 41) fKr83m = 0;
	    if ( SinglePhase ) //for a 1-phase det. don't propagate e-'s
	      NumElectrons = 0; //saves simulation time
	    
	    // reset material properties numExc, numIon, numPho, numEle, as
	    // their values have been used or stored elsewhere already
	    aMaterialPropertiesTable->AddConstProperty( numExc, 0 );
	    aMaterialPropertiesTable->AddConstProperty( numIon, 0 );
	    aMaterialPropertiesTable->AddConstProperty( numPho, 0 );
	    aMaterialPropertiesTable->AddConstProperty( numEle, 0 );
	    
	    double s1Hits[122], s2Hits[122], timing[100000], timeBase=-1.;
	    if ( FastSimBool ) {
	      double origin[3]; timeBase = t0+G4UniformRand()*(t1-t0)+evtStrt;
	      origin[0] = aMaterialPropertiesTable->GetConstProperty( xCoord );
	      origin[1] = aMaterialPropertiesTable->GetConstProperty( yCoord );
	      origin[2] = aMaterialPropertiesTable->GetConstProperty( zCoord );
//	      NumPhotons = floor(NumPhotons*luxManager->GetS1Gain()/0.14+0.5);
              // Fasts sims will take in number of quanta and deal with binomial fluctuations internally
	      fastSim.photonsToPHE(NumPhotons,origin,s1Hits);
	      fastSim.electronsToPHE(NumElectrons,origin,s2Hits);
	      TotElec = NumElectrons;
	      //for ( G4int ii=0; ii<122; ii++ )
	      //s2Hits[ii]=G4int(floor(G4RandGauss::shoot(s2Hits[ii],
	      //					  0.5*s2Hits[ii])+.5));
	      driftTime = (1e3*fabs(BORDER/mm-origin[2]/mm)/1.51)*ns;
	      for ( G4int ii=0; ii<NumElectrons; ii++ )
		timing[ii] = timeBase + G4RandGauss::shoot(driftTime,1e3*ns*
			     (10.*sqrt(2.*22.*driftTime/s))/1.51) - 
		  280.*ns*log(G4UniformRand()); //all e- time effects
	      NumPhotons = 0; NumElectrons = 1;
	    }
	    // start particle creation loop
	    if( InitialKinetEnergy < MAX_ENE && InitialKinetEnergy > MIN_ENE &&
	       !fMultipleScattering )
	      NumQuanta = NumPhotons + NumElectrons;
	    else NumQuanta = 0;
	    
	    for(k = 0; k < NumQuanta; k++) {
	      G4double sampledEnergy;
	      G4DynamicParticle* aQuantum;
	      
	      // Generate random direction
	      G4double cost = 1. - 2.*G4UniformRand();
	      G4double sint = std::sqrt((1.-cost)*(1.+cost));
	      G4double phi = twopi*G4UniformRand();
	      G4double sinp = std::sin(phi); G4double cosp = std::cos(phi);
	      G4double px = sint*cosp; G4double py = sint*sinp;
	      G4double pz = cost;
	      
	      // Create momentum direction vector
	      G4ParticleMomentum photonMomentum(px, py, pz);
	      
	      // case of photon-specific stuff
	      if (k < NumPhotons && !FastSimBool) {
		// Determine polarization of new photon 
		G4double sx = cost*cosp;
		G4double sy = cost*sinp; 
		G4double sz = -sint;
		G4ThreeVector photonPolarization(sx, sy, sz);
		G4ThreeVector perp = photonMomentum.cross(photonPolarization);
		phi = twopi*G4UniformRand();
		sinp = std::sin(phi);
		cosp = std::cos(phi);
		photonPolarization = cosp * photonPolarization + sinp * perp;
		photonPolarization = photonPolarization.unit();
		
		// Generate a new photon or electron:
		sampledEnergy = G4RandGauss::shoot(PhotMean,PhotWidth);
		
		aQuantum = 
		  new G4DynamicParticle(G4OpticalPhoton::OpticalPhoton(),
					photonMomentum);
		aQuantum->SetPolarization(photonPolarization.x(),
					  photonPolarization.y(),
					  photonPolarization.z());
	      }
	      
	      else { // this else statement is for ionization electrons
		if(ElectricField) {
		  // point all electrons straight up, for drifting
		  G4ParticleMomentum electronMomentum(0, 0, -FieldSign);
		  aQuantum = 
		    new G4DynamicParticle(G4ThermalElectron::ThermalElectron(),
					  electronMomentum);
		  if ( Phase == kStateGas ) {
		    sampledEnergy =
		      GetGasElectronDriftSpeed(ElectricField,nDensity);
		  }
		  else {
                    if (luxManager->GetDriftTimeFromFile()) {
                      G4double calculatedDriftTime = luxManager->GetXYZDependentDriftTime(x1);
		      sampledEnergy = GetLiquidElectronDriftSpeed(
		      Temperature, ElectricField, MillerDriftSpeed, z1, luxManager->GetDriftTimeFromFile(), x1[2], calculatedDriftTime);
                    }
                    else {
		      sampledEnergy = GetLiquidElectronDriftSpeed(
		      Temperature, ElectricField, MillerDriftSpeed, z1, luxManager->GetDriftTimeFromFile(), x1[0], 0.);
                    }
                  }
		}
		else {
		  // use "photonMomentum" for the electrons in the case of zero
		  // electric field, which is just randomized vector we made
		  aQuantum = 
		    new G4DynamicParticle(G4ThermalElectron::ThermalElectron(),
					  photonMomentum);
		  sampledEnergy = 1.38e-23*(joule/kelvin)*Temperature;
		}
	      }

	      //assign energy to make particle real
	      aQuantum->SetKineticEnergy(sampledEnergy);
	      if (verboseLevel>1) //verbosity stuff
		G4cout << "sampledEnergy = " << sampledEnergy << G4endl;
	      
	      // Generate new G4Track object:
	      // emission time distribution
	      
	      // first an initial birth time is provided that is typically
	      // <<1 ns after the initial interaction in the simulation, then
	      // singlet, triplet lifetimes, and recombination time, are
	      // handled here, to create a realistic S1 pulse shape/timing
	      G4double aSecondaryTime = t0+G4UniformRand()*(t1-t0)+evtStrt;
	      if (tau1<0) tau1=0; if (tau3<0) tau3=0; if (tauR<0) tauR=0;
	      if ( aQuantum->GetDefinition()->
		   GetParticleName()=="opticalphoton" ) {
		if ( abs(z2-z1) && !fAlpha && //electron recoil
		     abs(aParticle->GetPDGcode()) != 2112 ) {
		  LET = (energ/MeV)/(delta/cm)*(1/Density); //avg LET over all
		  //in future, this will be done interaction by interaction
		  // Next, find the recombination time, which is LET-dependent
		  // via ionization density (Kubota et al. Phys. Rev. B 20
		  // (1979) 3486). We find the LET-dependence by fitting to the
		  // E-dependence (Akimov et al. Phys. Lett. B 524 (2002) 245).
		  if ( Phase == kStateLiquid && z1 == 54 )
		    tauR = 3.5*((1+0.41*LET)/(0.18*LET))*ns
		              *exp(-0.00900*ElectricField);
		  //field dependence based on fitting Fig. 9 of Dawson et al.
		  //NIM A 545 (2005) 690
		  //singlet-triplet ratios adapted from Kubota 1979, converted
		  //into correct units to work here, and separately done for
		  //excitation and recombination processes for electron recoils
		  //and assumed same for all LET (may vary)
		  SingTripRatioX = G4RandGauss::shoot(0.17,0.05);
		  SingTripRatioR = G4RandGauss::shoot(0.8,0.2);
		  if ( z1 == 18 ) {
		    SingTripRatioR = 0.2701+0.003379*LET-4.7338e-5*pow(LET,2.)
		      +8.1449e-6*pow(LET,3.); SingTripRatioX = SingTripRatioR;
		    if( LET < 3 ) {
		      SingTripRatioX = G4RandGauss::shoot(0.36,0.06);
		      SingTripRatioR = G4RandGauss::shoot(0.5,0.2); }
		  }
		}
		else if ( fAlpha ) { //alpha particles
                  SingTripRatioR = G4RandGauss::shoot(2.3,0.51);
		  //currently based on Dawson 05 and Tey. 11 (arXiv:1103.3689)
		  //real ratio is likely a gentle function of LET
		  if (z1==18) SingTripRatioR = (-0.065492+1.9996
		    *exp(-dE/MeV))/(1+0.082154/pow(dE/MeV,2.)) + 2.1811;
                  SingTripRatioX = SingTripRatioR;
		}
		else { //nuclear recoil
		  //based loosely on Hitachi et al. Phys. Rev. B 27 (1983) 5279
		  //with an eye to reproducing Akimov 2002 Fig. 9
		  SingTripRatioR = G4RandGauss::shoot(7.8,1.5);
		  if (z1==18) SingTripRatioR = 0.22218*pow(energ/keV,0.48211);
		  SingTripRatioX = SingTripRatioR;
		}
		// now, use binomial distributions to determine singlet and
		// triplet states (and do separately for initially excited guys
		// and recombining)
		if ( k > NumExcitons ) {
		  //the recombination time is non-exponential, but approximates
		  //to exp at long timescales (see Kubota '79)
		  aSecondaryTime += tauR*(1./G4UniformRand()-1);
		  if(G4UniformRand()<SingTripRatioR/(1+SingTripRatioR))
		    aSecondaryTime -= tau1*log(G4UniformRand());
		  else aSecondaryTime -= tau3*log(G4UniformRand());
		}
		else {
		  if(G4UniformRand()<SingTripRatioX/(1+SingTripRatioX))
		    aSecondaryTime -= tau1*log(G4UniformRand());
		  else aSecondaryTime -= tau3*log(G4UniformRand());
		}
	      }
	      else { //electron trapping at the liquid/gas interface
		G4double gainField = fabs(luxMaterials->GasXe()->
		  GetMaterialPropertiesTable()->
		  GetConstProperty("ELECTRICFIELDANODE"))/(kilovolt/cm);
		G4double tauTrap = 1680./gainField;
		if ( Phase == kStateLiquid && gainField != 0 )
		  aSecondaryTime -= tauTrap*ns*log(G4UniformRand());
	      }
	      
	      if ( aQuantum->GetDefinition()-> //save total energy in e-'s
                   GetParticleName()=="thermalelectron" )
		aQuantum->SetPolarization ( 0, (G4double)TotElec, 0 );
	      
	      // emission position distribution -- 
	      // Generate the position of a new photon or electron, with NO
	      // stochastic variation because that could lead to particles
	      // being mistakenly generated outside of your active region by
	      // Geant4, but real-life finite detector position resolution
	      // wipes out any effects from here anyway...
	      x0[0] = aMaterialPropertiesTable->GetConstProperty( xCoord );
	      x0[1] = aMaterialPropertiesTable->GetConstProperty( yCoord );
	      x0[2] = aMaterialPropertiesTable->GetConstProperty( zCoord );
	      G4double radius = sqrt(pow(x0[0],2.)+pow(x0[1],2.));
	      //re-scale radius to ensure no generation of quanta outside
              //the active volume of your simulation due to Geant4 rounding
	      if ( radius >= R_TOL ) {
		if (x0[0] == 0) x0[0] = 1*nm; if (x0[1] == 0) x0[1] = 1*nm;
		radius -= R_TOL; phi = atan ( x0[1] / x0[0] );
		x0[0] = fabs(radius*cos(phi))*((fabs(x0[0]))/(x0[0]));
		x0[1] = fabs(radius*sin(phi))*((fabs(x0[1]))/(x0[1]));
	      }
	      //position of the new secondary particle is ready for use
	      G4ThreeVector aSecondaryPosition = x0; G4double tempDrift;
	      if ( k >= NumPhotons && diffusion && ElectricField > 0 ) {
		G4double D_T = 64*pow(1e-3*ElectricField,-.17);
		//fit to Aprile and Doke 2009, arXiv:0910.4956 (Fig. 12)
		G4double D_L = 13.859*pow(1e-3*ElectricField,-0.58559);
		//fit to Aprile and Doke and Sorensen 2011, arXiv:1102.2865
		if ( Phase == kStateLiquid && z1 == 18 ) {
		  D_T = 93.342*pow(ElectricField/nDensity,0.041322);
		  D_L = 0.15 * D_T; }
		if ( Phase == kStateGas && z1 == 54 ) {
		  D_L=4.265+19097/ElectricField-1.7397e6/pow(ElectricField,2.)+
		    1.2477e8/pow(ElectricField,3.); D_T *= 0.01;
		} D_T *= cm2/s; D_L *= cm2/s;
		if (ElectricField < 255 && Phase == kStateLiquid) {
		  D_L = (0.22388+0.11995*ElectricField)*cm2/s;
                }
                G4double sigmaDT;
                G4double sigmaDL;
                if (luxManager->GetDriftTimeFromFile()) { // determine whether to use drift velocity of COMSOL sim
                  driftTime = luxManager->GetXYZDependentDriftTime(x1);
 		  sigmaDT = sqrt(2*D_T*driftTime);
		  sigmaDL = sqrt(2*D_L*driftTime);
                }
                else {
 		  G4double vDrift = sqrt((2*sampledEnergy)/(EMASS));
		  if ( BORDER == 0 ) x0[2] = 0;
		  sigmaDT = sqrt(2*D_T*fabs(BORDER-x0[2])/vDrift);
		  sigmaDL = sqrt(2*D_L*fabs(BORDER-x0[2])/vDrift);
                }
		G4double dr = fabs(G4RandGauss::shoot(0.,sigmaDT));
		phi = twopi * G4UniformRand();
                if (luxManager->GetRadialDriftFromFile()) { // determine whether to use final radius from COMSOL sim
                  G4double driftedR = luxManager->GetXYZDependentRadialDrift(x1);
                  G4double theta;
                  if (x1[0] != 0) {
                    theta = atan(x1[1]/x1[0]); // get current theta position, if will stay the same
                  }
                  else {
                    theta = 3.14159; // approximately pi
                  }

                  // take new radius, calculate new X and Y (with same theta) and add diffusion (dr * cos(phi) or dr * sin (phi))
                  // Because of the range of atan in C++, we need to specifically ensure that events stay in the same quadrant in XY
		  if (x1[0] < 0 &&
                     x1[1] >= 0) {
                          aSecondaryPosition[0] = -1. * driftedR * fabs(cos(theta)) + dr * cos(phi);
                          aSecondaryPosition[1] = driftedR * fabs(sin(theta)) + dr * sin(phi);
                  }
		  else if (x1[0] >= 0 &&
                     x1[1] < 0)	{
                          aSecondaryPosition[0] = driftedR * fabs(cos(theta)) + dr * cos(phi);
                          aSecondaryPosition[1] = -1. * driftedR * fabs(sin(theta)) + dr * sin(phi);
                  }
		  else if (x1[0] < 0 &&
                     x1[1] < 0)	{
                          aSecondaryPosition[0] = -1. * driftedR * fabs(cos(theta)) + dr * cos(phi);
                          aSecondaryPosition[1] = -1. * driftedR * fabs(sin(theta)) + dr * sin(phi);
                  }
                  else {
			  aSecondaryPosition[0] = driftedR * fabs(cos(theta)) + dr * cos(phi);
			  aSecondaryPosition[1] = driftedR * fabs(sin(theta)) + dr * sin(phi);
		  }
                }
                else {
		  aSecondaryPosition[0] += cos(phi) * dr;
  		  aSecondaryPosition[1] += sin(phi) * dr;
                }
		aSecondaryPosition[2] += G4RandGauss::shoot(0.,sigmaDL);
		radius = sqrt(pow(aSecondaryPosition[0],2.)+
			      pow(aSecondaryPosition[1],2.));
		if(aSecondaryPosition[2] >= BORDER && Phase == kStateLiquid) {
		  if ( BORDER != 0 ) aSecondaryPosition[2] = BORDER - R_TOL; }
		if(aSecondaryPosition[2] <= PMT && !GlobalFields)
		  aSecondaryPosition[2] = PMT + R_TOL;
		tempDrift = fabs(BORDER-x0[2])/sqrt(2*sampledEnergy/(EMASS));
		if ( tempDrift > driftTime ) driftTime = tempDrift;
	      } //end of electron diffusion code
	      
	      // GEANT4 business: stuff you need to make a new track
              G4double randDphe;
	      if ( aSecondaryTime < 0 ) aSecondaryTime = 0; //no neg. time
	      if ( FastSimBool ) { G4String PMTvolName;
                LoadS1PulseShape("physicslist/src/S1PulseShape.dat");
		for(G4int q1 = 0; q1 < 122; q1++) {
		  for(unsigned int q2 = 0; q2 < s1Hits[q1]; q2++) {
		    double UniRand=G4UniformRand();
                    aSecondaryTime=timeBase;
		    double timer = 200. * G4UniformRand();
                    int lo = floor(timer);
                    int hi = ceil(timer);
		    while ( UniRand > (ceil(timer) - timer) * s1PulseShape[lo] +
                            (timer - floor(timer)) * s1PulseShape[hi]) {
                      timer = 200. * G4UniformRand();
		      UniRand=G4UniformRand();
                      lo = floor(timer);
                      hi = ceil(timer);
                    }
                    aSecondaryTime += timer*ns;
		    PMTvolName = ConvertNumberToString(q1);
		    aSecondaryPosition = luxManager->
		      GetComponentByName(PMTvolName)->GetGlobalCenter();
		    G4DynamicParticle * aPhe = new G4DynamicParticle(
		    G4ThermalElectron::ThermalElectron(),
		    G4ParticleMomentum(0,0,0));
		    aPhe->SetKineticEnergy(1*MeV);
		    G4Track *aSecondaryTrack = new G4Track(aPhe,aSecondaryTime,
							  aSecondaryPosition);
		    aParticleChange.AddSecondary(aSecondaryTrack);
                    //For each photon there is a 20% chance that two phe are produced... at least this is what we're told
                    randDphe = G4UniformRand();
                    if (luxManager->GetLUXDoublePheRateFromFile()) {
	              if (randDphe < doublePheProb[q1]) {
		        G4DynamicParticle * aPhe = new G4DynamicParticle(
  		        G4ThermalElectron::ThermalElectron(),
		        G4ParticleMomentum(0,0,0));
		        aPhe->SetKineticEnergy(1*MeV);
  		        G4Track *aSecondaryTrack = new G4Track(aPhe,aSecondaryTime,
							  aSecondaryPosition);
                       	aParticleChange.AddSecondary(aSecondaryTrack);
                      }
                    }
                    else {
                      if (randDphe < .2) {
		        G4DynamicParticle * aPhe = new G4DynamicParticle(
  		        G4ThermalElectron::ThermalElectron(),
		        G4ParticleMomentum(0,0,0));
		        aPhe->SetKineticEnergy(1*MeV);
  		        G4Track *aSecondaryTrack = new G4Track(aPhe,aSecondaryTime,
							  aSecondaryPosition);
                       	aParticleChange.AddSecondary(aSecondaryTrack);
                      }
                    }
		  } //end placement of S1 phe
		  for(unsigned int q2 = 0; q2 < s2Hits[q1]; q2++) {
                    aSecondaryTime = timing[rand() % TotElec]
		      -100.*ns*log(G4UniformRand())
		      +G4UniformRand()*1e3*ns*((GASGAP/mm)/4.92);
                    PMTvolName = ConvertNumberToString(q1);
                    aSecondaryPosition = luxManager->
		      GetComponentByName(PMTvolName)->GetGlobalCenter();
		    G4DynamicParticle * aPhe = new G4DynamicParticle(
		    G4ThermalElectron::ThermalElectron(),
		    G4ParticleMomentum(0,0,0));
		    aPhe->SetKineticEnergy(2*MeV);
                    G4Track*aSecondaryTrack = new G4Track(aPhe,aSecondaryTime,
							  aSecondaryPosition);
                    if ( ElectricField ) {
		      aParticleChange.AddSecondary(aSecondaryTrack);
                      //For each photon there is a 20% chance that two phe are produced... at least this is what we're told
                      randDphe = G4UniformRand();
                      if (luxManager->GetLUXDoublePheRateFromFile()) {
  	                if (randDphe < doublePheProb[q1]) {
		          G4DynamicParticle * aPhe = new G4DynamicParticle(
  		          G4ThermalElectron::ThermalElectron(),
		          G4ParticleMomentum(0,0,0));
		          aPhe->SetKineticEnergy(1*MeV);
  		          G4Track *aSecondaryTrack = new G4Track(aPhe,aSecondaryTime,
							  aSecondaryPosition);
                          aParticleChange.AddSecondary(aSecondaryTrack);
                        }
                      }
                      else {
                        if (randDphe < .2) {
		          G4DynamicParticle * aPhe = new G4DynamicParticle(
  		          G4ThermalElectron::ThermalElectron(),
		          G4ParticleMomentum(0,0,0));
		          aPhe->SetKineticEnergy(1*MeV);
  		          G4Track *aSecondaryTrack = new G4Track(aPhe,aSecondaryTime,
							  aSecondaryPosition);
                       	  aParticleChange.AddSecondary(aSecondaryTrack);
                        }
                      }
                    }
                  } //end placement of S2 phe
		} //end loop over all 122 PMTs
	      } //end fast simulation method which teleports final phe
	      else { G4Track * aSecondaryTrack = 
		  new G4Track(aQuantum,aSecondaryTime,aSecondaryPosition);
		if ( k < NumPhotons || radius < R_MAX )
		  aParticleChange.AddSecondary(aSecondaryTrack);
	      } //normal one at a time particle placement (no fast sim)
	    }

	    //reset bunch of things when done with an interaction site
	    aMaterialPropertiesTable->AddConstProperty( xCoord, 999*km );
	    aMaterialPropertiesTable->AddConstProperty( yCoord, 999*km );
	    aMaterialPropertiesTable->AddConstProperty( zCoord, 999*km );
	    aMaterialPropertiesTable->AddConstProperty( trackL, 0*um );
	    aMaterialPropertiesTable->AddConstProperty( energy, 0*eV );
	    aMaterialPropertiesTable->AddConstProperty( time00, DBL_MAX );
	    aMaterialPropertiesTable->AddConstProperty( time01, -1*ns );
	    
	    if (verboseLevel>0) { //more verbose stuff
	      G4cout << "\n Exiting from G4S1Light::DoIt -- "
		     << "NumberOfSecondaries = "
		     << aParticleChange.GetNumberOfSecondaries() << G4endl;
	    }
	  } //end of interaction site loop

	  //more things to reset...
	  aMaterialPropertiesTable->
	    AddConstProperty( "TOTALNUM_INT_SITES", 0 );
	  aMaterialPropertiesTable->
	    AddConstProperty( "ENERGY_DEPOSIT_TOT", 0*keV );
	  aMaterialPropertiesTable->
	    AddConstProperty( "ENERGY_DEPOSIT_GOL", 0*MeV );
	  fExcitedNucleus = false;
	  fAlpha = false;
	  
	  G4double MeanNumberOfLiquidElectrons=DAQWIN_MS*E_RATE_HZ;
	  G4int NumLiqElec = G4int(G4Poisson(
		MeanNumberOfLiquidElectrons));
	  G4ThreeVector RandomPos; G4double RandomTim;
	  if(NumQuanta == 0 || fVeryHighEnergy || FastSimBool || E_RATE_HZ == 0) NumLiqElec=0;
	  for ( G4int aa = 0; aa < NumLiqElec; aa++ ) {
	    G4DynamicParticle * aEle = new G4DynamicParticle(
				       G4ThermalElectron::ThermalElectron(),
				       G4ParticleMomentum(0, 0, 1));
	    aEle->SetKineticEnergy( 1e-5*eV ); aEle->SetPolarization(0,0,1);
	    RandomTim = (evtStrt+driftTime) + (2.*G4UniformRand()-1.) *
	      (DAQWIN_MS/2.);
	    G4double ph = twopi * G4UniformRand();
	    G4double rr = R_MAX * sqrt(G4UniformRand());
	    RandomPos[0] = rr*cos(ph); RandomPos[1] = rr*sin(ph);
	    RandomPos[2] = BORDER - 2*cm;
	    G4Track * aSecondaryTrack = new G4Track(aEle,RandomTim,
						    RandomPos);
	    aParticleChange.AddSecondary(aSecondaryTrack);
	  } //the single electron background
	}
	

	//don't do anything when you're not ready to scintillate
        else {
          aParticleChange.SetNumberOfSecondaries(0);
          return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
        }
	
	//the end (exiting)
	return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
}

// GetMeanFreePath
// ---------------
G4double G4S1Light::GetMeanFreePath(const G4Track&,
                                          G4double ,
                                          G4ForceCondition* condition)
{
        *condition = StronglyForced;
	// what this does is enforce the G4S1Light physics process as always
	// happening, so in effect scintillation is a meta-process on top of
	// any and all other energy depositions which may occur, just like the
	// original G4Scintillation (disregard DBL_MAX, this function makes the
	// mean free path zero really, not infinite)

        return DBL_MAX; //a C-defined constant
}

// GetMeanLifeTime
// ---------------
G4double G4S1Light::GetMeanLifeTime(const G4Track&,
                                          G4ForceCondition* condition)
{
        *condition = Forced;
	// this function and this condition has the same effect as the above
        return DBL_MAX;
}

G4double G4S1Light::GetGasElectronDriftSpeed(G4double efieldinput,
        G4double density)
{
    //Gas equation one coefficients(E/N of 1.2E-19 to 3.5E-19)
    double gas1a=395.50266631436,gas1b=-357384143.004642,gas1c=0.518110447340587;
    //Gas equation two coefficients(E/N of 3.5E-19 to 3.8E-17)
    double gas2a=-592981.611357632,gas2b=-90261.9643716643,
    gas2c=-4911.83213989609,gas2d=-115.157545835228, gas2f=-0.990440443390298,
    gas2g=1008.30998933704,gas2h=223.711221224885;
    G4double edrift=0, gasdep=efieldinput/density, gas1fix=0, gas2fix=0;
    
    if ( gasdep < 1.2e-19 && gasdep >= 0 ) edrift = 4e22*gasdep;
    if ( gasdep < 3.5e-19 && gasdep >= 1.2e-19 ) {
        gas1fix = gas1b*pow(gasdep,gas1c); edrift = gas1a*pow(gasdep,gas1fix);
    }
    if ( gasdep < 3.8e-17 && gasdep >= 3.5e-19 ) {
        gas2fix = log(gas2g*gasdep);
        edrift = (gas2a+gas2b*gas2fix+gas2c*pow(gas2fix,2)+gas2d*pow(gas2fix,3)+
                  gas2f*pow(gas2fix,4))*(gas2h*exp(gasdep));
    }
    if ( gasdep >= 3.8e-17 ) edrift = 6e21*gasdep-32279;
    
    return 0.5*EMASS*pow(edrift*(cm/s),2.);
}

G4double G4S1Light::GetLiquidElectronDriftSpeed(G4double tempinput,
        G4double efieldinput, G4bool Miller, G4int Z, G4bool driftTimeFromFile,
        G4double positionZ_mm, G4double calculatedDriftTime) {
  G4double edrift = 0.;
  if(driftTimeFromFile) {
    G4double zGate_mm = GAT;
    edrift = (zGate_mm - positionZ_mm) / calculatedDriftTime;
    edrift = edrift * 1e8; // convert to cm / s from mm / ns
  }
  else {
    if(efieldinput<0) efieldinput *= (-1);
    //Liquid equation one (165K) coefficients
    G4double onea=144623.235704015,
      oneb=850.812714257629,
      onec=1192.87056676815,
      oned=-395969.575204061,
      onef=-355.484170008875,
      oneg=-227.266219627672,
      oneh=223831.601257495,
      onei=6.1778950907965,
      onej=18.7831533426398,
      onek=-76132.6018884368;
    //Liquid equation two (200K) coefficients
    G4double twoa=17486639.7118995,
      twob=-113.174284723134,
      twoc=28.005913193763,
      twod=167994210.094027,
      twof=-6766.42962575088,
      twog=901.474643115395,
      twoh=-185240292.471665,
      twoi=-633.297790813084,
      twoj=87.1756135457949;
    //Liquid equation three (230K) coefficients
    G4double thra=10626463726.9833,
      thrb=224025158.134792,
      thrc=123254826.300172,
      thrd=-4563.5678061122,
      thrf=-1715.269592063,
      thrg=-694181.921834368,
      thrh=-50.9753281079838,
      thri=58.3785811395493,
      thrj=201512.080026704;
    G4double y1=0,y2=0,f1=0,f2=0,f3=0,
      t1=0,t2=0,frac=0,slope=0,intercept=0;

    //Equations defined
    f1=onea/(1+exp(-(efieldinput-oneb)/onec))+oned/
      (1+exp(-(efieldinput-onef)/oneg))+
      oneh/(1+exp(-(efieldinput-onei)/onej))+onek;
    f2=twoa/(1+exp(-(efieldinput-twob)/twoc))+twod/
      (1+exp(-(efieldinput-twof)/twog))+
      twoh/(1+exp(-(efieldinput-twoi)/twoj));
    f3=thra*exp(-thrb*efieldinput)+thrc*exp(-(pow(efieldinput-thrd,2))/
                                            (thrf*thrf))+
      thrg*exp(-(pow(efieldinput-thrh,2)/(thri*thri)))+thrj;

    if(efieldinput<20 && efieldinput>=0) {
      f1=2951*efieldinput;
      f2=5312*efieldinput;
      f3=7101*efieldinput;
    }
    //Cases for tempinput decides which 2 equations to use lin. interpolation
    if(tempinput<200.0 && tempinput>165.0) {
      y1=f1;
      y2=f2;
      t1=165.0;
      t2=200.0;
    }
    if(tempinput<230.0 && tempinput>200.0) {
      y1=f2;
      y2=f3;
      t1=200.0;
      t2=230.0;
    }
    if((tempinput>230.0 || tempinput<165.0) && !Miller) {
      G4cout << "\nWARNING: TEMPERATURE OUT OF RANGE (165-230 K)\n";
      return 0;
    }
    if (tempinput == 165.0) edrift = f1;
    else if (tempinput == 200.0) edrift = f2;
    else if (tempinput == 230.0) edrift = f3;
    else { //Linear interpolation
      frac=(tempinput-t1)/(t2-t1);
      slope = (y1-y2)/(t1-t2);
      intercept=y1-slope*t1;
      edrift=slope*tempinput+intercept;
    }
  
    if ( Miller ) {
      if ( efieldinput <= 40. )
        edrift = -0.13274+0.041082*efieldinput-0.0006886*pow(efieldinput,2.)+
        5.5503e-6*pow(efieldinput,3.);
      else
        edrift = 0.060774*efieldinput/pow(1+0.11336*pow(efieldinput,0.5218),2.);
      if ( efieldinput >= 1e5 ) edrift = 2.7;
      if ( efieldinput >= 500 )
        edrift -= 0.017 * ( tempinput - 163 );
      else if ( efieldinput < 100 )
        edrift += 0.017 * ( tempinput - 163 );
      else { ;}
      edrift *= 1e5; //put into units of cm/sec. from mm/usec.
    }
  }
  if ( Z == 18 ) edrift = 1e5 * (
    .097384*pow(log10(efieldinput),3.0622)-.018614*sqrt(efieldinput) );
  if ( edrift < 0 ) edrift = 0.;
  edrift = 0.5*EMASS*pow(edrift*cm/s,2.);
  return edrift;
}

G4double CalculateElectronLET ( G4double E, G4int Z ) {
  G4double LET;
  switch ( Z ) {
  case 54:
  //use a spline fit to online ESTAR data
  if ( E >= 1 ) LET = 58.482-61.183*log10(E)+19.749*pow(log10(E),2)+
    2.3101*pow(log10(E),3)-3.3469*pow(log10(E),4)+
    0.96788*pow(log10(E),5)-0.12619*pow(log10(E),6)+0.0065108*pow(log10(E),7);
  //at energies <1 keV, use a different spline, determined manually by
  //generating sub-keV electrons in Geant4 and looking at their ranges, since
  //ESTAR does not go this low
  else if ( E>0 && E<1 ) LET = 6.9463+815.98*E-4828*pow(E,2)+17079*pow(E,3)-
    36394*pow(E,4)+44553*pow(E,5)-28659*pow(E,6)+7483.8*pow(E,7);
  else
    LET = 0;
  break;
  case 18: default:
  if ( E >= 1 ) LET = 116.70-162.97*log10(E)+99.361*pow(log10(E),2)-
    33.405*pow(log10(E),3)+6.5069*pow(log10(E),4)-
    0.69334*pow(log10(E),5)+.031563*pow(log10(E),6);
  else if ( E>0 && E<1 ) LET = 100;
  else
    LET = 0;
  }
  return LET;
}

G4int BinomFluct ( G4int N0, G4double prob ) {
  G4double mean = N0*prob;
  G4double sigma = sqrt(N0*prob*(1-prob));
  G4int N1 = 0;
  if ( prob <= 0.00 ) return N1;
  if ( prob >= 1.00 ) return N0;
  
  if ( N0 < 10 ) {
    for(G4int i = 0; i < N0; i++) {
      if(G4UniformRand() < prob) N1++;
    }
  }
  else {
    N1 = G4int(floor(G4RandGauss::shoot(mean,sigma)+0.5));
  }
  if ( N1 > N0 ) N1 = N0;
  if ( N1 < 0 ) N1 = 0;
  return N1;
}

void InitMatPropValues ( G4MaterialPropertiesTable *nobleElementMat ) {
  char xCoord[80]; char yCoord[80]; char zCoord[80];
  char numExc[80]; char numIon[80]; char numPho[80]; char numEle[80];
  char trackL[80]; char time00[80]; char time01[80]; char energy[80];
  
  // for loop to initialize the interaction site mat'l properties
  for( G4int i=0; i<10000; i++ ) {
    sprintf(xCoord,"POS_X_%d",i); sprintf(yCoord,"POS_Y_%d",i);
    sprintf(zCoord,"POS_Z_%d",i);
    nobleElementMat->AddConstProperty( xCoord, 999*km );
    nobleElementMat->AddConstProperty( yCoord, 999*km );
    nobleElementMat->AddConstProperty( zCoord, 999*km );
    sprintf(numExc,"N_EXC_%d",i); sprintf(numIon,"N_ION_%d",i);
    sprintf(numPho,"N_PHO_%d",i); sprintf(numEle,"N_ELE_%d",i);
    nobleElementMat->AddConstProperty( numExc, 0 );
    nobleElementMat->AddConstProperty( numIon, 0 );
    nobleElementMat->AddConstProperty( numPho, 0 );
    nobleElementMat->AddConstProperty( numEle, 0 );
    sprintf(trackL,"TRACK_%d",i); sprintf(energy,"ENRGY_%d",i);
    sprintf(time00,"TIME0_%d",i); sprintf(time01,"TIME1_%d",i);
    nobleElementMat->AddConstProperty( trackL, 0*um );
    nobleElementMat->AddConstProperty( energy, 0*eV );
    nobleElementMat->AddConstProperty( time00, DBL_MAX );
    nobleElementMat->AddConstProperty( time01,-1*ns );
  }
  
  // we initialize the total number of interaction sites, a variable for
  // updating the amount of energy deposited thus far in the medium, and a
  // variable for storing the amount of energy expected to be deposited
  nobleElementMat->AddConstProperty( "TOTALNUM_INT_SITES", 0 );
  nobleElementMat->AddConstProperty( "ENERGY_DEPOSIT_TOT", 0*keV );
  nobleElementMat->AddConstProperty( "ENERGY_DEPOSIT_GOL", 0*MeV );
  return;
}

G4double UnivScreenFunc ( G4double E, G4double Z, G4double A ) {
  G4double a_0 = 5.29e-11*m;
  G4double epsilon_0 = 8.854e-12*(farad/m);
  G4double epsilon = .00105 * E;
  G4double zeta_0 = pow(Z,(1./6.)); G4double m_N = A*1.66e-27*kg;
  G4double hbar = 6.582e-16*eV*s;
  G4double epsilon_z = 1.068 * epsilon;
  G4double s_n = log(1+1.1383*epsilon_z)/(2.*(epsilon_z +
		       0.01321*pow(epsilon_z,0.21226) +
		       0.19593*sqrt(epsilon_z)));
  G4double s_e = 0.166 * sqrt(epsilon);
  G4double a = 0.423;
  G4double alpha = 2.44;
  return alpha*(s_e)/(s_e+s_n)*(1-exp(-E/a));
}

G4String ConvertNumberToString ( G4int pmtCall ) {
  G4String actualName, actualNumber; char temporary[16]; pmtCall++;
  sprintf(temporary,"%i",pmtCall);
  actualNumber = (G4String)(temporary);
  if ( pmtCall >= 1 && pmtCall <= 9 )
    actualName = "Top_PMT_PhotoCathode_0" + actualNumber;
  else if ( pmtCall >= 10 && pmtCall <= 60 )
    actualName = "Top_PMT_PhotoCathode_" + actualNumber;
  else if ( pmtCall == 121 )
    actualName = "Top_PMT_PhotoCathode_" + actualNumber;
  else
    actualName = "Bottom_PMT_PhotoCathode_" + actualNumber;

  return actualName;
}

int modPoisRnd(double poisMean, double preFactor) {

  int randomNumber;

  if ( preFactor >= 1. ) {
    poisMean = G4RandGauss::shoot(poisMean,sqrt((preFactor-1.)*poisMean));
    if ( poisMean < 0. ) poisMean = 0.;
    randomNumber = G4Poisson(poisMean);
  }
  else
    randomNumber =
      int(floor(double(G4Poisson(poisMean/preFactor))*preFactor+G4UniformRand()));

  if ( randomNumber < 0 ) randomNumber = 0; //jic

  return randomNumber;
}

void G4S1Light::LoadS1PulseShape(G4String fileName) {
  std::ifstream file;
  file.open(fileName);
  if (!file.is_open()) {
    G4cout<<G4endl<<G4endl<<G4endl;
    G4cout<<"S1 Pulse Shape File Not Found!"<<G4endl;
    G4cout<<G4endl<<G4endl<<G4endl;
    exit(0);
  }
  double sum = 0.;
  for (int i = 0; i < 101; i++) {
    file >> s1PulseShape[i];
    sum = sum + s1PulseShape[i];
  }
  for (int i = 0; i < 101; i++) {
    s1PulseShape[i] = s1PulseShape[i] / sum;
  }
  file.close();
}

void G4S1Light::SetLUXGeoValues()
{
    BORDER=544.2198*mm; //liquid-gas border z-coordinate
    
    // different field regions, for gamma-X studies
    WIN=595.0*mm; //top Cu block (also, quartz window)
    TOP=585.0*mm; //top grid wires
    ANE=549.0*mm; //anode mesh
    GAT=539.0*mm; //gate grid
    GASGAP=5.0*mm; //S2 generation region
    SRF=ANE - GASGAP;   //liquid-gas interface
    BORDER=SRF; // Same as SRF, because we need two identical variables, for
                // some reason
    CTH=55.90*mm; //cathode grid
    BOT=19.60*mm; //bottom PMT grid
    PMT=0.000*mm; //bottom Cu block and PMTs
    
    R_MAX=24.313*cm; //for corraling diffusing electrons
    
    E_RATE_HZ=100./s;
    
    usingLZ = false;
}

void G4S1Light::SetLZGeoValues()
{
    BORDER=1481.0*mm; //liquid-gas border z-coordinate
    
    // different field regions, for gamma-X studies
    WIN=1534.*mm; //top Cu block (also, quartz window)
    TOP=1524.*mm; //top grid wires
    ANE=1486.*mm; //anode mesh
    GAT=1476.*mm; //gate grid
    GASGAP=5.*mm; //S2 generation region
    SRF=BORDER; //liquid-gas interface
    //BORDER=SRF; // Same as SRF, because we need two identical variables, for
                // some reason
    CTH=-4.1*mm; //cathode grid
    BOT=-140*mm; //bottom PMT grid
    PMT=-150*mm; //bottom Cu block and PMTs
    
    R_MAX=730*mm; //for corraling diffusing electrons
    
    E_RATE_HZ=0./s;
    
    usingLZ = true;
}

void G4S1Light::LoadDoublePHEProb(G4String fileName) {
  std::ifstream file;
  file.open(fileName);
  if (!file.is_open()) {
    G4cout<<G4endl<<G4endl<<G4endl;
    G4cout<<"Double phe probability File Not Found!"<<G4endl;
    G4cout<<G4endl<<G4endl<<G4endl;
    exit(0);
  }
  for (int i = 0; i < 122; i++) {
    file >> doublePheProb[i];
  }
  file.close();
}
