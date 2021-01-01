//	LUXSim0_1Detector.cc

//	09.09.26 - Initial submission (Dave M.)
//	09.09.29 - Minor geometry update to reduce volume overlaps (Dave M.)
//	09.09.30 - Grids added + compiler warnings fixed (Dave M.)
//	09.10.01 - Cleaned up code + fixed PMT_Can_1 name (removed white space) 
//			   (Dave M.)
//	01.28.10 - Changed LogicalSkinSurfaces to LogicalBorderSurfaces.  Added
//			   Some optical properties that were missing.  Changed dielectric
//			   metial surfaces to have optical surface finish to be "polish" 
//			   rather than "groundbackpainted".  For surfaces that are 
//			   lxe,gxe->teflon,cirlex, changed from dielectric_metal
//			   to dielectric_dielectric, and gave them optical surface finish
//			   groundfrontpainted (Melinda)

//	01.29.10 - Added one final optical boundary (Melinda)

//	03.05.10 - Got rid of bool WIRES and made grid placement changes
//			   related to LUXSimGrid changes (Melinda)

//	07.22.10 - Altered the PMT placement and optical boundaries to account for
//			   the discrete dead and active regions (Kareem)
//  12.08.13 - Adjusted for a new version of LUXSimGrid::LUXSimGrid
//
//	C/C++ includes
//
#include <sstream>

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4Box.hh"
#include "G4Ellipsoid.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4Polycone.hh"
#include "G4Polyhedra.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"

//
//	LUXSim includes
//
#include "LUXSim0_1Detector.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"
#include "LUXSimExample8778PMT.hh"
#include "LUXSimGrid.hh"

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSim0_1Detector
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSim0_1Detector::LUXSim0_1Detector( G4bool GRIDS )
{
	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();
	
	
	
	typedef enum MotherLog { GXE_LOG, LXE_LOG } motherLog;
	
	// Dimensions taken from active region / trifoil CADs (oft-used)
	G4double ptfe_inner_diameter = 4.6711*cm;
	G4double ptfe_thickness = 0.8395*cm;
	G4double bottom_cathode_gap = 1.0305*cm;
	G4double drift_length = 5.0013*cm;
	G4double gate_anode_gap = 0.4039*cm;
	G4double anode_top_gap = 0.3327*cm;
	G4double top_grid_top_PMT_array_gap = 2.5692*cm;
	G4double trifoil_reflector_thickness = 0.8255*cm;
	G4double al_reflector_thickness = 1.27*cm;
	G4double al_reflector_inner_diameter = 8.265*cm;
	G4double al_reflector_outer_diameter = 15.18*cm; // measured by ruler on CAD
	G4double al_inner_ptfe_height = 1.4275*cm;
	G4double al_inner_ptfe_inner_diameter = 5.9436*cm;
	G4double al_top_ptfe_thickness = 
		(top_grid_top_PMT_array_gap - trifoil_reflector_thickness - 
			al_reflector_thickness)/2;
	G4double top_grid_reflector_gap = al_top_ptfe_thickness;
	G4double bottom_grid_ptfe_gap = al_top_ptfe_thickness;
	G4double bottom_grid_bottom_PMT_gap = 0.5778*cm;
	
	G4double cirlex_outer_diameter = 9.83*cm; // measured by ruler on CAD
	
	G4double cu_vertex_diameter = 17.53*cm; // Cu block PDF on TWiki
	
	G4double source_path_ptfe_gap = 0.556*cm;
	G4double source_path_top_radial_width = 3.9467*cm;
	G4double source_path_top_height = 1.62*cm; // measured by ruler on CAD
	G4double source_path_bottom_radial_width = 2.6190*cm;
	G4double source_tube_thickness = 0.0889*cm; // communication with Adam 
												// Bradley
	G4double source_tube_inner_diameter = 6.35*cm;  // communication with Adam 
													// Bradley
	G4double source_tube_center_radial_distance = 11*cm; // CAD value WRONG; 
										// recalculated from PTFE / source path
	
	G4double trifoil_PMT_hole_diameter = 5.08*cm;
	G4double trifoil_thickness = 0.8255*cm;
	G4double trifoil_slope = 45.0*deg;
	
	
	// Dimensions of LXe
	G4double lxe_r = 30.0*cm;
	G4double lxe_h = 26.0*cm; // Approximation, from CAD
	G4double lxe_Z = -lxe_h/2;
	
	// Dimensions of GXe
	G4double gxe_r = lxe_r;
	G4double gxe_h = lxe_h;
	G4double gxe_Z = gxe_h/2;
	
	// Dimensions of main PTFE reflector around active region
	G4double ptfe_outer_diameter = ptfe_inner_diameter + 2*ptfe_thickness;
	G4double ptfe_h = bottom_cathode_gap + drift_length;
	G4double ptfe_Z = lxe_h/2 - ptfe_h/2 - gate_anode_gap/2; // relative to LXe 
															 // volume
	
	// Aluminum reflector (regular hollow cylinder from CAD)
	G4double al_reflector_Z = -gxe_h/2 + gate_anode_gap/2 + anode_top_gap + 
							top_grid_reflector_gap + al_reflector_thickness/2; 
													// relative to GXe volume
	
	// PTFE within Al reflector
	G4double al_inner_ptfe_inner_radius_array[2] = 
		{ al_inner_ptfe_inner_diameter/2, al_reflector_inner_diameter/2 };
	G4double al_inner_ptfe_outer_radius_array[2] = 
		{ al_reflector_inner_diameter/2, al_reflector_inner_diameter/2 };
	G4double al_inner_ptfe_height_array[2] = 
		{ -al_inner_ptfe_height/2, al_inner_ptfe_height/2 };
	G4double al_inner_ptfe_Z = 
		al_reflector_Z + al_reflector_thickness/2 - al_inner_ptfe_height/2;
	
	// PTFE atop Al reflector
	G4double al_top_ptfe_inner_diameter = al_reflector_inner_diameter;
	G4double al_top_ptfe_outer_diameter = al_reflector_outer_diameter;
	G4double al_top_ptfe_Z = al_reflector_Z + al_reflector_thickness/2 + 
													al_top_ptfe_thickness/2;
	
	// Cirlex
	G4double cirlex_inner_diameter = ptfe_outer_diameter; // approximation to 
													 // avoid volume overlaps
	G4double cirlex_h = anode_top_gap;
	const int NUM_CIRLEX = 4;
	G4double cirlex_Z[NUM_CIRLEX] = { -gxe_h/2 + gate_anode_gap/2 + cirlex_h/2, 
						lxe_h/2 - gate_anode_gap/2 - cirlex_h/2, 
						lxe_h/2 - gate_anode_gap/2 - drift_length - cirlex_h/2, 
						ptfe_Z - ptfe_h/2 + cirlex_h/2 };
	MotherLog cirlex_mother_log[NUM_CIRLEX] = { GXE_LOG,
												LXE_LOG,
												LXE_LOG,
												LXE_LOG };
	
	// Grids
	//if (USE_GRIDS)
	//{
	const int NUM_GRIDS = 5;
	G4double grid_wire_dia[NUM_GRIDS] = 
		{ 0.125*mm, 0.125*mm, 0.04*mm, 0.04*mm, 0.04*mm };
	G4double grid_wire_space = 2.0*mm;
	//G4double grid_wire_length = cirlex_inner_diameter;
	// Z coords from cathode grid for a) top grid, b) anode grid, c) gate grid, 
	// d) cathode grid, e) bottom grid
	G4double grid_Z[NUM_GRIDS] = { cirlex_Z[0] + cirlex_h/2, 
								cirlex_Z[0] - cirlex_h/2, 
								cirlex_Z[1] + cirlex_h/2, 
								cirlex_Z[2] + cirlex_h/2, 
								cirlex_Z[3] - cirlex_h/2 };
//	MotherLog grid_mother_log[NUM_GRIDS] = { GXE_LOG,
//											GXE_LOG,
//											LXE_LOG,
//											LXE_LOG,
//											LXE_LOG };
//	G4String grid_names[NUM_GRIDS] = { "Top_Grid", 
//									"Anode_Grid", 
//									"Gate_Grid", 
//									"Cathode_Grid", 
//									"Bottom_Grid" };
	
	// Field shaping rings
	G4double rings_inner_diameter = ptfe_outer_diameter;
	G4double rings_outer_diameter = 
			rings_inner_diameter + 2*source_path_ptfe_gap;
	G4double rings_h = 0.2*cm; // guess
	const int NUM_RINGS = 4;
	G4double rings_Z[NUM_RINGS] = 
							{ cirlex_Z[1] - (cirlex_Z[1] - cirlex_Z[2])*1/5,
							cirlex_Z[1] - (cirlex_Z[1] - cirlex_Z[2])*2/5,
							cirlex_Z[1] - (cirlex_Z[1] - cirlex_Z[2])*3/5,
							cirlex_Z[1] - (cirlex_Z[1] - cirlex_Z[2])*4/5 };
	
	// Dimensions of PMTs (R8778)
	G4double PMT_width = 57*mm;
	G4double PMT_body_height = 112*mm;
	
	// Radial distance of top PMTs from detector center
	G4double top_pmt_radial_dist = 33.5*mm;
	
	// Cu holder for PMTs
	//G4double cu_h = PMT_body_height;
	G4double cu_h = 125*mm;
	G4double cu_num_sides = 6;
	G4double cu_radius_array[4] = 
			{ 0.0*cm, cu_vertex_diameter/2, cu_vertex_diameter/2, 0.0*cm };
	G4double cu_height_array[4] = { -cu_h/2, -cu_h/2, cu_h/2, cu_h/2 };
	G4double cu_Z = -gxe_h/2 + gate_anode_gap/2 + anode_top_gap + 
						top_grid_top_PMT_array_gap + cu_h/2;
	
	// Trifoil reflectors
	G4double trifoil_punchout_hole_inner_radius_array[2] = { 0.0*cm, 0.0*cm };
	G4double trifoil_punchout_hole_outer_radius_array[2] = 
		{ (trifoil_PMT_hole_diameter+trifoil_thickness*tan(trifoil_slope))/2, 
		trifoil_PMT_hole_diameter/2 };
	G4double trifoil_punchout_height_array[2] = 
		{ -trifoil_thickness/2-0.05*mm, trifoil_thickness/2+0.05*mm };
	G4double trifoil_outer_diameter = cu_vertex_diameter * sin(60.0*deg); 
												// Use inner dimension of Cu
	G4double trifoil_Z = cu_Z - cu_h/2 - trifoil_thickness/2;
	
	// Bottom PMT PTFE holder
	G4double ptfe_bottom_PMT_inner_diameter = ptfe_inner_diameter;
	G4double ptfe_bottom_PMT_outer_diameter = cirlex_outer_diameter;
	G4double ptfe_bottom_PMT_h = 
		bottom_grid_bottom_PMT_gap - bottom_grid_ptfe_gap;
	G4double ptfe_bottom_PMT_Z = lxe_h/2 - gate_anode_gap/2 - ptfe_h - 
								bottom_grid_ptfe_gap - ptfe_bottom_PMT_h/2;
	
	// PMT positions
	G4double pmt_top_x[3] = { top_pmt_radial_dist*cos(120*deg), 
							top_pmt_radial_dist*cos(240*deg), 
							top_pmt_radial_dist*cos(360*deg) };
	G4double pmt_top_y[3] = { top_pmt_radial_dist*sin(120*deg), 
							top_pmt_radial_dist*sin(240*deg), 
							top_pmt_radial_dist*sin(360*deg) };
	//G4double pmt_top_Z = cu_Z - cu_h/2 + 6.0*cm;
	//G4double pmt_bot_Z = ptfe_bottom_PMT_Z - ptfe_bottom_PMT_h/2 - 6*cm;
	
	// Source tube
	G4double source_tube_outer_diameter = 
		source_tube_inner_diameter + 2*source_tube_thickness;
	G4double source_tube_orient = -30.0*deg;
	
	// Source path (PTFE spacer)
	G4double source_path_Z = cirlex_Z[1] - cirlex_Z[2];
	G4double source_path_disp_R = 
		rings_outer_diameter/2 + source_path_top_radial_width/2;
	G4double source_path_disp_Z = (cirlex_Z[1] + cirlex_Z[2]) / 2;
	
	// Al block
	G4double al_block_bottom_inner_diameter = 
		rings_outer_diameter + source_path_bottom_radial_width;
	G4double al_block_top_inner_diameter = cu_vertex_diameter;
	G4double al_block_outer_diameter = 2*source_tube_center_radial_distance + 
	source_tube_outer_diameter + 5*cm; // ish
	const int NUM_AL_BLOCK_Z_PLANES = 4;
	G4double al_block_lxe_inner_radius_array[NUM_AL_BLOCK_Z_PLANES] =
											{ al_block_bottom_inner_diameter/2,
											al_block_bottom_inner_diameter/2,
											al_block_top_inner_diameter/2,
											al_block_top_inner_diameter/2 };
	G4double al_block_lxe_outer_radius_array[NUM_AL_BLOCK_Z_PLANES] = 
												{ al_block_outer_diameter/2,
												al_block_outer_diameter/2,
												al_block_outer_diameter/2,
												al_block_outer_diameter/2 };
	G4double al_block_lxe_height_array[NUM_AL_BLOCK_Z_PLANES] = 
			{ -lxe_h/2,
			lxe_h/2 - (gate_anode_gap/2 + cirlex_h + source_path_top_height),
			lxe_h/2 - (gate_anode_gap/2 + cirlex_h + source_path_top_height),
			lxe_h/2 };
	
	
	
	// world volume ************************************************************
	
	G4double worldRadius = 11.0*m;
	G4double worldHeight = 22.0*m;
	
	G4Tubs* world_tube =
						new G4Tubs(
							"world_tube",
							0.*cm,
							worldRadius,
							0.5*worldHeight,
							0.*deg,
							360.*deg
						);
	G4LogicalVolume * world_log =
				new G4LogicalVolume(
					world_tube,
					luxMaterials->Vacuum(),
					"world_log"
				);
	world_log->SetVisAttributes(luxMaterials->VacuumVis());
	logicalVolume = world_log;
	
	
	// LXe volume **************************************************************
	
	G4Tubs * lxe_sol = 
		new G4Tubs("lxe_sol", 0.0*cm, lxe_r, lxe_h/2, 0.0*deg, 360.0*deg);
	G4LogicalVolume * lxe_log = 
		new G4LogicalVolume(lxe_sol, luxMaterials->LiquidXe(), "lxe_log");
	lxe = new LUXSimDetectorComponent(0,
									G4ThreeVector(0.0*mm,
												0.0*mm,
												lxe_Z),
									lxe_log,
									"LXe",
									world_log,
									true,
									0);
	lxe_log->SetVisAttributes(luxMaterials->LiquidXeVis());
	
	
	
	
	// Gas Xe volume ***********************************************************
	
	G4Tubs * gxe_sol = 
		new G4Tubs("gxe_sol", 0.0*cm, gxe_r, gxe_h/2, 0.0*deg, 360.0*deg);
	G4LogicalVolume * gxe_log = 
		new G4LogicalVolume(gxe_sol, luxMaterials->GasXe(), "gxe_log");
	gxe = new LUXSimDetectorComponent(0,
									G4ThreeVector(0.0*mm,
												0.0*mm,
												gxe_Z),
									gxe_log,
									"GXe",
									world_log,
									true,
									0);
	gxe_log->SetVisAttributes(luxMaterials->GasXeVis());
	
	
	// PTFE reflector **********************************************************
	
	G4Tubs * ptfe_sol = new G4Tubs("ptfe_sol", ptfe_inner_diameter/2, 
		ptfe_outer_diameter/2, ptfe_h/2, 0.0*deg, 360.0*deg);
	G4LogicalVolume * ptfe_log = 
		new G4LogicalVolume(ptfe_sol, luxMaterials->Teflon(), "ptfe_log");
	ptfe = new LUXSimDetectorComponent(0,
										G4ThreeVector(0.0*mm,
													0.0*mm,
													ptfe_Z),
										ptfe_log,
										"Main_PTFE_reflector",
										lxe_log,
										true,
										0);
	ptfe_log->SetVisAttributes(luxMaterials->TeflonVis());
	
	// Create optical surface
	G4OpticalSurface * ptfe_opt_surf = new G4OpticalSurface("ptfe_opt_surf", 
		unified, groundfrontpainted, dielectric_dielectric);
	ptfe_opt_surf->SetMaterialPropertiesTable(
		luxMaterials->Teflon()->GetMaterialPropertiesTable());
	G4LogicalBorderSurface * ptfe_border_surf;
	ptfe_border_surf = new G4LogicalBorderSurface ("ptfe_border_surf",
			lxe, ptfe, ptfe_opt_surf);	
	
	// Al reflector ************************************************************
	
	G4Tubs * al_reflector_sol = 
		new G4Tubs("al_reflector_sol", 
			al_reflector_inner_diameter/2, al_reflector_outer_diameter/2, 
			al_reflector_thickness/2, 
			0.0*deg, 360.0*deg);
	G4LogicalVolume * al_reflector_log = new G4LogicalVolume(al_reflector_sol, 
		luxMaterials->Aluminum(), "al_reflector_log");
	al_reflector = new LUXSimDetectorComponent(0,
											G4ThreeVector(0.0*mm,
														0.0*mm,
														al_reflector_Z),
											al_reflector_log,
											"Al_reflector",
											gxe_log,
											true,
											0);
	al_reflector_log->SetVisAttributes(luxMaterials->AluminumVis());
	
	// Create optical surface
	G4OpticalSurface * al_opt_surf = new G4OpticalSurface("al_opt_surf", 
		unified, polished, dielectric_metal);
	al_opt_surf->SetMaterialPropertiesTable(
		luxMaterials->Aluminum()->GetMaterialPropertiesTable());
	G4LogicalBorderSurface * al_border_surf;
	al_border_surf = new G4LogicalBorderSurface ("al_border_surf",
			gxe, al_reflector, al_opt_surf);
	
	// Inner PTFE ring within Al reflector *************************************
	G4Polycone * al_inner_ptfe_sol = new G4Polycone("al_inner_ptfe_sol", 
		0.0*deg, 360.0*deg, 
		2, 
		al_inner_ptfe_height_array, al_inner_ptfe_inner_radius_array, 
		al_inner_ptfe_outer_radius_array);
	G4LogicalVolume * al_inner_ptfe_log = new G4LogicalVolume(al_inner_ptfe_sol, 
								luxMaterials->Teflon(), "al_inner_ptfe_log");
	ptfe_al_inner = new LUXSimDetectorComponent(0,
												G4ThreeVector(0.0*mm,
															0.0*mm,
															al_inner_ptfe_Z),
												al_inner_ptfe_log,
												"Al_inner_PTFE",
												gxe_log,
												true,
												0);
	al_inner_ptfe_log->SetVisAttributes(luxMaterials->TeflonVis());
	
	// Create optical surface
	G4OpticalSurface * al_inner_ptfe_opt_surf = 
		new G4OpticalSurface("al_inner_ptfe_opt_surf", 
			unified, groundfrontpainted, dielectric_dielectric);
	al_inner_ptfe_opt_surf->SetMaterialPropertiesTable(
		luxMaterials->Teflon()->GetMaterialPropertiesTable());
	G4LogicalBorderSurface * al_inner_ptfe_border_surf;
	al_inner_ptfe_border_surf = new G4LogicalBorderSurface("al_inner_ptfe_border_surf",
			gxe, ptfe_al_inner, al_inner_ptfe_opt_surf);

	// Top PTFE ring above Al reflector ****************************************
	G4Tubs * al_top_ptfe_sol = new G4Tubs("al_top_ptfe_sol", 
		al_top_ptfe_inner_diameter/2, al_top_ptfe_outer_diameter/2, 
		al_top_ptfe_thickness/2, 
		0.0*deg, 360.0*deg);
	G4LogicalVolume * al_top_ptfe_log = new G4LogicalVolume(al_top_ptfe_sol, 
									luxMaterials->Teflon(), "al_top_ptfe_log");
	ptfe_al_top = new LUXSimDetectorComponent(0,
												G4ThreeVector(0.0*mm,
															0.0*mm,
															al_top_ptfe_Z),
												al_top_ptfe_log,
												"Al_top_PTFE",
												gxe_log,
												true,
												0);
	al_top_ptfe_log->SetVisAttributes(luxMaterials->TeflonVis());
	
	// Create optical surface
	G4OpticalSurface * al_top_ptfe_opt_surf = 
		new G4OpticalSurface("al_top_ptfe_opt_surf", 
			unified, groundfrontpainted, dielectric_dielectric);
	al_top_ptfe_opt_surf->SetMaterialPropertiesTable(
		luxMaterials->Teflon()->GetMaterialPropertiesTable());
	G4LogicalBorderSurface * al_top_ptfe_border_surf;
	al_top_ptfe_border_surf = new G4LogicalBorderSurface("al_top_ptfe_border_surf",
			gxe,ptfe_al_top,al_top_ptfe_opt_surf);
	
	// Cirlex rings ************************************************************
	
	G4Tubs * cirlex_sol = new G4Tubs("cirlex_sol", 
		cirlex_inner_diameter/2, cirlex_outer_diameter/2, 
		cirlex_h/2, 
		0.0*deg, 360.0*deg);
	G4LogicalVolume * cirlex_log = new G4LogicalVolume(cirlex_sol, 
									luxMaterials->Kapton(), "cirlex_log");

	//	Create optical surface
	G4OpticalSurface * cirlex_opt_surf = 
		new G4OpticalSurface("cirlex_opt_surf", 
			unified, groundfrontpainted, dielectric_dielectric);
	cirlex_opt_surf->SetMaterialPropertiesTable(
		luxMaterials->Kapton()->GetMaterialPropertiesTable());
	G4LogicalBorderSurface * cirlex_border_surf[NUM_CIRLEX];
	for (int ii_cirlex=0; ii_cirlex < NUM_CIRLEX; ii_cirlex++)
	{
		G4LogicalVolume * mother_log;
		
		if (cirlex_mother_log[ii_cirlex] == LXE_LOG)
			{ mother_log = lxe_log; }
		else
			{ mother_log = gxe_log; }
		
		stringstream name;
		name << "Cirlex_ring_" << ii_cirlex+1;
		
		cirlex_ring[ii_cirlex] = new LUXSimDetectorComponent(0,
											G4ThreeVector(0.0*mm,
														0.0*mm,
														cirlex_Z[ii_cirlex]),
											cirlex_log,
											name.str(),
											mother_log,
											true,
											0);
		if(ii_cirlex == 0) {
			cirlex_border_surf[ii_cirlex] = new G4LogicalBorderSurface("cirlex_border_surf",
					gxe, cirlex_ring[ii_cirlex], cirlex_opt_surf);
		}
		else  {
			cirlex_border_surf[ii_cirlex] = new G4LogicalBorderSurface("cirlex_border_surf",
					lxe, cirlex_ring[ii_cirlex], cirlex_opt_surf);
		}
		
	}
	cirlex_log->SetVisAttributes(luxMaterials->KaptonVis());
	
	
	// Field shaping rings *****************************************************
	
	G4Tubs * rings_sol = new G4Tubs("rings_sol", 
		rings_inner_diameter/2, rings_outer_diameter/2, 
		rings_h/2,
		0.0*deg, 360.0*deg);
	G4LogicalVolume * rings_log = new G4LogicalVolume(rings_sol, 
										luxMaterials->Copper(), "rings_log");
	//	Create optical surface
	G4OpticalSurface * rings_opt_surf = 
		new G4OpticalSurface("rings_opt_surf", 
			unified, polished, dielectric_metal);
	rings_opt_surf->SetMaterialPropertiesTable(
		luxMaterials->Copper()->GetMaterialPropertiesTable());
	G4LogicalBorderSurface * rings_border_surf[NUM_RINGS];

	
	for (int ii_rings = 0; ii_rings < NUM_RINGS; ii_rings++)
	{
		stringstream name;
		name << "Field_shaping_ring_" << ii_rings+1;
		
		field_ring[ii_rings] = new LUXSimDetectorComponent(0,
												G4ThreeVector(0.0*mm,
															0.0*mm,
															rings_Z[ii_rings]),
												rings_log,
												name.str(),
												lxe_log,
												true,
												0);
		rings_border_surf[ii_rings] = new G4LogicalBorderSurface(
				"rings_border_surf",lxe, field_ring[ii_rings], cirlex_opt_surf);
	}
	rings_log->SetVisAttributes(luxMaterials->CopperVis());
	
	
	// Grids *******************************************************************
	
	if (GRIDS)
	{
		//	Create optical surface

		for (int ii_grid=0; ii_grid<NUM_GRIDS; ii_grid++)
		{
//			G4LogicalVolume * mother_log;
//			
//			if (grid_mother_log[ii_grid] == LXE_LOG)
//				{ mother_log = lxe_log; }
//			else
//				{ mother_log = gxe_log; }
			
			G4String gridMaterial = "berylliumCopper";
			G4String gridMotherMaterial = "LiquidXe";

			LUXSimGrid * grid_tmp = new LUXSimGrid(grid_wire_dia[ii_grid]/2,
			                                       1,
			                                       ptfe_inner_diameter/2 - 0.1*mm, 
			                                       ptfe_inner_diameter/2,
			                                       grid_wire_dia[ii_grid],
			                                       ptfe_inner_diameter/2,
			                                       gridMaterial,gridMotherMaterial);
			if(ii_grid<2){
				grid_tmp->Place0_1WiresAndFrame(grid_wire_space,
					grid_wire_dia[ii_grid]/2,
					ptfe_inner_diameter/2 - 0.1*mm,grid_Z[ii_grid],gxe);
				grid_tmp->Place0_1MeshWires(grid_wire_space,
					grid_wire_dia[ii_grid]/2,
					ptfe_inner_diameter/2 - 0.1*mm);

			}
			else {
				grid_tmp->Place0_1WiresAndFrame(grid_wire_space,
					grid_wire_dia[ii_grid]/2,
					ptfe_inner_diameter/2 - 0.1*mm,grid_Z[ii_grid],lxe);
				grid_tmp->Place0_1MeshWires(grid_wire_space,
					grid_wire_dia[ii_grid]/2,
					ptfe_inner_diameter/2 - 0.1*mm);
			}
//			LUXSimGrid * grid_tmp = new LUXSimGrid(WIRES, 
//				grid_wire_space, grid_wire_dia[ii_grid]/2, 
//				1, 1, 
//				ptfe_inner_diameter/2 - 0.1*mm, ptfe_inner_diameter/2, 
//				grid_wire_dia[ii_grid], 
//				gridMaterial, gridMotherMaterial);
			
//			grid[ii_grid] = new LUXSimDetectorComponent(0,
//				G4ThreeVector(0.0*mm,
//							0.0*mm,
//							grid_Z[ii_grid]),
//				(WIRES ? 
//					grid_tmp->GetHolderLogical() : 
//					grid_tmp->GetGridLogical()),
//				grid_names[ii_grid],
//				mother_log,
//				true,
//				0);
//			if(ii_grid<2){
//				grid_border_surf[ii_grid] = 
//						new G4LogicalBorderSurface("grid_border_surf",
//						gxe, grid[ii_grid], grid_opt_surf);
//			}
//			else {
//				grid_border_surf[ii_grid] = 
//						new G4LogicalBorderSurface("grid_border_surf",
//						lxe, grid[ii_grid], grid_opt_surf);
//			}
		}
	}
	
	
	// Cu holder for top PMTs **************************************************
	// PMT solid -- intentionally 2x as tall as regular PMT for correct solid 
	// subtraction
	G4Tubs * pmt_sol = new G4Tubs("pmt_sol", 
		0.0*mm, PMT_width/2, 
		PMT_body_height, 
		0.0*deg, 360.0*deg);
	
	// Cu solid
	G4Polyhedra * cu_block_sol = new G4Polyhedra("cu_block_sol", 
		0.0*deg, 360.0*deg, 
		G4int(cu_num_sides), 4, 
		cu_radius_array, cu_height_array);
	
	// Cu solid with 3 PMT holes in it
	G4SubtractionSolid * cu_sol = new G4SubtractionSolid("cu_sol", 
		cu_block_sol, pmt_sol, 
		0, G4ThreeVector(pmt_top_x[0], pmt_top_y[0], 0.0*mm));
	cu_sol = new G4SubtractionSolid("cu_sol", 
		cu_sol, pmt_sol, 
		0, G4ThreeVector(pmt_top_x[1], pmt_top_y[1], 0.0*mm));
	cu_sol = new G4SubtractionSolid("cu_sol", 
		cu_sol, pmt_sol, 
		0, G4ThreeVector(pmt_top_x[2], pmt_top_y[2], 0.0*mm));
	
	G4LogicalVolume * cu_log = 
		new G4LogicalVolume(cu_sol, luxMaterials->Copper(), "cu_log");
	cu = new LUXSimDetectorComponent(0,
									G4ThreeVector(0.0*mm,
												0.0*mm,
												cu_Z),
									cu_log,
									"Cu_PMT_holder",
									gxe_log,
									true,
									0);

	cu_log->SetVisAttributes(luxMaterials->CopperVis());

	//	Create optical surface	
	G4OpticalSurface * cu_opt_surf = new G4OpticalSurface(
			"cu_opt_surf",  unified, polished, dielectric_metal);
	cu_opt_surf->SetMaterialPropertiesTable(
			luxMaterials->Copper()->GetMaterialPropertiesTable());
	G4LogicalBorderSurface * cu_border_surf;
	cu_border_surf = new G4LogicalBorderSurface("cu_border_surf", gxe,
			cu, cu_opt_surf);

	
	// Create PTFE trifoils ****************************************************
	G4Tubs * trifoil_full_sol = new G4Tubs("trifoil_full_sol", 
		0.0*mm, trifoil_outer_diameter/2,
		trifoil_thickness/2, 
		0.0*deg, 360.0*deg);
	G4Polycone * trifoil_punchout_sol = new G4Polycone("trifoil_punchout_sol", 
		0.0*deg, 360.0*deg, 
		2, 
		trifoil_punchout_height_array, 
		trifoil_punchout_hole_inner_radius_array, 
		trifoil_punchout_hole_outer_radius_array);
	
	G4SubtractionSolid * trifoil_sol = new G4SubtractionSolid("trifoil_sol", 
		trifoil_full_sol, trifoil_punchout_sol, 
		0, G4ThreeVector(pmt_top_x[0], pmt_top_y[0], 0.0*mm));
	trifoil_sol = new G4SubtractionSolid("trifoil_sol", 
		trifoil_sol, trifoil_punchout_sol, 
		0, G4ThreeVector(pmt_top_x[1], pmt_top_y[1], 0.0*mm));
	trifoil_sol = new G4SubtractionSolid("trifoil_sol", 
		trifoil_sol, trifoil_punchout_sol, 
		0, G4ThreeVector(pmt_top_x[2], pmt_top_y[2], 0.0*mm));
	
	G4LogicalVolume * trifoil_log = new G4LogicalVolume(trifoil_sol, 
		luxMaterials->Teflon(), "trifoil_log");
	trifoils = new LUXSimDetectorComponent(0,
										G4ThreeVector(0.0*mm,
													0.0*mm,
													trifoil_Z),
										trifoil_log,
										"PTFE_Trifoils",
										gxe_log,
										true,
										0);
	trifoil_log->SetVisAttributes(luxMaterials->TeflonVis());
	
	//	Create optical surface
	G4OpticalSurface * trifoil_opt_surf = new G4OpticalSurface(
			"trifoil_opt_surf", unified, groundfrontpainted, 
			dielectric_dielectric);
	trifoil_opt_surf->SetMaterialPropertiesTable(
		luxMaterials->Teflon()->GetMaterialPropertiesTable());
	G4LogicalBorderSurface * trifoil_border_surf;
	trifoil_border_surf	= new G4LogicalBorderSurface("trifoil_border_surf",
			gxe, trifoils, trifoil_opt_surf);
	
	// PMTs ********************************************************************
	
	LUXSimExample8778PMT * PMT_template[4];
	for (int ii_pmt_tmp = 0; ii_pmt_tmp < 4; ii_pmt_tmp++)
	{
		PMT_template[ii_pmt_tmp] = new LUXSimExample8778PMT();
		
		stringstream name;
		name << "PMT_Window_" << ii_pmt_tmp+1;
		PMT_template[ii_pmt_tmp]->GetPMTWindow()->SetName( name.str() );
		name.str("");
		name << "PMT_Flashing_" << ii_pmt_tmp+1;
		PMT_template[ii_pmt_tmp]->GetPMTFlashing()->SetName( name.str() );
		name.str("");
		name << "PMT_Vacuum_" << ii_pmt_tmp+1;
		PMT_template[ii_pmt_tmp]->GetPMTVacuum()->SetName( name.str() );
		//name.str("");
		//name << "PMT_Photocathode_" << ii_pmt_tmp+1;
		//PMT_template[ii_pmt_tmp]->GetPMTPhotocathode()->SetName( name.str() );
	}
	
	/* *** Commented out 2011-04-04 by DCM -- why are surfaces set for PMTs?? *** */
	/* //	Create optical surface
	G4OpticalSurface * botpmt_win_opt_surf =
			new G4OpticalSurface("botpmt_win_opt_surf",
			unified, polished, dielectric_metal);
	botpmt_win_opt_surf->SetMaterialPropertiesTable(
			luxMaterials->Quartz()->GetMaterialPropertiesTable());
	G4OpticalSurface * botpmt_can_opt_surf =
			new G4OpticalSurface("botpmt_can_opt_surf",
			unified, polished, dielectric_metal);
	botpmt_win_opt_surf->SetMaterialPropertiesTable(
			luxMaterials->Steel()->GetMaterialPropertiesTable());

	G4OpticalSurface * toppmt_win_opt_surf =
			new G4OpticalSurface("toppmt_win_opt_surf",
			unified, polished, dielectric_metal);
	toppmt_win_opt_surf->SetMaterialPropertiesTable(
			luxMaterials->Quartz()->GetMaterialPropertiesTable());


	// Bottom PMT
	PMT[0] = new LUXSimDetectorComponent(0,
										G4ThreeVector(0.0*mm,
													  0.0*mm,
													  pmt_bot_Z),
										PMT_template[0]->GetLogicalVolume(),
										"PMT_Can_1",
										lxe_log,
										true,
										0);
	G4LogicalBorderSurface * botpmt_winact_border_surf;
	botpmt_winact_border_surf = new G4LogicalBorderSurface("botpmt_winact_border_surf",
			lxe, PMT_template[0]->GetPMTWindow(), botpmt_win_opt_surf);

	G4LogicalBorderSurface * botpmt_windead_border_surf;
	botpmt_windead_border_surf = new G4LogicalBorderSurface("botpmt_windead_border_surf",
			lxe, PMT_template[0]->GetPMTDeadWindow(), botpmt_win_opt_surf);

	G4LogicalBorderSurface * botpmt_can_border_surf;
	botpmt_can_border_surf = new G4LogicalBorderSurface("botpmt_can_border_surf",
			lxe, PMT_template[0]->GetPMTVacuum(), botpmt_can_opt_surf);

	// Top PMT
	G4RotationMatrix * yRot = new G4RotationMatrix();
	yRot->rotateY(180.0*deg);
	for (int ii_top_pmt = 0; ii_top_pmt < 3; ii_top_pmt++)
	{
		stringstream name;
		name << "PMT_Can_" << ii_top_pmt+2;
		
		PMT[ii_top_pmt+1] = new LUXSimDetectorComponent(yRot,
								G4ThreeVector(pmt_top_x[ii_top_pmt], 
											  pmt_top_y[ii_top_pmt], 
											  pmt_top_Z),
								PMT_template[ii_top_pmt+1]->GetLogicalVolume(),
								name.str(),
								gxe_log,
								true,
								0);
		G4LogicalBorderSurface * toppmt_winact_border_surf;
		toppmt_winact_border_surf = new G4LogicalBorderSurface("toppmt_winact_border_surf",
				lxe, PMT_template[ii_top_pmt+1]->GetPMTWindow(), toppmt_win_opt_surf);

		G4LogicalBorderSurface * toppmt_windead_border_surf;
		toppmt_windead_border_surf = new G4LogicalBorderSurface("toppmt_windead_border_surf",
				lxe, PMT_template[ii_top_pmt+1]->GetPMTDeadWindow(), toppmt_win_opt_surf);
	}
	*/
	
	
	// PTFE holder for bottom PMT **********************************************
	
	G4Tubs * ptfe_bottom_PMT_sol = new G4Tubs("ptfe_bottom_PMT_sol", 
		ptfe_bottom_PMT_inner_diameter/2, ptfe_bottom_PMT_outer_diameter/2,
		ptfe_bottom_PMT_h/2, 
		0.0*deg, 360.0*deg);
	G4LogicalVolume * ptfe_bottom_PMT_log = 
		new G4LogicalVolume(ptfe_bottom_PMT_sol, 
			luxMaterials->Teflon(), "ptfe_bottom_PMT_log");
	ptfe_bottom = new LUXSimDetectorComponent(0,
											G4ThreeVector(0.0*mm,
														0.0*mm,
														ptfe_bottom_PMT_Z),
											ptfe_bottom_PMT_log,
											"PTFE_PMT_holder",
											lxe_log,
											true,
											0);
	ptfe_bottom_PMT_log->SetVisAttributes(luxMaterials->TeflonVis());
	
	// Create optical surface
	G4OpticalSurface * ptfe_bottom_PMT_opt_surf = 
		new G4OpticalSurface("ptfe_bottom_PMT_opt_surf", 
			unified, groundfrontpainted, dielectric_dielectric);
	ptfe_bottom_PMT_opt_surf->SetMaterialPropertiesTable(
		luxMaterials->Teflon()->GetMaterialPropertiesTable());
	G4LogicalBorderSurface * ptfe_bottom_PMT_border_surf;
	ptfe_bottom_PMT_border_surf = new G4LogicalBorderSurface(
			"ptfe_bottom_PMT_border_surf", lxe, ptfe_bottom, 
			ptfe_bottom_PMT_opt_surf);
	
	// Source tube *************************************************************
	// Two halves -- LXe half and GXe half
	
	// LXe half
	G4Tubs * source_tube_vacuum_sol = new G4Tubs("source_tube_vacuum_sol", 0,
	source_tube_inner_diameter/2, lxe_h/2, 0.0*deg, 360.0*deg);
	G4LogicalVolume * source_tube_vacuum_log = 
		new G4LogicalVolume(source_tube_vacuum_sol, 
		luxMaterials->Vacuum(), "source_tube_vacuum_log");
	source_tube_vacuum_bottom = new LUXSimDetectorComponent(0,
			G4ThreeVector(
				source_tube_center_radial_distance * cos(source_tube_orient),
				source_tube_center_radial_distance * sin(source_tube_orient),
				0.0*mm),
			source_tube_vacuum_log,
			"Source_tube_vacuum_(LXe_half)",
			lxe_log,
			true,
			0);

	source_tube_vacuum_log->SetVisAttributes(luxMaterials->VacuumVis());
	G4cout << "Source tube center at (" 
		<< source_tube_center_radial_distance * cos(source_tube_orient)
		<< ", " << source_tube_center_radial_distance * sin(source_tube_orient) 
		<< ", " << lxe_Z << ") mm" << G4endl;
	
	G4Tubs * source_tube_wall_sol = new G4Tubs("source_tube_wall_sol", 
		source_tube_inner_diameter/2, source_tube_outer_diameter/2, 
		lxe_h/2, 
		0.0*deg, 360.0*deg);
	G4LogicalVolume * source_tube_wall_log = 
		new G4LogicalVolume(source_tube_wall_sol, 
		luxMaterials->Aluminum(), "source_tube_wall_log");
	source_tube_wall_bottom = new LUXSimDetectorComponent(0,
			G4ThreeVector(
				source_tube_center_radial_distance * cos(source_tube_orient),
				source_tube_center_radial_distance * sin(source_tube_orient),
				0.0*mm),
			source_tube_wall_log,
			"Source_tube_wall_(LXe_half)",
			lxe_log,
			true,
			0);
	source_tube_wall_log->SetVisAttributes(luxMaterials->AluminumVis());

	//	Create optical surface
	G4OpticalSurface * source_tube_bottom_opt_surf = 
			new G4OpticalSurface("source_tube_bottom_opt_surf",
			unified, polished, dielectric_metal);
	source_tube_bottom_opt_surf->SetMaterialPropertiesTable(
			luxMaterials->Aluminum()->GetMaterialPropertiesTable());
	G4LogicalBorderSurface * source_tube_bottom_border_surf;
	source_tube_bottom_border_surf = new G4LogicalBorderSurface(
			"source_tube_bottom_border_surf", lxe, source_tube_wall_bottom, 
			source_tube_bottom_opt_surf);

	// GXe half
	source_tube_vacuum_sol = new G4Tubs("source_tube_vacuum_sol", 
		0, source_tube_inner_diameter/2, 
		gxe_h/2, 
		0.0*deg, 360.0*deg);
	source_tube_vacuum_log = new G4LogicalVolume(source_tube_vacuum_sol, 
		luxMaterials->Vacuum(), "source_tube_vacuum_log");
	source_tube_vacuum_top = new LUXSimDetectorComponent(0,
			G4ThreeVector(
				source_tube_center_radial_distance * cos(source_tube_orient),
				source_tube_center_radial_distance * sin(source_tube_orient),
				0.0*mm),
			source_tube_vacuum_log,
			"Source_tube_vacuum_(GXe_half)",
			gxe_log,
			true,
			0);
	source_tube_vacuum_log->SetVisAttributes(luxMaterials->VacuumVis());
	
	source_tube_wall_sol = new G4Tubs("source_tube_wall_sol", 
		source_tube_inner_diameter/2, source_tube_outer_diameter/2, 
		gxe_h/2, 
		0.0*deg, 360.0*deg);
	source_tube_wall_log = new G4LogicalVolume(source_tube_wall_sol, 
		luxMaterials->Aluminum(), "source_tube_wall_log");
	source_tube_wall_top = new LUXSimDetectorComponent(0,
			G4ThreeVector(
				source_tube_center_radial_distance * cos(source_tube_orient),
				source_tube_center_radial_distance * sin(source_tube_orient),
				0.0*mm),
			source_tube_wall_log,
			"Source_tube_wall_(GXe_half)",
			gxe_log,
			true,
			0);
	source_tube_wall_log->SetVisAttributes(luxMaterials->AluminumVis());

	//	Create optical surface
	G4OpticalSurface * source_tube_top_opt_surf = 
			new G4OpticalSurface("source_tube_top_opt_surf",
			unified, polished, dielectric_metal);
	source_tube_top_opt_surf->SetMaterialPropertiesTable(
			luxMaterials->Aluminum()->GetMaterialPropertiesTable());
	G4LogicalBorderSurface * source_tube_top_border_surf;
	source_tube_top_border_surf = new G4LogicalBorderSurface(
			"source_tube_top_border_surf", gxe, source_tube_wall_top, 
			source_tube_top_opt_surf);

	
	// Source path *************************************************************
	
	G4RotationMatrix * sourcePathRot = new G4RotationMatrix();
	sourcePathRot->rotateZ(-source_tube_orient);
	
	G4Box * source_path_block_sol = new G4Box("source_path_block_sol", 
		source_tube_center_radial_distance/2, 
		ptfe_inner_diameter/2, 
		source_path_Z/2);
	G4Tubs * active_region_punchout_sol = 
		new G4Tubs("active_region_punchout_sol", 
				0.0*mm, rings_outer_diameter/2, 
				source_path_Z, 
				0.0*deg, 360.0*deg);
	G4Tubs * source_tube_punchout_sol = new G4Tubs("source_tube_punchout_sol", 
				0.0*mm, source_tube_outer_diameter/2, 
				lxe_h + gxe_h, 
				0.0*deg, 360.0*deg);
	G4Tubs * al_block_lip_punchout_sol = new G4Tubs("al_block_lip_punchout_sol", 
				al_block_bottom_inner_diameter/2, al_block_outer_diameter/2, 
				source_path_Z/2, 
				0.0*deg, 360.0*deg);
	
	// Punch out hole for active region
	G4SubtractionSolid * source_path_sol = 
			new G4SubtractionSolid("source_path_sol", 
						source_path_block_sol, 
						active_region_punchout_sol, 
						0, 
						G4ThreeVector(-source_path_disp_R, 0.0*mm, 0.0*mm));
	// Punch out hole for source tube
	source_path_sol = new G4SubtractionSolid("source_path_sol", 
		source_path_sol,
		source_tube_punchout_sol,
		0,
		G4ThreeVector(source_tube_center_radial_distance - source_path_disp_R, 
			0.0*mm, 
			0.0*mm));
	// Punch out hole for Al block
	source_path_sol = 
			new G4SubtractionSolid("source_path_sol",
			source_path_sol,
			al_block_lip_punchout_sol,
			0,
			G4ThreeVector(
				-source_path_disp_R, 
				0.0*mm, 
				-source_path_top_height));
	
	
	G4LogicalVolume * source_path_log = new G4LogicalVolume(source_path_sol, 
		luxMaterials->Teflon(), "source_path_log");
	source_path = new LUXSimDetectorComponent(sourcePathRot,
					G4ThreeVector(source_path_disp_R * cos(source_tube_orient),
								source_path_disp_R * sin(source_tube_orient),
								source_path_disp_Z),
					source_path_log,
					"Source_path",
					lxe_log,
					true,
					0);
	source_path_log->SetVisAttributes(luxMaterials->TeflonVis());

	// Create optical surface
	G4OpticalSurface * source_path_opt_surf = 
		new G4OpticalSurface("source_path_opt_surf", 
			unified, groundfrontpainted, dielectric_dielectric);
	source_path_opt_surf->SetMaterialPropertiesTable(
		luxMaterials->Teflon()->GetMaterialPropertiesTable());
	G4LogicalBorderSurface * source_path_border_surf;
	source_path_border_surf = new G4LogicalBorderSurface(
			"source_path_border_surf",lxe, source_path, source_path_opt_surf);

	
	G4cout << "Source path center at (" 
		<< source_path_disp_R * cos(source_tube_orient)
		<< ", " << source_path_disp_R * sin(source_tube_orient) 
		<< ", " << source_path_disp_Z + lxe_Z << ") mm" << G4endl;
	
	
	// Aluminum block **********************************************************
	
	// LXe lower half
	G4Polycone * al_block_lxe_block_sol = 
		new G4Polycone("al_block_lxe_block_sol", 
			0.0*deg, 360.0*deg, 
			NUM_AL_BLOCK_Z_PLANES,
			al_block_lxe_height_array, 
			al_block_lxe_inner_radius_array, 
			al_block_lxe_outer_radius_array);
	
	G4SubtractionSolid * al_block_lxe_sol = 
		new G4SubtractionSolid("al_block_lxe_sol",
			al_block_lxe_block_sol,
			source_tube_punchout_sol,
			sourcePathRot,
			G4ThreeVector(
				source_tube_center_radial_distance * cos(source_tube_orient),
				source_tube_center_radial_distance * sin(source_tube_orient),
				0.0*mm));
	
	G4LogicalVolume * al_block_lxe_log = new G4LogicalVolume(al_block_lxe_sol, 
		luxMaterials->Aluminum(), "al_block_lxe_log");
	al_block_bottom = new LUXSimDetectorComponent(0,
												G4ThreeVector(0.0*mm,
															0.0*mm,
															0.0*mm),
												al_block_lxe_log,
												"Al_block_(LXe_half)",
												lxe_log,
												true,
												0);
	al_block_lxe_log->SetVisAttributes(luxMaterials->AluminumVis());
	
	// Create optical surface
	G4OpticalSurface * al_block_opt_surf = 
		new G4OpticalSurface("al_block_opt_surf", 
			unified, polished, dielectric_metal);
	al_block_opt_surf->SetMaterialPropertiesTable(
		luxMaterials->Aluminum()->GetMaterialPropertiesTable());
	G4LogicalBorderSurface * al_block_border_surf;
	al_block_border_surf = new G4LogicalBorderSurface("al_block_border_surf",
		lxe, al_block_bottom, al_block_opt_surf);
	
	// GXe upper half
	G4double al_block_gxe_h = 
		gate_anode_gap/2 + top_grid_top_PMT_array_gap + cu_h;
	G4Tubs * al_block_gxe_block_sol = new G4Tubs("al_block_gxe_block_sol", 
		al_block_top_inner_diameter/2, al_block_outer_diameter/2, 
		al_block_gxe_h/2, 
		0.0*deg, 360.0*deg);
	
	G4SubtractionSolid * al_block_gxe_sol = 
		new G4SubtractionSolid("al_block_gxe_sol",
			al_block_gxe_block_sol,
			source_tube_punchout_sol,
			sourcePathRot,
			G4ThreeVector(
				source_tube_center_radial_distance * cos(source_tube_orient),
				source_tube_center_radial_distance * sin(source_tube_orient),
				0.0*mm));
	
	G4LogicalVolume * al_block_gxe_log = new G4LogicalVolume(al_block_gxe_sol, 
								luxMaterials->Aluminum(), "al_block_gxe_log");
	al_block_top = new LUXSimDetectorComponent(0,
									G4ThreeVector(0.0*mm,
												0.0*mm,
												-gxe_h/2 + al_block_gxe_h/2),
									al_block_gxe_log,
									"Al_block_(GXe_half)",
									gxe_log,
									true,
									0);
	al_block_gxe_log->SetVisAttributes(luxMaterials->AluminumVis());

	// Create optical surface
	G4OpticalSurface * al_block_top_opt_surf = 
		new G4OpticalSurface("al_block_top_opt_surf", 
			unified, polished, dielectric_metal);
	al_block_top_opt_surf->SetMaterialPropertiesTable(
		luxMaterials->Aluminum()->GetMaterialPropertiesTable());
	G4LogicalBorderSurface * al_block_top_border_surf;
	al_block_top_border_surf = new G4LogicalBorderSurface(
			"al_block_top_border_surf", gxe, al_block_top, al_block_top_opt_surf);

}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSim1_0Detector
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSim0_1Detector::~LUXSim0_1Detector()
{}

