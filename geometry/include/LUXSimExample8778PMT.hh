////////////////////////////////////////////////////////////////////////////////
/*	LUXSimExample8778PMT.hh
*
* This is the header file to define the 8778 model PMT by Hamamatsu.
*
********************************************************************************
* Change log
*	16 March 2009 - Initial submission (Kareem)
*	22 July 2010 - Added a PMTDeadFace component to this object, and changed the
*				   PMTWindow to PMTActiveWindow (Kareem)
*	13 September 2010 - Added PMT photocathode (activeVacuum) component 
*						(Matthew)
*	04 April 2011 - Changed PMT window / photocathode geometry to include Al 
*					flashing (Dave)
*	09 Feb 2012 - Added the photocathode geometry back in (Kareem)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimExample8778PMT_HH
#define LUXSimExample8778PMT_HH 1

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
class LUXSimExample8778PMT
{
	public:
		LUXSimExample8778PMT();
		~LUXSimExample8778PMT();
		
		inline G4LogicalVolume *GetLogicalVolume() { return logicalVolume; };
		inline LUXSimDetectorComponent *GetPMTFlashing() 
			{ return PMTFlashing; };
		inline LUXSimDetectorComponent *GetPMTPhotocathode()
			{ return PMTPhotocathode; };
		inline LUXSimDetectorComponent *GetPMTVacuum() { return PMTvacuum; };
		inline LUXSimDetectorComponent *GetPMTWindow() { return PMTWindow; };
		inline G4double GetPMTHeight() { return height; };
		inline G4double GetPMTRadius() { return outerDiameter/2.; };	
		
	private:
		
		G4LogicalVolume *logicalVolume;
		LUXSimDetectorComponent *PMTWindow;
		LUXSimDetectorComponent *PMTPhotocathode;
		LUXSimDetectorComponent *PMTFlashing;
		LUXSimDetectorComponent *PMTvacuum;
		G4double height;
		G4double outerDiameter;
};

#endif
