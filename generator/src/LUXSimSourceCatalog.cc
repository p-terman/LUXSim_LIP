////////////////////////////////////////////////////////////////////////////////
/*	LUXSimSourceCatalog.hh
*
* This is the code file to handle the various event generators. This catalog
* object is essentially a sub-manager for the source generators.
*
********************************************************************************
* Change log
*	20 April 2009 - Initial submission (Kareem)
*	4 January 2010 - Simplified single nucleus decays to one file (Nick)
*	15 June 2010 - Added MASN to catalog (Melinda)
*	13 Sep 2010 - Added fission neutrons to catalog (Kareem)
*	03 Mar 2011 - Changed "FissionNeutrons" to "CfFission" (Kareem)
*   21 Jul 2011 - Added DecayChain (Nick)
*   19 Aug 2011 - Added ScintPhoton (Nick)
*	25 Aug 2011 - Added p-Lithium Neutron generator to catalog (Mike)
*   13 Jan 2012 - Kr83m generator (Nick)
*   04 Mar 2012 - Single Particle Generator (Nick)
*   18 Jun 2012 - Wimp Generator (Daniel)
*	07 Nov 2012 - Pb210 generator (Dave)
*	16 Feb 2013 - Ra226 generator (Dave)
*	12 Mar 2013 - Xe129m and Xe131m generators (Dave)
*	23 Jul 2013 - Tritium generator (Kareem)
*       23 Jul 2013 - Gamma-X generator (Matthew)
*       10 May 2014 - YBe generator (Kevin)
*       12 May 2014 - EventsFile generator (Kevin)
*       25 Jan 2015 - TwoElectrons generator (Kevin)
*       25 Jan 2015 - DD generator (Kevin)
*	16 Feb 2015 - Rn220 generator (Simon)
*       31 Mar 2015 - LZ bkg neutron generator (Scott Haselschwardt)
*       19 May 2015 - LZ bkg gamma generator (Scott Haselschwardt)
*       06 Oct 2015 - G4Decay generator (David W)
*   18-Dec-2015 - Muon generator (David W) (merged into git by Doug T)
*/
////////////////////////////////////////////////////////////////////////////////

//
//	GEANT4 includes
//
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSimSourceCatalog.hh"
#include "LUXSimManager.hh"

#include "LUXSimSource.hh"
#include "LUXSimGeneratorAmBe.hh"
#include "LUXSimGeneratorCfFission.hh"
#include "LUXSimGeneratorMASN.hh"
#include "LUXSimGeneratorTh232.hh"
#include "LUXSimGeneratorDecayChain.hh" 
#include "LUXSimGeneratorU238.hh"
#include "LUXSimGeneratorRa226.hh"
#include "LUXSimGeneratorRn222.hh"
#include "LUXSimGeneratorPb210.hh"
#include "LUXSimGeneratorSingleDecay.hh"
#include "LUXSimGeneratorSingleParticle.hh"
#include "LUXSimGeneratorScintPhotons.hh"
#include "LUXSimGeneratorNeutronGenerator-p-Li.hh"
#include "LUXSimGeneratorKr83m.hh"
#include "LUXSimGeneratorXe129m.hh"
#include "LUXSimGeneratorXe131m.hh"
#include "LUXSimGeneratorWimp.hh"
#include "LUXSimGeneratorTritium.hh"
#include "LUXSimGeneratorGammaX.hh"
#include "LUXSimGeneratorYBe.hh"
#include "LUXSimGeneratorEventsFile.hh"
#include "LUXSimGeneratorTwoElectrons.hh"
#include "LUXSimGeneratorDD.hh"
#include "LUXSimGeneratorRn220.hh"
#include "LUXSimGeneratorLZbkgNeutrons.hh"
#include "LUXSimGeneratorLZbkgGammas.hh"
#include "LUXSimGeneratorG4Decay.hh"
#include "LUXSimGeneratorMUSUN.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimSourceCatalog()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimSourceCatalog::LUXSimSourceCatalog()
{
	luxManager = LUXSimManager::GetManager();
	luxManager->Register( this );
	
	sources.clear();
	
	sources.push_back( new LUXSimGeneratorAmBe );
	sources.push_back( new LUXSimGeneratorCfFission );
	sources.push_back( new LUXSimGeneratorRa226 );
	sources.push_back( new LUXSimGeneratorRn222 );
	sources.push_back( new LUXSimGeneratorPb210 );
	sources.push_back( new LUXSimGeneratorTh232 );
	sources.push_back( new LUXSimGeneratorDecayChain );
	sources.push_back( new LUXSimGeneratorU238 );
    sources.push_back( new LUXSimGeneratorSingleDecay ); 
    sources.push_back( new LUXSimGeneratorSingleParticle ); 
    sources.push_back( new LUXSimGeneratorScintPhotons ); 
	sources.push_back( new LUXSimGeneratorMASN );
	sources.push_back( new LUXSimGeneratorpLithium );
	sources.push_back( new LUXSimGeneratorKr83m );
	sources.push_back( new LUXSimGeneratorXe129m );
	sources.push_back( new LUXSimGeneratorXe131m );
    sources.push_back( new LUXSimGeneratorWimp );
	sources.push_back( new LUXSimGeneratorTritium );
	sources.push_back( new LUXSimGeneratorGammaX );
	sources.push_back( new LUXSimGeneratorYBe );
	sources.push_back( new LUXSimGeneratorEventsFile );
	sources.push_back( new LUXSimGeneratorTwoElectrons );
	sources.push_back( new LUXSimGeneratorDD );
	sources.push_back( new LUXSimGeneratorRn220 );
        sources.push_back( new LUXSimGeneratorLZbkgNeutrons );
        sources.push_back( new LUXSimGeneratorLZbkgGammas );
	sources.push_back( new LUXSimGeneratorG4Decay );
	sources.push_back(new LUXSimGeneratorMUSUN);
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					~LUXSimSourceCatalog()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimSourceCatalog::~LUXSimSourceCatalog()
{
	sources.clear();
}
