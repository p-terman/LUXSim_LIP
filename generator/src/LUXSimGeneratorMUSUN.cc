////////////////////////////////////////////////////////////////////////////////
/*   LUXSimGeneratorMUSUN.cc
*
* This is the code file for the MUSUN generator.
*
********************************************************************************
* Change log
*   22-Oct-2015 - Initial submission (David)
*
*
////////////////////////////////////////////////////////////////////////////////

//
//   General notes on this generator
//
//

This C++ code is a port of the musun-surf.f and test-musun-surf.f code written by Vitaly Kudryavtsev of the University of Sheffield. It generates muonswith energy, position, and direction specific to the Davis cavern at the Sanford Underground Research Facility in Lead, South Dakota.
This C++ code was ported by Kareem Kazkaz, kareem@llnl.gov, (925) 422-7208 and edited by David Woodward, dwoodward1@sheffield.ac.uk.

Here are the notes from Vitaly:

c       The code samples single atmospheric muons at the SURF                                                                                 
c       underground laboratory (Davis' cavern)                                                                                               
c       (taking into account the slant depth distribution)                                                                                   
c       in the energy range E1-E2, zenith angle range theta1-theta2 (0-90 degrees)                                                           
c       and azimuthal angle range phi1-phi2 (0-360 degrees).                                                                                 
c       At present only the following ranges of parameters are supported:                                                                   
c       E1 = 1 GeV, E2 = 10^6 GeV, theta1 = 0, theta2 = 90 deg, phi1 = 0, phi2 = 360 deg.     
c                                                                                                                                              
c       Program uses muon energy spectra at various depths and zenith                                                                          
c       angles obtained with MUSIC code for muon propagation and Gaisser's                                                                     
c       formula for muon spectrum at sea level                                                                                                 
c       (T.K.Gaisser, Cosmic Rays and Particle Physics, Cambridge                                                                              
c       University Press, 1990) modified for large zenith angles and                                                                           
c       prompt muon flux with normalisation and spectral index                                                                                 
c       that fit LVD data: gamma = 2.77, A = 0.14.                                                                                             
c       Density of rock is assumed to be 2.70 g/cm^3 but can be changed                                                                        
c       during initialisation (previous step, ask the author).                                                                                 
c                                                                                                                                              
c       Muon flux through a sphere (Chao's coordinates) = 6.33x10^(-9) cm^(-2) s^(-1) (gamma=2.77) - old                                       
c       Muon flux through a sphere (Martin's coordinates) = 6.16x10^(-9) cm^(-2) s^(-1) (gamma=2.77) - new                                     
c       Muon flux through the cuboid (30x22x24 m^3) = 0.0588 s^(-1) (gamma=2.77)                                                               
c                                                                                                                                              
c       Note: the muon spectrum at sea level does not take into account                                                                        
c       the change of the primary spectrum slope at and above the knee                                                                         
c       region (3*10^15-10^16 eV).                                                                                                             
c                                                                                                                                              
c       Program uses the tables of muon energy spectra at various                                                                              
c       zenith and azimuthal angles at SURF                                                                                                    
c       calculated with the muon propagation code MUSIC and the                                                                                
c       angular distribution of muon intensities at SURF (4850 ft level).                                                                     
c                                                                                                                                             
c       Coordinate system for the muon intensities                                                                                           
c       is determined by the mountain profile provided                                                                                       
c       by Chao Zhang (USD, South Dakota): x-axis is pointing to the East.                                                                   
c       Muons are sampled on a surface of a rectangular parallelepiped,                                                                      
c       the planes of which are parallel to the walls of the cavern.                                                                         
c       The long side of the cavern is pointing at 6.6 deg from the North                                                                    
c       to the East (or 90 - 6.6 deg from the East to the North).                                                                            
c       Muon coordinates and direction cosines are then given in the                                                                         
c       coordinate system related to the cavern with x-axis                                                                                  
c       pointing along the long side of the cavern at 6.6 deg from the                                                                       
c       North to the East.                                                                                                                   
c       The angle phi is measured from the positive x-axis towards                                                                           
c       positive y-axis (0-360 deg).                                                                                                         
c       Z-axis is pointing upwards.   
*/

//
//   GEANT4 includes
//
#include "globals.hh"
#include "G4Neutron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4Gamma.hh"
#include "G4GenericIon.hh"
#include "G4UnitsTable.hh"

//
//   LUXSim includes
//
#include "LUXSimGeneratorMUSUN.hh"

//
//   C++ includes & definitions
//
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cmath>
#define PI 3.14159265358979312
#define DEBUGGING 0

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//               LUXSimGeneratorMUSUN()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimGeneratorMUSUN::LUXSimGeneratorMUSUN()
{
   name = "MUSUN";
   activityMultiplier = 1;
   
   neutronDef = G4Neutron::Definition();
   muonplusDef = G4MuonPlus::Definition();
   muonminusDef = G4MuonMinus::Definition();
   gammaDef = G4Gamma::Definition();
   ionDef = G4GenericIon::Definition();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//               ~LUXSimGeneratorMUSUN()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimGeneratorMUSUN::~LUXSimGeneratorMUSUN() {}


//------++++++------++++++------++++++------++++++------++++++------++++++------
//                    GenerateEventList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimGeneratorMUSUN::GenerateEventList(G4double time) 
{
  G4cout << "GeneratingEventList..." << G4endl;

  G4ThreeVector position( 0,0,0 );
  G4int a=-1; G4int z=-1;
  G4double hl=-1;
  Isotope *currentIso = new Isotope(name, z, a, hl);
  G4int sourceByVolumeID = 0;
  G4int sourcesID = 0;
  luxManager->RecordTreeInsert( currentIso, time, position, 
				sourceByVolumeID, sourcesID );
}


//------++++++------++++++------++++++------++++++------++++++------++++++------
//                    GenerateFromEventList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimGeneratorMUSUN::GenerateFromEventList( G4GeneralParticleSource
       *particleGun, G4Event *event, decayNode *firstNode  )
{
  CalculateMuonVariables();
  
  //--- Start time = 0
  G4double time = 0/ns; 

  //--- Get muon position
  //    Convert cm --> m 
  G4double x0 = GetMuonX0()*10; 
  G4double y0 = GetMuonY0()*10;
  G4double z0 = GetMuonZ0()*10;
  G4ThreeVector pos( x0, y0, z0 ); 
  
  //--- Get muon direction
  G4double cosineX = GetMuonCx();
  G4double cosineY = GetMuonCy();
  G4double cosineZ = GetMuonCz();
  G4ThreeVector directionalCosine( cosineX, cosineY, cosineZ );

  //--- Get muon energy
  G4double muonEnergy = GetMuonE();

  //--- Get muon sign, 10 = pos muon, 11 = neg muon
  G4int muonSign = GetMuonSign();

  //--- Generate the Muon
  if(muonSign==10) particleGun->SetParticleDefinition( muonplusDef );
  else particleGun->SetParticleDefinition ( muonminusDef );
  particleGun->GetCurrentSource()->GetPosDist()->SetCentreCoords( pos );
  particleGun->GetCurrentSource()->SetParticleTime( time*ns );
  particleGun->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection( directionalCosine );
  particleGun->GetCurrentSource()->GetEneDist()->SetMonoEnergy( muonEnergy*GeV );  

  if(DEBUGGING){
    cout << "\n ==> Generating muon \n_____________________" << endl; 
    cout << "\t Position = " << G4BestUnit(pos[0], "Length") << "," << G4BestUnit(pos[1], "Length") << G4BestUnit(pos[2], "Length") << G4endl;
    cout << "\t Direction = " << directionalCosine << G4endl;
    cout << "\t Energy = " << G4BestUnit(muonEnergy*GeV, "Energy") << endl;    
  }
  
  particleGun->GeneratePrimaryVertex( event );
  luxManager->AddPrimaryParticle( GetParticleInfo(particleGun) );
     
}

//------++++++------++++++------++++++------++++++------++++++------++++++------                                     
//                    GetPosition()                                                                         
//------++++++------++++++------++++++------++++++------++++++------++++++------    
void LUXSimGeneratorMUSUN::CalculateMuonVariables()
{
  double E1, E2, theta1, theta2, phi1, phi2;
    
  E1 = 1.;        // Muon energy range (GeV): E1-E2, minimum energy E1 = 1 GeV
  E2 = 1.e6;      // (E2>=E1)
  theta1 = 0.;    // Range of zenith angles (degrees): theta1-theta2
  theta2 = 90.;   // (theta2>=theta1)
  phi1 = 0;       // Range of azimuthal angles (degrees): phi1-phi2
  phi2 = 360.;    // (phi2>=phi1)
    
  srand(time(NULL));
  
  //  Muons are sampled on the surface of a sphere with a unit area
  //  perpendicular to the muon flux.
  //  Zenith and azimuthal angles are sampled from the slant depth
  //  distribution (muon intensity distribution) without taking
  //  into account the detector or cavern geometries.
    
  double s_hor = 1.;
  double s_ver1 = 1.;
  double s_ver2 = 1.;

  //  Alternative way is to sample muons on a surface of a parallelepiped.
  //  If you want muons to be sampled on the surface of a parallelepiped
  //  decomment the following lines and specify the size (the areas of
  //  the planes) of a parallelepiped in cm.
    
  int igflag = 1;
    
  //  You have to change all these dimensions if you change the size of the
  //  parallelepiped. The dimensions are given in cm. The example shown here
  //  is for the lab+rock having the size of 30 x 24 x 24 m^3 in x, y and z,
  //  respectively. The parallelepiped on the surface of which the muons are
  //  sampled is extended by 5 m in all directions into rock, except top where
  //  the extension is 7m. The centre of the coordinate system is the centre
  //  of the water tank (approximately). **This is wrong, the centre of the 
  //  coordinate system is actually the top of the water tank. This means that
  //  the values of zh1 and zh2 are changed from -800 to -1100 and 1600 to 1300 
  //  respectively.**
  
  double zh1 = -1100.;
  double zh2 = 1300.;
  double xv1 = -1400.;
  double xv2 = +1600.;
  double yv1 = -1200.;
  double yv2 = +1200.;
    
  // area of the horizontal plane of the parallelepiped
  s_hor = (xv2-xv1)*(yv2-yv1);
  // area of the vertical plane of the parallelepiped, perpendicular to x-axis
  s_ver1 = (yv2-yv1)*(zh2-zh1);
  // area of the vertical plane of the parallelepiped, perpendicular to y-axis
  s_ver2 = (xv2-xv1)*(zh2-zh1);
    
  double FI = 0.;
    
  //---- perform the intialisation; 
  //     theta1 & theta2 are muon zenith bounds 
  //     phi1 & phi2 and azimuthal bounds, s_hor, s_ver1 & s_ver2 are related to the direction 
  //     components of the muons on the surface of the sphere.
  static bool firstInit=true;
  int i=1;
  if ( firstInit ) Initialization(theta1,theta2,phi1,phi2,igflag,s_hor,s_ver1,s_ver2,&FI); firstInit=false;
  
  double E, theta, phi, dep;
  Sampling( &E, &theta, &phi, &dep );
    
  theta1 = theta;
  theta *= PI / 180.;

  //  changing the angle phi so x-axis is positioned along the long side
  //  of the cavern pointing at 14 deg from the North to the East from
  //  Josh Whillhite
  double phi_cavern = phi - 90. + 14.; //--- azimuth relative to the cavern
  phi += (-90. - 14.0);                //--- azimuth relative to LUXSim coordinate system
  if( phi >= 360. )
    phi -= 360.;
  if( phi < 0 )
    phi += 360.;
  phi1 = phi;
  phi *= PI / 180.;
  
  //  Muon sign (particle ID - GEANT3 definition)
  int id_part = 10;   //  positive muon
  if( G4UniformRand() < 1./2.38 )
    id_part = 11;     //  mu+/mu- ratio is 1.38; change this if necessary

  //  The minus sign is for z-axis pointing up, so the z-momentum
  //  is always pointing down.  
  double cx = -sin(theta)*cos(phi);
  double cy = -sin(theta)*sin(phi);
  double cz = -cos(theta);
  
  //  Muon coordinates
  double sh1 = s_hor * cos(theta);
  double sv1 = s_ver1 * sin(theta) * fabs(cos(phi_cavern));
  double sv2 = s_ver2 * sin(theta) * fabs(sin(phi_cavern));
  double ss = sh1 + sv1 + sv2;
  double x0, y0, z0;
  double yfl1 = G4UniformRand();

  //--- pick a surface with a probability weighted by the angle of the muon, then pick a random point on this surface.
  if( yfl1 <= sh1/ss ) {
    x0 = (xv2 - xv1)*G4UniformRand() + xv1;
    y0 = (yv2 - yv1)*G4UniformRand() + yv1;
    z0 = zh2;
  } else if( yfl1 <= (sh1+sv1)/ss ) {
    if( cx >= 0 ) 
      x0 = xv1;
    else
      x0 = xv2;
    y0 = (yv2 - yv1)*G4UniformRand() + yv1;
    z0 = (zh2 - zh1)*G4UniformRand() + zh1;
  } else {
    x0 = (xv2 - xv1)*G4UniformRand() + xv1;
    if( cy >= 0 )
      y0 = yv1;
    else
      y0 = yv2;
    z0 = (zh2 - zh1)*G4UniformRand() + zh1;
  }
  
  //--- convert the x, y, z to LUXSim coordinate system
  double azimuth_conversion = 28*(PI/180);
  double xi=x0;
  double yi=y0;
  x0=xi*cos(azimuth_conversion) + yi*sin(azimuth_conversion);
  y0=-xi*sin(azimuth_conversion) + yi*cos(azimuth_conversion);

  //      E - total muon energy in GeV assuming ultrarelativistic muons
  //      x0, y0, z0 - muon coordinates on the surface of parallelepiped
  //          specified above; x-axis and y-axis are pointing in the
  //          directions such that the angle phi (from the slant depth
  //          distribution files) is measured from x to y. z-axis is
  //          pointing upwards.
  //      cx, cy, cz - direction cosines.
  
  //--- set the variables
  SetMuonX0(x0);
  SetMuonY0(y0);
  SetMuonZ0(z0);
  SetMuonE(E);
  SetMuonSign(id_part);
  SetMuonCx(cx);
  SetMuonCy(cy);
  SetMuonCz(cz);

}


//------++++++------++++++------++++++------++++++------++++++------++++++------                                     
//                    Initialization()                                                                         
//------++++++------++++++------++++++------++++++------++++++------++++++------    
void LUXSimGeneratorMUSUN::Initialization( double theta1, double theta2, double phi1, double phi2, int igflag, double s_hor, double s_ver1, double s_ver2, double *FI )
{
  //
  //  Read in the data files
  //
  //--- fmu array contains the contents of muint-davis-mr-new.dat 
  //    (flux of muons/cm^2/s/sr in units log_10(flux) - each array is for a zenith angle and there are 360 azimuthal angles)
  //--- spmu array contains contents of musup-davis-mr-new.dat (muon spectrum / survival probabilities)
  //--- depth array contains conents of depth-davis-mr-new.dat (muon depths - each array is a zenith angle and there are 360 azimuthal angles)
  
  int lineNumber = 0, index = 0;
  char inputLine[10000]; 
  
  FILE *infile;
  if ((infile=fopen("generator/datFiles/muint-davis-mr-new.dat","r")) == NULL){
    G4cout << "Error: Cannot find the muint-davis-mr-new.dat file" << G4endl;
  }
  ifstream file3( "generator/datFiles/muint-davis-mr-new.dat", ios::in ); 
  while( file3.good() ) { 
    file3.getline( inputLine, 9999 ); 
    char *token;
    token = strtok( inputLine, " " ); 
    while( token != NULL ) {
      fmu[index][lineNumber] = atof( token ); 
      token = strtok( NULL, " " );
      index++;
      if( index == 360 ) {
	index = 0;
	lineNumber++;
      }
    }
  }
  file3.close();

  ifstream file2( "generator/datFiles/musp-davis-mr-new.dat", ios::binary|ios::in );
  int i1 = 0, i2 = 0, i3 = 0;
  float readVal;
  while( file2.good() ) {
    file2.read((char *)(&readVal), sizeof(float));
    spmu[i1][i2][i3] = readVal;
    i1++;
    if( i1 == 121 ) {
      i2++;
      i1 = 0;
    }
    if( i2 == 62 ) {
      i3++;
      i2 = 0;
    }
  }
  file2.close();
  for( int i=0; i<120; i++ )
    for( int j=0; j<62; j++ )
      for( int k=0; k<51; k++ )
	spmu[i][j][k] = spmu[i+1][j][k];
  spmu[1][1][0] = 0.000853544;

    
  ifstream file1( "generator/datFiles/depth-davis-mr-new.dat", ios::in );
  lineNumber = index = 0;
  while( file1.good() ) {
    file1.getline( inputLine, 9999 );
    char *token;
    token = strtok( inputLine, " " );
    while( token != NULL ) {
      depth[index][lineNumber] = atof( token );
      token = strtok( NULL, " " );
      index++;
      if( index == 360 ) {
	index = 0;
	lineNumber++;
      }
    }
  }
  file1.close();

 
  //
  //  Set up variables
  //    
  double em1[121], c1, c2, dph, theta, dc, sc;
  int ipc       = 1;  //--- integer variable (intialised to 1)
  double theta0 = 0.; //--- value of theta (zenith) in rads
  double cc     = 0.; //--- cos(theta)
  double ash    = 0.; //--- area of the horiztonal plane of the parallelepiped
  double asv01  = 0.; //--- area of the vertical plane of the parallelepiped, perpendicular to x-axis
  double asv02  = 0.; //--- area of the vertical plane of the parallelepiped, perpendicular to y-axis
  int ic1       = 0;  //--- zenith array index
  int ic2       = 0;  //--- zenith array index 2
  double phi    = 0.; //--- value of azimuth angle
  double dp     = 0.; //--- azimuth bin interval
  double phi0   = 0.; //--- azimuth angle in rads
  double asv1   = 0.; //--- area of the vertical plane of the parallelepiped, perpendicular to x-axis (corrected for cavern geometry)
  double asv2   = 0.; //--- area of the vertical plane of the parallelepiped, perpendicular to y-axis (corrected for cavern geometry)
  double asv0   = 0.; //--- total area 'seen' by the muons 
  double fl     = 0.; //--- total area the muons pass through
  int ip1       = 0;  //--- azimuth array index 
  int ip2       = 0;  //--- azimuth array index 2
  double sp1    = 0.; //--- 'averaged' flux of muons  
  int i         = 0;  //--- loop variable
  int ii        = 0;  //--- loop variable
  int iic       = 0;  //--- dummy variable
  int iip       = 0;  //--- dummy variable
  int ipc1      = 0;  //--- loop variable

  em1[0] = log10(0.105658);
  for( i=1; i<121; i++ )
    em1[i] = 0.05*i;                   
    
  the1 = theta1;                       //--- set up zenith and azimuth angles 
  the2 = theta2;                       //---
  c1 = cos(PI/180.*theta1);            //--- 
  c2 = cos(PI/180.*theta2);            //---
  ph1 = PI/180.*phi1;                  //---
  ph2 = PI/180.*phi2;  
 
  theta = theta1;                      //--- start at theta1  
  dc = 1.;                             //--- zenith bin interval = 1. deg
  sc = 0.; 
  while( theta < theta2-dc/2. ) {      //--- loop from theta1 (lowest zenith angle) to theta2 (highest zenith angle)
    theta += dc/2.;                    //--- increment zenith angle
    theta0 = PI/180. * theta;          //--- convert to radians
    cc = cos(theta0); 
    ash = s_hor * cc;                  //--- horiztonal area of parallelepiped 'seen' by muons with this zenith angle 
    asv01 = s_ver1 * sqrt(1. - cc*cc); //--- vertical   "                                                           "
    asv02 = s_ver2 * sqrt(1. - cc*cc); //--- vertical   "                                                           "

    ic1 = (theta + 0.999);             //--- theta for each iteration is X.5 (where X = integer) - this rounds up to nearest degree
    ic2 = ic1 + 1;                     //---
    if( ic2 > 91 ) ic2 = 91;           //---
    if( ic1 < 1 ) ic1 = 1;             //--- 
      
    phi = phi1;                        //--- start at phi1
    dp = 1.;                           //--- azimuth bin interval = 1. deg

    while( phi < phi2-dp/2. ) { 
      phi += dp/2.;                    //--- increment azimuth angle

      //  the long side of the cavern is pointing 14 degrees from North
      //  the next few lines changes the coordinate system so that it is relative to the cavern
      phi0 = PI / 180. * (phi - 90. + 14.); 
      asv1 = asv01 * fabs(cos(phi0)); 
      asv2 = asv02 * fabs(sin(phi0)); 
      asv0 = ash + asv1 + asv2;        //--- total area muons pass through
      fl = 1.;                         //--- "                           " (reassignment based on bool below)
      if( igflag == 1 )
	fl = asv0; 

      ip1 = (phi + 0.999);             //--- round azimuth angle up to nearest degree 
      ip2 = ip1 + 1;                   //--- 
      if( ip2 > 360 ) ip2 = 1;         //--- 
      if( ip1 < 1 ) ip1 = 360;         //--- 
      sp1 = 0.;                        //--- sp1 intiialised to 0.
        
      for( ii=0; ii<4; ii++ ) {
	iic = ii/2;                //--- 0, 0.5, 1., 1.5 
	iip = ii%2;                //--- 0, 1, 0, 1
	if(ip1==360 && (ii==1 || ii==3) ) iip = -359;
	//--- the point of this is to pick out the four nearest flux elements for the zenith and azimuth angles
	//--- the average then gives the value of sp1
	if( fmu[ip1+iip-1][ic1+iic-1] < 0 ) 
	  sp1 = sp1 + pow(10.,fmu[ip1+iip-1][ic1+iic-1]) / 4; //--- this gives an averaged flux of 4 nearest values for given zenith & azimuth
      }
      sc = sc + sp1 * fl * dp * PI / 180.   //--- sc is initialsied to 0. sp1 is the flux in units muons/cm^2/sr/s so sc has units muons/s. 
	* sin(theta0) * dc * PI / 180.;
      ipc = ipc + 1;                   //--- ipc gives number of elements (bins) in fnmu  
      fnmu[ipc-1] = sc;                //--- fnmu is an array with muons/s for each angular bin.
      phi = phi + dp / 2.;             
    }

    theta = theta + dc / 2.; 
  }
    
  *FI = sc;                           
  //--- loop over fnmu and make a PDF of the flux. this will then be used to sample muon angle.
  for( ipc1 = 0; ipc1 < ipc; ipc1++ ) 
    fnmu[ipc1] = fnmu[ipc1] / fnmu[ipc-1];

  G4cout << "Finished initialization..." << G4endl;

}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//               Sampling()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimGeneratorMUSUN::Sampling( double *E, double *theta, double *phi, double *dep )
{
  double yfl = G4UniformRand(); 
  int loIndex = 0, hiIndex = 32400; 
  int i = (loIndex+hiIndex)/2; 
  bool foundIndex = false;
  //---- the following block (terminated by ****) samples theta, phi and depth from the muon flux PDF  
  //****
  if( yfl < fnmu[loIndex] ) { 
    i = loIndex;
    foundIndex = true;
  } else if ( yfl > fnmu[hiIndex] ) { 
    i = hiIndex;
    foundIndex = true;
  } else if ( yfl > fnmu[i-1] && yfl <= fnmu[i] ) 
    foundIndex = true;
  while( !foundIndex ) { 
    if( yfl < fnmu[i] ) 
      hiIndex = i;
    else
      loIndex = i; 
    i = (loIndex + hiIndex)/2; 
    
    if( yfl > fnmu[i-1] && yfl <= fnmu[i] )
      foundIndex = true;
  }
  
  int ic = (i-1)/360;  
  int ip = i-1-ic*360; 
  
  yfl = G4UniformRand();
  *theta = the1 + 1.*((double)ic+yfl);
  yfl = G4UniformRand();
  *phi = ph1 + 1.*((double)ip+yfl);
  *dep = depth[ip][ic] * 2.70;
  //****

  int ic1 = cos(PI/180.**theta) * 50. + 1.; 
  if( ic1 < 1 )
    ic1 = 1;
  if( ic1 > 51 )
    ic1 = 51; 
  int ip1 = *dep / 200. - 15; 
  if( ip1 < 1 )
    ip1 = 1; 
  if( ip1 > 62 )
    ip1 = 62; 
  
  //---- the following block (terminated by ****) samples muon energy from the survival probability data file  
  //****
  yfl = G4UniformRand();
  loIndex = 0, hiIndex = 120;
  i = (loIndex+hiIndex)/2;
  foundIndex = false;
  if( yfl < spmu[loIndex][ip1][ic1] ) {
    i = loIndex;
    foundIndex = true;
  } else if ( yfl > spmu[hiIndex][ip1][ic1] ) {
    i = hiIndex;
    foundIndex = true;
  } else if ( yfl > spmu[i-1][ip1][ic1] && yfl <= spmu[i][ip1][ic1] )
    foundIndex = true;
  while( !foundIndex ) {
    if( yfl < spmu[i][ip1][ic1] )
      hiIndex = i;
    else
      loIndex = i;
    i = (loIndex + hiIndex)/2;
    
    if( yfl > spmu[i-1][ip1][ic1] && yfl <= spmu[i][ip1][ic1] )
      foundIndex = true;
  }
  double En1 = 0.05 * (i-1);
  double En2 = 0.05 * (i);
  *E = pow(10.,En1 + (En2 - En1)*G4UniformRand());
  //****
  
  return;
}
