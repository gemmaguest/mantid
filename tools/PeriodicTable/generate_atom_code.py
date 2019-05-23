#!/usr/bin/env python
# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
#     NScD Oak Ridge National Laboratory, European Spallation Source
#     & Institut Laue - Langevin
# SPDX - License - Identifier: GPL - 3.0 +
#pylint: disable=invalid-name
from __future__ import (absolute_import, division, print_function)
from six.moves import range
import optparse
import sys
try:
    import periodictable
    if not periodictable.__version__.startswith("1.3."):
        print("*****Require periodictable 1.3.*")
        sys.exit(-1)
except ImportError as e:
    print("*****To use this you must 'easy_install periodictable'")
    sys.exit(-1)

VERSION = "1.0"

# elements not to put in the output file
BANNED = ['n', 'Bk', 'Cf', 'Es', 'Fm', 'Md', 'No', 'Lr', 'Rf', 'Db', 'Sg',
          'Bh', 'Hs', 'Mt', 'Ds', 'Rg', 'Cn', 'Uuq', 'Uuh']

DELIMITOR_START = "// ---------- START DO NOT EDIT AREA----------"
DELIMITOR_STOP = "// ---------- END DO NOT EDIT AREA----------"


def writeBegin(handle, label, symbol, z, a=0):
    handle.write("static const Atom %s(\"%s\", %d, %d, " % (label, symbol, z, a))


def writeMiddle(handle, abundance, atom):
    mass = atom.mass
    try:
        density = atom.density
        if density is None:
            density = "NAN"
        else:
            density = str(density)
    except TypeError:
        density = "NAN"
    handle.write("%f, %f, %s" % (abundance, mass, density))


def writeEnd(handle):
    handle.write(");\n")


def writeIsotope(handle, element, atomicnumber):
    isotope = element._isotopes[atomicnumber]
    label = "%s%d" % (element.symbol, atomicnumber)
    writeBegin(handle, label, element.symbol, element.number, atomicnumber)
    writeMiddle(handle, isotope.abundance, isotope)
    writeEnd(handle)
    return label


def writeElement(handle, element):
    label = element.symbol
    if label in BANNED:
        return []
    labels = [label]
    # Write the isotopic average first
    writeBegin(handle, label, element.symbol, element.number)
    writeMiddle(handle, 0., element)
    writeEnd(handle)
    # Now all isotopes ordered by A
    for key in sorted(element._isotopes.keys()):
        labels.append(writeIsotope(handle, element, key))
    return labels


def readExisting(filename):
    # open the file and scoop up everyting
    handle = open(filename, 'r')
    lines = handle.readlines()
    handle.close()

    # remove whitespace from lines to make matching easier
    lines_copy = [line.strip() for line in lines]

    # determine where the autogenerated stuff lives
    index_start = lines_copy.index(DELIMITOR_START.strip())
    index_stop = lines_copy.index(DELIMITOR_STOP.strip())

    # return the results
    before = "".join(lines[:index_start])
    after = "".join(lines[index_stop+1:])
    return (before, after)


if __name__ == "__main__":
    # command line options
    info = []
    info.append("Generate a source file (.cpp) with the supplied filename. The")
    info.append("source file will contain all of the information necessary for")
    info.append("the atoms and their isotopes.")
    parser = optparse.OptionParser("usage: %prog [options] <filename>",
                                   None, optparse.Option, VERSION, 'error',
                                   " ".join(info))
    (options, args) = parser.parse_args()

    # get the name of the output file
    if len(args) != 1:
        parser.error("Must supply one filename to write the results to.")
    filename = args[0]
    if not filename.endswith(".cpp"):
        filename = filename + ".cpp"

    # read in an existing file
    (code_before, code_after) = readExisting(filename)

    # setup the file for the elements and isotopes
    print("writing information to %s" % filename)
    handle = open(filename, 'w')
    handle.write(code_before) # write out what was before the delimitor
    handle.write(DELIMITOR_START+"\n")
    handle.write("/// @cond\n")

    # write the elements and isotopes
    atomNames = []
    spanTable = []
    for element in periodictable.elements:
        if element.symbol not in BANNED:
            spanTable.append([len(atomNames),len(element._isotopes.keys())+1])
        atomNames.extend(writeElement(handle, element))
    handle.write("/// @endcond\n")
    handle.write("\n")
    # write an array of all atoms and elements
    handle.write("// All of the atoms in a single array so it can be searched.\n")
    handle.write("// getAtom() expects them to be sorted first by Z number then by A number.\n")
    numAtoms = len(atomNames)
    handle.write("static std::array<Atom, %d> ATOMS_ARRAY = {{\n" % numAtoms)
    for i in range(0, numAtoms, 10):
        handle.write(", ".join(atomNames[i:i+10]))
        if i+10 < numAtoms:
            handle.write(",")
        handle.write("\n")
    handle.write("}};\n")
    handle.write("\n")

    handle.write("static std::array<tcb::span<Atom>,%d> ATOMS = {{\n" % len(spanTable))
    for i, elem in enumerate(spanTable):
        handle.write("tcb::span<Atom>(&ATOMS_ARRAY[%d], %d)" % (elem[0], elem[1]))
        if i + 1 < len(spanTable):
            handle.write(",")
        handle.write("\n")
    handle.write("}};\n")
    handle.write(DELIMITOR_STOP+"\n")

    # write out what was after the delimitor
    handle.write(code_after)

    # close the file
    handle.close()
