#include <iostream>
#include <fstream>
#include <cstdlib>
#include <map>
#include <fstream>
#include <cmath>
#include <vector>
#include "globals.hh"
#include "G4Poisson.hh"
#include "Randomize.hh"
#include "LUXSimManager.hh"

const int numIDs = 2341;
const int numSGPoints = 1000;

//Used to store plane coefficients in fastSim for quick access, and so
// they don't have to be recomputed every time photonsToPHe or
// electronsToPHE is called.

//20140916 289 replaced by the variable numIDs CFPS

struct FSimTriangle{
public:
    FSimTriangle();
    //[z-level][PMT#][coefficient]
    double pCoeffs[25][122][3];
    double tCoeffs[25][122][3];
    //[PMT#][coefficient]
    double s2pCoeffs[122][3];
    double s2tCoeffs[122][3];
    //Sorted least to greatest, < numIDs
    int cornerID[3];
    
    //Debug
    bool properlyInitialized;
};

class FastSim{
public:
    FastSim(const char* libraryFilename, const char* connectFileName);
    ~FastSim();
    void photonsToPHE(int numPhots, double pos[3], double hits[122]);
    void electronsToPHE(int numElectrons, double pos[3], double hits[122]);
    void print();

private: 
    //Library Variables
    std::vector<int> connections[numIDs];

  

    double xByID[numIDs]; //[position ID] mm
    double yByID[numIDs]; //[position ID] mm
    double zLevels[25];
    double*** probability; //[zLevel][position ID][pmt]
    double*** meanTime; //[zLevel][position ID][pmt]
    double** s2Probability; //[position ID][pmt]
    double** s2meanTime; //[position ID][pmt]

    G4double doublePheProb[122];

    int skewCDF_X[numSGPoints];
    double skewCDF_Y[numSGPoints];

    //Fast lookup variables
    double inscribedR; //mm - needs to be exact
    double outerR; //mm - can be larger than reality
    //For triLookup, see the fastSim note for computing indicies.
    FSimTriangle*** triLookup; //[xIndex][yIndex] (1000x1000)
    std::map<int,FSimTriangle> triangles;

    // Scratch variables are preallocated memory for speed-critical functions.
    double scratch[2][122];
    double scratchWeights[2];
    int scratchZPositions[2];

    //Helper Functions
    void getProbability(double prob[122], FSimTriangle* source, double pos[3]);
    void getS2Probability(double prob[122],FSimTriangle* source,double pos[3],LUXSimManager *luxManager);

    bool loadConnections(const char* name);
    void initLookupTable();
    int zToIndex(double inputZ);
    void zIndicies(int* a, int* b, double inputZ);
    void getInterpolatingPlane(double output[122][3], double** p,
            int posID[3]);
    void planeInterpolate(double prob[122], double params[122][3], double x1,
            double y1);
    bool isInside(double x1, double y1, int id0, int id1, int id2);
    bool findTriangle(int* indicies, double x, double y);
    bool confine(double* x1, double* y1, double reduce = 1);
    G4int BinomFluct (G4int N0, G4double prob);
    void LoadDoublePHEProb(G4String fileName);
    double skewGaussian(double x, double mean, double sigma, double skew);
    double gaus(double x, double mean, double sigma);
    int inverseSkewGaussianCDF(double beta);

    //Initializer function
    void initConnections();
    void initSkewGaussianCDF(double mean, double sigma, double skew);
};

