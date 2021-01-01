////////////////////////////////////////////////////////////////////////////////
/*	LUXSimDetectorComponent.cc
*
* This is the code file to define the detector component class. The detector
* components inherit from G4VPhysicalVolumes, but they contain additional
* information and methods related to event generation and output recording.
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*	20 April 2009 - Moved common code from the constructors to the 
*		Initialize method (Kareem)
*	22 April 2009 - Added AddSource, CalculateRatios, and GenerateEvent 
*		methods	(Kareem)
*	08-May-2009 - Added method to record the object's coordinates in the 
*		global reference frame (Kareem)
*	28-May-09 - Added dummy return value to GetEventLocation to avoid a 
*		run-time warning about reaching the end of a non-void function 
*		(Kareem)
*	27-Aug-09 - Put the volume overlap check in the constructor and added 
*		two more overloaded constructors in keeping with all possible
*		ways to create a G4PVPlacement (Kareem)
*	14-Sep-09 - Moved initialization of recordLevel and 
*		recordLevelOptPhot to the Initialize method (Kareem)
*	21-Sep-09 - Fixed a bug in GetEventLocation that was giving 
*		disproportionate weight to the center of the encompasing 
*		sphere (Kareem)
*   4-Jan-10 - Added SingleDecay change in GenerateEvent method.(Nick)
*	25-Jan-10 - Changed the calculation of the bounding volume to use
*				G4VSolid::CalculateExtent instead of the overlapping spheres
*				method. The bounding volume has been changed to a box instead
*				of a sphere. (Kareem)
*	26-Jan-10 - Added printout of Z,A information for SingleDecay sources
*				synopsis (Kareem)
*	31-Jan-11 - Fixed a bug where volume sampling did not take into account the
*				object's rotation (Kareem)
*   21-Jul-11 - Added Decay Chain Generating methods (Nick)
*	06-Feb-12 - Reduced the oversize tolerance for any volume's x, y, and z
*				extent to just 1 micron (used to be 100 microns) (Kareem)
*   04-Mar-12 - Added functionality to handle single particle generator (Nick)
*	01-May-12 - Added support for the thermal electron record level (Kareem)
*   18-Jun-12 - Added storage for the Wimp mass for that source (Daniel)
*   15-Jul-12 - Remove GenerateEvent, add GenerateFromEventList and 
*               GenerateEventList and calculate particle times (Nick)
*   22-Aug-12 - Added status messages for adding events into BST (Nick)
*   24-Aug-12 - Terminate generating events to be added to the binary search 
*               tree if they fall after the time window end (Nick)
*	08-Oct-12 - Reduced size of the bounding box in DetermineCenterAndExtent()
*				from 1 micron to 10 nanometers to increase the efficiency for
*				very small components. Also, fixed a bug where the last
*				orientation of a component wasn't getting set properly (Kareem)
*   26-Sep-13 - Added recording the minimum XYZ and maximum XYZ extent of the
*               object (Kareem)
*   03-Sep-14 - Added methods for calculating the volume and mass in a more
*               robust way than is available in standard Geant4 code. Also,
*               commented out the "CheckOverlaps" call in the Initialize()
*               method so the geometry loads more quickly. (Kareem)
*   03-Sep-14 - Small fix to the volume calculation's screen output (Kareem)
*   03-Sep-14 - Added check for volume overlaps, rather than performing overlap
*               check by default (Kareem)
*   06-Oct-14 - Fixed bug where the GetVolume calculation was going negative
*               because over 2,147,483,647 points were being sampled, but the
*               data type was a G4int (Kareem)
*   14-Oct-14 - Added Set methods for the mass and volume (Kareem)
*   28-Aug-15 - Edited AddSource method and EventPosition calculation to 
*               accommodate point sources (David W)
*   18-Dec-2015 - Muon generator Code (David W) (merged into git by Doug T)
*
*/
////////////////////////////////////////////////////////////////////////////////

//
//	CLHEP includes
//
#include "Randomize.hh"

//
//	GEANT4 includes
//
#include "G4GeneralParticleSource.hh"
#include "G4Event.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Sphere.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4TransportationManager.hh"
#include "G4RotationMatrix.hh"
#include "G4VoxelLimits.hh"
#include "G4AffineTransform.hh"
#include "G4Material.hh"

//
//	LUXSim includes
//
#include "LUXSimDetectorComponent.hh"
#include "LUXSimManager.hh"
#include "LUXSimMaterials.hh"
#include "LUXSimSource.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimDetectorComponent()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimDetectorComponent::LUXSimDetectorComponent(
		G4RotationMatrix *pRot,
		const G4ThreeVector &tlate,
		G4LogicalVolume *pCurrentLogical,
		const G4String &pName,
		G4LogicalVolume *pMotherLogical,
		G4bool pMany,
		G4int pCopyNo,
		G4bool pSurfChk )
: G4PVPlacement( pRot, tlate, pCurrentLogical, pName, pMotherLogical, pMany,
pCopyNo, pSurfChk )
{
	Initialize();
}

LUXSimDetectorComponent::LUXSimDetectorComponent(
		const G4Transform3D &Transform3D,
		G4LogicalVolume *pCurrentLogical,
		const G4String &pName,
		G4LogicalVolume *pMotherLogical,
		G4bool pMany,
		G4int pCopyNo,
		G4bool pSurfChk )
: G4PVPlacement( Transform3D, pCurrentLogical, pName, pMotherLogical, pMany,
pCopyNo, pSurfChk )
{
	Initialize();
}

LUXSimDetectorComponent::LUXSimDetectorComponent(
		G4RotationMatrix *pRot,
		const G4ThreeVector &tlate,
		const G4String &pName,
		G4LogicalVolume *pLogical,
		G4VPhysicalVolume *pMother,
		G4bool pMany,
		G4int pCopyNo,
		G4bool pSurfChk )
: G4PVPlacement( pRot, tlate, pName, pLogical, pMother, pMany, pCopyNo,
pSurfChk )
{
	Initialize();
}
		
LUXSimDetectorComponent::LUXSimDetectorComponent(
		const G4Transform3D &Transform3D,
		const G4String &pName,
		G4LogicalVolume *pLogical,
		G4VPhysicalVolume *pMother,
		G4bool pMany,
		G4int pCopyNo,
		G4bool pSurfChk )
: G4PVPlacement( Transform3D, pName, pLogical, pMother, pMany, pCopyNo,
pSurfChk )
{
	Initialize();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					~LUXSimDetectorComponent()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimDetectorComponent::~LUXSimDetectorComponent()
{
  if(LUXSimManager::GetManager()) LUXSimManager::GetManager()->Deregister( this );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					Initialize()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorComponent::Initialize()
{
	luxManager = LUXSimManager::GetManager();
	luxManager->Register( this );
	
	navigator = G4TransportationManager::GetTransportationManager()->
			GetNavigatorForTracking();

	eventRecord.clear();
	sources.clear();
	capturePhotons = false;
	
	globalCenter = G4ThreeVector(0,0,0);
    minXYZ = G4ThreeVector(0,0,0);
    maxXYZ = G4ThreeVector(0,0,0);
    maxX = minX = maxY = minY = maxZ = minZ = 0;

    if( luxManager->GetCheckVolumeOverlaps() )
        CheckOverlaps(1000,0.,false);
	
	recordLevel = 0;
	recordLevelOptPhot = 0;
	recordLevelThermElec = 0;
    
    volume = mass = -1;
    volumePrecision = 100000000;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					AddSource()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorComponent::AddSource( LUXSimSource *type, G4double act,
        G4int mass, G4int number, G4String parentIso, G4double sourceAge,
	G4String parName, G4double parEnergy, G4bool pointSource, G4ThreeVector posSource)
//, G4double wimpMass )
{
	source temp;
	temp.type = type;
	temp.activity = act;
	temp.ratio = 0;
    temp.mass = mass;
    temp.number = number;
    temp.parentIsotope = parentIso;
    temp.sourceAge = sourceAge;
    temp.particleName = parName;
    temp.particleEnergy = parEnergy;
    temp.pointSource = pointSource;
    temp.posSource = posSource;
//	temp.wimpMass = wimpMass;
	sources.push_back( temp );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					CalculateRatios()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorComponent::CalculateRatios()
{
	totalActivity = 0.;
	for( G4int i=0; i<(G4int)sources.size(); i++ ) {
		sources[i].ratio =
				sources[i].activity*sources[i].type->GetActivityMultiplier();
		totalActivity += sources[i].ratio;
	}
	for( G4int i=0; i<(G4int)sources.size(); i++ )
		sources[i].ratio = sources[i].ratio/totalActivity;
	
	if( totalActivity ) {
		G4cout << "For volume " << ((LUXSimDetectorComponent*)(this))->GetName()
			   << ", sources are as follows:" << G4endl;
		for( G4int i=0; i<(G4int)sources.size(); i++ ) {
			G4cout << "\t" << sources[i].type->GetName() << " ";
			if( sources[i].type->GetName() == "SingleDecay" || 
			        sources[i].type->GetName() == "G4Decay" )
				G4cout << "Z,A = " << sources[i].mass << ","<<sources[i].number;
            else if( sources[i].type->GetName() == "SingleParticle" )
                G4cout << sources[i].particleName;
            else if( sources[i].type->GetName() == "DecayChain" )
                G4cout << sources[i].parentIsotope ;
			G4cout << "\t\t" << sources[i].activity << "\t" << sources[i].ratio
				   << G4endl;

            if( sources[i].type->GetName().find("DecayChain")<G4String::npos )
                G4cout << "\t\tSource age = " << sources[i].sourceAge << " s" 
                       << G4endl;
		}
		G4cout << "\tTotal activity for this source (taking into account "
			   << "activity within" << G4endl
			   << "\tdecay chains) = " << totalActivity << " Bq" << G4endl;
	}
	
	for( G4int i=1; i<(G4int)sources.size(); i++ )
		sources[i].ratio += sources[i-1].ratio;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GenerateFromEventList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorComponent::GenerateFromEventList( G4GeneralParticleSource
        *particleGun, G4Event *event, decayNode *firstNode )
{
    //time sent and received in *ns
    sources[firstNode->sourcesID].type->GenerateFromEventList( particleGun, 
            event, firstNode);
}


//------++++++------++++++------++++++------++++++------++++++------++++++------
//				    	GenerateEventList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorComponent::GenerateEventList(G4int sourceByVolumeID)
{
    G4ThreeVector eventPosition;
    // Acitivty units in Bq, time units in seconds
    G4double startParticleTime;
    // Generate numOfEvents for all sources
    G4int numOfEvents = luxManager->GetNumEvents();
    G4double windowEndTime = luxManager->GetWindowEndTime();
    if( windowEndTime > 0 ) windowEndTime*=1.e9*ns;//convert s->ns

    //source activity and activity multiplier
    G4double sourceActivity = 0.;

    for( G4int i=0; i<(G4int)sources.size(); i++ ) {
        sourceActivity = sources[i].activity 
                           * sources[i].type->GetActivityMultiplier();

        // Reset time for each source of each different source
        startParticleTime = ( 0. )*ns ; 
        G4cout << "Adding source " << sources[i].type->GetName() 
		       << " in "<< ((LUXSimDetectorComponent*)(this))->GetName()
               << " to binary search tree (BST)." << G4endl;
        G4cout << "  Progress: " << i+1 << " of " << sources.size() 
               << " in this volume " << G4endl;

        if( sources[i].type->GetName().find("DecayChain") < G4String::npos ) {
            // Time is determined by DecayChain GenerateEventList method
            sources[i].type->CalculatePopulationsInEventList( 
                            sources[i].sourceAge, sources[i].activity, 
                            sources[i].parentIsotope );
            // Each DecayChain needs to generate events with the specific
            // Population just calculated before moving to the next
            G4cout << "Adding DecayChain to BST" << G4endl;
            for( G4int j=0; j<numOfEvents; j++ ) {
	        if( sources[i].pointSource ) eventPosition = sources[i].posSource;
	        else eventPosition = GetEventLocation();
                sources[i].type->GenerateEventList( eventPosition, 
                              sourceByVolumeID, i, sources[i].parentIsotope );
                if( sources[i].type->GetParentDecayTime() > windowEndTime/s )
                    j = numOfEvents;
            }
        }
        else if( sources[i].type->GetName().find("SingleDecay")<G4String::npos ||
		     sources[i].type->GetName().find("G4Decay")<G4String::npos )
        {
            for( G4int j=0; j<numOfEvents; j++ ) {
	        if( sources[i].pointSource ) eventPosition = sources[i].posSource;
                else eventPosition = GetEventLocation();
                startParticleTime += ( -log(G4UniformRand())
                                       / sourceActivity )*s ;
                if( startParticleTime > windowEndTime ) j = numOfEvents;
                sources[i].type->GenerateEventList( eventPosition,
                              sourceByVolumeID, i, sources[i].mass,
                              sources[i].number, startParticleTime/ns );
            }
        }
        else if(sources[i].type->GetName().find("SingleParticle")<G4String::npos)
        {
            for( G4int j=0; j<numOfEvents; j++ ) {
	        if( sources[i].pointSource ) eventPosition = sources[i].posSource;
		else eventPosition = GetEventLocation();
                startParticleTime += ( -log(G4UniformRand())
                                       / sourceActivity )*s ;
                if( startParticleTime > windowEndTime ) j = numOfEvents;
                sources[i].type->GenerateEventList( eventPosition,
                              sourceByVolumeID, i, sources[i].particleName,
                              sources[i].particleEnergy, startParticleTime/ns );
            }
        }
        else if(sources[i].type->GetName().find("Wimp")<G4String::npos)
        {
            for( G4int j=0; j<numOfEvents; j++ ) {
                if( sources[i].pointSource ) eventPosition = sources[i].posSource;  
		else eventPosition = GetEventLocation();
                startParticleTime += ( -log(G4UniformRand())
                                       / sourceActivity )*s ;
                if( startParticleTime > windowEndTime ) j = numOfEvents;
                sources[i].type->GenerateEventList( eventPosition,
                              sourceByVolumeID, i, sources[i].particleEnergy,
                              startParticleTime/ns);
            }
        }
		else if (sources[i].type->GetName().find("MUSUN")<G4String::npos)
		{
			for (G4int j = 0; j<numOfEvents; j++) {
				if (sources[i].pointSource) eventPosition = sources[i].posSource;
				else eventPosition = GetEventLocation();
				startParticleTime += (-log(G4UniformRand())
					/ sourceActivity)*s;
				if (startParticleTime > windowEndTime) j = numOfEvents;
				sources[i].type->GenerateEventList(startParticleTime / ns);
			}
		}
        else {
            for( G4int j=0; j<numOfEvents; j++ ) {
                if( sources[i].pointSource ) eventPosition = sources[i].posSource;
		else eventPosition = GetEventLocation();
                startParticleTime += ( -log(G4UniformRand()) 
                                      / sourceActivity  )*s ;
                if( startParticleTime > windowEndTime ) j = numOfEvents;
                sources[i].type->GenerateEventList( eventPosition,
                                sourceByVolumeID, i, startParticleTime/ns );
            }
        }
        if( !(sources[i].type->GetName().find("DecayChain") < G4String::npos) ) 
            G4cout << "  Time of primary events range from 0.*ns to " 
                   << startParticleTime/ns << "*ns" << G4endl;
    }
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					DetermineCenterAndExtent()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorComponent::DetermineCenterAndExtent( G4PVPlacement *ref )
{
	//	This method records the displacement and orientation of this sub-volume
	//	with respect to the physical volume that is passed to it. If you pass
	//	this method the world volume, it will record this object's global
	//	coordinates.
	
	G4ThreeVector translation(0,0,0);
	
	//	First check to make sure the reference object isn't this object itself
	if( ref == this ) {
		G4cout << "\"" << ((LUXSimDetectorComponent*)(this))->GetName()
			   << "\" passed as reference volume to itself in "
			   << "DetermineCenter()" << G4endl;
		globalCenter = translation;
		return;
	}
	
	//	Create a vector of physical volumes that begins with the reference
	//	volume and ends with the "this" volume.
	std::vector<G4VPhysicalVolume*> thePhysicalStack;
	thePhysicalStack.push_back( ref );

	std::vector<G4int> daughterIndices;
	daughterIndices.push_back(0);

	G4VPhysicalVolume *currentPhysicalVolume = 0;
	G4LogicalVolume *currentLogicalVolume = 0;

	while( thePhysicalStack.back() != this ) {
		currentPhysicalVolume = thePhysicalStack.back();
		currentLogicalVolume = currentPhysicalVolume->GetLogicalVolume();
		
		if( currentLogicalVolume->GetNoDaughters() > daughterIndices.back() ) {
			thePhysicalStack.push_back( currentLogicalVolume->GetDaughter(
					daughterIndices.back() ));
			daughterIndices.back()++;
			daughterIndices.push_back(0);
		} else {
			daughterIndices.pop_back();
			thePhysicalStack.pop_back();
		}
	}
	
	//	Now that we have a vector of physical volumes down to the current
	//	volume, add up the translations and rotations. Because of the
	//	potentially nested rotations, we have to start at the last volume
	//	and work our way back up to the world volume.
	globalOrientation.set( 0, 0, 0 );
	globalOrientation.transform( GetObjectRotationValue() );
	for( G4int i=thePhysicalStack.size()-1; i>0; i-- ) {
		translation += thePhysicalStack[i]->GetObjectTranslation();
		translation.transform( thePhysicalStack[i-1]->GetObjectRotationValue());
		globalOrientation.transform(
				thePhysicalStack[i-1]->GetObjectRotationValue() );
	}
	globalCenter = translation;
	
	//	Next, determine the X, Y, and Z extent of the shape. Augment the values
	//	by a small buffer to ensure complete coverage of the volume.
	GetLogicalVolume()->GetSolid()->CalculateExtent( kXAxis,
			G4VoxelLimits(), G4AffineTransform(), minX, maxX );
	GetLogicalVolume()->GetSolid()->CalculateExtent( kYAxis,
			G4VoxelLimits(), G4AffineTransform(), minY, maxY );
	GetLogicalVolume()->GetSolid()->CalculateExtent( kZAxis,
			G4VoxelLimits(), G4AffineTransform(), minZ, maxZ );
    
    //  Record the values of the extent variables before they are augmented
    minXYZ = G4ThreeVector(minX,minY,minZ);
    maxXYZ = G4ThreeVector(maxX,maxY,maxZ);
	
	minX -= 10.*nm;
	maxX += 10.*nm;
	minY -= 10.*nm;
	maxY += 10.*nm;
	minZ -= 10.*nm;
	maxZ += 10.*nm;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GetEventLocation()
//------++++++------++++++------++++++------++++++------++++++------++++++------
G4ThreeVector LUXSimDetectorComponent::GetEventLocation()
{
	//	First determine location of the source. Pick points at random in a
	//	sphere of the appropriate radius, centered on the center of this
	//	geometry object;
	G4bool insideVolume = false;
	G4int counter = 0;
	while( !insideVolume ) {
		counter++;
		if( !(counter%100000) )
			G4cout << "Warning: It has taken at least " << counter
				   << " attempts to find a point inside the "
				   << this->GetName() << " volume" << G4endl;
	
		//	Find a point at random in the box bounded by the dimensions
		//	calculated in DetermineCenterAndExtent.
		xPos = (maxX - minX)*G4UniformRand() + minX;
		yPos = (maxY - minY)*G4UniformRand() + minY;
		zPos = (maxZ - minZ)*G4UniformRand() + minZ;

		G4ThreeVector position( xPos, yPos, zPos );
		position.transform( globalOrientation );
		position += globalCenter;
		
		if( navigator->LocateGlobalPointAndSetup( position ) == this ) {
			insideVolume = true;
			return position;
		}
	}
	
	G4cout << "WARNING! Could not find a location inside volume "
		   << this->GetName();
	return( G4ThreeVector(0,0,0) );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetVolumePrecision()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorComponent::SetVolumePrecision( G4double prec )
{
	if( prec != volumePrecision ) {
        volumePrecision = prec;
        CalculateVolume();
        CalculateMass();
    }
    
    return;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					CalculateVolume()
//------++++++------++++++------++++++------++++++------++++++------++++++------
G4double LUXSimDetectorComponent::CalculateVolume(
        G4bool takeOutDaughters )
{
    if( !takeOutDaughters && volume >= 0 )
        return volume;
    
    if( takeOutDaughters )
        G4cout << "Calculating the volume of " << GetName() << "... "
               << std::flush;

    //	Next, determine the X, Y, and Z extent of the shape. Augment the
    //	values by a small buffer to ensure complete coverage of the volume.
    this->GetLogicalVolume()->GetSolid()->CalculateExtent( kXAxis,
            G4VoxelLimits(), G4AffineTransform(), minX, maxX );
    this->GetLogicalVolume()->GetSolid()->CalculateExtent( kYAxis,
            G4VoxelLimits(), G4AffineTransform(), minY, maxY );
    this->GetLogicalVolume()->GetSolid()->CalculateExtent( kZAxis,
            G4VoxelLimits(), G4AffineTransform(), minZ, maxZ );
    
    //  This sets the required number of "inside" hits as a function of the
    //  required precision. The precision is only approximately accurate, but
    //  it should be in the ballpark.
    G4int targetInsideSamples = volumePrecision;
    
    G4double totalSamples = 0;
    G4int totalSamplesInt = 0;
    G4double insideSamples = 0;
    
    G4ThreeVector position;
    EInside in;
    
    while( insideSamples < targetInsideSamples ) {
        totalSamples++;
        totalSamplesInt++;
        if( totalSamplesInt == 2000000000 )
            totalSamplesInt = 0;
        
		xPos = (maxX - minX)*G4UniformRand() + minX;
		yPos = (maxY - minY)*G4UniformRand() + minY;
		zPos = (maxZ - minZ)*G4UniformRand() + minZ;
        
		position = G4ThreeVector( xPos, yPos, zPos );
        in = GetLogicalVolume()->GetSolid()->Inside(position);
		
        if( in != kOutside )
            insideSamples++;
        
        if( !(totalSamplesInt%1000000000) ) {
            G4cout << "Sampled " << totalSamples << " points in "
                   << GetName() << "," << G4endl;
            G4cout << "\t" << insideSamples
                   << " successful hits, need "
                   << targetInsideSamples << G4endl;
        }
    }
    
    G4double outerTestVolume = (maxX-minX) * (maxY-minY) * (maxZ-minZ);
    
    volume = outerTestVolume * insideSamples / totalSamples;
    
    if( takeOutDaughters ) {
        for( G4int i=0; i<GetLogicalVolume()->GetNoDaughters(); i++ ) {
            LUXSimDetectorComponent *currentComponent =
                    (LUXSimDetectorComponent*)(
                    GetLogicalVolume()->
                    GetDaughter(i) );
            G4double subVolume =
                    currentComponent->CalculateVolume(false);

            volume -= subVolume;
        }
    }
    
    if( takeOutDaughters )
        G4cout << volume/cm3 << " cm3" << G4endl;
    
    return volume;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					CalculateMass()
//------++++++------++++++------++++++------++++++------++++++------++++++------
G4double LUXSimDetectorComponent::CalculateMass()
{
    if( mass >= 0 )
        return mass;
    
    if( volume < 0 )
        CalculateVolume();
    
    mass = volume/cm3 *
            this->GetLogicalVolume()->GetMaterial()->GetDensity() / (g/cm3);
    
    G4cout << GetName() << " has mass " << mass << " g" << G4endl;
    
    return mass;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GetVolume()
//------++++++------++++++------++++++------++++++------++++++------++++++------
G4double LUXSimDetectorComponent::GetVolume()
{
    if( volume < 0 )
        CalculateVolume();
    
    return volume;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetVolume()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorComponent::SetVolume( G4double v )
{
    volume = v/cm3;
    CalculateMass();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GetMass()
//------++++++------++++++------++++++------++++++------++++++------++++++------
G4double LUXSimDetectorComponent::GetMass()
{
    if( mass < 0 )
        CalculateMass();
    
    return mass;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetMass()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorComponent::SetMass( G4double m )
{
    mass = m/g;
    volume = mass*g /
             (this->GetLogicalVolume()->GetMaterial()->GetDensity() / (g/cm3));
}
