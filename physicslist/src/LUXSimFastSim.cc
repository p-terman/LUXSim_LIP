#include "LUXSimFastSim.hh"
#include "LUXSimManager.hh"

//20140916 289 replaced by the variable numIDs CFPS

//Some basic constants

const int numZLevels = 25;
const int numPMTs = 122;

FastSim::FastSim(const char* libraryFilename, const char* connectFileName){
    //The kludge mentioned here is marked with a TODO in electronsToPHE()
    std::cout << "Note: FastSim S2 is being corrected.  This is correct for\n"
        << " the library of md5sum 81c93911106288dff7807541ef809952.\n";

    //Initialize basic constants
    inscribedR = 234.85; //mm - needs to be exact
    outerR = 250; //mm - can be larger than reality

    //Initialize heap memory.
    // This has to be done because this class is so large it would cause a
    // stack overflow if we were not using heap memory instead.
    probability = new double**[numZLevels];
    meanTime = new double**[numZLevels];
    s2Probability = new double*[numIDs];
    s2meanTime = new double*[numIDs];
    for(int zLevel = 0; zLevel < numZLevels; zLevel++){
        probability[zLevel] = new double*[numIDs];
        meanTime[zLevel] = new double*[numIDs];
        for(int id = 0; id < numIDs; id++){
            probability[zLevel][id] = new double[numPMTs];
            meanTime[zLevel][id] = new double[numPMTs];
        }
    }
    for(int id = 0; id < numIDs; id++){
        s2Probability[id] = new double[numPMTs];
        s2meanTime[id] = new double[numPMTs];
    }
    triLookup = new FSimTriangle**[1000];
    for(int i = 0; i < 1000; i++){
        //We don't need to initialize the triangles themselves;
        // these are stored in the map "triangles", which manages
        // its own memory automatically.
        triLookup[i] = new FSimTriangle*[1000];
    }

    //"initialized" tracks if there are missing samples in the library
    bool initialized[25][numIDs];
    for(int i = 0; i < numIDs; i++){
        for(int j = 0; j < 25; j++){
            initialized[j][i] = false;
        }
    }

    //Initialize zLevels (manually)
    zLevels[0] = 1;
    zLevels[1] = 10;
    zLevels[2] = 20;
    zLevels[3] = 30;
    zLevels[4] = 40;
    zLevels[5] = 50;
    zLevels[6] = 60;
    zLevels[7] = 70;
    zLevels[8] = 80;
    zLevels[9] = 90;
    zLevels[10] = 100;
    zLevels[11] = 150;
    zLevels[12] = 200;
    zLevels[13] = 250;
    zLevels[14] = 300;
    zLevels[15] = 350;
    zLevels[16] = 400;
    zLevels[17] = 450;
    zLevels[18] = 490;
    zLevels[19] = 500;
    zLevels[20] = 510;
    zLevels[21] = 520;
    zLevels[22] = 530;
    zLevels[23] = 540;
    zLevels[24] = 546.4;

    //Load the library file
    std::ifstream libFile(libraryFilename);
    bool tempIsS1;
    bool overWriteError = false;
    double tempx;
    double tempy;
    double tempz;
    int tempZIndex;
    int tempID;
    double tempProb[122];
    if(libFile.is_open()){
        while(true){
            //Read positinos, indicies, and probabilities from the library file
            libFile >> tempx;
            libFile >> tempy;
            libFile >> tempz;
            tempZIndex = zToIndex(tempz);
            libFile >> tempIsS1;
            libFile >> tempID;
            tempID -= 1;
            for(int i = 0; i < 122; i++){
                libFile >> tempProb[i];
            }
            if(libFile.fail()) break;

            //Only considering S2 for positions, because these are always in the
            // right place; sometimes S1 samples are moved a bit out of position
            // because an inconvenient piece of metal was in the way.
            if(!tempIsS1){
                xByID[tempID] = tempx;
                yByID[tempID] = tempy;
            }
            if(initialized[tempZIndex][tempID] == true and tempIsS1
                    and !overWriteError){
                std::cerr << "Error in FastSim: Overwriting library"
                    << " sample." << std::endl;
                overWriteError = true;
            }

            //Now load the probabilities into their appropriate arrays
            for(int i = 0; i < 122; i++){
                if(tempIsS1){
                    probability[tempZIndex][tempID][i] = tempProb[i];
                }
                else{
                    s2Probability[tempID][i] = tempProb[i];
                }
            }
            initialized[tempZIndex][tempID] = true;
        }
    }
    else{
        std::cerr << "Couldn't open library file." << std::endl;
        exit(1);
    }

    //Search for uninitialized samples
    int numUninitialized = 0;
    for(int i = 0; i < 25; i++){
        for(int j = 0; j < numIDs; j++){
            if(!initialized[i][j]){
                numUninitialized += 1;
                for(int pmt = 0; pmt < 122; pmt++){
                    //Very Rough approximation - the price of not 
                    // having a sample initialized, hence the warning.
                    probability[i][j][pmt] = .16/122.0;
                }
            }
        }
    }
    if(numUninitialized > 0){
        std::cerr << "Warning: FastSim has " << numUninitialized <<
            " uninitialized samples." << std::endl;
    }

    //Load the connections file that says which samples connect to which others.
    if(!loadConnections(connectFileName)){
        std::cerr << "Couldn't open connections file." << std::endl;
        exit(1);
    }

    //Calculate the lookup table for fast computation This can take several
    // seconds to do, but speeds up computation dramatically, and only needs to
    // be done once.  See fastSim note for the explanation of how it works
    initLookupTable();

    // Load table of double phe probabilities in case the user wants them.
    LoadDoublePHEProb("physicslist/src/DoublePHEperDPH.txt");

    // Initiliaze the skew gaussian
    initSkewGaussianCDF(25.86, 5.56, 0.29);

    //Finally done.
    std::cout << "Done loading fastSim.\n";
    return;
}

//Returns the probabilities of landing in each pmt via for a given point
// Expects prob is already initialized.
void FastSim::getProbability(double prob[122], FSimTriangle* source,
        double pos[3]){
    //Determine what z range we are operating in
    zIndicies(&scratchZPositions[0],&scratchZPositions[1],pos[2]);

    //Deal with cases where no z interpolation is required.
    if(scratchZPositions[0] == scratchZPositions[1]){//No z interpolation
        //Interpolate using the coefficients found in the source triangle.
        planeInterpolate(prob,source->pCoeffs[scratchZPositions[0]],
                pos[0],pos[1]);
    }
    else{ //Still need to interpolate on Z
        //Interpolate using the coefficients found in the source triangle.
        planeInterpolate(scratch[0],source->pCoeffs[scratchZPositions[0]],
                pos[0],pos[1]);
        planeInterpolate(scratch[1],source->pCoeffs[scratchZPositions[1]],
                pos[0],pos[1]);

        //Get the linear z-interpolation weights.
        scratchWeights[1] = (pos[2] - zLevels[scratchZPositions[0]]) /
            (zLevels[scratchZPositions[1]] - zLevels[scratchZPositions[0]]);
        scratchWeights[0] = 1 - scratchWeights[1];
        
        //Apply the weights to z interpolate, and we're done
        for(int i = 0; i < 122; i++){
            prob[i] = scratchWeights[0] * scratch[0][i] +
                scratchWeights[1]*scratch[1][i];
        }
    }
}

//Returns the probabilities of landing in each pmt via for a given point
// Expects prob is already initialized.
void FastSim::getS2Probability(double prob[122], FSimTriangle* source,
        double pos[3],LUXSimManager *luxManager){
    //Interpolate using the coefficients found in the source triangle.
	//LUXSimManager *luxManager = LUXSimManager::GetManager();
	G4ThreeVector x;
	x[0]=pos[0]; x[1]=pos[1]; x[2]=pos[2]; 
if(luxManager->GetEFieldFromFile()){
    planeInterpolate(prob, source->s2pCoeffs, luxManager->GetXYZDependentS2_X(x), luxManager->GetXYZDependentS2_Y(x));
}
else
{
planeInterpolate(prob, source->s2pCoeffs, pos[0], pos[1]);}
}

//Finds the two adjacent z levels which contain the desired point
void FastSim::zIndicies(int* a, int* b, double inputZ){
    //Check if it is outside the bounds of the samples
    if(inputZ < zLevels[0]){
        *a = 0;
        *b = 0;
        return;
    }
    if(inputZ > zLevels[numZLevels-1]){
        *a = numZLevels-1;
        *b = numZLevels-1;
        return;
    }

    for(int i = 1; i < numZLevels; i++){
        if(inputZ == zLevels[i]){
            *a = i;
            *b = i;
            return;
        }
        if(inputZ < zLevels[i]){
            *a = i-1;
            *b = i;
            return;
        }
    }

    //Couldn't find the z bounds
    std::cerr << "Error in FastSim: cant find bounds of z-levels "
        << "in zIndicies()." << std::endl;
    return;
}

void FastSim::initLookupTable(){
    double tempx;
    double tempy;
    double tempID = 0;
    int triangleID[3];
    for(int i = 0; i < 1000; i++){
        for(int j = 0; j < 1000; j++){
            //Samples the center of the box
            tempx = ((i+.5) / 1000) * 2 * outerR - outerR; //mm
            tempy = ((j+.5) / 1000) * 2 * outerR - outerR; //mm
            //Inverse (confine to -outerR,+outerR):
            //i = int(999.999*(x1 + outerR)/(2* outerR));
            confine(&tempx,&tempy);
            if(!findTriangle(triangleID,tempx,tempy)){
                confine(&tempx,&tempy,.999);
                if(!findTriangle(triangleID,tempx,tempy)){
                    std::cerr << "Couldn't interpolate.";
                    exit(1);
                }
            }

            //This is my conversion from three triangle IDs to an int that goes
            // into a the "triangles" map: sort them least to greatest, then
            // do id[0] + id[1]*300 + id[2]*300*300. Since ids are always less
            // than numIDs, this always generates a unique id.
            //Sort the triangleIDs
            if(triangleID[0] > triangleID[1]){
                tempID = triangleID[0];
                triangleID[0] = triangleID[1];
                triangleID[1] = tempID;
            }
            if(triangleID[1] > triangleID[2]){
                tempID = triangleID[1];
                triangleID[1] = triangleID[2];
                triangleID[2] = tempID;
                if(triangleID[0] > triangleID[1]){
                    tempID = triangleID[0];
                    triangleID[0] = triangleID[1];
                    triangleID[1] = tempID;
                }
            }
            tempID = triangleID[0] + triangleID[1]*300 + triangleID[2]*300*300;

            //Check if the triangle is already logged.
            if(triangles.count(tempID) == 0){
                //The Triangle does not exists, so we must create a new one to
                // reference it in the lookup table.
                // Create a triangle for this set of IDs
                FSimTriangle* tri = &triangles[tempID];

                for(int zLevel = 0; zLevel < numZLevels; zLevel++){
                    getInterpolatingPlane(tri->pCoeffs[zLevel],
                            probability[zLevel], triangleID);
                }
                getInterpolatingPlane(tri->s2pCoeffs,s2Probability,
                        triangleID);
                tri->properlyInitialized = true;
                triLookup[i][j] = tri;
            }
            else{
                //The triangle already exists, and the lookup can just refer to
                // it rather than making a new one.
                triLookup[i][j] = &(triangles[tempID]);
            }
        }
    }

    return;
}

FSimTriangle::FSimTriangle(){
}

//Ensures that a point lies within the dodecagon and return 1 if it already did
// This function is used both to check that an event is valid (inside a volume
// that will actually produce light), and to make sure
// the lookup table entries always refer to locations inside the detector.
bool FastSim::confine(double* x, double* y, double reduce){
    *x *= reduce;
    *y *= reduce;
    double r = pow(pow(*x,2)+pow(*y,2),.5);
    double theta = atan2(*y,*x) + M_PI;
    theta -= (M_PI/6)*(int)(theta / (M_PI/6)) + M_PI/12;
    double detectorR = inscribedR / cos(theta);

    if(r > detectorR){
        *x *= .999*detectorR / r;
        *y *= .999*detectorR / r;
        return false;
    }

    return true;
}

//Takes an array of 122 planes (described by their x,y, and constant
// coefficients in params) and evaluates them at the position (x,y).
// Expects params is size [122][3] and is the output of getInterpolatingPlane()
// Expects prob is size [122] and is already allocated
void FastSim::planeInterpolate(double prob[122], double params[122][3],
        double x, double y){
    for(int i = 0; i < 122; i++){
        prob[i] = params[i][0]*x + params[i][1]*y + params[i][2];
    }
    return;
}

//Simply searches for the nearest z level. Inefficient because I'm rushed and
// this particular function isn't speed-critical - it is only called in the
// initialization.
int FastSim::zToIndex(double inputZ){
    double distance = 999;
    int id = -1;
    for(int i = 0; i < 25; i++){
        if(distance > abs(inputZ - zLevels[i])){
            id = i;
            distance = inputZ - zLevels[i];
        }
    }
    return id;
}

//Takes the array of samples "p" at a particular z-level and a position
// "posID" and generates an interpolating plane (coefficients are written to
// "output")on the triangle they refer to using linear algebra, for
// each of the 122 planes required.  See the FastSim note for a more complete
// description of the algebra and motivation.
// This isn't especially optimized much because it is expected that this will
// be // pre-computed, and therefore not speed-critical, in the final version.
// Expects that all inputs are allocated, and that all but output are
// initialized.
// p should be size [numIDs][122]
void FastSim::getInterpolatingPlane(double output[122][3], double** p,
        int posID[3]){

    double x[3] = {xByID[posID[0]], xByID[posID[1]], xByID[posID[2]]};
    double y[3] = {yByID[posID[0]], yByID[posID[1]], yByID[posID[2]]};
    double det=x[0]*(y[1]-y[2]) + x[1]*(y[2]-y[0]) + x[2]*(y[0]-y[1]);
    double m[3][3];
    m[0][0] = y[1] - y[2];
    m[1][0] = x[2] - x[1];
    m[0][1] = y[2] - y[0];
    m[1][1] = x[0] - x[2];
    m[0][2] = y[0] - y[1];
    m[1][2] = x[1] - x[0];
    m[2][0] = x[1]*y[2] - x[2]*y[1];
    m[2][1] = x[2]*y[0] - x[0]*y[2];
    m[2][2] = x[0]*y[1] - x[1]*y[0];

    for(int k = 0; k < 3; k++){// which coefficient a,b,c in a*x + b*y + c
        for(int pmt = 0; pmt < 122; pmt++){
            output[pmt][k] = 0;
            for(int i = 0; i < 3; i++){ //3x3 matrix multiplication sum
                output[pmt][k] += p[posID[i]][pmt] * m[k][i] / det;
            }
        }
    }

    return;
}

//Expects that indicies is size 3 and is already allocated.
// Identifies the triangle which contains the point (x,y)
// indicies is the output, three sample IDs defining the triangle, and the
// return bool indicates if the search was successful.  Asking for positions
// outisde the detector (or inside, but outside the max sampled area,
// slightly smaller by a mm or so) will fail.
bool FastSim::findTriangle(int* indicies, double x, double y){
    double dist;
    double leastDist = 99999;
    int nearID = -1;
    int nextNearID = -1;
    double nextNearDist = 99999;

    //Search for the nearest and next-nearest samples to the point given
    for(int id = 0; id < numIDs; id++){
        dist = sqrt(pow(xByID[id]-x,2)+pow(yByID[id]-y,2));
        if(dist < leastDist){
            if(leastDist < nextNearDist){
                nextNearDist = leastDist;
                nextNearID = nearID;
            }
            leastDist = dist;
            nearID = id;
        }
        else if(dist < nextNearDist){
            nextNearDist = dist;
            nextNearID = id;
        }
    }
    if(nearID < 0 or nearID > numIDs-1){
        std::cerr << "Recieved invalid ID. (fastSim.findTriangle()): "
            << nearID <<  std::endl;
        exit(1);
    }

    
    //See if the point is inside a triangle made up of first the nearest, then
    //the next-nearest sample.
    for(int r = 0; r < 2; r++){
        //If we failed the first time, search for a triangle attached to the
        // NEXT nearest sample instead.
        if(r == 1) nearID = nextNearID;

        //Form the triangle
        for(size_t i = 0; i < connections[nearID].size(); i++){
            for(size_t j = 0;j<connections[connections[nearID][i]].size();j++){
                for(size_t k = 0; k < connections[nearID].size(); k++){
                    if(connections[nearID][k] ==
                            connections[connections[nearID][i]][j]){
                        if(isInside(x,y,nearID,connections[nearID][i],
                                    connections[nearID][k])){
                            //Then these ARE the IDs we're looking for.
                            indicies[0] = nearID;
                            indicies[1] = connections[nearID][i];
                            indicies[2] = connections[nearID][k];
                            return true;
                        }
                    }
                }
            }
        }
    }

    //Cannot find a triangle which contains the requested point
    return false;
}

//Checks if a point is inside the triangle defined by the three sample IDs
bool FastSim::isInside(double x, double y, int id0, int id1, int id2){
    if(id0 < 0 or id0 > numIDs-1 or id1 < 0 or id1 > numIDs-1 or id2 < 0 or id2 > numIDs-1){
        std::cerr << "Invalid ID passed to FastSim.isInside()."
            << id0 << " " << id1 << " " << id2 << std::endl;
        exit(1);
    }

    //Get the positions for each of the points from their IDs
    double triX[3] = {xByID[id0], xByID[id1], xByID[id2]};
    double triY[3] = {yByID[id0], yByID[id1], yByID[id2]};

    //If for every pair of triangle corners the point is on the same
    // side of the line as the far corner, then the point is inside
    // the triangle.
    double m = 0;
    double b = 0;
    for(int i = 0; i < 3; i++){
        int j = i+1; //Next point
        if(j == 3) j = 0;
        int k = i - 1; //Far point
        if(k < 0) k = 2;
        //Case: the connecting line is vertical
        if(triX[i] == triX[j]){
            if((triX[i] - triX[k]) * (triX[i] - x) >= 0)
                continue;
            else
                return false;
        }
        //Case: the connecting line is horizontal
        if(triY[i] == triY[j]){
            if((triY[i] - triY[k]) * (triY[i] - y) >= 0)
                continue;
            else
                return false;
        }
        //Case: the connecting line is diagonal: y = m*x + b, m != 0, infinity
        m = (triY[i] - triY[j]) / (triX[i] - triX[j]);
        b = triY[i] - m * triX[i];
        if((b + m * triX[k] - triY[k]) * (b + m * x - y) >= 0)
            continue;
        else
            return false;
    }
    return true;
}

//Reads the connections data file, which defines which sample points are
// connected to which others as sides of a triangle.  This is stored as a
// vector of connections in memory.
bool FastSim::loadConnections(const char* name){
    int temp1 = 0;
    int temp2 = 0;
    std::ifstream conFile(name);
    int i = 0;
    if(conFile.is_open()){
        while(true){
            i++;
            conFile >> temp1;
            conFile >> temp2;
            if(conFile.fail()) break;
            if(temp1 == temp2) return 1;
            if(temp1 > numIDs or temp2 > numIDs) return 1;
            if(temp1 < 1 or temp2 < 1) return 1;
            connections[temp1-1].push_back(temp2-1);
            connections[temp2-1].push_back(temp1-1);
        }
    }
    else return 0;

    return 1;
}

//Takes in a number of photons and the position they were generated, and
// returns a list of hits in each PMT, distributed in a reasonable way.  This
// is the function to be called from G4S1Light.cc
void FastSim::photonsToPHE(int numPhots, double pos[3], double hits[122]){
    double s1Prob[122];
    for(int i = 0; i < 122; i++){
        s1Prob[i] = 0;
        hits[i]=0;
    }
    if(numPhots == 0) return;

    //Ensure the event is in a region which can be detected
    if(!confine(&pos[0],&pos[1])) return;

    //Get the indicies for the lookup table for the event position
    int xi = int(999.999*(pos[0] + outerR)/(2* outerR));
    int yi = int(999.999*(pos[1] + outerR)/(2* outerR));
    if(xi > 999 or xi < 0 or yi > 999 or yi <0){
        std::cerr << "Error: FastSim triangle lookup out of range."
            << std::endl;
    }
    
    //Get the s1 probabilities
    getProbability(s1Prob,triLookup[xi][yi],pos);
    
    //Now, deposit the photons
    //We start by summing the S1 probabilities so they can be normalized 
    G4double survival = 0.; G4int survivors = 0;
    for(int i = 0; i < 122; i++) {
      survival = survival +  s1Prob[i];
    }
    //Now we calculate a cumulative distribution function
    //This will be used to determine to which PMT each photon goes
    double s1CDF[122];
    s1CDF[0] = s1Prob[0] / survival;
    for(int i = 1; i < 122; i++) {
      s1CDF[i] = s1CDF[i-1] + s1Prob[i] / survival;
    }
    //Determine total number of survivors
//    survivors = G4int(floor(G4RandGauss::shoot(survival*double(numPhots),0.9*sqrt(survival*(double)numPhots))+0.5));
     //Above line is a kludge to broaden S1 response, here we assume pure binomial flucctuation
     //Which is already accounted for in G4S1Light before calling fast sims
    LUXSimManager *luxManager = LUXSimManager::GetManager();
    survivors = BinomFluct(numPhots, luxManager->GetS1Gain() * survival);

    //Loop over phtons and for each one, generate a random number between 0 and 1 and use the CDF to pick a PMT
    double temp;
    for(int i = 0; i < survivors; i++) {
      temp = G4UniformRand();
      int j = floor(temp*121.) + 1;
      bool found = false;
      int maxPMT = 122;
      int minPMT = 0;
      //Find the PMT corresponding to that random number
      while(!found) {
        if(j == 0 &&
           temp < s1CDF[j]) {
          found = true;
        }
        else if(temp < s1CDF[j-1]){
          maxPMT = j;
          j = floor((minPMT + j) / 2.);
        }
        else if(temp > s1CDF[j]){
          minPMT = j;
          j = floor((maxPMT + j) / 2.);
        }
        else {
          found = true;
        }
      }
      //Add a hit to PMT j
      hits[j]++;
    }

    //For each photon there is a 20% chance that two phe are produced... at least this is what we're told
//    int numDoubles;
//    if (luxManager->GetLUXDoublePheRateFromFile()) {
//        for(int i = 0; i < 122; i++) {
//            numDoubles = BinomFluct(hits[i], doublePheProb[i]);
//            hits[i] = hits[i] + numDoubles;
//        }
//    }
//    else {
//        for(int i = 0; i < 122; i++) {
//            numDoubles = BinomFluct(hits[i], .2);
//            hits[i] = hits[i] + numDoubles;
//        }
//    }

    return;
}

//Similar to the photonsToPHE, but this one is for S2 light generated by
// electrons at a given position.  It also returns a list of hits, and is
// to be called from G4S1Light.cc
void FastSim::electronsToPHE(int numElectrons, double pos[3],
        double hits[122]){
 LUXSimManager *luxManager = LUXSimManager::GetManager();
    double s2Prob[122];
    for(int i = 0; i < 122; i++){
        s2Probability[i] = 0;
        hits[i] = 0;
    }

    if(numElectrons == 0) return;

    //Ensure the event is in a region which can be detected
    if(!confine(&pos[0],&pos[1])) return;

    //Electric fields below 5.6 cm are in fields that don't produce S2, and
    // events above 54.64 are no longer in the Liquid Xenon
    if(pos[2] < 56 || pos[2] > 546.4) return;
	G4ThreeVector xx;
	xx[0]=pos[0]; xx[1]=pos[1]; xx[2]=pos[2]; 
int xi,yi;	
    //Get the indicies for the lookup table for the event position
   if(luxManager->GetEFieldFromFile())
{
xi = int(999.999*(luxManager->GetXYZDependentS2_X(xx) + outerR)/(2* outerR));
yi = int(999.999*(luxManager->GetXYZDependentS2_Y(xx) + outerR)/(2* outerR));
}
else
{
xi = int(999.999*(pos[0] + outerR)/(2* outerR));
yi = int(999.999*(pos[1] + outerR)/(2* outerR));
}
if(xi>=0 && xi<1000&&yi>=0 &&yi<1000){
    getS2Probability(s2Prob,triLookup[xi][yi],pos,luxManager);}

   
    //if(luxManager->GetLUXFastSimSkewGaussianS2()) {
      if(1){
 //Need to renormalize S2 probability
        double sum = 0;
        for(int i = 0; i < 122; i++) {
            sum = sum + s2Prob[i];
        }

        //Next, need to determine total number of phe
        int numPhot = 0;
        int electronSize;
        for(int i = 0; i < numElectrons; i++) {
            electronSize = inverseSkewGaussianCDF(G4UniformRand());
            numPhot = numPhot + electronSize;
        }
        //Now we calculate a cumulative distribution function
        //This will be used to determine to which PMT each photon goes
        double s2CDF[122];
        s2CDF[0] = s2Prob[0] / sum;
        for(int i = 1; i < 122; i++) {
          s2CDF[i] = s2CDF[i-1] + s2Prob[i] / sum;
        }
        //Loop over phtons and for each one, generate a random number between 0 and 1 and use the CDF to pick a PMT
        double temp;
        for(int i = 0; i < numPhot; i++) {
          temp = G4UniformRand();
          int j = floor(temp*121.)+1;
          bool found = false;
          int maxPMT = 122;
          int minPMT = 0;
          //Find the PMT corresponding to that random number
          while(!found) {
            if(j == 0 &&
               temp < s2CDF[j]) {
              found = true;
            }
            else if(temp < s2CDF[j-1]){
              maxPMT = j;
              j = floor((minPMT + j) / 2.);
            }
            else if(temp > s2CDF[j]){
              minPMT = j;
              j = floor((maxPMT + j) / 2.);
            }
            else {
              found = true;
            }
          }
          //Add a hit to PMT j
          hits[j]++;
        }
      }
      else {
  	//Deposit the photons as PHE
        const double kludgeFactor = 1.32; //TODO: Ideally, this should be 1.0
        int numPhe = 0;
        for(int i = 0; i < 122; i++){
          hits[i] = G4int(floor(G4RandGauss::shoot(s2Prob[i]*kludgeFactor*double(numElectrons),sqrt(s2Prob[i]*kludgeFactor*(double)numElectrons))+0.5));//G4Poisson(kludgeFactor * n$
          numPhe = numPhe + hits[i];
        }
    }

    //For each photon there is a 20% chance that two phe are produced... at least this is what we're told
//    int numDoubles;
//    if (luxManager->GetLUXDoublePheRateFromFile()) {
//        for(int i = 0; i < 122; i++) {
//            numDoubles = BinomFluct(hits[i], doublePheProb[i]);
//            hits[i] = hits[i] + numDoubles;
//        }
//    }
//    else {
//        for(int i = 0; i < 122; i++) {
//            numDoubles = BinomFluct(hits[i], .2);
//            hits[i] = hits[i] + numDoubles;
//        }
//    }
}

FastSim::~FastSim(){
    //Delete all the heap memory.
    for(int zLevel = 0; zLevel < 25; zLevel++){
        for(int id = 0; id < numIDs; id++){
            delete[] probability[zLevel][id];
            delete[] meanTime[zLevel][id];
        }
        delete[] probability[zLevel];
        delete[] meanTime[zLevel];
        delete[] s2Probability[zLevel];
        delete[] s2meanTime[zLevel];
    }
    delete[] probability;
    delete[] meanTime;
    delete[] s2Probability;
    delete[] s2meanTime;

    for(int i = 0; i < 1000; i++){
        delete[] triLookup[i];
    }
    delete[] triLookup;
    return;
}

//The function below is a skew gaussian for generating the S2 response
double FastSim::skewGaussian(double x, double mean, double sigma, double skew) {
    double pi = 3.1415926;
    double omega = sigma / sqrt(1 - 2 * (skew * skew / (1 + skew * skew)) / pi);
    double mu = mean - omega * (skew / sqrt(1 + skew * skew)) * sqrt(2/pi);
    double alpha = (x - mu) / (sqrt(2) * omega);
    return gaus(x, mu, omega) * (1 + erf(skew * alpha / sqrt(2)));
}

//Gaussian function. I should be able to steal this from somewhere else, I'll fix that later.
double FastSim::gaus(double x, double mean, double sigma) {
    double out = exp(-1. * (x - mean) * (x - mean) / (2 * sigma * sigma));
    return out;
}

//Generates the cumulative distribution function for the skew Gaussian
void FastSim::initSkewGaussianCDF(double mean, double sigma, double skew) {
    skewCDF_X[0] = 0.;
    skewCDF_Y[0] = 0.;
    for(int i = 1; i < numSGPoints; i++) {
        skewCDF_X[i] = i;
        skewCDF_Y[i] = skewCDF_Y[i-1] + skewGaussian(i, mean, sigma, skew);
    }
    for(int i = 0; i < numSGPoints; i++) {
        skewCDF_Y[i] = skewCDF_Y[i] / skewCDF_Y[numSGPoints-1];
    }
}

//For a given beta between 0 and 1 (inclusive) find the value of skewCDF_Y closest to alpha
//and return the corresponding X value
int FastSim::inverseSkewGaussianCDF(double beta) {
    int indexMin = -1;
    double min = 99999999.;
    for(int i = 0; i < numSGPoints; i++) {
        if (fabs(beta - skewCDF_Y[i]) < min) {
            min = fabs(beta - skewCDF_Y[i]);
            indexMin = i;
        }
    }
    return skewCDF_X[indexMin];
}

G4int FastSim::BinomFluct ( G4int N0, G4double prob ) {
  G4double mean = N0*prob;
  G4double sigma = sqrt(N0*prob*(1-prob));
  G4int N1 = 0;
  if ( prob == 0.00 ) return N1;
  if ( prob == 1.00 ) return N0;

  if ( N0 < 10 ) {
    for(G4int i = 0; i < N0; i++) {
      if(G4UniformRand() < prob) N1++;
    }
  }
  else {
    N1 = G4int(floor(G4RandGauss::shoot(mean,sigma)+0.5));
  }
  if ( N1 > N0 ) N1 = N0;
  if ( N1 < 0 ) N1 = 0;
  return N1;
}

void FastSim::LoadDoublePHEProb(G4String fileName) {
  std::ifstream file;
  file.open(fileName);
  if (!file.is_open()) {
    G4cout<<G4endl<<G4endl<<G4endl;
    G4cout<<"Double phe probability File Not Found!"<<G4endl;
    G4cout<<G4endl<<G4endl<<G4endl;
    exit(0);
  }
  for (int i = 0; i < 122; i++) {
    file >> doublePheProb[i];
  }
  file.close();
}

//A debug function in case you want to see the values in FastSim's class
// variables.
void FastSim::print(){
    for(size_t i = 0; i < 25; i++){
        for(int k = 0; k < numIDs; k++){
            std::cout << probability[i][k][20] << " ";
        }
        std::cout << std::endl;
    }
    /*
    for(size_t i = 0; i < numIDs; i++){
        std::cout << i << ": ";
        for(size_t j = 0; j < connections[i].size(); j++){
            std::cout << connections[i][j] << " ";
        }
        std::cout << std::endl;
    }
    */
    //for(size_t i = 0; i < numIDs; i++){
        //std::cout << xByID[i] << " " << yByID[i] << std::endl;
    //}
    return;
}

/* ===== Instructions for Adding Timing to FastSim =====
 * Sadly I ran out of time before I could finish the timing part of this
 * system.  This note is for whoever intends to resolve my failure.
 *
 * The first step is to modify photonsToPHE and electronsToPHE to give results
 * (in the hits variable) not as an array of photon counts (size 122, one for
 * each pmt), but as a vector<double> hitTimes[122].  Each PMT gets a vector of
 * hit times, one entry for each PHE which survives and hits that PMT.  You will
 * know it is working if replacing references to hits[i] with hits.size()[i] in
 * G4S1Light.cc produces basically the same results as it did before (but now we
 * have a way to send hit times to NEST).
 *
 * Next, figure out what variable you want to use and interpolate with fastSim.
 * Originally, I wanted to use the mean time to hit, and distribute them as a
 * Gaussian, but I now think that may be naive.  A better
 * idea may be to get the fraction that occur less than 5 ns (direct hits), and
 * say that the rest are distributed on an exponential of time constant ~25 ns.
 * Up to you though.  The memory is already allocated as s2meanTime and
 * meanTime, so you can just piggyback the interpolation system for probability
 * (wherever probability  and s2Probability are used, have a timing equivalent,
 * but don't forget about variables that are generated as processed versions of
 * them - it doesnt help to go halfway just follow the numbers through, or read
 * the FastSim note to understand how it works - especially the optimization
 * part).
 *
 * Finally, you need to modify the sample reader to write the times into
 * fastSimLibrary.dat, and regenerate the library.  Currently, the format is:
 * x:y:z:isS1?:positionID:probability[122]
 * I reccommend changing this to
 * x:y:z:isS1?:positionID:probability[122]:timingVariable[122]
 * Once that is done and working, modify the part of FastSim::FastSim that reads
 * the library file to read the values into meanTime and s2MeanTime (or whatever
 * you rename them to).
 *
 * Then, all that is left is 72 hours of debugging.  Huzzah.  Try to test each
 * step to keep this to a minimum.
 */
