# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'diffraction_run_setup.ui'
#
# Created: Tue Feb 26 11:24:56 2013
#      by: PyQt4 UI code generator 4.9.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_Frame(object):
    def setupUi(self, Frame):
        Frame.setObjectName(_fromUtf8("Frame"))
        Frame.resize(949, 814)
        Frame.setFrameShape(QtGui.QFrame.StyledPanel)
        Frame.setFrameShadow(QtGui.QFrame.Raised)
        self.verticalLayout_3 = QtGui.QVBoxLayout(Frame)
        self.verticalLayout_3.setObjectName(_fromUtf8("verticalLayout_3"))
        self.frame = QtGui.QFrame(Frame)
        self.frame.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame.setFrameShadow(QtGui.QFrame.Raised)
        self.frame.setObjectName(_fromUtf8("frame"))
        self.label_4 = QtGui.QLabel(self.frame)
        self.label_4.setGeometry(QtCore.QRect(20, 20, 88, 31))
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.runnumbers_edit = QtGui.QLineEdit(self.frame)
        self.runnumbers_edit.setGeometry(QtCore.QRect(150, 20, 721, 27))
        self.runnumbers_edit.setObjectName(_fromUtf8("runnumbers_edit"))
        self.label_5 = QtGui.QLabel(self.frame)
        self.label_5.setGeometry(QtCore.QRect(20, 80, 94, 16))
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_5.sizePolicy().hasHeightForWidth())
        self.label_5.setSizePolicy(sizePolicy)
        self.label_5.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_5.setObjectName(_fromUtf8("label_5"))
        self.extension_combo = QtGui.QComboBox(self.frame)
        self.extension_combo.setGeometry(QtCore.QRect(150, 80, 117, 27))
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.extension_combo.sizePolicy().hasHeightForWidth())
        self.extension_combo.setSizePolicy(sizePolicy)
        self.extension_combo.setBaseSize(QtCore.QSize(200, 0))
        self.extension_combo.setObjectName(_fromUtf8("extension_combo"))
        self.extension_combo.addItem(_fromUtf8(""))
        self.extension_combo.addItem(_fromUtf8(""))
        self.extension_combo.addItem(_fromUtf8(""))
        self.label = QtGui.QLabel(self.frame)
        self.label.setGeometry(QtCore.QRect(20, 130, 134, 21))
        self.label.setObjectName(_fromUtf8("label"))
        self.calfile_edit = QtGui.QLineEdit(self.frame)
        self.calfile_edit.setGeometry(QtCore.QRect(150, 130, 601, 27))
        self.calfile_edit.setObjectName(_fromUtf8("calfile_edit"))
        self.calfile_browse = QtGui.QPushButton(self.frame)
        self.calfile_browse.setGeometry(QtCore.QRect(780, 130, 85, 27))
        self.calfile_browse.setObjectName(_fromUtf8("calfile_browse"))
        self.preserveevents_checkbox = QtGui.QCheckBox(self.frame)
        self.preserveevents_checkbox.setGeometry(QtCore.QRect(350, 80, 137, 22))
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.preserveevents_checkbox.sizePolicy().hasHeightForWidth())
        self.preserveevents_checkbox.setSizePolicy(sizePolicy)
        self.preserveevents_checkbox.setObjectName(_fromUtf8("preserveevents_checkbox"))
        self.sum_checkbox = QtGui.QCheckBox(self.frame)
        self.sum_checkbox.setGeometry(QtCore.QRect(590, 80, 59, 22))
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.sum_checkbox.sizePolicy().hasHeightForWidth())
        self.sum_checkbox.setSizePolicy(sizePolicy)
        self.sum_checkbox.setObjectName(_fromUtf8("sum_checkbox"))
        self.verticalLayout_3.addWidget(self.frame)
        self.frame_2 = QtGui.QFrame(Frame)
        self.frame_2.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame_2.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_2.setObjectName(_fromUtf8("frame_2"))
        self.label_2 = QtGui.QLabel(self.frame_2)
        self.label_2.setGeometry(QtCore.QRect(10, 10, 131, 41))
        font = QtGui.QFont()
        font.setPointSize(10)
        self.label_2.setFont(font)
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.charfile_edit = QtGui.QLineEdit(self.frame_2)
        self.charfile_edit.setGeometry(QtCore.QRect(150, 20, 601, 27))
        self.charfile_edit.setObjectName(_fromUtf8("charfile_edit"))
        self.charfile_browse = QtGui.QPushButton(self.frame_2)
        self.charfile_browse.setGeometry(QtCore.QRect(780, 20, 85, 27))
        self.charfile_browse.setObjectName(_fromUtf8("charfile_browse"))
        self.emptyrun_edit = QtGui.QLineEdit(self.frame_2)
        self.emptyrun_edit.setGeometry(QtCore.QRect(180, 100, 113, 27))
        self.emptyrun_edit.setObjectName(_fromUtf8("emptyrun_edit"))
        self.vanrun_edit = QtGui.QLineEdit(self.frame_2)
        self.vanrun_edit.setGeometry(QtCore.QRect(670, 100, 81, 27))
        self.vanrun_edit.setObjectName(_fromUtf8("vanrun_edit"))
        self.vannoiserun_edit = QtGui.QLineEdit(self.frame_2)
        self.vannoiserun_edit.setEnabled(False)
        self.vannoiserun_edit.setGeometry(QtCore.QRect(180, 140, 113, 27))
        self.vannoiserun_edit.setObjectName(_fromUtf8("vannoiserun_edit"))
        self.vanbkgdrun_edit = QtGui.QLineEdit(self.frame_2)
        self.vanbkgdrun_edit.setGeometry(QtCore.QRect(670, 140, 81, 27))
        self.vanbkgdrun_edit.setObjectName(_fromUtf8("vanbkgdrun_edit"))
        self.label_3 = QtGui.QLabel(self.frame_2)
        self.label_3.setGeometry(QtCore.QRect(70, 100, 91, 17))
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.label_10 = QtGui.QLabel(self.frame_2)
        self.label_10.setGeometry(QtCore.QRect(20, 140, 141, 17))
        self.label_10.setObjectName(_fromUtf8("label_10"))
        self.label_11 = QtGui.QLabel(self.frame_2)
        self.label_11.setGeometry(QtCore.QRect(560, 100, 101, 20))
        self.label_11.setObjectName(_fromUtf8("label_11"))
        self.label_12 = QtGui.QLabel(self.frame_2)
        self.label_12.setGeometry(QtCore.QRect(510, 140, 151, 17))
        font = QtGui.QFont()
        font.setPointSize(9)
        self.label_12.setFont(font)
        self.label_12.setObjectName(_fromUtf8("label_12"))
        self.disablevancorr_chkbox = QtGui.QCheckBox(self.frame_2)
        self.disablevancorr_chkbox.setGeometry(QtCore.QRect(760, 100, 151, 31))
        font = QtGui.QFont()
        font.setPointSize(10)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.disablevancorr_chkbox.setFont(font)
        self.disablevancorr_chkbox.setObjectName(_fromUtf8("disablevancorr_chkbox"))
        self.label_16 = QtGui.QLabel(self.frame_2)
        self.label_16.setGeometry(QtCore.QRect(350, 70, 201, 17))
        self.label_16.setObjectName(_fromUtf8("label_16"))
        self.line_3 = QtGui.QFrame(self.frame_2)
        self.line_3.setGeometry(QtCore.QRect(30, 70, 311, 20))
        self.line_3.setFrameShape(QtGui.QFrame.HLine)
        self.line_3.setFrameShadow(QtGui.QFrame.Sunken)
        self.line_3.setObjectName(_fromUtf8("line_3"))
        self.line_4 = QtGui.QFrame(self.frame_2)
        self.line_4.setGeometry(QtCore.QRect(570, 70, 311, 20))
        self.line_4.setFrameShape(QtGui.QFrame.HLine)
        self.line_4.setFrameShadow(QtGui.QFrame.Sunken)
        self.line_4.setObjectName(_fromUtf8("line_4"))
        self.disablevanbkgdcorr_chkbox = QtGui.QCheckBox(self.frame_2)
        self.disablevanbkgdcorr_chkbox.setGeometry(QtCore.QRect(760, 140, 151, 31))
        font = QtGui.QFont()
        font.setPointSize(10)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.disablevanbkgdcorr_chkbox.setFont(font)
        self.disablevanbkgdcorr_chkbox.setObjectName(_fromUtf8("disablevanbkgdcorr_chkbox"))
        self.disablebkgdcorr_chkbox = QtGui.QCheckBox(self.frame_2)
        self.disablebkgdcorr_chkbox.setGeometry(QtCore.QRect(300, 100, 151, 31))
        font = QtGui.QFont()
        font.setPointSize(10)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.disablebkgdcorr_chkbox.setFont(font)
        self.disablebkgdcorr_chkbox.setObjectName(_fromUtf8("disablebkgdcorr_chkbox"))
        self.line_5 = QtGui.QFrame(self.frame_2)
        self.line_5.setGeometry(QtCore.QRect(470, 100, 20, 81))
        self.line_5.setFrameShape(QtGui.QFrame.VLine)
        self.line_5.setFrameShadow(QtGui.QFrame.Sunken)
        self.line_5.setObjectName(_fromUtf8("line_5"))
        self.verticalLayout_3.addWidget(self.frame_2)
        self.frame_3 = QtGui.QFrame(Frame)
        self.frame_3.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame_3.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_3.setObjectName(_fromUtf8("frame_3"))
        self.binning_edit = QtGui.QLineEdit(self.frame_3)
        self.binning_edit.setGeometry(QtCore.QRect(160, 50, 111, 27))
        self.binning_edit.setObjectName(_fromUtf8("binning_edit"))
        self.binind_checkbox = QtGui.QCheckBox(self.frame_3)
        self.binind_checkbox.setGeometry(QtCore.QRect(360, 50, 135, 22))
        self.binind_checkbox.setObjectName(_fromUtf8("binind_checkbox"))
        self.label_6 = QtGui.QLabel(self.frame_3)
        self.label_6.setGeometry(QtCore.QRect(60, 100, 76, 41))
        self.label_6.setObjectName(_fromUtf8("label_6"))
        self.resamplex_edit = QtGui.QLineEdit(self.frame_3)
        self.resamplex_edit.setGeometry(QtCore.QRect(160, 110, 111, 27))
        self.resamplex_edit.setObjectName(_fromUtf8("resamplex_edit"))
        self.bintype_combo = QtGui.QComboBox(self.frame_3)
        self.bintype_combo.setGeometry(QtCore.QRect(360, 110, 171, 27))
        self.bintype_combo.setObjectName(_fromUtf8("bintype_combo"))
        self.bintype_combo.addItem(_fromUtf8(""))
        self.bintype_combo.addItem(_fromUtf8(""))
        self.lineEdit_5 = QtGui.QLineEdit(self.frame_3)
        self.lineEdit_5.setGeometry(QtCore.QRect(730, 50, 113, 27))
        self.lineEdit_5.setObjectName(_fromUtf8("lineEdit_5"))
        self.lineEdit_6 = QtGui.QLineEdit(self.frame_3)
        self.lineEdit_6.setGeometry(QtCore.QRect(730, 110, 113, 27))
        self.lineEdit_6.setObjectName(_fromUtf8("lineEdit_6"))
        self.label_13 = QtGui.QLabel(self.frame_3)
        self.label_13.setGeometry(QtCore.QRect(650, 50, 66, 17))
        self.label_13.setObjectName(_fromUtf8("label_13"))
        self.label_14 = QtGui.QLabel(self.frame_3)
        self.label_14.setGeometry(QtCore.QRect(650, 110, 66, 17))
        self.label_14.setObjectName(_fromUtf8("label_14"))
        self.label_15 = QtGui.QLabel(self.frame_3)
        self.label_15.setGeometry(QtCore.QRect(80, 50, 66, 17))
        self.label_15.setObjectName(_fromUtf8("label_15"))
        self.usebin_button = QtGui.QRadioButton(self.frame_3)
        self.usebin_button.setGeometry(QtCore.QRect(20, 50, 41, 22))
        self.usebin_button.setText(_fromUtf8(""))
        self.usebin_button.setObjectName(_fromUtf8("usebin_button"))
        self.resamplex_button = QtGui.QRadioButton(self.frame_3)
        self.resamplex_button.setGeometry(QtCore.QRect(20, 110, 41, 22))
        self.resamplex_button.setText(_fromUtf8(""))
        self.resamplex_button.setObjectName(_fromUtf8("resamplex_button"))
        self.line = QtGui.QFrame(self.frame_3)
        self.line.setGeometry(QtCore.QRect(290, 10, 20, 171))
        self.line.setFrameShape(QtGui.QFrame.VLine)
        self.line.setFrameShadow(QtGui.QFrame.Sunken)
        self.line.setObjectName(_fromUtf8("line"))
        self.line_2 = QtGui.QFrame(self.frame_3)
        self.line_2.setGeometry(QtCore.QRect(570, 10, 20, 171))
        self.line_2.setFrameShape(QtGui.QFrame.VLine)
        self.line_2.setFrameShadow(QtGui.QFrame.Sunken)
        self.line_2.setObjectName(_fromUtf8("line_2"))
        self.verticalLayout_3.addWidget(self.frame_3)
        self.frame_4 = QtGui.QFrame(Frame)
        self.frame_4.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame_4.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_4.setObjectName(_fromUtf8("frame_4"))
        self.label_7 = QtGui.QLabel(self.frame_4)
        self.label_7.setGeometry(QtCore.QRect(20, 10, 117, 51))
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_7.sizePolicy().hasHeightForWidth())
        self.label_7.setSizePolicy(sizePolicy)
        self.label_7.setObjectName(_fromUtf8("label_7"))
        self.outputdir_edit = QtGui.QLineEdit(self.frame_4)
        self.outputdir_edit.setGeometry(QtCore.QRect(160, 20, 621, 27))
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.outputdir_edit.sizePolicy().hasHeightForWidth())
        self.outputdir_edit.setSizePolicy(sizePolicy)
        self.outputdir_edit.setObjectName(_fromUtf8("outputdir_edit"))
        self.outputdir_browse = QtGui.QPushButton(self.frame_4)
        self.outputdir_browse.setGeometry(QtCore.QRect(800, 20, 85, 27))
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.outputdir_browse.sizePolicy().hasHeightForWidth())
        self.outputdir_browse.setSizePolicy(sizePolicy)
        self.outputdir_browse.setObjectName(_fromUtf8("outputdir_browse"))
        self.label_8 = QtGui.QLabel(self.frame_4)
        self.label_8.setGeometry(QtCore.QRect(80, 60, 51, 51))
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_8.sizePolicy().hasHeightForWidth())
        self.label_8.setSizePolicy(sizePolicy)
        self.label_8.setObjectName(_fromUtf8("label_8"))
        self.saveas_combo = QtGui.QComboBox(self.frame_4)
        self.saveas_combo.setGeometry(QtCore.QRect(160, 70, 261, 27))
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.saveas_combo.sizePolicy().hasHeightForWidth())
        self.saveas_combo.setSizePolicy(sizePolicy)
        self.saveas_combo.setObjectName(_fromUtf8("saveas_combo"))
        self.saveas_combo.addItem(_fromUtf8(""))
        self.saveas_combo.addItem(_fromUtf8(""))
        self.saveas_combo.addItem(_fromUtf8(""))
        self.saveas_combo.addItem(_fromUtf8(""))
        self.saveas_combo.addItem(_fromUtf8(""))
        self.saveas_combo.addItem(_fromUtf8(""))
        self.saveas_combo.addItem(_fromUtf8(""))
        self.saveas_combo.addItem(_fromUtf8(""))
        self.saveas_combo.addItem(_fromUtf8(""))
        self.label_9 = QtGui.QLabel(self.frame_4)
        self.label_9.setGeometry(QtCore.QRect(500, 60, 65, 41))
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_9.sizePolicy().hasHeightForWidth())
        self.label_9.setSizePolicy(sizePolicy)
        self.label_9.setObjectName(_fromUtf8("label_9"))
        self.unit_combo = QtGui.QComboBox(self.frame_4)
        self.unit_combo.setGeometry(QtCore.QRect(600, 70, 141, 27))
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.unit_combo.sizePolicy().hasHeightForWidth())
        self.unit_combo.setSizePolicy(sizePolicy)
        self.unit_combo.setObjectName(_fromUtf8("unit_combo"))
        self.unit_combo.addItem(_fromUtf8(""))
        self.unit_combo.addItem(_fromUtf8(""))
        self.verticalLayout_3.addWidget(self.frame_4)

        self.retranslateUi(Frame)
        QtCore.QMetaObject.connectSlotsByName(Frame)

    def retranslateUi(self, Frame):
        Frame.setWindowTitle(QtGui.QApplication.translate("Frame", "Frame", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("Frame", "Run numbers", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("Frame", "File Extension", None, QtGui.QApplication.UnicodeUTF8))
        self.extension_combo.setItemText(0, QtGui.QApplication.translate("Frame", "_event.nxs", None, QtGui.QApplication.UnicodeUTF8))
        self.extension_combo.setItemText(1, QtGui.QApplication.translate("Frame", "_histo.nxs", None, QtGui.QApplication.UnicodeUTF8))
        self.extension_combo.setItemText(2, QtGui.QApplication.translate("Frame", "_runinfo.xml", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("Frame", "Calibration File          ", None, QtGui.QApplication.UnicodeUTF8))
        self.calfile_browse.setText(QtGui.QApplication.translate("Frame", "Browse", None, QtGui.QApplication.UnicodeUTF8))
        self.preserveevents_checkbox.setText(QtGui.QApplication.translate("Frame", "Preserve Events", None, QtGui.QApplication.UnicodeUTF8))
        self.sum_checkbox.setText(QtGui.QApplication.translate("Frame", "Sum", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("Frame", "Characterization File", None, QtGui.QApplication.UnicodeUTF8))
        self.charfile_browse.setText(QtGui.QApplication.translate("Frame", "Browse", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("Frame", "Empty Run", None, QtGui.QApplication.UnicodeUTF8))
        self.label_10.setText(QtGui.QApplication.translate("Frame", "Vanadium Noise Run", None, QtGui.QApplication.UnicodeUTF8))
        self.label_11.setText(QtGui.QApplication.translate("Frame", "Vanadium Run", None, QtGui.QApplication.UnicodeUTF8))
        self.label_12.setText(QtGui.QApplication.translate("Frame", "Vanadium Background Run", None, QtGui.QApplication.UnicodeUTF8))
        self.disablevancorr_chkbox.setText(QtGui.QApplication.translate("Frame", "Disable Correction", None, QtGui.QApplication.UnicodeUTF8))
        self.label_16.setText(QtGui.QApplication.translate("Frame", "Override Characterization File ", None, QtGui.QApplication.UnicodeUTF8))
        self.disablevanbkgdcorr_chkbox.setText(QtGui.QApplication.translate("Frame", "Disable Correction", None, QtGui.QApplication.UnicodeUTF8))
        self.disablebkgdcorr_chkbox.setText(QtGui.QApplication.translate("Frame", "Disable Correction", None, QtGui.QApplication.UnicodeUTF8))
        self.binind_checkbox.setText(QtGui.QApplication.translate("Frame", "Bin In d-spacing", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setText(QtGui.QApplication.translate("Frame", "ResampleX", None, QtGui.QApplication.UnicodeUTF8))
        self.bintype_combo.setItemText(0, QtGui.QApplication.translate("Frame", "Linear Binning", None, QtGui.QApplication.UnicodeUTF8))
        self.bintype_combo.setItemText(1, QtGui.QApplication.translate("Frame", "logarithmic binning", None, QtGui.QApplication.UnicodeUTF8))
        self.label_13.setText(QtGui.QApplication.translate("Frame", "TOF Min", None, QtGui.QApplication.UnicodeUTF8))
        self.label_14.setText(QtGui.QApplication.translate("Frame", "TOF Max", None, QtGui.QApplication.UnicodeUTF8))
        self.label_15.setText(QtGui.QApplication.translate("Frame", "Binning", None, QtGui.QApplication.UnicodeUTF8))
        self.label_7.setText(QtGui.QApplication.translate("Frame", "Output Directory", None, QtGui.QApplication.UnicodeUTF8))
        self.outputdir_browse.setText(QtGui.QApplication.translate("Frame", "Browse", None, QtGui.QApplication.UnicodeUTF8))
        self.label_8.setText(QtGui.QApplication.translate("Frame", "Save As", None, QtGui.QApplication.UnicodeUTF8))
        self.saveas_combo.setItemText(0, QtGui.QApplication.translate("Frame", "gsas", None, QtGui.QApplication.UnicodeUTF8))
        self.saveas_combo.setItemText(1, QtGui.QApplication.translate("Frame", "fullprof", None, QtGui.QApplication.UnicodeUTF8))
        self.saveas_combo.setItemText(2, QtGui.QApplication.translate("Frame", "gsas and fullprof", None, QtGui.QApplication.UnicodeUTF8))
        self.saveas_combo.setItemText(3, QtGui.QApplication.translate("Frame", "gsas and fullprof and pdfgetn", None, QtGui.QApplication.UnicodeUTF8))
        self.saveas_combo.setItemText(4, QtGui.QApplication.translate("Frame", "NeXus", None, QtGui.QApplication.UnicodeUTF8))
        self.saveas_combo.setItemText(5, QtGui.QApplication.translate("Frame", "gsas and NeXus", None, QtGui.QApplication.UnicodeUTF8))
        self.saveas_combo.setItemText(6, QtGui.QApplication.translate("Frame", "fullprof and NeXus", None, QtGui.QApplication.UnicodeUTF8))
        self.saveas_combo.setItemText(7, QtGui.QApplication.translate("Frame", "gsas and fullprof and NeXus", None, QtGui.QApplication.UnicodeUTF8))
        self.saveas_combo.setItemText(8, QtGui.QApplication.translate("Frame", "gsas and fullprof and pdfgetn and NeXus", None, QtGui.QApplication.UnicodeUTF8))
        self.label_9.setText(QtGui.QApplication.translate("Frame", "Final Unit", None, QtGui.QApplication.UnicodeUTF8))
        self.unit_combo.setItemText(0, QtGui.QApplication.translate("Frame", "dSpacing", None, QtGui.QApplication.UnicodeUTF8))
        self.unit_combo.setItemText(1, QtGui.QApplication.translate("Frame", "MomentumTransfer", None, QtGui.QApplication.UnicodeUTF8))

