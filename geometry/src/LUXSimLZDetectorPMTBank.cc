/*	LUXSimLZDetectorPMTBank.hh
*	
*	This is the code to define the LZDetector bank of R11410-model PMTs.
*	Taking dimensions as best-guesses from 
*		http://teacher.pas.rochester.edu:8080/wiki/pub/Lz/LZMtg20120518/
*		LZ7_proposed_radial_dimensions.pdf
*
*	The bank is made up of two plates of titanium that hold onto the front of
*	the face of the PMTs and the back of the body of the PMTs. There needs to be
*	a PTFE reflective liner in front of the titanium plates, but because of the
*	peculiarities of how the geometry is constructed, that liner plate cannot be
*	placed as part of this class. Rather, the reflective liner will have its
*	logical volume constructed in this class, but it must be placed separately.
*	
*	The titanium plates have holes that are about 1 mm larger in radius
*	than the faces of the PMTs. The PTFE liner, on the other hand, covers
*	down to the flashing on the PMTs. This minimizes the optically dead
*	regions, and also provides a reliable optical barrier between the xenon
*	skin and the main xenon target volumes.
*	
*	Both the titanium plates and the PTFE liner are polygons, where the
*	number of sides in the polygon and the outer flat radius (as opposed to
*	outer corner radius) are passed as parameters to the class. 
*
********************************************************************************
*
*	Change log
*	
*	2012-05-28 - Initial submission (Dave and Kareem)
*       2013-09-06 - This is completely overhauled.  There is only 1 Ti plate and 241 PMTS 
*                    each with 3 rods and a base added at the bottom of the PMT.  The truss
*                    is also implemented.  Again see 
*                    http://teacher.pas.rochester.edu:8080/wiki/pub/Lz/LZSolidModels/LZ_August_2013_CAD_Model_Tour_20130817.pdf (Monica)
*   2015-06-12 - Changed the outer radius of logical volume to outerFlatRadius
*	
********************************************************************************
*/

//
//	C/C++ includes
//
#include <sstream>
#include <string>

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4Polyhedra.hh"
#include "G4Tubs.hh"
#include "G4Material.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4Box.hh"

//
//	LUXSim includes
//
#include "LUXSimLZDetectorPMTBank.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"


using namespace std;

#define PI 3.14159265358979312


//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSimLZDetectorPMTBank
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimLZDetectorPMTBank::LUXSimLZDetectorPMTBank( G4Material *medium,
		const char *prefix, G4double outerFlatRadius )  {

	//	Get the LUXSimMaterials pointer
	luxMaterials = LUXSimMaterials::GetMaterials();
	numPMTs = 241;
	
	//
	//	Set dimensions
	//
	
	ptfeLinerThickness = 2.*mm;
	G4double titaniumPlateThickness = 0.635*cm;
	G4double pmtPlateClearance = 2.*mm;

	LUXSimR11410PMT *tempPMT = new LUXSimR11410PMT( false );
	pmtHeight = tempPMT->GetPMTHeight();
	G4double titaniumHoleRadius = tempPMT->GetPMTRadius() + pmtPlateClearance;
	G4double ptfeHoleRadius = tempPMT->GetPhotocathodeRadius();
 	delete tempPMT;

	//Titanium hole spacing is 3.45" based on drawings
	G4double holeSpacing = 3.45*2.54*cm; //
	
	string testcase = prefix;
	if( testcase != "TEMP" ) {
	
		G4cout << "Building the " << prefix << " PMT bank... " << flush;

		//
		//	Entire volume
		//
		fullVolumeHeight = pmtHeight + 6*cm; 

		G4Tubs *fullVolume_solid = new G4Tubs(
			"fullVolumePMTBank_solid",
			0, outerFlatRadius,  //put in by hand but can use outerFlatRadius + 2*cm but check for overlaps!
			fullVolumeHeight/2,
			0.*deg, 360.*deg );
		
		stringstream name;
		name << prefix << "PMTArrayVolume_log";
		logicalVolume = new G4LogicalVolume( fullVolume_solid, medium,
				name.str() );
		
		if( medium == luxMaterials->GasXe() )
			logicalVolume->SetVisAttributes( luxMaterials->GasXeVis() );
		else if( medium == luxMaterials->LiquidXe() )
			logicalVolume->SetVisAttributes( luxMaterials->LiquidXeVis() );
		else {
			G4cout << G4endl << G4endl << G4endl;
			G4cout << "You're trying to create the encapsulating volume for "
				   << "the banks of PMTs out of" << G4endl;
			G4cout << "something other that gaseous or liquid xenon. "
				   << "Something's not quite right, and" << G4endl;
			G4cout << "you should fix it." << G4endl;
			exit(0);
		}
		
		//
		//	Set up the Y coordinates of the PMTs and holes
		//
		   const G4int num_rows = 19;
		G4int num_holes_to_add[num_rows] = 
				{ 4,  9,  10, 13, 14, 15, 16, 15, 16, 17,
				  16, 15, 16, 15, 14, 13, 10, 9, 4 };
		//
		
		G4double y_coord_row[num_rows];
		for( G4int i=0; i<num_rows; i++ )
		  y_coord_row[i] = holeSpacing*((num_rows-1.)/2. - i) * sin(PI/3.);
		
		
		//
		//	Titanium plates and PTFE liners
		//

		//	Create solid plates for holders and liner, as well as the holes that
		//	get punched into them. Note that the holes are twice the thickness
		//	of the plates and liner so that we guarantee sufficient overlap.

		G4double innerFlatRadius = outerFlatRadius - 2*cm;
		G4Tubs *titaniumPlate_solid1 = new G4Tubs(
			"titaniumPlate_solid1",
			0, innerFlatRadius,
			titaniumPlateThickness/2,
			0.*deg, 360.*deg );

		G4Tubs *titaniumPlate_solid2 = new G4Tubs( "titaniumPlate_solid2",
				0.*cm, titaniumHoleRadius, titaniumPlateThickness,
				0.*deg, 360.*deg );
		
		G4Tubs *ptfeLiner_solid1 = new G4Tubs(
			"titaniumPlate_solid1",
			0, innerFlatRadius,
			ptfeLinerThickness/2,
			0.*deg, 360.*deg );

		G4Tubs *ptfeLiner_solid2 = new G4Tubs( "ptfeLiner_solid2",
				0.*cm, ptfeHoleRadius, ptfeLinerThickness,
				0.*deg, 360.*deg );
		
		G4Tubs *xeSkinptfeLiner_solid2 = new G4Tubs( "xeSkinptfeLiner_solid2",
						       0.*cm, titaniumHoleRadius, ptfeLinerThickness,
						       0.*deg, 360.*deg );
		
		//	Subtract the holes from plate and liner
		G4VSolid *titaniumPlate_solid3 = titaniumPlate_solid1;
		G4VSolid *ptfeLiner_solid3 = ptfeLiner_solid1;
		G4VSolid *xeSkinptfeLiner_solid3 = ptfeLiner_solid1;
		for( G4int i=0; i<num_rows; i++ ) {
			G4double disp_y = y_coord_row[i];
			G4double disp_x = ((num_holes_to_add[i]-1)/2.0)*holeSpacing;

			for( G4int j=0; j<num_holes_to_add[i]; j++ ) {
				titaniumPlate_solid3 = new G4SubtractionSolid(
						"titaniumPlate_solid3", 
						titaniumPlate_solid3, titaniumPlate_solid2, 
						0, G4ThreeVector(disp_x, disp_y, 0) );
				ptfeLiner_solid3 = new G4SubtractionSolid(
						"ptfeLiner_solid3", 
						ptfeLiner_solid3, ptfeLiner_solid2, 
						0, G4ThreeVector(disp_x, disp_y, 0) );

				xeSkinptfeLiner_solid3 = new G4SubtractionSolid(
									  "xeSkinptfeLiner_solid3",
									  xeSkinptfeLiner_solid3, xeSkinptfeLiner_solid2,
									  0, G4ThreeVector(disp_x, disp_y, 0) );
				disp_x -= holeSpacing;
			}
		}
		
		//	Finish creating and placing plates and liners
		G4LogicalVolume *topTitaniumPlate_log = new G4LogicalVolume(
				titaniumPlate_solid3, luxMaterials->Titanium(),
				"topTitaniumPlate_log" );
		topTitaniumPlate_log->SetVisAttributes( luxMaterials->TitaniumVis() );
		teflonLinerLogical = new G4LogicalVolume(
				ptfeLiner_solid3, luxMaterials->Teflon(), "ptfeLiner_log" );
		teflonLinerLogical->SetVisAttributes( luxMaterials->TeflonVis() );
		
		G4LogicalVolume *xeSkinptfeLiner_log = new G4LogicalVolume(
									    xeSkinptfeLiner_solid3, luxMaterials->Teflon(),
									    "xeSkinptfeLiner_log" );

		
		name.str("");
		name << prefix << "FrontTitaniumPlate";
		G4double zOffset = fullVolumeHeight/2 - titaniumPlateThickness/2;
		TopPlate = new LUXSimDetectorComponent( 
				0, G4ThreeVector(0,0,zOffset), 
				topTitaniumPlate_log, name.str(), logicalVolume, 
				0, 0, false );
		
		//Add LXe Skin liner
		name.str("");
                name << prefix << "XeSkinPTFELiner";
		zOffset = fullVolumeHeight/2 - titaniumPlateThickness - ptfeLinerThickness/2;

		XeSkinLiner = new LUXSimDetectorComponent(
							   0, G4ThreeVector(0,0,zOffset),
							   xeSkinptfeLiner_log, name.str(), logicalVolume,
							   0, 0, false );
		
		
		//Making the truss
		// The truss
		
		G4double truss_width_1 = .75*2.54*cm;
		G4double truss_width_2 = .25*2.54*cm;
		G4double truss_offset = 6.*2.54*cm;
		G4double trussBottomHeightPeriphery = truss_offset;
		G4double truss_ring_diameter = 4.33*2.54*cm;
		G4double truss_long_bar_length = 76.9*cm - 2.33*2.54*cm; 
		G4Box *trussVessel_solid1 = new G4Box( "trussVessel_solid1",
						       truss_width_1/2, truss_width_2/2,
						       trussBottomHeightPeriphery/2 + truss_width_1/2 );

		G4Box *trussVessel_solid2 = new G4Box( "trussVessel_solid2",
						       truss_long_bar_length/2, truss_width_2/2,
						       truss_width_1/2 );
		
			       
		G4UnionSolid *trussVessel_solid3 = new G4UnionSolid(
							     "trussVessel_solid3", trussVessel_solid2, trussVessel_solid1,
							     0, G4ThreeVector(-truss_long_bar_length/2,0,trussBottomHeightPeriphery/2));
		
		
		G4Tubs *trussVessel_solid4 = new G4Tubs( "trussVessel_solid4",
						  truss_ring_diameter/2,(truss_ring_diameter + truss_width_1)/2, truss_width_1/2,
						  0.*deg, 360.*deg);

		G4double bar_offset = (truss_ring_diameter + truss_long_bar_length)/2;
		G4double xOffset = -bar_offset;
		G4double yOffset = 0;
                zOffset = fullVolumeHeight/2 -pmtHeight - truss_offset - truss_width_1;

		G4UnionSolid *trussVessel_solid5 = new G4UnionSolid(
                                                                    "trussVessel_solid5", trussVessel_solid4, trussVessel_solid3,
                                                                    0, G4ThreeVector(xOffset,yOffset,0));

		
		
		G4RotationMatrix *rotZtruss = new G4RotationMatrix();
		rotZtruss->rotateZ( 30*deg );
		xOffset = -cos(30. * deg)*bar_offset;						
		yOffset = sin(30. * deg)*bar_offset;
		

		G4UnionSolid *trussVessel_solid6 = new G4UnionSolid(
								     "trussVessel_solid6", trussVessel_solid5, trussVessel_solid3,
								     rotZtruss, G4ThreeVector(xOffset,yOffset,0));

		rotZtruss->rotateZ( 30*deg );
		xOffset =-cos(60. * deg)*bar_offset;
		yOffset =sin(60.* deg)*bar_offset;

		G4UnionSolid *trussVessel_solid7 = new G4UnionSolid(
								    "trussVessel_solid7", trussVessel_solid6, trussVessel_solid3,
								    rotZtruss, G4ThreeVector(xOffset,yOffset,0));

		rotZtruss->rotateZ( 30*deg );
		xOffset =0;
		yOffset =bar_offset;

                G4UnionSolid *trussVessel_solid8 = new G4UnionSolid(
                                                                    "trussVessel_solid8", trussVessel_solid7, trussVessel_solid3,
                                                                    rotZtruss, G4ThreeVector( xOffset,yOffset,0));

		rotZtruss->rotateZ( 30*deg );
		xOffset =cos(60. * deg)*bar_offset;
		yOffset =sin(60. * deg)*bar_offset;
                G4UnionSolid *trussVessel_solid9 = new G4UnionSolid(
                                                                    "trussVessel_solid9", trussVessel_solid8, trussVessel_solid3,
                                                                    rotZtruss, G4ThreeVector(xOffset,yOffset,0));
		rotZtruss->rotateZ( 30*deg );
		xOffset =cos(30. * deg)*bar_offset;
		yOffset =sin(30. * deg)*bar_offset;
                G4UnionSolid *trussVessel_solid10 = new G4UnionSolid(
                                                                    "trussVessel_solid10", trussVessel_solid9, trussVessel_solid3,
                                                                    rotZtruss, G4ThreeVector(xOffset,yOffset,0));


		rotZtruss->rotateZ( 30*deg );
		xOffset =bar_offset;
		yOffset =0;
                G4UnionSolid *trussVessel_solid11 = new G4UnionSolid(
                                                                    "trussVessel_solid11", trussVessel_solid10, trussVessel_solid3,
                                                                    rotZtruss, G4ThreeVector(xOffset,yOffset,0));

		rotZtruss->rotateZ( 30*deg );
		xOffset =cos(30. * deg)*bar_offset;
		yOffset =-sin(30. * deg)*bar_offset;
                G4UnionSolid *trussVessel_solid12 = new G4UnionSolid(
                                                                    "trussVessel_solid12", trussVessel_solid11, trussVessel_solid3,
                                                                    rotZtruss, G4ThreeVector(xOffset,yOffset,0));


		rotZtruss->rotateZ( 30*deg );
		xOffset =cos(60. * deg)*bar_offset;
		yOffset =-sin(60. * deg)*bar_offset;
                G4UnionSolid *trussVessel_solid13 = new G4UnionSolid(
                                                                    "trussVessel_solid13", trussVessel_solid12, trussVessel_solid3,
                                                                    rotZtruss, G4ThreeVector(xOffset,yOffset,0));
		rotZtruss->rotateZ( 30*deg );
		xOffset =0;
		yOffset =-bar_offset;
                G4UnionSolid *trussVessel_solid14 = new G4UnionSolid(
                                                                    "trussVessel_solid14", trussVessel_solid13, trussVessel_solid3,
                                                                    rotZtruss, G4ThreeVector(xOffset,yOffset,0));

		rotZtruss->rotateZ( 30*deg );
		xOffset =-cos(60. * deg)*bar_offset;
		yOffset =-sin(60. * deg)*bar_offset;
                G4UnionSolid *trussVessel_solid15 = new G4UnionSolid(
                                                                    "trussVessel_solid15", trussVessel_solid14, trussVessel_solid3,
                                                                    rotZtruss, G4ThreeVector(xOffset,yOffset,0));

		rotZtruss->rotateZ( 30*deg );
		xOffset =-cos(30. * deg)*bar_offset;
		yOffset =-sin(30. * deg)*bar_offset;
                G4UnionSolid *trussVessel_solid16 = new G4UnionSolid(
                                                                    "trussVessel_solid16", trussVessel_solid15, trussVessel_solid3,
                                                                    rotZtruss, G4ThreeVector(xOffset,yOffset,0));


		G4LogicalVolume *truss_log = new G4LogicalVolume( 
								 trussVessel_solid16, luxMaterials->Titanium(), "truss_log" ); 

                truss_log->SetVisAttributes( luxMaterials->TitaniumVis() );   
		name.str("");
                name << prefix << "Truss";
                truss_offset = 6.*2.54*cm;
                zOffset = fullVolumeHeight/2 - truss_offset - truss_width_1;
		xOffset = -bar_offset;
		xOffset = 0;

		Truss = new LUXSimDetectorComponent(
									      0, G4ThreeVector(xOffset,0,zOffset), truss_log, name.str(),
									      logicalVolume, 0, 0, false );
		


		//
		//	Create and place the PMTs
		//
		
		
		G4int pmt_ctr = 0;
		for( G4int i=0; i<num_rows; i++ ) {
		  
			G4double disp_x = ((num_holes_to_add[i]-1)/2.0)*holeSpacing;
			G4double disp_y = y_coord_row[i];
			
			
			for( G4int j=0; j<num_holes_to_add[i]; j++ ) {

			  PMT[pmt_ctr] = new LUXSimR11410PMT( true );
				
				stringstream number;
				
				number.width(3);
				number.fill('0');
				number << pmt_ctr+1;
				
				zOffset = fullVolumeHeight/2 - pmtHeight/2;

				//	Place and name outer volume
				name.str("");
				name << prefix << "_PMT_Body_" << number.str();
				PMTs[pmt_ctr] = new LUXSimDetectorComponent( 0, 
						G4ThreeVector(disp_x,disp_y,zOffset), 
						PMT[pmt_ctr]->GetLogicalVolume(), name.str(),
						logicalVolume, 0, 0, false );
				
				
				//Base
				G4double baseThickness = 2*cm;
				G4double bodyOuterDiameter = 5.33*cm;
				G4Tubs *base_solid = new G4Tubs( "base_solid1",
								 0*cm, bodyOuterDiameter/2., baseThickness/2.,
								 0.*deg, 360.*deg );

			        G4LogicalVolume *base_log = new G4LogicalVolume(base_solid, luxMaterials->Teflon(), "base_log"); 
				
				name.str("");
                                name << prefix << "_PMT_Base_" << number.str();

				zOffset = fullVolumeHeight/2 - pmtHeight - baseThickness/2;
				PMTsBase[pmt_ctr] = new LUXSimDetectorComponent( 0,
									     G4ThreeVector(disp_x,disp_y,zOffset), base_log, name.str(), 
								     logicalVolume, 0, 0, false );
				

                                //PTFE rods
				G4double rodDiameter = 0.95*cm;
                                G4double rodHeight = 3.*cm;
                                G4Tubs *rod_solid = new G4Tubs( "rod_solid",
                                                                 0*cm, rodDiameter/2., rodHeight/2.,
                                                                 0.*deg, 360.*deg );

                                G4LogicalVolume *rod_log = new G4LogicalVolume(rod_solid, luxMaterials->Teflon(), "rod_log");
				G4double zoffset = fullVolumeHeight/2 - rodHeight/2 - titaniumPlateThickness - ptfeLinerThickness; 
			        G4double xoffset = 0;
				G4double rodSpacing = titaniumHoleRadius + 3*mm;
				G4double yoffset = rodSpacing;
                                name.str("");
                                name << prefix << "_PMT_Rod1_" << number.str();
				
                                PMTsRod1[pmt_ctr] = new LUXSimDetectorComponent( 0,
                                                                             G4ThreeVector(disp_x + xoffset,disp_y + yoffset,zoffset), rod_log, name.str(),
									     logicalVolume, 0, 0, false );
				
				xoffset = -rodSpacing*0.5;
				yoffset = -rodSpacing*sqrt(3)/2;

				name.str("");
                                name << prefix << "_PMT_Rod2_" << number.str();

                                PMTsRod2[pmt_ctr] = new LUXSimDetectorComponent( 0,
                                                                             G4ThreeVector(disp_x + xoffset,disp_y + yoffset,zoffset), rod_log, name.str(),
                                                                             logicalVolume, 0, 0, false );

				
                                xoffset= rodSpacing*sin(PI/6.);
				yoffset= -rodSpacing*cos(PI/6.);

				name.str("");
                                name << prefix << "_PMT_Rod3_" << number.str();

                                PMTsRod3[pmt_ctr] = new LUXSimDetectorComponent( 0,
                                                                             G4ThreeVector(disp_x + xoffset,disp_y + yoffset,zoffset), rod_log, name.str(),
                                                                             logicalVolume, 0, 0, false );
				
				//	Rename vacuum
				name.str("");
				name << prefix << "_PMT_Vacuum_" << number.str();
				PMT[pmt_ctr]->GetPMTVacuum()->SetName( name.str() );
				
				//	Rename window
				name.str("");
				name << prefix << "_PMT_Window_" << number.str();
				PMT[pmt_ctr]->GetPMTWindow()->SetName( name.str() );
				
				//	Rename photocathode
				name.str("");
				name << prefix << "_PMT_Photocathode_" << number.str();
				PMT[pmt_ctr]->GetPMTPhotocathode()->SetName( name.str() );
				
				//	Rename flashing
				name.str("");
				name << prefix << "_PMT_Flashing_" << number.str();
				PMT[pmt_ctr]->GetPMTFlashing()->SetName( name.str() );

				// Rename Base
				
				pmt_ctr++;

				disp_x -= holeSpacing;
				
			}
		}
		 
		G4cout << "Done" << G4endl;
	}
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSimLZDetectorPMTBank
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimLZDetectorPMTBank::~LUXSimLZDetectorPMTBank() {}
