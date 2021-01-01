////////////////////////////////////////////////////////////////////////////////
/*	LUXSimGNARRLIDetector.hh
 *
 * This is the header file to define the geometry of the GNARRLI Detector 
 * operating in either 1 or 4 PMT mode in an experimental hall.
 *
 ********************************************************************************
 * Change log
 *	23 Aug 11 - Initial submission (Tenzing)
 *
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGNARRLIDetector_HH
#define LUXSimGNARRLIDetector_HH 1

//
//	GEANT4 includes
//
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSimDetector.hh"


class G4LogicalVolume;
class LUXSimDetectorComponent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class LUXSimGNARRLIDetector : public LUXSimDetector
{
  public:
  
     LUXSimGNARRLIDetector();
    ~LUXSimGNARRLIDetector();

  public:
  
	G4double GetWorldFullLength()  {return fWorldLength;}; 
	G4double GetBeamLocation()		{return beamDepth;};
     
   
  private:

//	G4LogicalVolume*	logic8778holder;
//	LUXSimDetectorComponent*	physi8778holder;
	
	 
    G4double fWorldLength;           // Full length of the world volume
   
	G4double beamDepth;
	G4double liquidHeight;
	G4double gainGap;
	
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
