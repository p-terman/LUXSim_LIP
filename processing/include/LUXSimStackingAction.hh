////////////////////////////////////////////////////////////////////////////////
/*    LUXSimStackingAction.hh
*
* This is the header file for the LUXSimStackingAction class.
*
********************************************************************************
* Change log
*     06-Oct-2015 - Initial submission (David W)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSIMSTACKINGACTION_H
#define LUXSIMSTACKINGACTION_H 1


//
//    C/C++ includes
//
#include "globals.hh"
#include "G4UserStackingAction.hh"
#include <vector>

//
//    LUXSim includes
//
#include "LUXSimManager.hh"

//
//    Class forward declarations
//
class G4Track;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimStackingAction : public G4UserStackingAction {

        public:
                LUXSimStackingAction();
                virtual ~LUXSimStackingAction();

                virtual G4ClassificationOfNewTrack ClassifyNewTrack( const G4Track* aTrack );

        private:
                LUXSimManager *luxManager;
                double lastTime;
};
#endif
