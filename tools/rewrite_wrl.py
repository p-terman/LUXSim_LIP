#!/usr/bin/env python
docs = \
"""
rewrite_wrl.py

Rewrite .wrl files (plaintext VRML files) for use in typical viewers. This was
written originally because the best Mac OS viewer had very few commands to
control the representation of the visualization and I needed a way of making
some pretty LUXSim images and didn't have the patience for the ray tracer!

Usage:
  1) From this file itself:
        ./rewrite_wrl.py g4_00.wrl
    Creates g4_00_edit.wrl with the changes that are in rewrite_wrl.py at the
    very bttom of the file (the if __name__ section). Change these commands
    as you see fit.
  2) Create another python file and import this file. E.g. in my_write.py:
        import rewrite_wrl
        f = VRML_File("g4_00.wrl")
        f.set_transparency(Wire, 0)
        f.write("new_g4_00.wrl")
     Then execute "python my_write.py"
"""
# Log
#   Nov 10, 2013 - Initial submission (Mike)

import re

class VRML_Object:
  def __init__(self, text):
    """ Given the raw text string of the VRML object, parse out the name
    and category of the shape.
    """
    self.text = text

    # Category
    start_of_cat = 11 # Length of #----------
    self.category = self.text.split(":")[0][start_of_cat:]

    # Name
    firstline = text.split("\n", 1)[0]
    self.name = firstline.split(":")[-1].strip()

  def set_transparency(self, t):
    """ Use regex to replace an existing transp. line with a new one."""
    self.text = re.sub(r'(transparency)(.*\n)', r'\1 %f\n' % t, self.text)

  def set_color(self, r, g, b):
    """ Use regex to replace an existing color line with a new color."""
    self.text = re.sub(r'(diffuseColor)(.*\n)', r'\1 %d %d %d\n' % (r, g, b), self.text)

  def set_viewpoint(self, x, y, z):
    """ Use regex to replace an existing position line with a new set."""
    self.text = re.sub(r'(position)(.*\n)', r'\1 %d %d %d\n' % (x, y, z), self.text)

class VRML_File:
  def __init__(self, filename):
    """ Initialize with a .wrl filename to open."""
    self.input_filename = filename
    self.input_file = open(self.input_filename)
    self.raw_text = self.input_file.read()
    self.input_file.close()
    self.vrml_version, self.comment, bulk  = self.raw_text.split("\n", 2)
    self.raw_objects = ["#" + shape for shape in bulk.split("#") if shape.strip()]
    self.objects = [VRML_Object(raw_object) for raw_object in self.raw_objects]
    self.camera = self.objects.pop(0)

  def write(self, output_filename=None):
    """ Write the output .wrl file. With no arguments, the original filename
    will get _edit" appeneded to the filename but before the sufix.
    This loops through all shapes and writes their text to a file.
    """
    if not output_filename:
      output_filename = self.input_filename.replace(".wrl", "_edit.wrl")
    output_file = open(output_filename, "w")
    output_file.write(self.vrml_version + "\n")
    output_file.write(self.comment + "\n")
    output_file.write(self.camera.text)
    for vrml_object in self.objects:
      output_file.write(vrml_object.text)
    output_file.close()


  def set_viewpoint(self, x, y, z):
    """ Set the initial viewpoint of the camera. Not sure what to use..."""
    self.camera.set_viewpoint(x, y, z)

  def set_transparency(self, name, t, partial_match=True):
    """ Set the transparency of all shapes that have the name provided. 
    The "partial_match" arg can be used to set the transp. of many entries when
    you providea substring of the volume name. E.g. name = "PTFE" will set
      PTFESheets.0
      PTFESourcePanels_01.0
      PTFESourcePanels_02.0
      etc.
    Transparency of 0 is opaque, 1 is completely transparent.
    """
    if not partial_match:
      for vrml_object in self.objects:
        if name==vrml_object.name:
          vrml_object.set_transparency(t)
    else:
      for vrml_object in self.objects:
        if name in vrml_object.name:
          vrml_object.set_transparency(t)

  def set_color(self, name, r, g, b, partial_match=True):
    """ Set the color of all shapes given that have the name provided. 
    The "partial_match" arg can be used to set the color of many entries when 
    you providea substring of the volume name. E.g. name = "PTFE" will set
      PTFESheets.0
      PTFESourcePanels_01.0
      PTFESourcePanels_02.0
      etc.
    Colors should be integers between 0 and 255.
    """
    if not partial_match:
      for vrml_object in self.objects:
        if name==vrml_object.name:
          vrml_object.set_color(r, g, b)
    else:
      for vrml_object in self.objects:
        if name in vrml_object.name:
          vrml_object.set_color(r, g, b)

  def remove(self, name, partial_match = True):
    """ Remove all shapes from the list of shapes have the name provided. 
    The "partial_match" arg can be used to remove entries when you provide
    a substring of the volume name. E.g. name = "PTFE" will remove
      PTFESheets.0
      PTFESourcePanels_01.0
      PTFESourcePanels_02.0
      etc.
    """

    if not partial_match:
      for vrml_object, raw_object in zip(self.objects, self.raw_objects):
        if name==vrml_object.name:
          self.objects.remove(vrml_object)
          self.raw_objects.remove(raw_object)
    else:
      for vrml_object, raw_object in zip(self.objects, self.raw_objects):
        if name in vrml_object.name:
          self.objects.remove(vrml_object)
          self.raw_objects.remove(raw_object)

  def set_field_of_view(self, deg):
    """ Set the field of view for the camera. This is given as some number of
    degrees between 0 and 180. That value, in radians, is written to the file.
    This is not normally in a Geant4 wrl file, so a check will add it if such
    is needed.
    """
    rad = deg*2*3.14159/360.
    if "fieldOfView" in self.camera.text:
      self.camera.text = re.sub(r'(fieldOfView)(.*\n)', r'\1 %f\n' % (rad), self.text)
    else:
      new_line = "\t" + "fieldOfView %f" % rad
      self.camera.text = self.camera.text.replace("}", new_line + "\n" + "}")
      

if __name__ == "__main__":
  import sys
  if len(sys.argv) == 1 or len(sys.argv) > 2:
    print docs
    sys.exit(0)
  vrml = VRML_File(sys.argv[1])

  # Zoom in a bit.
  vrml.set_viewpoint(0, 0, 230)

  # An outter volume that we don't need.
  vrml.remove("EncompassingVolume.0")

  # Can speed things up.
  #vrml.remove("Wire")

  # These are the volumes the wires are placed in. They are ugly.
  vrml.remove("LiquidHolderXenon")
  vrml.remove("GasHolderXenon")
  vrml.remove("BottomReflector")

  vrml.set_transparency("BottomPMTHolder", 0)
  vrml.set_transparency("Bottom_PMT_Can", 0)
  vrml.set_transparency("PTFE", 0)
  vrml.set_field_of_view(100)

  vrml.write()



