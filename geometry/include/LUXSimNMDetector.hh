//	LUXSimNMDetector.hh 

//	This is the header file to define the Neutron Multiplicity Detector.

//	04.12.10 - Initial submission based on code provided and writen by 
//		Christine Nielsen at UC Santa Barbara (Melinda)
//	05.27.10 - Added the source (Melinda)
//	06.15.10 - Added PlaceBottomLead, PlaceSource, and PlaceSideLead 
//		(Melinda)

#ifndef LUXSimNMDetector_HH
#define LUXSimNMDetector_HH 1

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4ThreeVector.hh"
//
//	LUXSim includes
//
#include "LUXSimDetector.hh"
//
//	Class forwarding
//
class G4LogicalVolume;
class LUXSimDetectorComponent;
//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimNMDetector : public LUXSimDetector
{

	public:
		LUXSimNMDetector(bool NORTH);
		~LUXSimNMDetector();
		
		void PlaceTopLead(G4double offsetY, 
						  G4double offsetZ, 
						  LUXSimDetectorComponent *leadMother);
		void PlaceBottomLead(LUXSimDetectorComponent *leadMother);
		void PlaceSource(G4double xPos, G4double yPos);
		void PlaceSideLead(LUXSimDetectorComponent *leadMother, G4double offsetZ);

	private:

		LUXSimDetectorComponent *lid;
		LUXSimDetectorComponent *lookingGlass[2];
		LUXSimDetectorComponent *water;
		LUXSimDetectorComponent *halon;
		LUXSimDetectorComponent *pmt[2];
		LUXSimDetectorComponent *topLead;
		LUXSimDetectorComponent *sideLead;
		LUXSimDetectorComponent *bottomLead;
		LUXSimDetectorComponent *source;
};

#endif

