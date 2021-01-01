################################################################################
# photonBomb.py for the LUXSim package
#
# Change log:
#  2 Mar 2010 - Initial Submission (Michael Woods)
# 25 Mar 2010 - Fixed but that only printed integer values preventing sub-cm
#               resolution. Also add ability to specify resolution in as an arg
#               when the file is ran. 
#
################################################################################
#
# This python file will output to stdout a macro file that will place a general
# particle source (gps) at various locations in detector volume. Run with:
#     $ python photonBomb.py > myPhotonBomb.mac
# 
# Then you can run this macro in the usual way:
#     $ ./LUXSimExecutable ./LUXSimMacros/myPhotonBomb.mac
# 
# You should edit the header variable to adjust what the base macro file looks
# like. The default header when this file was checked out from the svn was the
# s2photonBomb.mac from LUXSim revision 117.
# 
# The use of this script is complementary to photonBomb.sh. After a Feburary
# 2010 change in LUXSim, multiple "/LUXSim/beamOn" calls create separate binary
# files. The shell script photonBomb.sh was not needed to call multiple
# instances of LUXSimExecutable to get multiple binary output files.
#
################################################################################

import sys
should_I_print_xy = False

if "--print" in sys.argv:
    should_I_print_xy = True
    sys.argv.remove("--print")

header = """

#
# This file created by photonBomb.py
#

/run/verbose 0
/control/verbose 0
/tracking/verbose 0
/grdm/verbose 0
/process/verbose 0

#/vis/open OGLIX
#/vis/drawVolume
#/vis/scene/add/trajectories
#/vis/scene/endOfEventAction accumulate
#/tracking/storeTrajectory 1
#/vis/viewer/set/viewpointVector 1 0 0 
#/vis/viewer/zoom 4


#   This next set of lines is to, as it appears, initialize the simulation
/run/initialize

#   Next are the LUXSim-specific commands

#   Set the output directory
/LUXSim/io/outputDir ./

#   Select the kind of detector to use
#/LUXSim/detector/select 1_0Detector

#   Set the muon veto (a.k.a. the water shield) on or off
#/LUXSim/detector/muonVeto off

#   Set the cryostat stand on or off (not fully developed)
#/LUXSim/detector/cryoStand off

#   Set the level of grid wires:
# wires makes individual wires inside a grid frame (optical)
# plane places a thin sheet inside the grid frame rather than wires (background)
# off places nothing
/LUXSim/detector/gridWires on

# For the new changes, we specify to build the geometry here:
/LUXSim/detector/update


#   Select the record levels of the volumes. The default record level is 0.

#   These next commands demonstrate how you can set the record level of
#   all components with the same base name in one command, but then set the
#   record level of individual components on a finer scale.
/LUXSim/detector/recordLevelOptPhot PMT_Window 3

#   Set the event generation.

#   This can be used in place of the regular the event generators. Be sure you
#   know what you're doing if you're going to use commands like these."""


if len(sys.argv) == 1:
    sys.stderr.write("What kind of spacing between grid points do you want (cm)? ")
    resolution = input()
else:
    try:
        resolution = eval(sys.argv[-1])
    except:
        sys.stderr.write("Problem with input.")
        sys.exit()

radius = 49.33/2.0  # Detector diameter divided by two.

npoints = int(radius/resolution)    # Number of points to set up the grid.


if not should_I_print_xy:
    print header

for i in xrange(-npoints-1, npoints+1):
    for j in xrange(-npoints-1, npoints+1):
        x = i * resolution
        y = j * resolution


        if x**2 + y**2 <= radius**2:
            # Just for testing purposes, if you include an argument
            # when you call the script it will output the points used
            # to place photon bombs.
            if should_I_print_xy:
                print "(%f, %f)" % (x,y)
                continue
            print "/gps/particle opticalphoton"
            print "/gps/energy 7 eV"
            print "/gps/pos/type Volume"
            print "/gps/pos/shape Cylinder"
            print "/gps/pos/centre " + str(x) + " " + str(y) + " " + "151.5 mm"
            print "/gps/pos/radius 0.1 mm"
            print "/gps/pos/halfz 23.5 mm"
            print "/gps/ang/type iso"
            print "/LUXSim/beamOn 10000"
            print ""




if not should_I_print_xy:
    print "exit"
