///////////////////////////////////////////////////////////////////////////////
/*      LUXSimQuantumEfficiency.hh
 *     
 * Handles exactly what the file's name says it does.  
 *                           
 ******************************************************************************
 * Change log           
 *       20 Feb   2012 - Initial submission (Matthew)    
 *                                 
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimQuantumEfficiency_h
#define LUXSimQuantumEfficiency_h 1

#include "globals.hh"
#include "G4VRestDiscreteProcess.hh"
class G4Track;
class G4Step;
class G4ParticleDefinition;
class G4VParticleChange;

class LUXSimQuantumEfficiency : public G4VRestDiscreteProcess //class def'n
{
private:
 
public: // constructor and destructor

  LUXSimQuantumEfficiency(const G4String& processName = "pheConv");
  
  ~LUXSimQuantumEfficiency();

public: // methods, with descriptions

  G4bool IsApplicable ( const G4ParticleDefinition& aParticleType );
  G4VParticleChange* AtRestDoIt(const G4Track& aTrack,const G4Step& aStep);
  G4VParticleChange* PostStepDoIt(const G4Track& aTrack, const G4Step& aStep );
  G4double GetMeanFreePath(const G4Track& aTrack,G4double,G4ForceCondition*);
  G4double GetMeanLifeTime ( const G4Track& aTrack, G4ForceCondition*);
  
protected:
  
private:

  void LoadDoublePHEProb(G4String fileName);
  void LoadQEValuesFromFile(G4String fileName);
  G4double doublePheProb[122];
  G4double QEvals[122];
  
};

////////////////////
// Inline methods
////////////////////

inline 
G4bool LUXSimQuantumEfficiency::IsApplicable(
		  const G4ParticleDefinition& aParticleType)
{
   if (aParticleType.GetParticleName() == "opticalphoton")
     return true;
   else
     return false;
}

#endif /* LUXSimQuantumEfficiency_h */
