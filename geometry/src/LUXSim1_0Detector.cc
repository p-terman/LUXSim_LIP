//	LUXSim1_0Detector.cc

//	04.07.09 - Initial submission (Melinda)
//		This is the LUX1.0 detector design as far as I know it.  The parameters
//		come from a combination of the GEANT4 code that students at Brown were
//		using, the detecter mass spreadsheet (update 2.23.09), and the Ability 
//		Technology specs from the twiki (update 2.18.09).

//		The components are built from the top down, therefore the inner radius
//		of the vacuum vessel is the outer radius of the volume inside it 
//		(vacuum).  Only the copper thermal shield and xenonVessel are not 
//		defined in such a way; they have an inner and outer radius. 
//	  M. Sweany (sweany1@llnl.gov) 

//	04.23.09: Changed from 60 to 61 PMTs (Melinda)

//	04.28.09: Fixed the numbering of the top PMT array so they are axial 
//		with the bottom array (Melinda)

//	05.26.09: Changed the PMT placement to occur here rather than in the 
//		copper block.  PMTs are now rotated individually, which makes 
//		numbering easier (Melinda)

//	06.05.09: Added honeycomb-shaped teflon reflectors that sit on pmt 
//		holders, changed liquid/gas xenon to dodecagon-shaped. 
//		(Melinda)

//	06.12.09: Changed setup so that internals are daughters of liquid/gas
//		xenon (Melinda)

//	08.27.09 - Changed the overlap checking to false, added explicit 
//		overlap check (Kareem)

//	09.25.09 - Significant changes in layout to accommodate dimensions of 
//		internals from TexasA&M.  Made liquid/gas xenon mother volume of all 
//		internals to ensure that it fills in gaps. (Melinda)

//	09.29.09 - Added grid placement code (Melinda)
//	10.01.09 - Code cleanup (Melinda)
//

//	11.09.09 - Added optical surfaces to the PTFE sheets, bottom and top
//		PMT reflectors and grids (Melinda)

//	11.09.09 - Changed a few variable declarations to avoid compilation warnings
//		about unused variables (Kareem)

//	12.02.09 - Grid wires rotation (Alex)

//	12.03.09 - Changed grid overlap check to false (Melinda)

//	12.13.09 - Added optical boundary surfaces between the xenon (either liquid
//		or gaseous) and any part they touch: xenon vessel, radiation shield,
//		HDPE holder, PTFE sheets, PMT banks (top and bottom), PMT bank
//		reflectors, and PMT windows. In the process, I removed the material-
//		dependent material property tables for all but the liquid and gaseous
//		xenon, and applied the tables to the optical boundaries instead (Kareem)

//	12.17.09 - Fixed measurement bug in the height of the small 
//		copper block (Melinda)

//	01.07.10 - Gave PMTs a two-digit number (i.e. PMT_01 rather than PMT_1)
//		(Melinda)

//	01.25.10 - Initialized the *gridXeSurface array values to 0 to avoid a
//		seg fault when looking to see if they've yet been defined (Kareem)

//	01.25.10 - Renamed the grids to include the word "Grid" so that the names
//		"Top" and "Bottom" don't interfere with "Top_PMT" and "Bottom_PMT"
//		(Kareem)

//	01.29.10 - Changed optical surface finish of dielectric dielectric boundries
//		from ground to groundfrontpainted (Melinda)

//	01.29.10 - Turned on the overlap check for all LUXSimDetectorComponents, and
//		removed all PMT window optical boundary surfaces. Now all optical
//		interactions with the windows will be goverened solely by the
//		index of refraction. (Kareem)

//	02.26.10 - Adding cones of xenon to the reflectors rather than using
//	   G4Subration solid to cut out the cones in LUXSim1_0PMTBank.
//	   Added optical boundary between spacer and reflector. (Melinda)

//	03.05.10 - Made changes related to new methods in 1_0PMTBank and LUXSimGrid
//	   (Melinda)

//	03.09.10 - Changes due to PMT orientation and numbering scheme.  PMT 
//		numbering scheme now conforms to the document at
//		http://teacher.pas.rochester.edu:8080/wiki/pub/Lux/PMT/PMT_map.pdf 
//		(Melinda)

//	03.27.10 - Changed reflector overlap checks to false, added cout statements
//		to indicate placement has begin. (Melinda)

//	03.29.10 - Added field shaping rings, and made changes to gas xenon and
//		grid dimensions to accomidate them.  Changes ALL overlap checks to 
//		false (Melinda)

//	03.30.10 - Changes to fix grid hierarchy (Melinda) 

//	04.01.10 - Additional changes to accommodate field rings (Melinda)

//	08.06.10 - Changed the diameter of the bottom grid shield wires to
//		0.01016 cm (Kareem)

//	08.13.10 - Light code formatting (didn't change any of the actual code,
//		just moved some around slightly) (Kareem)

//	11.16.10 - Using new optical surfaces for Teflon interfaces defined in
//		LUXSimMaterials class (Dave)

//	21.12.10 - Added a logical border surface between the grids holders and the
//		PTFE sheets to get proper reflection of optical photons (Kareem)

//	21.07.11 - Added optical surfaces between PMT windows and reflectors (Dave)

//	30.11.11 - Extended gas to remove strange LXe pocket at top (Matthew)

//	05.12.11 - Fixed liquid level issue caused by last update. (Matthew)

//	13.12.11 - Extended gas a little further up (to 4.908 from 4.90795 cm)
//			   to further erase liquid vestiges (Matthew)

//	06.02.12 - Gave individual string names to each of the field rings (Kareem)

//	28.02.12 - Changed grid positions to cold using CHF diagram (Matthew)

//	14.03.12 - Reduced the outer radius of the HDPE holder to create a 0.5-cm
//			   xenon skin just inside the titanium cryostat. That xenon skin is
//			   a volume distinct from the active liquid xenon target volume.
//			   (Kareem)

//	26-Mar-2012 - Reduced the anode grid pitch to 0.25 mm, as recently measured
//				  after Run02 (Dave)

//	28.03.12 - Extended PTFE, HDPE up and down to close gaps. (Matthew)
//	02.04.12 - Redid last change with better numbers, to avoid overlap.(Matthew)

//	18.05.12 - Updated anode grid wire dia. to microscope-measured value, and 
//                 standardized all others into microns (Matthew)

//	11.07.12 - Changed anode grid wire diameter to final, final value, from 28.1
//			   to 28.4u, based on Woods' last SEM image measurement (Matthew)

//	08.16.12 - Added ability to use real PMT numbering system (or old) (Rich)

//  20.08.12 - Turned the xenon skin into a dodecagonal prism rather than a cylinder (Vic)
//	C/C++ includes

//	09.11.12 - Brought grids back into sync with design page for Run03.(Matthew)

//  09.11.12 - Realized that the HDPE holder was not in fact dodecagonal, and fixed that too (Vic)

//  16.09.12 - Fixed the geometry of the skin so that the liquid doesn't float up too high, and made
//             the inside of the skin dodecagonal so that it matches the HDPE holder (Vic)

//	09.25.12 - Added toggle for switching the LUX geometry between the surface
//			   (i.e., Run2) and underground (i.e., Run3 and beyond) geometries
//			   (Kareem)

//	09.25.12 - Added toggle for LUX1_0Detector gas run (Dave)

//	09.27.12 - Gas run toggle now switches optical boundaries as well (Dave)

//	09.28.12 - Bug fix for gas run toggle (Dave)

//	10.08.12 - Added the ptfeSourcePanels volume, which can be used to source
//			   "surface" events from the inner walls of the PTFE. See comments
//			   in the relevant section. (Kareem)

//	10.08.12 - Decreased the height of the PTFESourcePanels volumes to prevent
//			   volume overlap (Kareem)

//  10.30.12 - Mistake in surface geometry grid wire pitch for bottom grid (Matthew)

//  11.09.12 - Changed dead LXe skin thickness to 1cm (better match to Run02 data)
//                                     (Matthew)

//  01.07.13 - Moving all the definition of all volume dimensions to the start of the file so that 
//             we can more easily spot multiply defined dimensions (Vic)

//  01.08.13 - Fixing construction of the gaseous xenon layer to get rid of last liquid creep (Vic)

//  02.04.13 - Thermal contractions of HDPE, PTFE now roughly correct.
//                                    (Matthew)

//  03.15.13 - Fixed dimensions of all detector internals to cold, as-built values.  Removed as many
//             hard-coded dimensions as possible.  General code cleanup and increasing of comments 
//             for clarity.  Beware the Ides of March!!!  (Vic)

//	04.03.13 - Put the underground geometry hooks back in (the 17-degree
//			   rotation and the grid spacing and diameter) (Kareem)

//	09.04.13 - The 17-degree underground rotation now occurs in the water tank
//			   rather than the detector internals, to keep the x- and y-axes
//			   symmetric on the PMTs (Kareem)

//  10.04.2013 - Fixed the grid wire diameters so that they are, you know...  Right. (Vic)

//	15.04.13 - Changed the underground overall detector rotation from 17 degrees
//			   to 15 degrees. (Kareem)

//  2013-04-23 - Changed ptfeSheetsRInner to be 24.313461097 cm (was 23.495 cm).  This means that
//               radial distance to the corner of the PTFE sheets is now right: 23.485 cm (NOT 
//               23.495.  Oops...) is the radial distance to the flat face of the PTFE sheets.
//               (Vic)

//  2013-04-25 - Fixed the HDPE holder to be the right outer radius and shaved off the corners so
//               it fits inside the xenon vessel. (Vic)

//  2013-04-26 - Added top and bottom skin regions above the top and below the bottom PMT holders
//               so that the volumes "liquidXenon" and "gasXenon" only exist in the active part of 
//               the detector.  Also reset the grid placement so that the their z position is 
//               relative to the front face of the PMT bank.  (Vic)

//  2013-05-01 - Fixed a dumb mistake where I misplaced some curly brackets.  (Vic)

// 2013-05-04 - Made the bottom skin a little thicker to soak up some more of the liquidXenon 
//              volume bellow the PMTs.  (Vic)

// 2013-07-08 - Made outer cryostat wall thickness match inner (0.231") - same stock. (Matthew)

// 2013-12-08 - Added the radius of the extent of the wire grids (not grid rames) as an argument to 
//              the grid creation functions.  (Vic)

// 2014-01-13 - Fixed internals dimensions to match those in LuxDB00000216.  (Vic)

// 2014-08-20 - Fixed progress report bug, said "bottom PMTs", should have said
//              "top PMTs" (Kareem)

// 2014-08-27 - Adding a 3/16" layer to the outside of the xenon vessel and
//              inside the thermal shield. (Vic)

// 2014-10-31 - Corrected the width of field rings in FieldRingRCorners from 2.54 cm to 1.32 cm
//              according to the drawings on TWiki. (Lucie)

// 2015-03-18 - Made the PMT reflector thickness read out from LUXSim1_0PMTBank instead of 
//              redefined here. This requires we instantiate the PMT banks at the top of the file 
//              instead of where the rest of the PMT bank stuff lives. (Vic)

#define Cos15deg 9.65925826289068312e-1
#define HDPEShrinkageFactor 0.979
#define PTFEShrinkageFactor 0.991

//	C/C++ includes
//
#include <sstream>

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Ellipsoid.hh"
#include "G4Polyhedra.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4Material.hh"

//
//	LUXSim includes
//
#include "LUXSim1_0Detector.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSim1_0PMTBank.hh"
#include "LUXSimMaterials.hh"
#include "LUXSimExample8778PMT.hh"
#include "LUXSimGrid.hh"

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSim1_0Detector
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSim1_0Detector::LUXSim1_0Detector( G4bool GRIDS, G4bool useRealNumbering, G4bool gasRun )
{
// Detector Dimensions
  // Vacuum Vessel/outer cryostat
  const G4double vacuumVesselRadius = 39.53 * cm;
  const G4double vacuumVesselHeight = 145.93 * cm;
  const G4double vacuumVesselThickness = 0.58674 * cm;
  // Vacuum inside the vessel
  const G4double vacuumRadius = vacuumVesselRadius - vacuumVesselThickness;
  const G4double vacuumHeight = vacuumVesselHeight - vacuumVesselThickness;
  // Thermal shield
  const G4double thermalShieldRadius = 32.131 * cm;
  const G4double thermalShieldThickness = 0.1524 * cm;
  const G4double thermalShieldHeight = 96.52 * cm;
  // Xenon vessel
  const G4double xenonVesselRadius = 30.74924 * cm;//23.75" ID + 0.231" vessel thickness
  const G4double xenonVesselThickness = 0.58674 * cm;
  const G4double xenonVesselHeight = 113.03 * cm;
  // Liquid xenon--Fill the entire vessel with liquid xenon, then add volumes of other materials to 
  //               displace it as appropriate
  const G4double liquidXenonHeight = xenonVesselHeight;
  const G4double liquidXenonRadius = xenonVesselRadius - xenonVesselThickness;
  // Instantiate the PMT banks and read in the reflector thickness from them.
  banks[0] = new LUXSim1_0PMTBank();
  banks[1] = new LUXSim1_0PMTBank();
  const G4double ReflectorThickness = banks[0]->GetReflectorHeight();
  // Distances between grids:
  const G4double    PMTBanktoTop =  1.   * cm;
  const G4double      ToptoAnode =  3.63 * cm;
  const G4double     AnodetoGate =  1.   * cm;
  const G4double   GatetoCathode = 48.32 * cm;
  const G4double CathodetoBottom =  3.63 * cm;
  const G4double BottomtoPMTBank =  1.97 * cm;
  const G4double PMTBanktoPMTBank = PMTBanktoTop + ToptoAnode + AnodetoGate + GatetoCathode 
                                  + CathodetoBottom + BottomtoPMTBank;//Full vertical distance 
                                                                      //between top and  bottom PMT 
                                                                      //banks.
  // PTFE Sheets--There are two PTFE reflectors: one below the gate grid and one above, but we are 
  //              going to make them one volume as long as they don't cause overlap problems with 
  //              the grid frames.
  const G4double ptfeSheetsRInner = 23.65 * cm / Cos15deg;//Comes from 23.485 to the dodecagonal 
                                                           //face, divided by Cos(15 deg.) to get 
                                                           //the distance to the corner.
  //The PTFE is 1 cm warm and shrinks according to the above constant when cold
  const G4double ptfeSheetsROuter = ptfeSheetsRInner + (PTFEShrinkageFactor * 1. * cm / Cos15deg);
  // HDPE holder (field cage ring support).
  const G4double hdpeHolderInnerRadius  = ptfeSheetsROuter;//All of the radial distances here are 
                                                           //also face to face for the dodecagonal 
                                                           //parts, so we again divide through by 
                                                           //Cos(15 deg) to get the corner to 
                                                           //corner distance.
  const G4double hdpeHolderMiddleRadius = hdpeHolderInnerRadius 
                                        + (HDPEShrinkageFactor * 3.83 * cm / Cos15deg);
  const G4double hdpeHolderOuterRadius  = hdpeHolderInnerRadius 
                                        + (HDPEShrinkageFactor * 5.46 * cm / Cos15deg);
  const G4double hdpeHolderHeight = HDPEShrinkageFactor * 66.6242 * cm;
  const G4double hdpeHolderInnerHi =  (0.5 * PMTBanktoPMTBank) - ReflectorThickness;
  const G4double hdpeHolderInnerLo = (-0.5 * PMTBanktoPMTBank) + ReflectorThickness;
  const G4double hdpeHolderZ = -10. * cm;
  // HDPE insulation outside the xenon vessel and inside the thermal shield
  const G4double hdpeInsulationInnerRadius = xenonVesselRadius;
  const G4double hdpeInsulationThickness = 0.47625 * cm;
  const G4double hdpeInsulationHeight = thermalShieldHeight;
  // Superinsulation outside the thermal shield
  const G4double mylarInsulationInnerRadius = thermalShieldRadius;
  const G4double mylarInsulationThickness = 0.1 * cm;
  const G4double mylarInsulationHeight = thermalShieldHeight;
  // Gaseous xenon--Built in two parts: a cylendrical portion above the HDPE, and a dodecagonal 
  //                G4Polyhedra that fits down inside the profile of the HDPE and PTFE to halfway 
  //                between the anode and gate grids
  const G4double gasXenonOuterRadius  = liquidXenonRadius;
  const G4double gasXenonMiddleRadius = hdpeHolderMiddleRadius;
  const G4double gasXenonInnerRadius  = ptfeSheetsRInner; 
  const G4double gasXenonHeight = (0.5 * liquidXenonHeight) - (0.5 * hdpeHolderHeight) - hdpeHolderZ;
  const G4double gasXenonDodecTopZ = (0.5 * hdpeHolderHeight) - hdpeHolderInnerHi;
  const G4double gasXenonDodecMidZ = 0.;//set zero at the the ledge in the HDPE
  const G4double gasXenonDodecBotZ = -1. * (PMTBanktoTop + ToptoAnode) + (1.7 * mm);
  const G4double gasXenonZ = (0.5 * liquidXenonHeight) - (0.5 * gasXenonHeight);
  // Reflector sheets and PMT holders--these are defined separately in LUXSim1_0PMTBank.cc.  We will 
  //                                   therefore take them as they are, and simply define thier 
  //                                   offset here.  Note that all of the wire grids are defined by 
  //                                   their distance to the reflector on the top PMT bank!
  const G4double PMTBankHeight = 11.049 * cm; //From LUXSim1_0PMTBank.cc
  const G4double PMTBankGapSpace = 0.889 * cm;//From LUXSim1_0PMTBank.cc
  //const G4double PMTBankDodecDepth = 2.54 * cm;
  const G4double TopReflectorZ = hdpeHolderZ + hdpeHolderInnerHi + ReflectorThickness;
  const G4double   TopPMTBankZ = TopReflectorZ + (0.5 * PMTBankHeight) + (17.2 * mm);
  const G4double BotReflectorZ = hdpeHolderZ + hdpeHolderInnerLo - ReflectorThickness;
  const G4double   BotPMTBankZ = BotReflectorZ - (0.5 * PMTBankHeight) - (17.2 * mm);
  // Top copper radiation shield: It's a plate hanging from the lid of the xenon vessel
  const G4double radShieldRadius = xenonVesselRadius-xenonVesselThickness;
  const G4double radShieldHeight = 5.08 * cm;
  const G4double radShieldZ = (0.5 * liquidXenonHeight) - (0.5 * radShieldHeight);
  // Lower copper radiation shield: it's a big dome filling up the bottom of the xenon vessel
  const G4double xSemiAxis = radShieldRadius;
  const G4double ySemiAxis = xSemiAxis;
  const G4double zSemiAxis = radShieldRadius/2.;
  const G4double zBot = -radShieldRadius/2.;
  const G4double zTop = 0.*cm;
  const G4double radDomeZ = -xenonVesselHeight/2.;
  // Skin layers - There is a skin layer above and below the active volume so that LiquidXenon is, 
  // as much as possible only in the active region of the detector.  There is also a skin at the 
  // same z as the active region, but ouside the HDPE.  We will take up that entire space with a 
  // liquid skin, and then fill the upper region with gas to match the liquid level inside.
  const G4double LiqSkinHeight = hdpeHolderHeight + (2. * PMTBankGapSpace);
  const G4double GasSkinHeight = gasXenonDodecTopZ - gasXenonDodecBotZ - (0. * mm);
  const G4double LiqSkinOffset = hdpeHolderZ - PMTBankGapSpace;
  const G4double GasSkinOffset = (0.5 * LiqSkinHeight) - (0.5 * GasSkinHeight);//The gas skin is a daughter of the 
                                                                               //liquid skin, so we just need to 
                                                                               //make sure it is at the top of its 
                                                                               //mother volume.
  const G4double BotSkinHeight = BotPMTBankZ - (0.5 * PMTBankHeight) - radDomeZ + (0.75 * cm);
  const G4double BotSkinZ = radDomeZ + (0.5 * BotSkinHeight);
  const G4double BotSkinRadius = liquidXenonRadius;
  const G4double TopSkinHeight = (radShieldZ - (0.5 * radShieldHeight)) - ((TopPMTBankZ) + (0.5 * PMTBankHeight));
  const G4double TopSkinZ = radShieldZ - (0.5 * radShieldHeight) - (0.5 * TopSkinHeight);
  const G4double TopSkinRadius = liquidXenonRadius;

// Get the LUXSimMaterials pointer
  LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();
  // Build the vacuum vessel
  G4cout << "\tPlacing vacuum vessel..." << G4endl;
  G4Tubs *vacuumVessel_solid = new G4Tubs("vacuumVessel_solid",
	                                  0. * cm, vacuumVesselRadius, 
	                                  0.5 * vacuumVesselHeight,
	                                  0. * deg, 360. * deg);
  logicalVolume = new G4LogicalVolume(vacuumVessel_solid, luxMaterials->Titanium(),
			              "vacuumVessel_log" );
  logicalVolume->SetVisAttributes(luxMaterials->TitaniumVis());
  // now put vacuum inside the vessel
  G4Tubs * vacuum_solid = new G4Tubs("vacuum_solid", 0. * cm, vacuumRadius,
	                             0.5 * vacuumHeight,
	                             0. * deg, 360. * deg);
  G4LogicalVolume *vacuum_log = new G4LogicalVolume(vacuum_solid, luxMaterials->Vacuum(),
	                                            "vacuum_log");
  vacuum_log->SetVisAttributes(luxMaterials->VacuumVis());
  G4RotationMatrix *rotZsurface = new G4RotationMatrix();
  if( !luxManager->GetLUXSurfaceGeometry() && luxManager->GetMuonVetoSelection() == "on" ){
    rotZsurface->rotateZ(15.*deg);
  }
  vacuum = new LUXSimDetectorComponent(rotZsurface, G4ThreeVector(),vacuum_log, "Vacuum", 
	                               logicalVolume, 0, 0, false);
  // HDPE insulation
  G4cout << "\tPlacing HDPE thermal insulation..." << G4endl;
  G4Tubs *hdpeInsulation_solid = new G4Tubs("hdpeInsulation_solid",
	                                   hdpeInsulationInnerRadius, 
	                                   hdpeInsulationInnerRadius + hdpeInsulationThickness,
	                                   0.5 * hdpeInsulationHeight,
	                                   0. * deg, 360. * deg);
  G4LogicalVolume *hdpeInsulation_log = new G4LogicalVolume(hdpeInsulation_solid,
	                                                        luxMaterials->Polyethylene(),
	                                                       "hdpeInsulation_log");
  hdpeInsulation_log->SetVisAttributes(luxMaterials->PolyethyleneVis());
  hdpeInsulation = new LUXSimDetectorComponent(0, G4ThreeVector(), "HDPEInsulation",
                                               hdpeInsulation_log, vacuum, 0, 0, false );
  // Copper Thermal Shield 
  G4cout << "\tPlacing thermal shield..." << G4endl;
  G4Tubs *thermalShield_solid = new G4Tubs("thermalShield_solid",
	                                   thermalShieldRadius - thermalShieldThickness, 
	                                   thermalShieldRadius,
	                                   0.5 * thermalShieldHeight,
	                                   0. * deg, 360. * deg);
  G4LogicalVolume *thermalShield_log = new G4LogicalVolume(thermalShield_solid,
	                                                   luxMaterials->Copper(),
	                                                   "thermalShield_log");
  thermalShield_log -> SetVisAttributes (luxMaterials->CopperVis());
  thermalShield = new LUXSimDetectorComponent(0, G4ThreeVector(), "ThermalShield",thermalShield_log,
	                                      vacuum, 0, 0, false );
  // Super insulation
  G4cout << "\tPlacing mylar insulation..." << G4endl;
  G4Tubs *mylarInsulation_solid = new G4Tubs("mylarInsulation_solid",
	                                         mylarInsulationInnerRadius, 
	                                         mylarInsulationInnerRadius + mylarInsulationThickness,
	                                         0.5 * mylarInsulationHeight,
	                                         0. * deg, 360. * deg);
  G4LogicalVolume *mylarInsulation_log = new G4LogicalVolume(mylarInsulation_solid,
	                                                        luxMaterials->Polyethylene(),//polyethylene has pretty close to the same density as aluminized mylar...
	                                                       "mylarInsulation_log");
  mylarInsulation_log->SetVisAttributes(luxMaterials->PolyethyleneVis());
  mylarInsulation = new LUXSimDetectorComponent(0, G4ThreeVector(), "MylarInsulation",
                                                mylarInsulation_log, vacuum, 0, 0, false);
  // Xenon Vessel (or inner cryostat)
  G4cout << "\tPlacing inner cryostat..." << G4endl;
  G4Tubs *xenonVessel_solid = new G4Tubs("xenonVessel_solid",
	                                 xenonVesselRadius - xenonVesselThickness,xenonVesselRadius,
	                                 0.5 * xenonVesselHeight,
	                                 0. * deg, 360. * deg);
  G4LogicalVolume *xenonVessel_log = new G4LogicalVolume(xenonVessel_solid,
	                                                 luxMaterials->Titanium(),
	                                                 "xenonVessel_log");
  xenonVessel_log->SetVisAttributes(luxMaterials->TitaniumVis());
  xenonVessel = new LUXSimDetectorComponent(0, G4ThreeVector(),"XenonVessel", xenonVessel_log, 
	                                    vacuum, 0, 0, false);
  // Liquid Xenon
  G4cout << "\tFilling with liquid xenon..." << G4endl;
  G4Tubs * liquidXenon_solid = new G4Tubs("liquidXenon_solid",
	                                  0. * cm, liquidXenonRadius,
	                                  0.5 * liquidXenonHeight,
	                                  0. * deg, 360. * deg);
  G4LogicalVolume *liquidXenon_log;
  if (gasRun) {
    liquidXenon_log = new G4LogicalVolume(  liquidXenon_solid,
					    luxMaterials->GasXe(),"liquidXenon_log");
    liquidXenon_log->SetVisAttributes( luxMaterials->GasXeVis() );
  }
  else {
    liquidXenon_log = new G4LogicalVolume(liquidXenon_solid,
					  luxMaterials->LiquidXe(),
					  "liquidXenon_log");
    liquidXenon_log->SetVisAttributes(luxMaterials->LiquidXeVis());
  }
  liquidXenon = new LUXSimDetectorComponent(0, G4ThreeVector(), "LiquidXenon",
	                                    liquidXenon_log,vacuum, 0, 0, false );
  // Gas Xenon
  G4cout << "\tAdding gas layer on top..." << G4endl;
  G4VSolid *gasXenonTop_solid = new G4Tubs("gasXenonTop_solid",          
                                           0.*cm, gasXenonOuterRadius,
	                                   0.5 * gasXenonHeight,
	                                   0.*deg,360.*deg);
  G4double gasXenonRCorners[6] = {0.               , gasXenonInnerRadius, gasXenonInnerRadius, gasXenonMiddleRadius, gasXenonMiddleRadius, 0.};
  G4double gasXenonZCorners[6] = {gasXenonDodecBotZ, gasXenonDodecBotZ  , gasXenonDodecMidZ  , gasXenonDodecMidZ   , gasXenonDodecTopZ   , gasXenonDodecTopZ};
  G4VSolid *gasXenonDodec_solid = new G4Polyhedra("gasXenonDodec_solid",
	                                          0.*deg,360.*deg, 12,
	                                          6, gasXenonRCorners, gasXenonZCorners);
  G4VSolid * gasXenon_solid = new G4UnionSolid("gasXenon_solid",
	                                       gasXenonTop_solid, gasXenonDodec_solid, 0,
                                               G4ThreeVector(0., 0. ,(-0.5 * gasXenonHeight) - gasXenonDodecTopZ));
  G4LogicalVolume *gasXenon_log = new G4LogicalVolume(gasXenon_solid, 
	                                              luxMaterials->GasXe(),
	                                              "gasXenon_log"); 
  gasXenon_log->SetVisAttributes( luxMaterials->GasXeVis() );
  gasXenon = new LUXSimDetectorComponent(0, G4ThreeVector(0.*cm,0.*cm,gasXenonZ), "GasXenon",
	                                 gasXenon_log, liquidXenon, 0, 0, false ); 
  // Optical boundary between the inner cryostat and gaseous xenon
  G4LogicalBorderSurface *titaniumGasXeSurface;
  titaniumGasXeSurface = new G4LogicalBorderSurface("titaniumGasXeSurface", gasXenon, xenonVessel, 
                                                    luxMaterials->GXeTiSurface());
  // Copper Radiation Shields
  G4cout << "\tAdding copper radiation shields..." << G4endl;
  G4Tubs *radShield_solid = new G4Tubs("radShield_solid", 0.*cm, radShieldRadius, 
                                       radShieldHeight/2., 0.*deg,360.*deg);
  G4LogicalVolume *radShield_log = new G4LogicalVolume(radShield_solid, luxMaterials->Copper(),
				                       "radShield_log");
  radShield_log->SetVisAttributes(luxMaterials->CopperVis());//                           |
  radShield = new LUXSimDetectorComponent(0,//Not the offset of the mother volume here!  \|/
	                                  G4ThreeVector(0. * cm, 0. * cm, radShieldZ - gasXenonZ), 
	                                  "RadShield", radShield_log, gasXenon, 0, 0, false );
  G4Ellipsoid *radDome_solid = new G4Ellipsoid("radDome_solid", xSemiAxis, ySemiAxis, zSemiAxis,
			                       zBot, zTop);
  G4LogicalVolume *radDome_log = new G4LogicalVolume(radDome_solid, luxMaterials->Copper(),
			                             "radDome_log");
  radDome_log -> SetVisAttributes (luxMaterials->CopperVis());
  radDome = new LUXSimDetectorComponent(0, G4ThreeVector(0.*cm,0.*cm,radDomeZ), "RadDome", 
	                                radDome_log, vacuum, 0, 0, false );
  // Optical boundary between the radiation shield and gaseous xenon. No optical boundary is needed 
  // between the radiation dome and anything else.
  G4LogicalBorderSurface *radShieldGasXeSurface;
  radShieldGasXeSurface = new G4LogicalBorderSurface("radShieldGasXeSurface", gasXenon, radShield,
			                             luxMaterials->GXeCopperSurface());
  //HDPE Holder:
  //The HDPE holder gets built in two steps.  The first is the creation of a set of neseted 
  //dodecagonal pieces with different heights so that the shorter one sits inide the taller.  We 
  //build this with a G4Polyhedra object.  We then subtract off a G4Tubs volume with an inner 
  //radius of liquidXenonRadius to ensure that the corners fit inside the xenon vessel.
  G4cout << "\tAdding HDPE holder..." << G4endl;
  G4double hdpeHolderRCorners[8] = {hdpeHolderInnerRadius, hdpeHolderMiddleRadius, hdpeHolderMiddleRadius,   hdpeHolderOuterRadius,  hdpeHolderOuterRadius,  hdpeHolderMiddleRadius, hdpeHolderMiddleRadius, hdpeHolderInnerRadius};
  G4double hdpeHolderZCorners[8] = {hdpeHolderInnerLo,     hdpeHolderInnerLo,     -0.5 * hdpeHolderHeight,  -0.5 * hdpeHolderHeight, 0.5 * hdpeHolderHeight, 0.5 * hdpeHolderHeight, hdpeHolderInnerHi,      hdpeHolderInnerHi};
  G4VSolid *hdpeHolder_dodec = new G4Polyhedra("hdpeHolder_dodec",
	                                       0.*deg, 360.*deg, 12,
	                                       8, hdpeHolderRCorners, hdpeHolderZCorners);
  G4VSolid *hdpeHolder_subtr = new G4Tubs("hdpeHolder_subtr", 
                                          liquidXenonRadius, liquidXenonRadius + (1. * m),
                                          1. * m, 0. * deg, 360. * deg);
  G4VSolid *hdpeHolder_solid = new G4SubtractionSolid("hdpeHolder_solid", 
                                                      hdpeHolder_dodec, hdpeHolder_subtr,
                                                      0, G4ThreeVector());
  G4LogicalVolume * hdpeHolder_log = new G4LogicalVolume(hdpeHolder_solid, 
                                                         luxMaterials->Polyethylene(),
			                                 "hdpeHolder_log");
  hdpeHolder_log->SetVisAttributes( luxMaterials->PolyethyleneVis() );
  hdpeHolder = new LUXSimDetectorComponent(0, G4ThreeVector(0.,0.,hdpeHolderZ), "HDPEHolder", 
                                           hdpeHolder_log, liquidXenon, 0, 0, false );
  // Optical boundary between the HDPE and liquid xenon
  G4LogicalBorderSurface *hdpeLiquidXeSurface;
  hdpeLiquidXeSurface = new G4LogicalBorderSurface("hdpeLiquidXeSurface", liquidXenon, hdpeHolder,
                                                   luxMaterials->LXeHDPESurface() );
  // Optical boundary between the HDPE and gaseous xenon
  G4LogicalBorderSurface *hdpeGasXeSurface;
  hdpeGasXeSurface = new G4LogicalBorderSurface("hdpeGasXeSurface", gasXenon, hdpeHolder, 
                                                luxMaterials->GXeHDPESurface());
  // Liquid xenon skin
  G4cout << "\tAdding liquid skin volume..." << G4endl;
  G4Tubs *liquidXenonSkin_outer = new G4Tubs("liquidXenonSkin_outer",
                                             0. * cm, liquidXenonRadius, 
                                             0.5 * LiqSkinHeight, 
                                             0.*deg, 360.*deg);
  G4double LiquidSkinRCorners[4] = {0.,                   hdpeHolderOuterRadius + (0. * um),   hdpeHolderOuterRadius + (0. * um), 0.};
  G4double LiquidSkinZCorners[4] = {-0.5 * LiqSkinHeight, -0.5 * LiqSkinHeight,                0.5 * LiqSkinHeight,               0.5 * LiqSkinHeight};
  G4VSolid *liquidXenonSkin_inner = new G4Polyhedra("liquidXenonSkin_inner", 0.*deg, 360.*deg, 12,
	                                            4, LiquidSkinRCorners, LiquidSkinZCorners);
  G4VSolid *liquidXenonSkin_solid = new G4SubtractionSolid("liquidXenonSkin_solid", 
                                                           liquidXenonSkin_outer, 
                                                           liquidXenonSkin_inner, 
	                                                   0, G4ThreeVector());
  G4LogicalVolume *liquidXenonSkin_log; 
  if (gasRun) {
    liquidXenonSkin_log = new G4LogicalVolume(
					      liquidXenonSkin_solid, luxMaterials->GasXe(),
					      "liquidXenonSkin_log" );
    liquidXenonSkin_log->SetVisAttributes( luxMaterials->GasXeVis() );
  }
  else {
    liquidXenonSkin_log = new G4LogicalVolume(liquidXenonSkin_solid, 
					      luxMaterials->LiquidXe(),
					      "liquidXenonSkin_log" );
    liquidXenonSkin_log->SetVisAttributes( luxMaterials->LiquidXeVis() );
  }
  liquidXenonSkin = new LUXSimDetectorComponent(0, G4ThreeVector(0. * cm, 0, LiqSkinOffset),
	                                        "LiquidSkinXenon", liquidXenonSkin_log, 
	                                        liquidXenon, 0, 0, false );
  // Gaseous xenon skin
  G4cout << "\tand, gaseous skin..." << G4endl;
  G4Tubs *gaseousXenonSkin_outer = new G4Tubs("gaseousXenonSkin_outer",
                                              0. * cm, liquidXenonRadius, 
                                              0.5 * GasSkinHeight, 
                                              0.*deg, 360.*deg );
  G4double GasSkinRCorners[4] = {LiquidSkinRCorners[0], LiquidSkinRCorners[1], LiquidSkinRCorners[2], LiquidSkinRCorners[3]};
  G4double GasSkinZCorners[4] = {-0.5 * GasSkinHeight,  -0.5 * GasSkinHeight,  0.5 * GasSkinHeight,   0.5 * GasSkinHeight};
  G4VSolid *gaseousXenonSkin_inner = new G4Polyhedra("gasousXenonSkin_inner",
	                                             0.*deg, 360.*deg, 12,
	                                             4, GasSkinRCorners, GasSkinZCorners);
  G4VSolid *gaseousXenonSkin_solid = new G4SubtractionSolid("gaseousXenonSkin_solid",
	                                                    gaseousXenonSkin_outer,
	                                                    gaseousXenonSkin_inner, 
	                                                    0, G4ThreeVector());
  G4LogicalVolume *gaseousXenonSkin_log = new G4LogicalVolume(gaseousXenonSkin_solid, 
	                                                      luxMaterials->GasXe(),
	                                                      "gaseousXenonSkin_log");
  gaseousXenonSkin_log->SetVisAttributes( luxMaterials->GasXeVis() );
  gaseousXenonSkin = new LUXSimDetectorComponent(0, G4ThreeVector( 0. * cm, 0., GasSkinOffset),
	                                         "GaseousSkinXenon", gaseousXenonSkin_log, 
	                                         liquidXenonSkin, 0, 0, false );
  // Bottom skin layer
  G4Tubs *bottomXenonSkin_solid = new G4Tubs("bottomXenonSkin_solid", 0. * cm, BotSkinRadius, 
                                             0.5 * BotSkinHeight, 0. * deg, 360. * deg);
  G4LogicalVolume *bottomXenonSkin_log = new G4LogicalVolume(bottomXenonSkin_solid,
                                                             luxMaterials->LiquidXe(),
                                                             "bottomXenonSkin_log");
  bottomXenonSkin = new LUXSimDetectorComponent(0, G4ThreeVector(0. * cm, 0. * cm, BotSkinZ),
                                                "bottomXenonSkin", bottomXenonSkin_log,
                                                liquidXenon, 0, 0, false);
// Top skin layer
  G4Tubs *topXenonSkin_solid = new G4Tubs("topXenonSkin_solid", 0. * cm, TopSkinRadius,
                                             0.5 * TopSkinHeight, 0. * deg, 360. * deg);
  G4LogicalVolume *topXenonSkin_log = new G4LogicalVolume(topXenonSkin_solid,
                                                          luxMaterials->GasXe(),
                                                          "topXenonSkin_log");
  topXenonSkin = new LUXSimDetectorComponent(0, G4ThreeVector(0. * cm, 0. * cm, TopSkinZ - gasXenonZ),
                                             "topXenonSkin", topXenonSkin_log,
                                             gasXenon, 0, 0, false);

  // Optical boundary between the inner cryostat and liquid xenon skin
  G4LogicalBorderSurface *titaniumLXeSkinSurface;
  titaniumLXeSkinSurface = new G4LogicalBorderSurface("titaniumLXeSkinSurface", liquidXenonSkin, 
	                                              xenonVessel, luxMaterials->LXeTiSurface());
  // Optical boundary between the HDPE and liquid xenon skin
  G4LogicalBorderSurface *hdpeLXeSkinSurface;
  hdpeLXeSkinSurface = new G4LogicalBorderSurface("hdpeLXeSkinSurface", liquidXenonSkin, hdpeHolder,
                                                  luxMaterials->LXeHDPESurface() );
	//	PTFE Sheets:
  G4cout << "\tBuilding PTFE reflectors..." << G4endl;
  G4double ptfeSheetsRCorners[4] = {ptfeSheetsRInner,  ptfeSheetsROuter,  ptfeSheetsROuter,  ptfeSheetsRInner};
  G4double ptfeSheetsZCorners[4] = {hdpeHolderInnerLo, hdpeHolderInnerLo, hdpeHolderInnerHi, hdpeHolderInnerHi};
	G4Polyhedra * ptfeSheets_solid = new G4Polyhedra("ptfeSheets_solid",
	                                                 0.*deg, 360.*deg, 12,
	                                                 4, ptfeSheetsRCorners, ptfeSheetsZCorners);
	G4LogicalVolume *ptfeSheets_log = new G4LogicalVolume(ptfeSheets_solid,
			                                                  luxMaterials->Teflon(),
			                                                  "ptfeSheets_log");
	ptfeSheets_log->SetVisAttributes( luxMaterials->TeflonVis() );
	ptfeSheets = new LUXSimDetectorComponent(0, G4ThreeVector(0.*cm,0.*cm,hdpeHolderZ), "PTFESheets",
	                                         ptfeSheets_log, liquidXenon, 0, 0, false );
	//	Optical boundary between the Teflon (PTFE) and liquid xenon
	G4LogicalBorderSurface *ptfeLiquidXeSurface;
	if (gasRun) {
	  ptfeLiquidXeSurface = new G4LogicalBorderSurface( "ptfeLiquidXeSurface",
							    liquidXenon, ptfeSheets, luxMaterials->GXeTeflonSurface() );
        } else {
	ptfeLiquidXeSurface = new G4LogicalBorderSurface( "ptfeLiquidXeSurface",
			liquidXenon, ptfeSheets, luxMaterials->LXeTeflonSurface() );
	}
	//	Optical boundary between the Teflon (PTFE) and gaseous xenon
	G4LogicalBorderSurface *ptfeGasXeSurface;
	ptfeGasXeSurface = new G4LogicalBorderSurface( "ptfeGasXeSurface", gasXenon,
			ptfeSheets, luxMaterials->GXeTeflonSurface() );
	//	Surface source panels
	//	These panels are used to generate "surface" events from the PTFE walls.  "Surface" is in 
	//  quotes because these volumes are buried 1 nm inside the PTFE Sheets. This is so that the 
	//  optical boundary between the liquid/gas xenon and the PTFE remains intact. The 1 nm value is 
	//  based on personal communications with Vince Guiseppe at the University of South Dakota, who 
	//  measured radon daughters having an implantation depth of 100 nm in acrylic. This 100 nm comes 
	//  from direct implantation, and does not incorporate diffusion. --Kareem
	G4double angle = (360. / 12) * deg;
	G4double width = (2. * ptfeSheetsRInner * sin(0.5 * angle)) - (1. * mm);
	G4double depth = 100. * nm;
	G4double height = hdpeHolderInnerHi - hdpeHolderInnerLo - (0.5 * mm);
	G4Box *ptfeSourcePanels_solid = new G4Box("ptfeSourcePanels_solid",
			                                      0.5 * depth, 0.5 * width, 0.5 * height);
	G4LogicalVolume *ptfeSourcePanels_log[12];
	for( G4int i = 0; i < 12; i++ ) {
		stringstream name;
		name << "ptfeSourcePanels_log";
		if( i < 9 ){name << "0";}
		name << i + 1;
		ptfeSourcePanels_log[i] = new G4LogicalVolume(ptfeSourcePanels_solid, 
		                                              luxMaterials->Teflon(), 
		                                              name.str().c_str());
		ptfeSourcePanels_log[i]->SetVisAttributes(luxMaterials->TeflonVis());
	}
	G4double theta = -0.5 * angle;
	G4double faceRadius = (ptfeSheetsRInner * Cos15deg) + (0.5 * depth) + (1. * nm);
	G4double xOff = 0., yOff = 0., zOff = 0.;
	for(G4int i = 0; i < 12; i++){
		theta += angle;
		xOff = faceRadius * cos(theta);
		yOff = faceRadius * sin(theta);
		zOff = 0.5 * (hdpeHolderInnerHi + hdpeHolderInnerLo);
		G4RotationMatrix *rotZpanels = new G4RotationMatrix();
		                  rotZpanels->rotateZ(-1. * theta);
		stringstream name;
		name << "PTFESourcePanels_";
		if( i < 9 ){name << "0";}
		name << i + 1;
		ptfeSourcePanels[i] = new LUXSimDetectorComponent(rotZpanels, G4ThreeVector(xOff, yOff, zOff), 
		                                                  name.str().c_str(), ptfeSourcePanels_log[i], 
		                                                  ptfeSheets, 0, 0, false );
	}
	//	Field Shaping rings: these are modeled as 47 individual dodecagon rings with OD = 20.5" and 
	//                       ID = 19.5".  Based on conversations with Ty Stiegler from TAMU
  G4cout << "\tAdding field rings..." << G4endl;
  G4double FieldRingThickness = 0.3302 * cm;
  G4double FieldRingWidth = 1.32 * cm;
  G4double FieldRingRCorners[4] = {hdpeHolderInnerRadius,     hdpeHolderInnerRadius + FieldRingWidth, hdpeHolderInnerRadius + FieldRingWidth, hdpeHolderInnerRadius};
  G4double FieldRingZCorners[4] = {-0.5 * FieldRingThickness, -0.5 * FieldRingThickness,           0.5 * FieldRingThickness,            0.5 * FieldRingThickness};
  G4Polyhedra * fieldRing_solid = new G4Polyhedra("fieldRing_solid", 
                                                  0. * deg, 360. * deg, 12,
                                                  4, FieldRingRCorners, FieldRingZCorners);
	G4LogicalVolume *fieldRing_log = new G4LogicalVolume(fieldRing_solid,
			                                                 luxMaterials->Copper(),
			                                                 "fieldRings_log");
	G4double fieldRingZ = hdpeHolderInnerLo + CathodetoBottom + (0.5 * FieldRingThickness) + (20. * mm);
	stringstream shapingRingName;
	const G4int NFieldRings = 47;
	for(int f = 0; f < NFieldRings; f++){
		shapingRingName.str("");
		shapingRingName << "FieldRing_" << f + 1;
		fieldRings[f] = new LUXSimDetectorComponent(0, G4ThreeVector(0. * cm, 0. * cm, fieldRingZ),
				                                        shapingRingName.str(), fieldRing_log, hdpeHolder, 
				                                        0, 0, false);
		fieldRingZ += (HDPEShrinkageFactor * 1. * cm);
	}
	//	PMT Banks: top and bottom copper banks, each with 61 PMTs.  First the bottom bank...
	G4cout << "\tPlacing bottom PMT bank..." << G4endl;
	xOff = 0.; yOff = 0.;
	G4RotationMatrix *rotY180 = new G4RotationMatrix();
	rotY180->rotateY(180.*deg);
	G4RotationMatrix *rotY0 = new G4RotationMatrix();
	rotY0->rotateY(0.*deg);
	bottomBank = new LUXSimDetectorComponent(0, G4ThreeVector(xOff,yOff,BotPMTBankZ), 
	                                         "BottomPMTHolder", banks[0]->GetCopperLogical(), 
	                                         liquidXenon, 0, 0, false );
	//	Optical boundary between the bottom PMT bank and liquid xenon
	G4LogicalBorderSurface *bottomBankLiquidXeSurface;
	bottomBankLiquidXeSurface = new G4LogicalBorderSurface(
			"bottomBankLiquidXeSurface", liquidXenon, bottomBank,
			luxMaterials->LXeCopperSurface() );
	//	Now the bottom reflector
	G4cout << "\tPlacing bottom reflector...\n";
	bottomReflector = new LUXSimDetectorComponent(0, G4ThreeVector(xOff,yOff,BotReflectorZ),
	                                              "BottomReflector", banks[0]->GetReflectorLogical(), 
	                                              liquidXenon, 0, 0, false );
  BottomPMTZ = BotReflectorZ;
	if (gasRun)
	  banks[0]->PlaceXenonCones("gasXe", bottomReflector );
	else
	  banks[0]->PlaceXenonCones("liquidXe", bottomReflector );
	//	Optical boundary between the bottom PMT bank reflector and liquid xenon
	G4LogicalBorderSurface *bottomBankRefLiquidXeSurface;
	if (gasRun) {
	  bottomBankRefLiquidXeSurface = new G4LogicalBorderSurface(
								    "bottomBankRefLiquidXeSurface", liquidXenon, bottomReflector,
								    luxMaterials->GXeTeflonSurface() );
	} else {
	  bottomBankRefLiquidXeSurface = new G4LogicalBorderSurface(
			"bottomBankRefLiquidXeSurface", liquidXenon, bottomReflector,
			luxMaterials->LXeTeflonSurface() );  //std::abort();
	}
  // Then the top bank...                               Again, note the gas volume offset----+
	G4cout << "\tPlacing top PMT bank..." << G4endl;//                                         |
	topBank = new LUXSimDetectorComponent(rotY180, G4ThreeVector(xOff, yOff, TopPMTBankZ - gasXenonZ), 
	                                      "TopPMTHolder", banks[1]->GetCopperLogical(), 
	                                      gasXenon, 0, 0, false );
	//	Optical boundary between the top PMT bank and gaseous xenon
	G4LogicalBorderSurface *topBankGasXeSurface;
	topBankGasXeSurface = new G4LogicalBorderSurface( "topBankGasXeSurface",
			gasXenon, topBank, luxMaterials->GXeCopperSurface() );
	//	Now the top reflector                                             The gas volume offset again---+
	G4cout << "\tPlacing top reflector...\n";//                                                        \|/
	topReflector = new LUXSimDetectorComponent(rotY180, G4ThreeVector(xOff,yOff,TopReflectorZ - gasXenonZ), 
	                                           "TopReflector", banks[1]->GetReflectorLogical(), 
	                                           gasXenon, 0, 0, false );
  TopPMTZ = TopReflectorZ + banks[1]->GetReflectorHeight();
	banks[1]->PlaceXenonCones("gasXe", topReflector );
	//	Optical boundary between the top PMT bank reflector and gaseous xenon
	G4LogicalBorderSurface *topBankRefGasXeSurface;
	topBankRefGasXeSurface = new G4LogicalBorderSurface("topBankRefGasXeSurface", gasXenon, topReflector, 
	                                                    luxMaterials->GXeTeflonSurface());  //std::abort();
	//	PMT Placement and numbering                         
	const G4double PMTinBankOffset = 12.445 * mm;//             +-----To bring the PMTs up to the same vertical 
	G4cout << "\tPlacing bottom PMTs...\n";//                  \|/    position as the back face of the reflectors
	banks[0]->PlacePMTs(liquidXenon_log, BotPMTBankZ + PMTinBankOffset, useRealNumbering);
	G4cout << "\tPlacing top PMTs...\n";
	banks[1]->PlacePMTs(gasXenon_log, TopPMTBankZ - gasXenonZ - PMTinBankOffset, useRealNumbering);
	/*                                                 /|\
	                                                    +-----Vertical offset for gas!!!*/
	// Create optical surfaces between PMT windows and reflectors
	// To 0th order, assuming reflectivity between quartz and PTFE is similar to 
	// reflectivity between LXe and PTFE (similar refractive indices...)
	for (int ii=0; ii<61; ii++){
		new G4LogicalBorderSurface("bottomBankPMTWindowSurface", banks[0]->GetPMT(ii)->GetPMTWindow(), 
		                           bottomReflector, luxMaterials->QuartzTeflonSurface());
		new G4LogicalBorderSurface("topBankPMTWindowSurface", banks[1]->GetPMT(ii)->GetPMTWindow(), 
		                           topReflector, luxMaterials->QuartzTeflonSurface());  
	}  //std::abort();
	//	Grid Placement
	if(GRIDS){
		G4String          gridNames[5] = { "Top",     "Anode",    "Gate",     "Cathode", "Bottom"};
		G4double        wireSpacing[5] = { 0.5 * cm,  0.025 * cm, 0.5 * cm,   0.5 * cm,  1. * cm};
		G4double       wireDiameter[5] = { 50.8 * um, 28.4 * um,  101.6 * um, 206. * um, 206. * um};
		if( luxManager->GetLUXSurfaceGeometry() ) {
		  wireSpacing[3] = 1.0 * cm;
		  wireSpacing[4] = 0.5 * cm;
		  wireDiameter[3] = wireDiameter[4] = 101.6*um;
		}//                          +---This number is different on purpose to get rid of an
//                                          \|/  overlap error.  I'm sorry...  --Vic
  	G4double      frameHeight[5] = { 0.280 * cm, 1.016 * cm, 0.635 * cm, 0.635 * cm, 0.635 * cm};
	G4String gridMotherMaterial[5] = {"GasXe",     "GasXe",    "LiquidXe", "LiquidXe", "LiquidXe"};
		if (gasRun) {
		  gridMotherMaterial[2] = "GasXe";
		  gridMotherMaterial[3] = "GasXe";
		  gridMotherMaterial[4] = "GasXe";
		}
		const G4double frameInnerRadius = hdpeHolderInnerRadius;
		const G4double frameOuterRadius = hdpeHolderInnerRadius + (1. * cm);
    //Set grid z positions
		TopGridZ     = TopReflectorZ - PMTBanktoTop + (0. * mm);
		AnodeGridZ   = TopGridZ - ToptoAnode;
		GateGridZ    = AnodeGridZ - AnodetoGate;//Recall that AnodetoGate = 1 cm, and the LXe/GXe border is halfway in between
		CathodeGridZ = GateGridZ - GatetoCathode;
		BottomGridZ  = CathodeGridZ - CathodetoBottom;
    S2VolumeHeight = GateGridZ - CathodeGridZ;
		G4double                       gridZ[5] = {TopGridZ, AnodeGridZ, GateGridZ,   CathodeGridZ, BottomGridZ};
		LUXSimDetectorComponent * gridMother[5] = {gasXenon, gasXenon,   liquidXenon, liquidXenon,  liquidXenon};
		G4LogicalBorderSurface *xenonHolderSurface;
		for(int g = 0; g < 5; g++){
		  G4cout << G4endl << "Placing " << gridNames[g] << " grid into " << gridMotherMaterial[g] << ".  ";
		  grids[g] = new LUXSimGrid(wireDiameter[g] / 2., "Dodecagon", 
		                            frameInnerRadius, frameOuterRadius, frameHeight[g], 
		                            ptfeSheetsRInner, "steel", gridMotherMaterial[g]);
			if(g < 2){
			  grids[g]->Place1_0WiresAndFrame(wireSpacing[g], 0.5 * wireDiameter[g],
					                              ptfeSheetsRInner * Cos15deg,
					                              gridZ[g] - gasXenonZ, hdpeHolderZ - gasXenonZ, hdpeHolder,
					                              gridMother[g]);
			  if(g==1){
			    grids[g]->Place1_0MeshWires(wireSpacing[g], 0.5 * wireDiameter[g], ptfeSheetsRInner * Cos15deg);
			  }
			}else{
			  grids[g]->Place1_0WiresAndFrame(wireSpacing[g], 0.5 * wireDiameter[g],
					                              ptfeSheetsRInner * Cos15deg, gridZ[g],
					                              hdpeHolderZ, hdpeHolder, gridMother[g]);
			}
		  if(gridMotherMaterial[g] == "GasXe"){
        xenonHolderSurface = new G4LogicalBorderSurface("xenonHolderSurface", grids[g]->GetHolder(), 
                                                         ptfeSheets, luxMaterials->GXeTeflonSurface());
		  }else{
        xenonHolderSurface = new G4LogicalBorderSurface("xenonHolderSurface", grids[g]->GetHolder(),
                                                        ptfeSheets, luxMaterials->LXeTeflonSurface());
		  }
    }
  }
  	G4cout << "\n\n\t\tAll components are placed!\n\n";  //std::abort();
    // Fixing offset values for Getable absolute Z coordinates:
    TopPMTZ -= BottomPMTZ;
    TopGridZ -= BottomPMTZ;
    AnodeGridZ -= BottomPMTZ;
    GateGridZ -= BottomPMTZ;
    CathodeGridZ -= BottomPMTZ;
    BottomGridZ -= BottomPMTZ;
    BottomPMTZ -= BottomPMTZ;
    //LiqGasBoundaryZ  = GateGridZ + (0.5 * (AnodeGridZ - GateGridZ));
    LiqGasBoundaryZ = gasXenonZ - 0.5*gasXenonHeight - gasXenonDodecTopZ + gasXenonDodecBotZ;
	LiqLevelAboveGate = LiqGasBoundaryZ - GateGridZ;
    //G4cout << "Getable dimensions:" << G4endl;
    //G4cout << "S2VolumeHeight = " << S2VolumeHeight << G4endl;
    //G4cout << "LiqGasBoundaryZ = " << LiqGasBoundaryZ << G4endl;
    //G4cout << "TopPMTZ = " << TopPMTZ << G4endl;
    //G4cout << "TopGridZ = " << TopGridZ << G4endl;
    //G4cout << "AnodeGridZ = " << AnodeGridZ << G4endl;
    //G4cout << "GateGridZ = " << GateGridZ << G4endl;
    //G4cout << "CathodeGridZ = " << CathodeGridZ << G4endl;
    //G4cout << "BottomGridZ = " << BottomGridZ << G4endl;
    //G4cout << "BottomPMTZ = " << BottomPMTZ << G4endl << G4endl;
}
//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSim1_0Detector
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSim1_0Detector::~LUXSim1_0Detector()
{}
