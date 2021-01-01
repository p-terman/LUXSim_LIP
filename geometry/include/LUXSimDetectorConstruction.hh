////////////////////////////////////////////////////////////////////////////////
/*	LUXSimDetectorConstruction.hh
*
* This is the header file to build the detector geometry within LUXSim.
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*	7 April 2009 - Added support for the water tank (Melinda)
*	08-May-2009 - Added method to return the world physical volume (Kareem)
*	10-Feb-2010 - Added switch to indicate if detector has been specified (Dave)
*	16-Apr-2010 - Added Get* functions to retrieve detector components (Dave)
*	27-Sep-2010 - Removed the SetNewMaterials method prototype (Kareem)
*	18-Jan-2012 - Added method for changing materials properties (Dave)
*	06-Mar-2012	- Now saving detZ, and added methods for changing detector
*				  Z location (Dave)
*	09-Mar-2012	- Added variables and methods for pyramid (Dave)
*	09-Mar-2012	- Added support for Rayleigh UI hooks (Dave)
*	23-Apr-2013 - Added a class variable for the muon detector (a.k.a., the
*				  water tank) (Kareem)
*   26-Sep-2013 - Renamed the world volume "universe" (Kareem)
*
*/
////////////////////////////////////////////////////////////////////////////////


#ifndef LUXSimDetectorConstruction_HH
#define LUXSimDetectorConstruction_HH 1

//
//	GEANT4 includes
//
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

//
//	Class forwarding
//

class LUXSimManager;
class LUXSimDetectorComponent;
class LUXSimMaterials;
class LUXSimStand;
class LUXSimLZFlex;
class LUXSimMuonVetoSystem;

class LUXSimDetector;
class LUXSimExampleSimpleCryostat;
class LUXSimExampleComplexCryostat;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimDetectorConstruction : public G4VUserDetectorConstruction
{
	public:

		LUXSimDetectorConstruction();
		~LUXSimDetectorConstruction();

		G4VPhysicalVolume *Construct();
		void UpdateGeometry();
		
		LUXSimDetectorComponent	*GetWorldVolume()	{ return universe;};
		LUXSimDetector 			*GetDetectorType()	{ return detectorType; };
		LUXSimDetectorComponent	*GetDetector()		{ return detector; };
		LUXSimDetectorComponent *GetWaterTank()		{ return waterTank; };
		LUXSimDetectorComponent *GetPyramid()		{ return pyramid; };
		LUXSimStand				*GetCryoStand()		{ return cryoStand; };
		
		//	Materials methods
		void SetLXeTeflonRefl( G4double r );
		void SetLXeSteelRefl( G4double r );
		void SetLXeAbs( G4double l );
		void SetLXeRayleigh( G4double l );
		void SetGXeTeflonRefl( G4double r );
		void SetGXeSteelRefl( G4double r );
		void SetGXeAbs( G4double l );
		void SetGXeRayleigh( G4double l );
		void SetAlUnoxQuartzRefl( G4double r );
		
		//	Geometry methods
		void SetDetectorZ( G4double z );
		
		bool DetectorSpecified() { return detectorSpecified; };

	private:

		//	Detector components
		//	Just the experimentalHall should be a G4VPhysicalVolume. The rest
		//	should be LUXSimDetectorComponents.
		LUXSimDetectorComponent *universe;
		LUXSimDetector *detectorType;
		LUXSimDetectorComponent *detector;
		LUXSimDetectorComponent *waterTank;
		LUXSimDetectorComponent *pyramid;
		LUXSimStand *cryoStand;
		LUXSimMuonVetoSystem *muonDetector;

		//	Support for internal workings
		LUXSimManager *luxManager;
		LUXSimMaterials *materials;
		G4VPhysicalVolume *ConstructDetector();
		
		bool detectorSpecified;
		
		//	Detector Z position (default: 0 cm)
		G4double detZ;
		
};

#endif

