#include "LUXSim2evtNoiseGenerator.cc"
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <vector>

using namespace std;

/************************************************
*This is a simple macro that demonstrates the usage of the random noise generator function by plotting the noise
*waveform for a user-given time window and a set 10 ns time step. 
************************************************/
int main(int argc, char **argv){

	//Setup business
	double tFinal = .01, tStep = 10e-9;
	
	int N = (int)(tFinal/tStep);
	
	vector <double> waveform;

	// Generate baseline
	waveform = randNoiseGeneratorNoRoot(tFinal);

	// Write baseline sample to text file
	ofstream output;
	output.open("LUXSim2evtBaseline.hh");

	output << "double baseline_voltages [] = {";
	for(int i=0;i<N;i++){
		if(i != (N-1)) {
			output << waveform[i] << ",";
		}
		else {
			output << waveform[i] << "};" << endl;
		}
	}

	output.close();

	return 0;
}
