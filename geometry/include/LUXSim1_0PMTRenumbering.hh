//     LUXSim1_0PMTRenumbering.hh

//     This class implements a two way map from "old" simulation PMT numbers to
//     physical PMT numbers.

//     2012-08-16: Initial submission (Rich)

#include <iostream>

#ifndef LUXSIM1_0PMTRENUMBERING_HH
#define LUXSIM1_0PMTRENUMBERING_HH

class LUXSim1_0PMTRenumbering {

 public:
  //Clever trick to "initialize" a static class
  class Initializer {
  public:
    Initializer();
  };

  friend class Initializer;

 private:
  static int simToReal[123];
  static int realToSim[123];
  static Initializer init;

 public:
  static int GetRealFromOldSim(int simOldPMT);
  static int GetOldSimFromReal(int realPMT);
  static bool IsTop(int realPMT);
  static bool IsBottom(int realPMT);
  static int GetTopArrayIndexFromReal(int realPMT, bool useOldSim=false);
  static int GetBottomArrayIndexFromReal(int realPMT, bool useOldSim=false);

};

#endif
