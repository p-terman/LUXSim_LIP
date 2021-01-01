#ifndef LUXSIM2EVTTRIGGER_H
#define LUXSIM2EVTTRIGGER_H 1

#include "LUXSim2evtPulse.hh"

using namespace std;

class LUXSim2evtTrigger {
  public:
    //double something;
    vector<int> dropped_events;
    bool trigger_active;

    LUXSim2evtTrigger();
    ~LUXSim2evtTrigger();

    //unsigned int GetPulseNumber ();

    //void SetPulseNumber (unsigned int pn); 
    bool Phase1Trigger(vector< vector< double> > & timingInfoVecEvent, unsigned int event);
    bool Phase2Trigger(vector< vector<LUXSim2evtPulse*> > & pod_pulseEvent, unsigned int event);

    vector<int> GetDroppedEvents();
    void EnableTrigger();
    void DisableTrigger();

};

#endif

