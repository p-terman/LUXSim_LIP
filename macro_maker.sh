
#for ((l=0.06; l<0.1;l+=0.005))
for l in $(seq 0.45 0.05 0.95)
do
rm ./RunOne-$l.mac

echo -e "/tracking/verbose 0">> "RunOne-$l.mac"
echo -e "/monopole/setup 0 -$l 105.66 MeV">> "RunOne-$l.mac"
echo -e "/run/initialize">> "RunOne-$l.mac"
echo -e "/LUXSim/io/outputDir /global/project/projectdirs/lux/scratch/pterman/${l}/">> "RunOne-$l.mac"
echo -e "/LUXSim/io/outputName  monopole">> "RunOne-$l.mac"
echo -e "/LUXSim/detector/select 1_0Detector">> "RunOne-$l.mac"
echo -e "/LUXSim/io/updateFrequency 1">> "RunOne-$l.mac"
echo -e "/LUXSim/detector/gridWires on">> "RunOne-$l.mac"
echo -e "/LUXSim/detector/cryoStand on">> "RunOne-$l.mac"
echo -e "/LUXSim/detector/muonVeto on">> "RunOne-$l.mac"
echo -e "/LUXSim/detector/topGridVoltage -1 kV">> "RunOne-$l.mac"
echo -e "/LUXSim/detector/anodeGridVoltage 3.5 kV">> "RunOne-$l.mac"
echo -e "/LUXSim/detector/gateGridVoltage -1.5 kV">> "RunOne-$l.mac"
echo -e "/LUXSim/detector/cathodeGridVoltage -10 kV">> "RunOne-$l.mac"
echo -e "/LUXSim/detector/bottomGridVoltage -2 kV">> "RunOne-$l.mac"
echo -e "/LUXSim/detector/printEFields">> "RunOne-$l.mac"
echo -e "/LUXSim/detector/LUXFastSimSkewGaussianS2 true">> "RunOne-$l.mac"
echo -e "#/run/initialize">> "RunOne-$l.mac"
echo -e "/LUXSim/detector/update">> "RunOne-$l.mac"
echo -e "/LUXSim/materials/LXeAbsorption 0 m">> "RunOne-$l.mac"
echo -e "/LUXSim/materials/GXeAbsorption 0 m">> "RunOne-$l.mac"
echo -e "/LUXSim/detector/recordLevel LiquidXenon 2">> "RunOne-$l.mac"
echo -e "/LUXSim/detector/recordLevelThermElec PMT_PhotoCathode 3">> "RunOne-$l.mac"
echo -e "#/LUXSim/materials/LXeAbsorption 0.1 m">> "RunOne-$l.mac"
echo -e "#/LUXSim/materials/LXeTeflonRefl 1">> "RunOne-$l.mac"
echo -e "/gps/particle monopole">> "RunOne-$l.mac"
echo -e "/gps/energy 10  GeV">> "RunOne-$l.mac"
echo -e "/gps/position 0 0 1 m">> "RunOne-$l.mac"
echo -e "/gps/direction 0 0 -1">> "RunOne-$l.mac"
echo -e "/LUXSim/physicsList/useOpticalProcesses true">> "RunOne-$l.mac"
echo -e "/LUXSim/physicsList/s1gain 0.117">> "RunOne-$l.mac"
echo -e "/LUXSim/physicsList/s2gain 0.489">> "RunOne-$l.mac"
echo -e "/LUXSim/physicsList/driftElecAttenuation 1 m">> "RunOne-$l.mac"
echo -e "/LUXSim/beamOn 100">> "RunOne-$l.mac"
echo -e "exit">> "RunOne-$l.mac"

done

rm ~/LUXSim_LIP/SubmitAllRunOneJobs.sh

#Begin Job Submission Script 

#for ((l=0.06; l<0.1;l+=0.005))
for l in $(seq 0.45 0.05 0.95)
do

rm ./SubmitRunOne-$l.sh
echo -e "#!/bin/bash\n"  >> "SubmitRunOne-$l.sh"

echo -e "module load CLHEP ;"  >> "SubmitRunOne-$l.sh"
echo -e "module load Geant4 ;"  >> "SubmitRunOne-$l.sh" 
echo -e "module load ROOT ;"  >> "SubmitRunOne-$l.sh"
echo -e "mkdir /global/project/projectdirs/lux/scratch/pterman/${l}/" >> "SubmitRunOne-$l.sh"
echo -e "cd /global/u2/p/pterman/LUXSim_LIP\n"  >> "SubmitRunOne-$l.sh"

#run LUXSim
echo -e "/global/u2/p/pterman/LUXSim_LIP/LUXSimExecutable /global/u2/p/pterman/LUXSim_LIP/RunOne-$l.mac\n" >> "SubmitRunOne-$l.sh"
#convert to evt
echo -e "cd  /global/project/projectdirs/lux/scratch/pterman/${l}/" >> "SubmitRunOne-$l.sh"
echo -e "BINFILE=*.bin" >> "SubmitRunOne-$l.sh"
echo -e '/global/u2/p/pterman/LUXSim_LIP/tools/LUXSim2evt/LUXSim2evt ./$BINFILE\n' >> "SubmitRunOne-$l.sh"


#run matlab
echo -e "EVTFILE=*.evt" >> "SubmitRunOne-$l.sh"
echo -e 'for f in $EVTFILE' >> "SubmitRunOne-$l.sh"
echo -e "do" >> "SubmitRunOne-$l.sh"
echo -e 'foldername="${f%_*}"' >> "SubmitRunOne-$l.sh"
echo -e 'mkdir /global/project/projectdirs/lux/scratch/pterman/${foldername}/' >> "SubmitRunOne-$l.sh"
echo -e 'mv ./$EVTFILE /global/project/projectdirs/lux/scratch/pterman/${foldername}/' >> "SubmitRunOne-$l.sh"
echo -e "done\n" >> "SubmitRunOne-$l.sh"
echo -e "module load matlab ;" >> "SubmitRunOne-$l.sh"
echo -e "cd" >> "SubmitRunOne-$l.sh" 

#This last line here generates a single file with all the necessary qsub commands. 
echo -e "./SubmitRunOne-$l.sh" >> "SubmitAllRunOneJobs.sh"


done

