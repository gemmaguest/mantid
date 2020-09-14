# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
#   NScD Oak Ridge National Laboratory, European Spallation Source,
#   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
# SPDX - License - Identifier: GPL - 3.0 +
from mantid.api import (ADSValidator, AlgorithmFactory, DataProcessorAlgorithm,
                        IEventWorkspace,
                        MatrixWorkspaceProperty, PropertyMode)
from mantid.dataobjects import MaskWorkspaceProperty
from mantid.simpleapi import (AnalysisDataService, ConvertSpectrumAxis, ConjoinWorkspaces, Transpose,
                              ResampleX, CopyInstrumentParameters,
                              Divide, DeleteWorkspace, Scale,
                              MaskAngle, ExtractMask, Minus,
                              ExtractUnmaskedSpectra, mtd,
                              BinaryOperateMasks, Integration)
from mantid.kernel import StringListValidator, Direction, Property, FloatBoundedValidator


class WANDPowderReduction(DataProcessorAlgorithm):
    temp_workspace_list = ['__data_tmp', '__cal_tmp', '__mask_tmp', '__bkg_tmp']

    def category(self):
        return "Diffraction\\Reduction"

    def seeAlso(self):
        return ['LoadWAND','SaveFocusedXYE']

    def summary(self):
        return 'Performs powder diffraction data reduction for WAND'

    def name(self):
        return "WANDPowderReduction"

    def validateInputs(self):
        issues = dict()
        return issues

    def PyInit(self):

        # self.declareProperty(MatrixWorkspaceProperty("InputWorkspace", '',
        #                                              direction=Direction.Input),
        #                      doc='The main input workspace.')
        self.declareProperty(StringArrayProperty(
                                "InputWorkspace",
                                direction=Direction.Input,
                                validator=ADSValidator(),
                                ),
                            doc='The main input workspace[s].',
                        )

        # self.declareProperty(MatrixWorkspaceProperty("BackgroundWorkspace", '',
        #                                              optional=PropertyMode.Optional,
        #                                              direction=Direction.Input),
        #                      doc='The background workspace to be subtracted.')
        self.declareProperty(StringArrayProperty(
                                "BackgroundWorkspace",
                                optional=PropertyMode.Optional,
                                direction=Direction.Input,
                                validator=ADSValidator(),
                                ),
                             doc='The background workspace[s] to be subtracted.',
                        )

        self.declareProperty(MatrixWorkspaceProperty("CalibrationWorkspace", '',
                                                     optional=PropertyMode.Optional,
                                                     direction=Direction.Input),
                             doc='The calibration (vandiaum) workspace.')

        self.declareProperty("BackgroundScale", 1.0,
                             validator=FloatBoundedValidator(0.0),
                             doc="The background will be scaled by this number before being subtracted.")

        self.declareProperty(MaskWorkspaceProperty("MaskWorkspace", '',
                                                   optional=PropertyMode.Optional,
                                                   direction=Direction.Input),
                             doc='The mask from this workspace will be applied before reduction')

        self.copyProperties('ConvertSpectrumAxis', ['Target', 'EFixed'])

        self.copyProperties('ResampleX', ['XMin', 'XMax', 'NumberBins', 'LogBinning'])

        self.declareProperty('NormaliseBy', 'Monitor', StringListValidator(['None', 'Time', 'Monitor']), "Normalise to monitor or time. ")

        self.declareProperty('MaskAngle', Property.EMPTY_DBL,
                             "Phi angle above which will be masked. See :ref:`MaskAngle <algm-MaskAngle>` for details.")

        self.declareProperty(MatrixWorkspaceProperty("OutputWorkspace", "",
                                                     direction=Direction.Output),
                             doc="Output Workspace")

    def PyExec(self):
        data = self.getProperty("InputWorkspace").value             # [1~n]
        bkg = self.getProperty("BackgroundWorkspace").value         # [1~n]
        cal = self.getProperty("CalibrationWorkspace").value        # [1]
        mask = self.getProperty("MaskWorkspace").value              # [1]
        target = self.getProperty("Target").value
        eFixed = self.getProperty("EFixed").value
        xMin = self.getProperty("XMin").value
        xMax = self.getProperty("XMax").value
        numberBins = self.getProperty("NumberBins").value
        normaliseBy = self.getProperty("NormaliseBy").value
        maskAngle = self.getProperty("MaskAngle").value
        outWS = self.getPropertyValue("OutputWorkspace")

        _get_scale = lambda x : {
                None: 1,
                "Monitor": x.run().getProtonCharge(),
                "Time":    x.run().getLogData("duration").value,
            }[normaliseBy]

        _xMin = 1e16
        _xMax = -1e16
        for n, _wsn in enumerate(data):
            temp_workspace_list.append(f"__data_tmp_{n}")
            _ws = AnalysisDataService.retrieve(_wsn)

            Scale(InputWorkspace=_ws, OutputWorkspace=_ws, Factor=_get_scale(cal)/_get_scale(_ws), EnableLogging=False)

            ExtractMask(_ws, OutputWorkspace="__mask_tmp", EnableLogging=False)
        if maskAngle != Property.EMPTY_DBL:
                MaskAngle(Workspace="__mask_tmp", MinAngle=maskAngle, Angle="Phi", EnableLogging=False)

        if mask is not None:
                BinaryOperateMasks(InputWorkspace1="__mask_tmp", InputWorkspace2=mask,
                               OperationType="OR", OutputWorkspace=f"__mask_tmp", EnableLogging=False)

            ExtractUnmaskedSpectra(InputWorkspace=_ws, MaskWorkspace="__mask_tmp", OutputWorkspace=f"__data_tmp_{n}", EnableLogging=False)

            if isinstance(mtd[f"__data_tmp_{n}"], IEventWorkspace):
                Integration(InputWorkspace=f"__data_tmp_{n}", OutputWorkspace=f"__data_tmp_{n}", EnableLogging=False)

            ConvertSpectrumAxis(InputWorkspace=f"__data_tmp_{n}", Target=target, EFixed=eFixed, OutputWorkspace=f"__data_tmp_{n}", EnableLogging=False)
            Transpose(InputWorkspace=f"__data_tmp_{n}", OutputWorkspace=f"__data_tmp_{n}", EnableLogging=False)

            _ws = AnalysisDataService.retrieve(f"__data_tmp_{n}")

            _xMin, _xMax = min(_xMin, _ws.readX(0).min()), max(_xMax, _ws.readX(0).max())
        
        xMin = _xMin if xMin is None else xMin
        xMax = _xMax if xMax is None else xMax

        _data_tmp_list = [me for me in temp_workspace_list if '__data_tmp_' in me]
        for i, _wsn in enumerate(_data_tmp_list):
            ResampleX(InputWorkspace=_wsn, OutputWorkspace=_wsn, XMin=xMin, XMax=xMax, NumberBins=numberBins, EnableLogging=False)

        if bkg is not None:
            for n, bgn in enumerate(bkg):
                temp_workspace_list.append(f'__bkg_tmp_{n}')
                ExtractUnmaskedSpectra(InputWorkspace=bkg, MaskWorkspace='__mask_tmp', OutputWorkspace=f'__bkg_tmp_{n}', EnableLogging=False)
                if isinstance(mtd[f'__bkg_tmp_{n}'], IEventWorkspace):
                    Integration(InputWorkspace=f'__bkg_tmp_{n}', OutputWorkspace=f'__bkg_tmp_{n}', EnableLogging=False)
                CopyInstrumentParameters(data, f'__bkg_tmp_{n}', EnableLogging=False)
                ConvertSpectrumAxis(InputWorkspace=f'__bkg_tmp_{n}', Target=target, EFixed=eFixed, OutputWorkspace=f'__bkg_tmp_{n}', EnableLogging=False)
                Transpose(InputWorkspace=f'__bkg_tmp_{n}', OutputWorkspace=f'__bkg_tmp_{n}', EnableLogging=False)
                ResampleX(InputWorkspace=f'__bkg_tmp_{n}', OutputWorkspace=f'__bkg_tmp_{n}', XMin=xMin, XMax=xMax, NumberBins=numberBins, EnableLogging=False)
                Scale(InputWorkspace=f'__bkg_tmp_{n}', OutputWorkspace=f'__bkg_tmp_{n}', Factor=_get_scale(cal)/_get_scale(bkg), EnableLogging=False)
                Scale(InputWorkspace=f'__bkg_tmp_{n}', OutputWorkspace=f'__bkg_tmp_{n}', Factor=self.getProperty('BackgroundScale').value, EnableLogging=False)

        if cal is not None:
            ExtractUnmaskedSpectra(InputWorkspace=cal, MaskWorkspace='__mask_tmp', OutputWorkspace='__cal_tmp', EnableLogging=False)
            if isinstance(mtd['__cal_tmp'], IEventWorkspace):
                Integration(InputWorkspace='__cal_tmp', OutputWorkspace='__cal_tmp', EnableLogging=False)
            CopyInstrumentParameters(data, '__cal_tmp', EnableLogging=False)
            ConvertSpectrumAxis(InputWorkspace='__cal_tmp', Target=target, EFixed=eFixed, OutputWorkspace='__cal_tmp', EnableLogging=False)
            Transpose(InputWorkspace='__cal_tmp', OutputWorkspace='__cal_tmp', EnableLogging=False)
            ResampleX(InputWorkspace='__cal_tmp', OutputWorkspace='__cal_tmp', XMin=xMin, XMax=xMax, NumberBins=numberBins, EnableLogging=False)

            Divide(LHSWorkspace='__data_tmp_0', RHSWorkspace='__cal_tmp', OutputWorkspace=outWS, EnableLogging=False)
            if bkg is not None:
                Minus(LHSWorkspace=outWS, RHSWorkspace='__bkg_tmp_0', OutputWorkspace=outWS, EnableLogging=False)
        else:
            CloneWorkspace(InputWorkspace="__data_tmp_0", OutputWorkspace=outWS)

        for i, _wsn in enumerate(_data_tmp_list[1:]):
            if cal is not None:
                Divide(LHSWorkspace=_wsn, RHSWorkspace='__cal_tmp', OutputWorkspace=_wsn, EnableLogging=False)
        if bkg is not None:
                Minus(LHSWorkspace=_wsn, RHSWorkspace='__bkg_tmp_0', OutputWorkspace=_wsn, EnableLogging=False)
            ConjoinWorkspaces(InputWorkspace1=outWS,
                              InputWorkspace2=_wsn,
                              CheckOverlapping=False,
                            )

        SumSpectra(InputWorkspace=outWS, OutputWorkspace=outWS, WeightedSum=True, MultiplyBySpectra=False, StoreInADS=False)

        self.setProperty("OutputWorkspace", outWS)

        # remove temp workspaces
        [DeleteWorkspace(ws, EnableLogging=False) for ws in self.temp_workspace_list if mtd.doesExist(ws)]


AlgorithmFactory.subscribe(WANDPowderReduction)
