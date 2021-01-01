////////////////////////////////////////////////////////////////////////////////
/*	LUXSimSourceCatalog.hh
*
* This is the header file to handle the various event generators. This catalog
* object is essentially a sub-manager for the source generators.
*
********************************************************************************
* Change log
*	20 Apr 2009 - Initial submission (Kareem)
*	26 Jun 2009 - Added event generator sources (Nick)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimSourceCatalog_HH
#define LUXSimSourceCatalog_HH 1

//
//	C/C++ includes
//
#include <vector>

//
//	GEANT4 includes
//
#include "globals.hh"

//
//	Class forwarding
//
class LUXSimManager;
class LUXSimSource;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimSourceCatalog
{
	public:
		LUXSimSourceCatalog();
		~LUXSimSourceCatalog();

	public:
		G4int GetNumSourceTypes() { return sources.size(); };
		LUXSimSource *GetSourceType( G4int i ) { return sources[i]; };

	private:
		LUXSimManager *luxManager;
		
		std::vector<LUXSimSource*> sources;
};

#endif
