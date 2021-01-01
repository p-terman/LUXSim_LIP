#!/usr/bin/env bash
################################################################################
# Shell script to run multiple photonBombMultiRun.mac macros with varying
# bomb positions to simulate S1 and S2 light.
# 
#
# Change log:
#
#  7-Jan-2010 - Initial submission (Michael)
# 12-Jan-2010 - Further documentation & rewrite for new directory
#                   /tools/photonBomb.sh instead of the /LUXSimMacros.
#  2-Mar-2010 - Fixed bug with "exit" not being put at end of macro
#                   if it was in the source macro.
#
################################################################################

# === Instructions ===
# 
# From base LUXSim dir run the shell script ./tools/photonBomb.sh
# 
# If you run "./tools/photonBomb.sh --print" then LUXSim will not
# run and instead only the (x,y) coords of each bomb will be printed to screen.
#
# Spacing is controlled by the resolution variable that sets the space in cm
# between photon bombs.
#
# The macros executed are called photonBombMultiRun.mac and require a
# photonBomb.mac to be build from. Any lines about beamOn or the position of
# the photon bomb are ignored from photonBomb.mac and specially appeneded to
# the photonBombMultiRun file using function bombXY ().


bombXY ()
{
    echo "#"                                     > ./LUXSimMacros/photonBombMultiRun.mac
    echo "# Macro created with photonBomb.sh"   >> ./LUXSimMacros/photonBombMultiRun.mac
    echo "#"                                    >> ./LUXSimMacros/photonBombMultiRun.mac

    cat ./LUXSimMacros/photonBomb.mac | grep -v '^LUXSim/beamOn' \
            | grep -v '^pos/centre' | grep -v '^exit'\
                                                >> ./LUXSimMacros/photonBombMultiRun.mac

    echo "/gps/pos/centre $1 $2 15.95 cm"       >> ./LUXSimMacros/photonBombMultiRun.mac
    echo "/LUXSim/beamOn 1"                     >> ./LUXSimMacros/photonBombMultiRun.mac
    echo "exit"                                 >> ./LUXSimMacros/photonBombMultiRun.mac
}

touch ./tools/.photonBombLock
radius=$(echo "scale=10;49.33/2.0" | bc)
resolution=24               # In centimeters
npoints=$(echo "scale=0;$radius/$resolution" | bc)

echo " "
echo Radius of photon bomb circle is $radius cm
echo Resolution selected is $resolution cm between points
echo Number of points in +x direction is $npoints.
echo 


# Extra points in plus and minus directions are added... just in case.
for (( i=-npoints-1; i<=npoints+1; i++ ));do
    for (( j=-npoints-1; j<=npoints+1; j++ ));do
    # The following if statement can be used to break the script between
    # runs of LUXSim without having to unhappily break out of it. To do so,
    # delete the .photonBombLock file listed. The shell script will then
    # exit at the next loop.
	if test -a ./tools/.photonBombLock; then
		sleep .01
	else break
	fi

        x=$(echo "scale=7;$i*$resolution" | bc)
        y=$(echo "scale=7;$j*$resolution" | bc)

        check=$(echo "($x*$x+$y*$y)<=$radius*$radius" | bc)

        if [[ ${check} -eq '1' ]]; then
            # Function above that creates the macro.
            bombXY $x $y


            # Just for testing purposes, if you include an argument
            # when you call the script it will output the points used
            # to place photon bombs. "$#" is like argc, the number of
            # command line arguments.
            # pseudocode: if [num of args == 1] then echo (x,y)
            if [ $# -eq 1 ] ; then  
                echo \($x, $y\)
            else
                ./LUXSimExecutable ./LUXSimMacros/photonBombMultiRun.mac

            fi

            
        fi
    done
done

# Remove the Lock file in case the user didn't. Have to remember to be tidy.
if test-a ./tools/.photonBombLock; then
    rm ./tools/.photonBombLock
fi

