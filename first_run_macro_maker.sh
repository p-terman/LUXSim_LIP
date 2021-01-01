charge=$1
seed=$2
nEvents=$3
cd /global/homes/p/pterman/LUXSim_LIP/sim_files/

echo -e "/tracking/verbose 0">> "RunOne-${charge}-${seed}.mac"
echo -e "/monopole/setup 0 -${charge} 105.66 MeV">> "RunOne-${charge}-${seed}.mac"
echo -e "/run/initialize">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/io/outputDir /global/project/projectdirs/lux/scratch/pterman/iso/">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/io/outputName  monopole-${charge}-">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/detector/select 1_0Detector">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/io/updateFrequency 1">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/detector/gridWires on">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/detector/cryoStand on">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/detector/muonVeto on">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/detector/topGridVoltage -1 kV">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/detector/anodeGridVoltage 3.5 kV">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/detector/gateGridVoltage -1.5 kV">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/detector/cathodeGridVoltage -10 kV">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/detector/bottomGridVoltage -2 kV">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/detector/printEFields">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/detector/LUXFastSimSkewGaussianS2 true">> "RunOne-${charge}-${seed}.mac"
echo -e "#/run/initialize">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/detector/update">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/materials/LXeAbsorption 0 m">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/materials/GXeAbsorption 0 m">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/detector/recordLevel LiquidXenon 2">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/detector/recordLevelThermElec PMT_PhotoCathode 3">> "RunOne-${charge}-${seed}.mac"
echo -e "#/LUXSim/materials/LXeAbsorption 0.1 m">> "RunOne-${charge}-${seed}.mac"
echo -e "#/LUXSim/materials/LXeTeflonRefl 1">> "RunOne-${charge}-${seed}.mac"
echo -e "/gps/particle monopole">> "RunOne-${charge}-${seed}.mac"
echo -e "/gps/energy 328 MeV">> "RunOne-${charge}-${seed}.mac"
echo -e "/gps/position 0 0 1 m">> "RunOne-${charge}-${seed}.mac"
echo -e "/gps/direction 0 0 -1">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/physicsList/useOpticalProcesses false">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/physicsList/driftElecAttenuation 1 m">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/randomSeed ${seed}">> "RunOne-${charge}-${seed}.mac"
echo -e "/LUXSim/beamOn ${nEvents}">> "RunOne-${charge}-${seed}.mac"
echo -e "exit">> "RunOne-${charge}-${seed}.mac"

