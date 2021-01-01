////////////////////////////////////////////////////////////////////////////////
/*  LUXSimBST.hh
*
* This is the header for the Binary Search Tree
*
********************************************************************************
* Change log
*  21 Jul 2011 - Initial Submission (Nick)
*  14 Jul 2012 - Add to decayNode variables to accept all generators (Nick)
*  22 Aug 2012 - Fix BST timing to *ns and add warning messages (Nick)
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimBST_HH
#define LUXSimBST_HH 1

//  GEANT4 includes
//
#include "globals.hh"
#include "G4ThreeVector.hh"

//  LUXSim includes
//
#include "LUXSimIsotope.hh"

//  C++ includes
//
#include <fstream>
#include <iostream>
#include <vector>

struct decayNode {
    G4int Z;
    G4int A;
    G4String particleName;//singleParticle
    G4double energy;//singleParticle and WimpMass
    G4double timeOfEvent;//nanoseconds
    G4ThreeVector pos;
    // IDs used to call GenereateFromEventList methods
    G4int sourceByVolumeID;//for manager->geometry
    G4int sourcesID;//geometry->generators
    decayNode *left;
    decayNode *right;
};

class LUXSimBST
{
  public:
    LUXSimBST( G4int, G4double, G4double, G4int );
    ~LUXSimBST();
    void Insert( Isotope*, G4double, G4ThreeVector, G4int, G4int);
    decayNode *GetEarliest();
    decayNode *GetLast();
    void PopEarliest();
    void PopLast();
    void PrintNodes();
    inline G4bool HasNodes() { return hasNodes; };
    inline G4int GetNumNonemptyNodes() { return numNonemptyNodes; };
    
    typedef decayNode pubDecayNode;

  private:
    G4int numEvents;
    G4int numNonemptyNodes;
    G4int numNodes;
    G4bool hasNodes;
    decayNode *topNode;
    decayNode *Insert( Isotope*, G4double, G4ThreeVector, G4int, G4int,
        decayNode* );
    void PrintNodes( decayNode* );

    G4int nodeCount;
    G4int warningsWritten,warningsWritten1e4, warningsWritten1e5;
    std::vector<Isotope*> allIsotopes;
  
};
#endif
