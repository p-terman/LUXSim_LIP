//	LUXSim0_1Detector.hh

//	This is the header file to define the LUX 0.1 Detector.

//	09.09.09 - Initial submission (Dave M.)

#ifndef LUXSim0_1Detector_HH
#define LUXSim0_1Detector_HH 1

//
//	GEANT4 includes
//
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSimDetector.hh"
#include "LUXSimExample8778PMT.hh"
//
//	Class forwarding
//
class G4LogicalVolume;
class LUXSimDetectorComponent;
//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSim0_1Detector : public LUXSimDetector
{
		
	public:
		LUXSim0_1Detector(G4bool);
		~LUXSim0_1Detector();
		
		inline LUXSimDetectorComponent *GetLXe() 
			{ return lxe; };
		inline LUXSimDetectorComponent *GetGXe() 
			{ return gxe; };
		inline LUXSimDetectorComponent *GetPTFELiquidReflector() 
			{ return ptfe; };
		inline LUXSimDetectorComponent *GetAlReflector() 
			{ return al_reflector; };
		inline LUXSimDetectorComponent *GetAlInnerPTFE() 
			{ return ptfe_al_inner; };
		inline LUXSimDetectorComponent *GetAlTopPTFE() 
			{ return ptfe_al_top; };
		inline LUXSimDetectorComponent *GetCirlexRing(int index) 
			{ return cirlex_ring[index]; };
		inline LUXSimDetectorComponent *GetGrid(int index) 
			{ return grid[index]; };
		inline LUXSimDetectorComponent *GetFieldRing(int index) 
			{ return field_ring[index]; };
		inline LUXSimDetectorComponent *GetCopperBlock() 
			{ return cu; };
		inline LUXSimDetectorComponent *GetTrifoils() 
			{ return trifoils; };
		inline LUXSimDetectorComponent *GetPMT(int index) 
			{ return PMT[index]; };
		inline LUXSimDetectorComponent *GetBottomPTFE() 
			{ return ptfe_bottom; };
		inline LUXSimDetectorComponent *GetSourceTubeVacuumBottom() 
			{ return source_tube_vacuum_bottom; };
		inline LUXSimDetectorComponent *GetSourceTubeVacuumTop() 
			{ return source_tube_vacuum_top; };
		inline LUXSimDetectorComponent *GetSourceTubeWallBottom() 
			{ return source_tube_wall_bottom; };
		inline LUXSimDetectorComponent *GetSourceTubeWallTop() 
			{ return source_tube_wall_top; };
		inline LUXSimDetectorComponent *GetSourcePath() 
			{ return source_path; };
		inline LUXSimDetectorComponent *GetAlBlockBottom() 
			{ return al_block_bottom; };
		inline LUXSimDetectorComponent *GetAlBlockTop() 
			{ return al_block_top; };

	private:
		LUXSimDetectorComponent *lxe;
		LUXSimDetectorComponent *gxe;
		LUXSimDetectorComponent *ptfe;
		LUXSimDetectorComponent *al_reflector;
		LUXSimDetectorComponent *ptfe_al_inner;
		LUXSimDetectorComponent *ptfe_al_top;
		LUXSimDetectorComponent *cirlex_ring[4];
		LUXSimDetectorComponent *grid[5];
		LUXSimDetectorComponent *field_ring[4];
		LUXSimDetectorComponent *cu;
		LUXSimDetectorComponent *trifoils;
		LUXSimDetectorComponent *PMT[4];
		LUXSimDetectorComponent *ptfe_bottom;
		LUXSimDetectorComponent *source_tube_vacuum_bottom;
		LUXSimDetectorComponent *source_tube_vacuum_top;
		LUXSimDetectorComponent *source_tube_wall_bottom;
		LUXSimDetectorComponent *source_tube_wall_top;
		LUXSimDetectorComponent *source_path;
		LUXSimDetectorComponent *al_block_bottom;
		LUXSimDetectorComponent *al_block_top;

};

#endif
