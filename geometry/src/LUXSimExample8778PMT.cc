////////////////////////////////////////////////////////////////////////////////
/*	LUXSimExample8778PMT.cc
*
* This is the code file to define the 8778 model PMT by Hamamatsu.
*
********************************************************************************
* Change log
*	16 March 2009 - Initial submission (Kareem)
*	27-Aug-09 - Changed the overlap checking to false, added explicit 
*			overlap check (Kareem)
*	25-Oct-09 - Changed PMT material to VacuumDetector for catching
*			opticalphotons (Dave M.)
*	18-Feb-10 - Changed the PMT material back to vacuum (Kareem)
*	22-Jul-10 - Added a 6-mm dead region around the outer rim of the PMT face.
*				This dead region includes a 1-mm thickness of aluminum, so the
*				dead region in the window itself is actually just 5 mm, so that
*				the radius of the active area is ( PMT radius -
*				aluminum thickness - 5 mm ) (Kareem)
*   13-Sep-10 - Resized active window so serves as better analog for photocathode.
*               Additionally, created active window in vacuum to correspond,
*               so reflected photons are not spuriously taken into account
*               as when using PMT_Active_Window (Matthew and Mike W.)
*   04-Apr-11 -	Changed PMT geometry to have a single window, a smaller 
*				photocathode beneath, and an Al border with appropriate 
*				reflectivity (Dave)
*	04-Apr-11 - Modified the geometry of the aluminum flashing to have the
*				appropriate thickness as well as the radial traces (Kareem)
*   29-Nov-11 - Removed photocathode volume, as it is obsolete (Matthew)
*	09-Feb-12 - Added the photocathode back in, but made it a sub-volume wholly
*				contained inside the PMT window (Kareem)
*	02-Nov-12 - Changed the PMT window thickness to 2.5 mm (was 2.0 mm) (Kareem)
*       25-Nov-12 - Moved photocathode to back of window (Matthew)
*       30-Dec-12 - Made photocathode thickness more realistic (Matthew)
*      2013-05-01 - Updated PMT size with updated information from Carlos (Vic)
*      2013-05-03 - Fixed PMT wall thickness and aluminum flashing with updated information from Carlos (Vic)
*/
////////////////////////////////////////////////////////////////////////////////

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4Color.hh"
#include "G4VisAttributes.hh"

//
//	LUXSim includes
//
#include "LUXSimExample8778PMT.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSimExample8778PMT
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimExample8778PMT::LUXSimExample8778PMT()
{
  // Get the LUXSimMaterials pointer
  LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

  // This PMT is very simply modeled. It is a hollow stainless steel can with
  // a quartz window. The steel is 2.65 mm thick, and the quartz is 2.5 mm.

  // First make the body of the PMT
  outerDiameter = 55.6 * mm;
  height = 12. * cm;
  G4double thickness = 2.65 * mm;
  G4Tubs *PMT8778_solid = new G4Tubs("PMT8778_solid", 0 * mm, 0.5 * outerDiameter, 0.5 * height, 
                                     0. * deg, 360. * deg);
  logicalVolume = new G4LogicalVolume(PMT8778_solid, luxMaterials->Steel(), "PMT8778_log");
  logicalVolume->SetVisAttributes(luxMaterials->SteelVis());

  // Now put a vacuum inside the PMT
  G4double vacuumRadius = (0.5 * outerDiameter) - thickness;
  G4double vacuumHeight = height - thickness;
  G4Tubs *PMTvacuum_solid = new G4Tubs("PMT8778Vacuum_solid", 0 * mm, vacuumRadius, 
                                       0.5 * vacuumHeight, 0. * deg, 360. * deg);
  G4LogicalVolume *PMTvacuum_log = new G4LogicalVolume(PMTvacuum_solid,  luxMaterials->Vacuum(),
                                                       "PMT8778Vacuum_log" );
  PMTvacuum_log->SetVisAttributes(luxMaterials->VacuumVis());
  G4double xOff = 0, yOff = 0, zOff = 0.5 * thickness;
  PMTvacuum = new LUXSimDetectorComponent(0, G4ThreeVector(xOff, yOff, zOff), PMTvacuum_log, 
                                          "PMT8778Vacuum", logicalVolume, 0, 0, false );
	
  // Put a quartz window atop the PMT
  G4double windowThickness = 2.5 * mm;
  G4double windowRadius = vacuumRadius;
  zOff = 0.5 * (vacuumHeight - windowThickness);
  G4Tubs *PMTwindow_solid = new G4Tubs("PMT8778Window_solid", 0. * mm, windowRadius, 
                                       0.5 * windowThickness, 0. * deg, 360. * deg);
  G4LogicalVolume *PMTwindow_log = new G4LogicalVolume(PMTwindow_solid, luxMaterials->Quartz(), 
                                                       "PMT8778Window_log");
  PMTwindow_log->SetVisAttributes(luxMaterials->QuartzVis());
  PMTWindow = new LUXSimDetectorComponent(0, G4ThreeVector(xOff, yOff, zOff), "PMT8778Window", 
                                          PMTwindow_log, PMTvacuum, 0, 0, false);
	
//	Put flashing embedded in the inside surface of the PMT window, so that
//	it sits between the window and the photocathode

//	Now put the aluminum traces on the back surface of the active window.
//	These traces are actually a very thin layer embedded in the quartz to
//	ensure that optical photons are properly reflected. There are several
//	parts to the traces:
//		The outer ring (IR = 23.9 mm, OD = 25.15 mm)
//		4 long traces (19 mm x 1 mm)
//		4 short traces (10 mm x 1 mm)
//	The total mass in the traces is 0.25 g, so the thickness is 100 microns.
  G4double traceRingInnerR = 22.65 * mm,
           traceThickness = 0.1 * mm,
           traceLineWidth = 1. * mm,
           traceLineLengthLong = 19. * mm,
           traceLineLengthShort = 10. * mm;
  // Solid primatives for the traces
  G4Tubs *alTrace_RingSolid = new G4Tubs("alTrace_RingSolid", traceRingInnerR, windowRadius, 
                                       0.5 * traceThickness, 0. * deg, 360. * deg);
  G4Box *alTrace_LineSolid = new G4Box("alTrace_LineSolid", traceRingInnerR + (1. * mm), 
                                                            0.5 * traceLineWidth, 
                                                            0.5 * traceThickness);
  G4Box *alTrace_LongLineCutoutSolid = new G4Box("alTrace_LongLineCutoutSolid", 
                                                 traceRingInnerR - traceLineLengthLong,
                                                 0.5 * traceLineWidth, 
                                                 0.5 * traceThickness);
  G4Box *alTrace_ShortLineCutoutSolid = new G4Box("alTrace_ShortLineCutoutSolid", 
                                                  traceRingInnerR - traceLineLengthShort,
                                                  0.5 * traceLineWidth, 
                                                  0.5 * traceThickness);
  G4SubtractionSolid *alTrace_2LongLineSolid = new G4SubtractionSolid("alTrace_2LongLineSolid",
                                                                      alTrace_LineSolid,
                                                                      alTrace_LongLineCutoutSolid);
  G4SubtractionSolid *alTrace_2ShortLineSolid = new G4SubtractionSolid("alTrace_2ShortLineSolid",
                                                                      alTrace_LineSolid,
                                                                      alTrace_ShortLineCutoutSolid);
  // Some rotation matricies for clocking the line traces into position
  G4RotationMatrix *rot000Z = new G4RotationMatrix; rot000Z->rotateZ(0.*deg);
  G4RotationMatrix *rot045Z = new G4RotationMatrix; rot045Z->rotateZ(45.*deg);
  G4RotationMatrix *rot090Z = new G4RotationMatrix; rot090Z->rotateZ(90.*deg);
  G4RotationMatrix *rot135Z = new G4RotationMatrix; rot135Z->rotateZ(135.*deg);
  // A running sum of the Al traces to get the flashing, first add the ring and the first two long 
  // line traces...
  G4UnionSolid *alTrace_SumSolid1 = new G4UnionSolid("alTrace_SumSolid1", alTrace_RingSolid, 
                                                     alTrace_2LongLineSolid, rot000Z, G4ThreeVector());
  // And the second two long line traces...
  G4UnionSolid *alTrace_SumSolid2 = new G4UnionSolid("alTrace_SumSolid2", alTrace_SumSolid1, 
                                                     alTrace_2LongLineSolid, rot090Z, G4ThreeVector());
  // Add the first two short line traces...
  G4UnionSolid *alTrace_SumSolid3 = new G4UnionSolid("alTrace_SumSolid3", alTrace_SumSolid2,
                                                     alTrace_2ShortLineSolid, rot045Z, G4ThreeVector());
  // Add the second two short line traces and we're done.
  G4UnionSolid *PMTflashing_solid = new G4UnionSolid("PMTflashing_solid", alTrace_SumSolid3, 
                                                     alTrace_2ShortLineSolid, rot135Z, G4ThreeVector());
  // And finish putting this thing in the simulation...
  G4LogicalVolume *PMTflashing_log = new G4LogicalVolume(PMTflashing_solid,
			                                 luxMaterials->Aluminum(), 
                                                         "PMT8778Flashing_log" );
  G4VisAttributes *flashingVis = new G4VisAttributes( G4Color::Black() );
  PMTflashing_log->SetVisAttributes( flashingVis );
  zOff = 0.5 * traceThickness;
  PMTFlashing = new LUXSimDetectorComponent(0, G4ThreeVector(xOff, yOff, zOff), "PMTFlashing", 
                                            PMTflashing_log, PMTWindow, 0, 0, false );
  //Optical boundary between the Al flashing and window
  G4LogicalBorderSurface *flashingWindowSurface;
  flashingWindowSurface = new G4LogicalBorderSurface("flashingWindowSurface", PMTWindow, PMTFlashing, 
		                                     luxMaterials->AlQuartzSurface());
  // Make the "photocathode" nothing more than a volume of quartz wholly
  // inside the PMT window, located behind the aluminum flashing.
  G4double photocathodeRadius = windowRadius;
  G4double photocathodeThickness = 20 * nm;
  zOff = (-0.5 * photocathodeThickness) - traceThickness - (1.14 * mm) - (8. * um) - (0.00198 * mm);
  G4Tubs *PMTphotocathode_solid = new G4Tubs("PMT8778Photocathode_solid", 0, photocathodeRadius, 
                                             0.5 * photocathodeThickness, 0. * deg, 360. * deg);
  G4LogicalVolume *PMTphotocathode_log = new G4LogicalVolume(PMTphotocathode_solid, 
                                                             luxMaterials->Quartz(),
                                                             "PMT8778Photocathode_log");
  G4VisAttributes *photocathodeVis = new G4VisAttributes( G4Color::Magenta());
  PMTphotocathode_log->SetVisAttributes(photocathodeVis);
  PMTPhotocathode = new LUXSimDetectorComponent(0, G4ThreeVector(xOff, yOff, zOff), 
                                                "PMT8778Photocathode", PMTphotocathode_log, 
                                                PMTWindow, 0, 0, false);
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSimExample8778PMT
//------++++++------++++++------++++++------++++++------++++++------++++++------

