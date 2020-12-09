# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2020 ISIS Rutherford Appleton Laboratory UKRI,
#   NScD Oak Ridge National Laboratory, European Spallation Source,
#   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
# SPDX - License - Identifier: GPL - 3.0 +
from mantid.api import AlgorithmFactory, IMDEventWorkspaceProperty, IMDHistoWorkspaceProperty, IPeaksWorkspaceProperty,\
    PythonAlgorithm, PropertyMode
from mantid.kernel import Direction, EnabledWhenProperty, FloatArrayLengthValidator, FloatArrayProperty, \
    PropertyCriterion, IntArrayProperty
from mantid.simpleapi import BinMD, DeleteWorkspace, SetMDFrame, TransformHKL, mtd
import numpy as np


class ConvertQtoHKLHisto(PythonAlgorithm):

    _lattice = None

    def category(self):
        return "Crystal\\Creation"

    def seeAlso(self):
        return ["ConvertWANDSCDtoQ", "BinMD"]

    def name(self):
        return "ConvertQtoHKLHisto"

    def summary(self):
        return 'Converts a workspace from Q sample to a MDHisto in HKL; the UB matrix can be used from the input, ' \
               'or calculated from peaks found from the input.'

    def PyInit(self):
        self.declareProperty(IMDEventWorkspaceProperty("InputWorkspace", defaultValue="",
                                                       optional=PropertyMode.Mandatory,
                                                       direction=Direction.Input),
                             doc="Input MDEvent workspace to convert to a MDHisto in HKL")

        self.declareProperty("FindUBFromPeaks", True,
                             doc="Whether to find peaks and use them to compute the UB matrix.")

        self.declareProperty(IPeaksWorkspaceProperty("PeaksWorkspace", defaultValue="", optional=PropertyMode.Optional,
                                                     direction=Direction.Input),
                             doc="Peaks workspace containing peaks to integrate")

        self.declareProperty(FloatArrayProperty("Transformation", [],
                                                direction=Direction.Input),
                             "Optional HKL transformation matrix to apply to the peaks workspace, each value of the "
                             "matrix should be comma separated.")

        ub_settings = EnabledWhenProperty('FindUBFromPeaks', PropertyCriterion.IsDefault)
        self.setPropertySettings("PeaksWorkspace", ub_settings)
        self.setPropertySettings("Transformation", ub_settings)

        self.declareProperty(FloatArrayProperty("Uproj", [1, 0, 0], validator=FloatArrayLengthValidator(3),
                                                direction=Direction.Input),
                             doc="Defines the first projection vector of the target Q coordinate system in HKL mode")
        self.declareProperty(FloatArrayProperty("Vproj", [0, 1, 0], validator=FloatArrayLengthValidator(3),
                                                direction=Direction.Input),
                             doc="Defines the second projection vector of the target Q coordinate system in HKL mode")
        self.declareProperty(FloatArrayProperty("Wproj", [0, 0, 1], validator=FloatArrayLengthValidator(3),
                                                direction=Direction.Input),
                             doc="Defines the third projection vector of the target Q coordinate system in HKL mode")

        self.declareProperty(FloatArrayProperty("Extents", [-6.02, 6.02, -6.02, 6.02, -6.02, 6.02],
                                                direction=Direction.Input),
                             "Binning parameters for each dimension. Enter it as a"
                             "comma-separated list of values with the"
                             "format: 'minimum,maximum,'.")

        self.declareProperty(IntArrayProperty("Bins", [301, 301, 301],
                                              direction=Direction.Input),
                             "Number of bins to use for each dimension, Enter it as a"
                             "comma-separated list of integers.")

        self.declareProperty(IMDHistoWorkspaceProperty("OutputWorkspace", "",
                                                       optional=PropertyMode.Mandatory,
                                                       direction=Direction.Output),
                             doc="Output MDWorkspace in Q-space, name is prefix if multiple input files were provided.")

    def validateInputs(self):
        issues = dict()

        input_ws = self.getProperty("InputWorkspace").value

        if input_ws.getSpecialCoordinateSystem().name != "QSample":
            issues["InputWorkspace"] = "Input workspace expected to be in QSample, " \
                                       "workspace is in '{}'".format(input_ws.getSpecialCoordinateSystem().name)

        ndims = input_ws.getNumDims()
        if ndims != 3:
            issues["InputWorkspace"] = "Input workspace needs 3 dimensions, it has {} dimensions.".format(ndims)

        # Check that extents and bins were provided for every dimension
        extents = self.getProperty("Extents").value
        bins = self.getProperty("Bins").value

        if len(extents) != ndims * 2:
            issues["Extents"] = "Expected a min and max value for each " \
                                "dimension (got {}, expected {}).".format(len(extents), ndims*2)

        if len(bins) != ndims:
            issues["Bins"] = "Expected a number of bins for each dimension."

        if self.getProperty("FindUBFromPeaks").value:
            peak_ws = self.getProperty("PeaksWorkspace")
            if peak_ws.isDefault:
                issues["PeaksWorkspace"] = "A peaks workspace must be supplied."
            elif not mtd.doesExist(self.getPropertyValue("PeaksWorkspace")):
                issues["PeaksWorkspace"] = "Provided peaks workspace does not exist in the ADS."

            tmatrix = self.getProperty("Transformation")
            if not tmatrix.isDefault and len(tmatrix.value) != 9:
                issues["Transformation"] = "Invalid transformation matrix, there should be 9 values comma separated"
        else:
            # Check that the workspace has a UB matrix
            if not (input_ws.getNumExperimentInfo() > 0 and input_ws.getExperimentInfo(0).sample().hasOrientedLattice()):
                issues["InputWorkspace"] = "Could not find a UB matrix in this workspace."

        return issues

    def PyExec(self):
        input_ws = self.getProperty("InputWorkspace").value

        extents = self.getProperty("Extents").value
        bins = self.getProperty("Bins").value

        if self.getProperty("FindUBFromPeaks").value:
            peak_ws = self.getProperty("PeaksWorkspace").value

            if not self.getProperty("Transformation").isDefault:
                tmatrix = self.getProperty("Transformation").value

                # Apply transformation if specified
                TransformHKL(PeaksWorkspace=peak_ws, HKLTransform=tmatrix)

            self._lattice = peak_ws.sample().getOrientedLattice()
        else:
            self._lattice = input_ws.getExperimentInfo(0).sample().getOrientedLattice()

        # Get axis names and units from u,v,w projections, as done in ConvertWANDSCDtoQ:
        w = np.eye(3)
        w[:, 0] = self.getProperty("Uproj").value
        w[:, 1] = self.getProperty("Vproj").value
        w[:, 2] = self.getProperty("Wproj").value
        char_dict = {0: '0', 1: '{1}', -1: '-{1}'}
        chars = ['H', 'K', 'L']
        names = ['[' + ','.join(char_dict.get(j, '{0}{1}')
                                .format(j, chars[np.argmax(np.abs(w[:, i]))]) for j in w[:, i]) + ']' for i in range(3)]

        q = [self._lattice.qFromHKL(w[i]) for i in range(3)]

        units = ['in {:.3f} A^-1'.format(q[i].norm()) for i in range(3)]

        mdhist = BinMD(InputWorkspace=input_ws, AxisAligned=False, NormalizeBasisVectors=False,
                       BasisVector0='{},{},{},{},{}'.format(names[0], units[0], q[0].X(), q[0].Y(), q[0].Z()),
                       BasisVector1='{},{},{},{},{}'.format(names[1], units[1], q[1].X(), q[1].Y(), q[1].Z()),
                       BasisVector2='{},{},{},{},{}'.format(names[2], units[2], q[2].X(), q[2].Y(), q[2].Z()),
                       OutputExtents=extents,
                       OutputBins=bins)

        SetMDFrame(mdhist, MDFrame='HKL', Axes='0, 1, 2')

        self.setProperty("OutputWorkspace", mdhist)

        DeleteWorkspace(mdhist)


AlgorithmFactory.subscribe(ConvertQtoHKLHisto)
