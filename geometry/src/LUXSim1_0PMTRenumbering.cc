//    LUXSim1_0PMTRenumbering.cc

//    Keeps track of mapping between real and simulation PMT numbering

//    2012.08.16 - Initial Submission (Rich)

#include "LUXSim1_0PMTRenumbering.hh"

int LUXSim1_0PMTRenumbering::simToReal[123] = {0};
int LUXSim1_0PMTRenumbering::realToSim[123] = {0};
LUXSim1_0PMTRenumbering::Initializer init;

LUXSim1_0PMTRenumbering::Initializer::Initializer() {
  //Set up map from sim to real
  //This is copied from Matthew, note the 1-based counting
  LUXSim1_0PMTRenumbering::simToReal[0]=0;
  LUXSim1_0PMTRenumbering::simToReal[1] = 1  ;
  LUXSim1_0PMTRenumbering::simToReal[2] = 54 ;
  LUXSim1_0PMTRenumbering::simToReal[3] = 53 ;
  LUXSim1_0PMTRenumbering::simToReal[4] = 52 ;
  LUXSim1_0PMTRenumbering::simToReal[5] = 51 ;
  LUXSim1_0PMTRenumbering::simToReal[6] = 2  ;
  LUXSim1_0PMTRenumbering::simToReal[7] = 5  ;
  LUXSim1_0PMTRenumbering::simToReal[8] = 57 ;
  LUXSim1_0PMTRenumbering::simToReal[9] = 56 ;
  LUXSim1_0PMTRenumbering::simToReal[10] = 55;
  LUXSim1_0PMTRenumbering::simToReal[11] = 44;
  LUXSim1_0PMTRenumbering::simToReal[12] = 3 ;
  LUXSim1_0PMTRenumbering::simToReal[13] = 6 ;
  LUXSim1_0PMTRenumbering::simToReal[14] = 8 ;
  LUXSim1_0PMTRenumbering::simToReal[15] = 59;
  LUXSim1_0PMTRenumbering::simToReal[16] = 58;
  LUXSim1_0PMTRenumbering::simToReal[17] = 47;
  LUXSim1_0PMTRenumbering::simToReal[18] = 43;
  LUXSim1_0PMTRenumbering::simToReal[19] = 4 ;
  LUXSim1_0PMTRenumbering::simToReal[20] = 7 ;
  LUXSim1_0PMTRenumbering::simToReal[21] = 9 ;
  LUXSim1_0PMTRenumbering::simToReal[22] = 10;
  LUXSim1_0PMTRenumbering::simToReal[23] = 60;
  LUXSim1_0PMTRenumbering::simToReal[24] = 49;
  LUXSim1_0PMTRenumbering::simToReal[25] = 46;
  LUXSim1_0PMTRenumbering::simToReal[26] = 42;
  LUXSim1_0PMTRenumbering::simToReal[27] = 11;
  LUXSim1_0PMTRenumbering::simToReal[28] = 15;
  LUXSim1_0PMTRenumbering::simToReal[29] = 18;
  LUXSim1_0PMTRenumbering::simToReal[30] = 20;
  LUXSim1_0PMTRenumbering::simToReal[31] = 121;
  LUXSim1_0PMTRenumbering::simToReal[32] = 50;
  LUXSim1_0PMTRenumbering::simToReal[33] = 48;
  LUXSim1_0PMTRenumbering::simToReal[34] = 45;
  LUXSim1_0PMTRenumbering::simToReal[35] = 41;
  LUXSim1_0PMTRenumbering::simToReal[36] = 12;
  LUXSim1_0PMTRenumbering::simToReal[37] = 16;
  LUXSim1_0PMTRenumbering::simToReal[38] = 19;
  LUXSim1_0PMTRenumbering::simToReal[39] = 30;
  LUXSim1_0PMTRenumbering::simToReal[40] = 40;
  LUXSim1_0PMTRenumbering::simToReal[41] = 39;
  LUXSim1_0PMTRenumbering::simToReal[42] = 37;
  LUXSim1_0PMTRenumbering::simToReal[43] = 34;
  LUXSim1_0PMTRenumbering::simToReal[44] = 13;
  LUXSim1_0PMTRenumbering::simToReal[45] = 17;
  LUXSim1_0PMTRenumbering::simToReal[46] = 28;
  LUXSim1_0PMTRenumbering::simToReal[47] = 29;
  LUXSim1_0PMTRenumbering::simToReal[48] = 38;
  LUXSim1_0PMTRenumbering::simToReal[49] = 36;
  LUXSim1_0PMTRenumbering::simToReal[50] = 33;
  LUXSim1_0PMTRenumbering::simToReal[51] = 14;
  LUXSim1_0PMTRenumbering::simToReal[52] = 25;
  LUXSim1_0PMTRenumbering::simToReal[53] = 26;
  LUXSim1_0PMTRenumbering::simToReal[54] = 27;
  LUXSim1_0PMTRenumbering::simToReal[55] = 35;
  LUXSim1_0PMTRenumbering::simToReal[56] = 32;
  LUXSim1_0PMTRenumbering::simToReal[57] = 21;
  LUXSim1_0PMTRenumbering::simToReal[58] = 22;
  LUXSim1_0PMTRenumbering::simToReal[59] = 23;
  LUXSim1_0PMTRenumbering::simToReal[60] = 24;
  LUXSim1_0PMTRenumbering::simToReal[61] = 31;    //end of top 61 PMTs
  LUXSim1_0PMTRenumbering::simToReal[62] = 61;    //beginning of bottom 61 PMTs
  LUXSim1_0PMTRenumbering::simToReal[63] = 114;
  LUXSim1_0PMTRenumbering::simToReal[64] = 113;
  LUXSim1_0PMTRenumbering::simToReal[65] = 112;
  LUXSim1_0PMTRenumbering::simToReal[66] = 111;
  LUXSim1_0PMTRenumbering::simToReal[67] = 62;
  LUXSim1_0PMTRenumbering::simToReal[68] = 65;
  LUXSim1_0PMTRenumbering::simToReal[69] = 117;
  LUXSim1_0PMTRenumbering::simToReal[70] = 116;
  LUXSim1_0PMTRenumbering::simToReal[71] = 115;
  LUXSim1_0PMTRenumbering::simToReal[72] = 104;
  LUXSim1_0PMTRenumbering::simToReal[73] = 63;
  LUXSim1_0PMTRenumbering::simToReal[74] = 66;
  LUXSim1_0PMTRenumbering::simToReal[75] = 68;
  LUXSim1_0PMTRenumbering::simToReal[76] = 119;
  LUXSim1_0PMTRenumbering::simToReal[77] = 118;
  LUXSim1_0PMTRenumbering::simToReal[78] = 107;
  LUXSim1_0PMTRenumbering::simToReal[79] = 103;
  LUXSim1_0PMTRenumbering::simToReal[80] = 64;
  LUXSim1_0PMTRenumbering::simToReal[81] = 67;
  LUXSim1_0PMTRenumbering::simToReal[82] = 69;
  LUXSim1_0PMTRenumbering::simToReal[83] = 70;
  LUXSim1_0PMTRenumbering::simToReal[84] = 120;
  LUXSim1_0PMTRenumbering::simToReal[85] = 109;
  LUXSim1_0PMTRenumbering::simToReal[86] = 106;
  LUXSim1_0PMTRenumbering::simToReal[87] = 102;
  LUXSim1_0PMTRenumbering::simToReal[88] = 71;
  LUXSim1_0PMTRenumbering::simToReal[89] = 75;
  LUXSim1_0PMTRenumbering::simToReal[90] = 78;
  LUXSim1_0PMTRenumbering::simToReal[91] = 80;
  LUXSim1_0PMTRenumbering::simToReal[92] = 122;
  LUXSim1_0PMTRenumbering::simToReal[93] = 110;
  LUXSim1_0PMTRenumbering::simToReal[94] = 108;
  LUXSim1_0PMTRenumbering::simToReal[95] = 105;
  LUXSim1_0PMTRenumbering::simToReal[96] = 101;
  LUXSim1_0PMTRenumbering::simToReal[97] = 72;
  LUXSim1_0PMTRenumbering::simToReal[98] = 76;
  LUXSim1_0PMTRenumbering::simToReal[99] = 79;
  LUXSim1_0PMTRenumbering::simToReal[100] = 90;
  LUXSim1_0PMTRenumbering::simToReal[101] = 100;
  LUXSim1_0PMTRenumbering::simToReal[102] = 99;
  LUXSim1_0PMTRenumbering::simToReal[103] = 97;
  LUXSim1_0PMTRenumbering::simToReal[104] = 94;
  LUXSim1_0PMTRenumbering::simToReal[105] = 73;
  LUXSim1_0PMTRenumbering::simToReal[106] = 77;
  LUXSim1_0PMTRenumbering::simToReal[107] = 88;
  LUXSim1_0PMTRenumbering::simToReal[108] = 89;
  LUXSim1_0PMTRenumbering::simToReal[109] = 98;
  LUXSim1_0PMTRenumbering::simToReal[110] = 96;
  LUXSim1_0PMTRenumbering::simToReal[111] = 93;
  LUXSim1_0PMTRenumbering::simToReal[112] = 74;
  LUXSim1_0PMTRenumbering::simToReal[113] = 85;
  LUXSim1_0PMTRenumbering::simToReal[114] = 86;
  LUXSim1_0PMTRenumbering::simToReal[115] = 87;
  LUXSim1_0PMTRenumbering::simToReal[116] = 95;
  LUXSim1_0PMTRenumbering::simToReal[117] = 92;
  LUXSim1_0PMTRenumbering::simToReal[118] = 81;
  LUXSim1_0PMTRenumbering::simToReal[119] = 82;
  LUXSim1_0PMTRenumbering::simToReal[120] = 83;
  LUXSim1_0PMTRenumbering::simToReal[121] = 84;
  LUXSim1_0PMTRenumbering::simToReal[122] = 91;

  //Reverse this linkage
  for(int i=0; i < 123; i++)
    realToSim[i] = 0;

  for(int i=1; i < 123; i++) {
    if(LUXSim1_0PMTRenumbering::simToReal[i] > 0 && LUXSim1_0PMTRenumbering::simToReal[i] < 123)
      LUXSim1_0PMTRenumbering::realToSim[LUXSim1_0PMTRenumbering::simToReal[i]] = i;
  }

}

int LUXSim1_0PMTRenumbering::GetRealFromOldSim(int simPMT) {
  if(simPMT > 0 && simPMT < 123)
    return simToReal[simPMT];
  else
    return 0;
}

int LUXSim1_0PMTRenumbering::GetOldSimFromReal(int realPMT) {
  if(realPMT > 0 && realPMT < 123)
    return realToSim[realPMT];
  else
    return 0;
}

bool LUXSim1_0PMTRenumbering::IsTop(int realPMT) {
  if((realPMT >= 1 && realPMT <= 60) || realPMT == 121)
    return true;
  else
    return false;
}

bool LUXSim1_0PMTRenumbering::IsBottom(int realPMT) {
  if((realPMT >= 61 && realPMT <= 120) || realPMT == 122)
    return true;
  else
    return false;
}

int LUXSim1_0PMTRenumbering::GetTopArrayIndexFromReal(int realPMT, 
						      bool useOldSim) {
  //If useOldSim = true, this returns the array index for the 
  //*old* numbering scheme.  If false (the default), it returns
  //for the *new*, reality-matching scheme
  //Returns -1 if not a Top PMT
  if(realPMT >= 1 && realPMT <= 60) {
    if(useOldSim) {
      return GetOldSimFromReal(realPMT)-1;
    } else {
      return realPMT - 1;
    }
  }

  if(realPMT == 121) {
    if(useOldSim) {
      return GetOldSimFromReal(realPMT)-1;
    } else {
      return 60;
    }
  }

  return -1;
  
}

int LUXSim1_0PMTRenumbering::GetBottomArrayIndexFromReal(int realPMT, 
							 bool useOldSim) {
  //If useOldSim = true, this returns the array index for the 
  //*old* numbering scheme.  If false (the default), it returns
  //for the *new*, reality-matching scheme
  //Returns -1 if not a Bottom PMT
  if(realPMT >= 61 && realPMT <= 120) {
    if(useOldSim) {
      return GetOldSimFromReal(realPMT)-62;
    } else {
      return realPMT - 61;
    }
  }

  if(realPMT == 122) {
    if(useOldSim) {
      return GetOldSimFromReal(realPMT)-62;
    } else {
      return 60;
    }
  }

  return -1;
}
