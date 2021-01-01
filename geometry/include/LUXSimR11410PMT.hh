////////////////////////////////////////////////////////////////////////////////
/*	LUXSimR11410PMT.hh
*
* This is the header file to define the R11410 model PMT by Hamamatsu.
*
********************************************************************************
* Change log
*	
*	2012-05-28 - Initial submission (Dave and Kareem)
*	2014-03-07 - Setting up the constructor so that G4bool buildGeometry
*                    is initialized in the .hh file not the .cc.
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimR11410PMT_HH
#define LUXSimR11410PMT_HH 1

//
//	GEANT4 includes
//
#include "globals.hh"

//
//	Class forwarding
//
class G4LogicalVolume;
class LUXSimDetectorComponent;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimR11410PMT
{
	public:
		LUXSimR11410PMT( G4bool buildGeometry="true" );
		~LUXSimR11410PMT();
		
		inline G4LogicalVolume *GetLogicalVolume() { return logicalVolume; };
		inline LUXSimDetectorComponent *GetPMTFlashing() 
			{ return PMTFlashing; };
		inline LUXSimDetectorComponent *GetPMTPhotocathode()
			{ return PMTPhotocathode; };
		inline LUXSimDetectorComponent *GetPMTVacuum() { return PMTVacuum; };
		inline LUXSimDetectorComponent *GetPMTWindow() { return PMTWindow; };
		inline G4double GetPMTHeight() { return pmtHeight; };
		inline G4double GetPMTRadius() { return faceOuterDiameter/2.; };
		inline G4double GetPhotocathodeRadius()
				{ return photocathodeDiameter/2.; };
		
	private:
		
		G4LogicalVolume *logicalVolume;
		LUXSimDetectorComponent *PMTWindow;
		LUXSimDetectorComponent *PMTPhotocathode;
		LUXSimDetectorComponent *PMTFlashing;
		LUXSimDetectorComponent *PMTVacuum;
		G4double pmtHeight;
		G4double faceOuterDiameter;
		G4double photocathodeDiameter;
};

#endif
