//	LUXSimGrid.hh
	
//	This is the header for the LUXSimGrid

//	09.25.09 - Initial submission (Melinda and Alex)

//	09.29.09 - changed to accept G4bool WIRES parameter (Melinda)

//	03.05.10 - Added PlaceWires and PlaceMeshWires.  (Melinda)

//	03.30.10 - Added new zOffset to make frame daughter of PTFE (Melinda)

//	21.12.10 - Added GetHolder methods for the purpose of creating appropriate logical border 
//             surfaces between the holders and other materials (Kareem)

//  08.12.13 - Added the radius of the wire grid span as a separate argument passed to these 
//             functions.  (Vic)

#ifndef LUXSimGrid_HH
#define LUXSimGrid_HH 1

//
//	GEANT4 includes
//
#include "globals.hh"

//
//	LUXSim includes
//

//
//	Class forwarding
//
class G4LogicalVolume;
class LUXSimDetectorComponent;
//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimGrid
{
		
	public:
		LUXSimGrid( G4double wireRadius,
				G4String frameShapeStr,
				G4double innerRadius,
				G4double frameOuterRadius,
				G4double frameHeight,
				G4double wireGridRadius,
				G4String gridMaterialStr,
				G4String gridMotherMaterialStr);
		~LUXSimGrid();

		void Place1_0WiresAndFrame(G4double wireSpacing,
								   G4double wireRadius,
								   G4double frameInnerRadius,
								   G4double zOff,
								   G4double hdpeHolderZ,
								   LUXSimDetectorComponent * frameMother,
								   LUXSimDetectorComponent * gridMother);
		void Place1_0MeshWires(G4double wireSpacing,
							   G4double wireRadius,
							   G4double frameInnerRadius);
		void Place0_1WiresAndFrame(G4double wireSpacing,
								   G4double wireRadius,
								   G4double frameInnerRadius,
								   G4double zOff,
								   LUXSimDetectorComponent * mother);
		void Place0_1MeshWires(G4double wireSpacing,
						 	   G4double wireRadius,
							   G4double frameInnerRadius);

	public:
		inline LUXSimDetectorComponent *GetHolder() { return holder; };

	private:
		G4LogicalVolume * frame_log;
		G4LogicalVolume * holder_log;
		LUXSimDetectorComponent * frame;
		LUXSimDetectorComponent * holder;
};

#endif

