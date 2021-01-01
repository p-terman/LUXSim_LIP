////////////////////////////////////////////////////////////////////////////////
/*      LUXSimLZ20Simple.hh
*
* This is the header file to define a "simple" version of the LZ20 detector.
*
********************************************************************************
* Change log
*       26 May 2010 - Initial submission (Monica)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimLZ20Simple_HH
#define LUXSimLZ20Simple_HH 1

//
//      GEANT4 includes
//
#include "globals.hh"

//
//      LUXSim includes
//
#include "LUXSimDetector.hh"

//
//      Class forwarding
//
class G4LogicalVolume;
class LUXSimDetectorComponent;
//class LUXSimExamplePMTBank;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimLZ20Simple : public LUXSimDetector
{
        public:
                LUXSimLZ20Simple();
                ~LUXSimLZ20Simple();

                inline LUXSimDetectorComponent *GetLXeTarget() {return liquidXeTarget;};
  //inline LUXSimDetectorComponent *GetTopBank() { return topBank; };
  //inline LUXSimDetectorComponent *GetBottomBank() { return bottomBank; };

        private:

                LUXSimDetectorComponent *liquidXeTarget;
  //LUXSimExamplePMTBank *banks[2];
  //LUXSimDetectorComponent *topBank;
  //LUXSimDetectorComponent *bottomBank;
};

#endif

