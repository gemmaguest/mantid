""" 
Algorithm to mask detectors with scattering angles in a given interval (in degrees)
By default MinAngle=0, MaxAngle=180, so if no keywords are set, all detectors are going to be masked
Returns a list of detectors that were masked
"""

import mantid.simpleapi  
import mantid.kernel
import mantid.api
import numpy

class MaskAngle(mantid.api.PythonAlgorithm):
    """ Class to generate grouping file
    """

    def category(self):
        """ Mantid required
        """
        return "PythonAlgorithms;Transforms\\Masking"

    def name(self):
        """ Mantid require
        """
        return "MaskAngle"
    
    def PyInit(self):
        self.declareProperty(mantid.api.WorkspaceProperty("Workspace", "", direction=mantid.kernel.Direction.Input), "Input workspace")
        angleValidator=mantid.kernel.FloatBoundedValidator()
        angleValidator.setBounds(0.,180.)
        self.declareProperty(name="MinAngle", defaultValue=0.0, validator=angleValidator, direction=mantid.kernel.Direction.Input, doc="Angles above StartAngle are going to be masked")
        self.declareProperty(name="MaxAngle", defaultValue=0.0, validator=angleValidator, direction=mantid.kernel.Direction.Input, doc="Angles above StartAngle are going to be masked")
        self.declareProperty(mantid.kernel.IntArrayProperty(name="MaskedDetectors", direction=mantid.kernel.Direction.Output), doc="List of detector masked, with scatterin angles between MinAngle and MaxAngle")

    def PyExec(self):
        ws = self.getProperty("Workspace").value
        ttmin = self.getProperty("MinAngle").value
        ttmax = self.getProperty("MaxAngle").value
        if ttmin > ttmax :
            raise ValueError("MinAngle > MaxAngle, please check angle range for masking")

        detlist=[]


        numspec = ws.getNumberHistograms()
        for i in range(numspec):
            det=ws.getDetector(i)
            if not det.isMonitor():
                tt=numpy.degrees(det.getTwoTheta(mantid.kernel.V3D(0,0,0),mantid.kernel.V3D(0,0,1)))
                if tt>= ttmin and tt<= ttmax:
                    detlist.append(det.getID())

        if len(detlist)> 0:
            mantid.simpleapi.MaskDetectors(Workspace=ws,DetectorList=detlist)
        else:
            self.log().information("no detectors within this range")
        self.setProperty("MaskedDetectors", numpy.array(detlist))

mantid.api.AlgorithmFactory.subscribe(MaskAngle)
