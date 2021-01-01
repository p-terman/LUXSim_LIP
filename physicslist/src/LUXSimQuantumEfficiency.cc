///////////////////////////////////////////////////////////////////////////////
/*      LUXSimQuantumEfficiency.cc
 *
 * Handles exactly what the file's name says it does.
 *
 ******************************************************************************
 * Change log
 *       20 Feb 2012 - Initial submission for QE per PMT (Matthew)
 *       22 Feb 2012 - Fixed PMT numbering scheme (Matthew)
 *       26 Feb 2012 - Clearer normalization (Matthew)
 *       11 Jul 2012 - Corrected overestimate of cold QE increase (Matthew)
 *       16 Aug 2012 - Added handling of both PMT numbering schemes (Rich)
 *       14 Oct 2012 - Fixed typo in temperature dependence (Matthew)
 *       02 Nov 2012 - Better QE extrapolation to below ~160nm (Matthew)
 *       25 Nov 2012 - QE -> phe conv (Matthew)
 *       30 Dec 2012 - Lin extrap of phe conv eff to hi-E (Matthew)
 *       29 Jan 2013 - Simpler phe eff, weaker T dep. (Matthew)
 *       16 Feb 2013 - Filled in rest of PMTs (Matthew)
 *       11 Jun 2013 - New T-dep. for Run03 Kr-83m data (Matt)
 *       15 Jun 2013 - Kludge for the S2 phe/e-, bot/tot (Matthew)
 *       07 Jul 2013 - Added pulse classifier trick using phe energy (Matthew)
 *       08 Jul 2013 - Tweaked QEs again to match S1 size (Matthew)
 *       13 Sep 2013 - Tweaked QEs to match 1e-S2 size (Matthew)
 *       13 Sep 2013 - Added ET to pulse classification scheme (Matthew)
 *       28 Jul 2014 - Taking into account proper VUV QE now (Matthew)
 *       31 Aug 2014 - Added QE for water tank PMTs, LZ and LUX (Curt/Scott Ha.)
 *       18 Sep 2014 - QE values are now set by reading a file, rather than a 
 *                     huge if/else block.  Also, the double-phe probabilities 
 *                     are handled per-pmt.  This is done two places: in the 
 *                     modification to the QE values themselves, and also in the
 *                     process that randomly adds double photoelectrons. (Aaron)
 *       15 Apr 2015 - Adjusting the top PMT QE according to the output of the 
 *                     light collection simulations and applying the "cold 
 *                     bonus" to the PMT QE too.  (Vic)
 */
///////////////////////////////////////////////////////////////////////////////

#define QE_AVG (0.3373) //CHF (RMS for 175 nm wavelength)
//number a little high: compensates for Geant4 optical model (Matthew)
//added the /1.2 to account for VUV vs. blue photon QEs (Matthew)
// taken out the 1.2, because now each PMT has its own double-phe probability (Aaron)

#include "Randomize.hh"
#include "G4ParticleDefinition.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ForceCondition.hh"
#include "G4ThermalElectron.hh"
#include "G4OpticalPhoton.hh"
#include "G4DynamicParticle.hh"
#include "G4VParticleChange.hh"
#include "G4ThreeVector.hh"

#include "LUXSimQuantumEfficiency.hh"
#include "LUXSimManager.hh"
#include "LUXSim1_0PMTRenumbering.hh"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

LUXSimQuantumEfficiency::LUXSimQuantumEfficiency(const G4String& processName)
      : G4VRestDiscreteProcess(processName) 
{
  LoadDoublePHEProb("physicslist/src/DoublePHEperDPH.txt");
  LoadQEValuesFromFile("physicslist/src/PMTQEvals.txt");
}

LUXSimQuantumEfficiency::~LUXSimQuantumEfficiency() {}

G4VParticleChange*
LUXSimQuantumEfficiency::AtRestDoIt(const G4Track& aTrack, const G4Step& aStep)
{
  return LUXSimQuantumEfficiency::PostStepDoIt(aTrack, aStep);
}

G4VParticleChange*
LUXSimQuantumEfficiency::PostStepDoIt(
const G4Track& aTrack, const G4Step& aStep)
{
    aParticleChange.Initialize(aTrack);
    const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
    G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();
    const G4Material* aMaterial = pPostStepPoint->GetMaterial();
    G4ThreeVector x1 = pPostStepPoint->GetPosition();
    G4double      t1 = pPostStepPoint->GetGlobalTime();
    
    //find current direction of the photon, and angle
    G4ParticleMomentum oldMoment = aParticle->GetMomentumDirection().unit();
    G4double Angle = acos ( fabs(oldMoment.z()) / 
     sqrt(pow(oldMoment.x(),2.)+pow(oldMoment.y(),2.)+pow(oldMoment.z(),2.)) );
    if ( oldMoment.x() == 0 && oldMoment.y() == 0 )
      Angle = 0.; //point photon straight up when x and y are zero
    //G4double Direction = oldMoment.z();
    G4int sign = 0; //which way are you going? Up or down?
    if ( x1[2] < 0 ) sign = -1;
    if ( x1[2] > 0 ) sign = 1; //(zero not going to happen)
    
    //kill the optical photon at the photo-cathode
    G4double KE = aParticle->GetKineticEnergy();
    aParticleChange.ProposeTrackStatus(fStopAndKill);
    //aParticleChange.ProposeEnergy(0.*eV);
    aParticleChange.ProposeLocalEnergyDeposit(KE);
    
    //calculate the quantum efficiency
    G4double fOptPhoWaveLength_nm = (1.2398/(KE/keV));
    G4double QE = 1.5343 - 0.0062269*fOptPhoWaveLength_nm;
    QE *= (1-exp(-1.7/cos(Angle)))/(1-exp(-1.7)); //angular dependence (Luiz)
    //fprintf(stderr,"%f rad\n",Angle*sign);
    G4double Temperature = aMaterial->GetTemperature()/kelvin;
    if ( Temperature < 200 ) {
      QE *= 1.122;//Here is the cold bonus that Kevin turned up. (Vic)
      if ( sign < 0 ) {
	      if ( aParticle->GetCharge() >= 1e-3 ) {
          QE *= 1.00;
	      }
        else QE *= 1.00;
      }
      if ( sign > 0 ) {
        QE *= 1.024;//This correction comes from Vic's optical model simulations.  (Vic)
	      if ( aParticle->GetCharge() >= 1e-3 ) {
          QE *= 1.00;
        }
	      else QE *= 1.00;
      }
    }
    if(QE<0) QE=0; if(QE>1) QE=1; //physicality enforced on poly. spline
    G4double DE = 0.90; //dynode efficiency (from CHF)

    string volumeName = aTrack.GetVolume()->GetName();
    //Need to get at the numbering-tracking variable
    LUXSimManager *luxManager = LUXSimManager::GetManager();
    G4bool useRealNumber = luxManager->GetPMTNumberingScheme();
    //Manipulate volumeName to match real PMT numbers, if useRealNumbers false
    if(!useRealNumber && 
       (volumeName.substr(0,21) == "Top_PMT_PhotoCathode_" ||
	volumeName.substr(0,24) == "Bottom_PMT_PhotoCathode_")) {
      int pmtNamePosition = 0;
      if(volumeName[0] == 'T')
	pmtNamePosition = 21;
      else
	pmtNamePosition = 24;
      string pmtNumString = volumeName.substr(pmtNamePosition);
      stringstream str;
      str << pmtNumString;
      int pmtNum = 0;
      str >> pmtNum;
      //Convert pmtNum to real number
      if(volumeName[0] == 'B')
	pmtNum += 61;
      pmtNum = LUXSim1_0PMTRenumbering::GetRealFromOldSim(pmtNum);
      //Clear stringstream to allow writing
      pmtNumString.clear();
      str.str(pmtNumString);
      str.clear();
      str.width(2);
      str.fill('0');
      str << pmtNum;
      //Rewrite volume name
      volumeName.replace(pmtNamePosition, 5, str.str());
    }
    char * volName = new char[volumeName.size()+1];
    volName[volumeName.size()] = 0;
    memcpy(volName,volumeName.c_str(),volumeName.size());
    string vStr = volName;

    //get PMT number
    unsigned pos = vStr.rfind("_");
    stringstream stream(vStr.substr(pos + 1));
    unsigned pmtNum;
    stream >> pmtNum;
    
    // QE values should have been loaded into the array 'QEvals[122]' in the
    // constructor function.  Now the following snippet sets the QE values, in 
    // addition to taking into account the ratio of VUV_gains/LED_gains.
	QE *= QEvals[pmtNum-1] / (doublePheProb[pmtNum-1]+1.);
    /*
    else {
      if ( sign > 0 ) QE *= 0.30413;
      if ( sign < 0 ) QE *= 0.34454;
    } */
    QE /= QE_AVG;

    G4ParticleMomentum pheMomentum(0, 0, sign); // for Xe PMTs
 
    //G4cout << "QE (which PMT)= " << QE << G4endl;
    //G4ParticleMomentum pheMomentum(0, 0, sign); // for Xe PMTs
    
    int vetoFlag = 0;
    // R5912/R7081 water tank PMTs
    if ( volumeName.substr(0,23) == "Water_PMT_PhotoCathode_" ) {
      vetoFlag = 1;
      if ( fOptPhoWaveLength_nm < 290 || fOptPhoWaveLength_nm > 620)
        QE = 0;
      else { // Cubic poly. fit to digitized plot from Hamamatsu datasheet.
             // Not perfect... but conservatively so.
        QE = ( -366.1 + 2.402*fOptPhoWaveLength_nm
	        - .004742*pow(fOptPhoWaveLength_nm,2)
	     + .000002940*pow(fOptPhoWaveLength_nm,3))*.01;
      }
      pheMomentum = oldMoment; // propagate phe in direction of photon.
    }
    
    //phe conversion fails OR 1st dynode not attained
    if ( G4UniformRand() > QE || G4UniformRand() > DE ) {
      aParticleChange.SetNumberOfSecondaries(0);
      return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }
    
    //generate the photo-electron in the PMT otherwise
    int phePerDetPhot;
    if (luxManager->GetLUXDoublePheRateFromFile()) {
      if ( G4UniformRand() < doublePheProb[pmtNum-1] && fOptPhoWaveLength_nm < 300. && vetoFlag == 0 ) {
        phePerDetPhot = 2;
      }
      else {
        phePerDetPhot = 1;
      }
    }
    else {
      if ( G4UniformRand() < 0.2 && fOptPhoWaveLength_nm < 300. && vetoFlag == 0 ) {
        phePerDetPhot = 2;
      }
      else {
        phePerDetPhot = 1;
      }
    }
    aParticleChange.SetNumberOfSecondaries ( phePerDetPhot );
    for ( int i = 0; i < phePerDetPhot; i++ ) {
      G4DynamicParticle* aPhotoElectron;
      //G4ParticleMomentum pheMomentum(0, 0, sign);
      aPhotoElectron =
	new G4DynamicParticle(G4ThermalElectron::ThermalElectron(),
			      pheMomentum);
      if ( aParticle->GetCharge() == 3e-3 )
	aPhotoElectron->SetKineticEnergy ( 4*MeV ); //SE
      else if ( aParticle->GetCharge() == 2e-3 )
	aPhotoElectron->SetKineticEnergy ( 3*MeV ); //ET
      else if ( aParticle->GetCharge() == 1e-3 )
	aPhotoElectron->SetKineticEnergy ( 2*MeV ); //S2
      else
	aPhotoElectron->SetKineticEnergy ( 1*MeV ); //S1
      G4Track* aSecondaryTrack = new G4Track(aPhotoElectron, t1, x1);
      aSecondaryTrack->SetTouchableHandle(pPostStepPoint->
					  GetTouchableHandle());
      aParticleChange.AddSecondary(aSecondaryTrack);
    }
    
    //the end (exiting)
    return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);

}

// GetMeanFreePath
// ---------------
G4double LUXSimQuantumEfficiency::GetMeanFreePath(const G4Track& aTrack,
                                          G4double,
                                          G4ForceCondition*)
{
  string volumeName = aTrack.GetVolume()->GetName();
  size_t found = volumeName.find("PhotoCathode"); //only absorb in PMTs
  if ( found != string::npos ) return 0*nm;
  else return DBL_MAX; //otherwise do nothing at all
}

// GetMeanLifeTime
// ---------------
G4double LUXSimQuantumEfficiency::GetMeanLifeTime(const G4Track&,
				    G4ForceCondition* condition)
{
  *condition = StronglyForced;
  return DBL_MAX;
}

void LUXSimQuantumEfficiency::LoadDoublePHEProb(G4String fileName)
{
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

void LUXSimQuantumEfficiency::LoadQEValuesFromFile(G4String fileName)
{
	std::ifstream file;
	file.open(fileName);
	if (!file.is_open()) {
		G4cout<<G4endl<<G4endl<<G4endl;
		G4cout<<"Quantum Efficiency file not found!"<<G4endl;
		G4cout<<G4endl<<G4endl<<G4endl;
		exit(0);
	}
	for (int i=0; i < 122; i++)
	{
		file >> QEvals[i];
	}
	file.close();
}


