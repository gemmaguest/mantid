# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'diffraction_van_setup.ui'
#
# Created: Mon Feb 25 14:48:57 2013
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
        Frame.resize(879, 556)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(Frame.sizePolicy().hasHeightForWidth())
        Frame.setSizePolicy(sizePolicy)
        Frame.setFrameShape(QtGui.QFrame.StyledPanel)
        Frame.setFrameShadow(QtGui.QFrame.Raised)
        self.verticalLayout_3 = QtGui.QVBoxLayout(Frame)
        self.verticalLayout_3.setObjectName(_fromUtf8("verticalLayout_3"))
        self.emptycan_gbox = QtGui.QGroupBox(Frame)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.emptycan_gbox.sizePolicy().hasHeightForWidth())
        self.emptycan_gbox.setSizePolicy(sizePolicy)
        self.emptycan_gbox.setTitle(_fromUtf8(""))
        self.emptycan_gbox.setObjectName(_fromUtf8("emptycan_gbox"))
        self.frame_2 = QtGui.QFrame(self.emptycan_gbox)
        self.frame_2.setGeometry(QtCore.QRect(10, 0, 741, 241))
        self.frame_2.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame_2.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_2.setObjectName(_fromUtf8("frame_2"))
        self.lowres_edit = QtGui.QLineEdit(self.frame_2)
        self.lowres_edit.setGeometry(QtCore.QRect(10, 90, 111, 27))
        self.lowres_edit.setObjectName(_fromUtf8("lowres_edit"))
        self.label_6 = QtGui.QLabel(self.frame_2)
        self.label_6.setGeometry(QtCore.QRect(10, 140, 142, 39))
        self.label_6.setObjectName(_fromUtf8("label_6"))
        self.label_5 = QtGui.QLabel(self.frame_2)
        self.label_5.setGeometry(QtCore.QRect(540, 50, 192, 39))
        self.label_5.setObjectName(_fromUtf8("label_5"))
        self.label_4 = QtGui.QLabel(self.frame_2)
        self.label_4.setGeometry(QtCore.QRect(320, 50, 208, 39))
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.unwrap_edit = QtGui.QLineEdit(self.frame_2)
        self.unwrap_edit.setGeometry(QtCore.QRect(150, 90, 101, 27))
        self.unwrap_edit.setObjectName(_fromUtf8("unwrap_edit"))
        self.cropwavelengthmin_edit = QtGui.QLineEdit(self.frame_2)
        self.cropwavelengthmin_edit.setGeometry(QtCore.QRect(320, 90, 131, 27))
        self.cropwavelengthmin_edit.setObjectName(_fromUtf8("cropwavelengthmin_edit"))
        self.filterbadpulses_chkbox = QtGui.QCheckBox(self.frame_2)
        self.filterbadpulses_chkbox.setGeometry(QtCore.QRect(550, 180, 141, 22))
        self.filterbadpulses_chkbox.setObjectName(_fromUtf8("filterbadpulses_chkbox"))
        self.label_3 = QtGui.QLabel(self.frame_2)
        self.label_3.setGeometry(QtCore.QRect(10, 50, 131, 39))
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.removepromptwidth_edit = QtGui.QLineEdit(self.frame_2)
        self.removepromptwidth_edit.setGeometry(QtCore.QRect(540, 90, 151, 27))
        self.removepromptwidth_edit.setObjectName(_fromUtf8("removepromptwidth_edit"))
        self.label_2 = QtGui.QLabel(self.frame_2)
        self.label_2.setGeometry(QtCore.QRect(160, 50, 80, 39))
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.maxchunksize_edit = QtGui.QLineEdit(self.frame_2)
        self.maxchunksize_edit.setGeometry(QtCore.QRect(10, 180, 161, 27))
        self.maxchunksize_edit.setObjectName(_fromUtf8("maxchunksize_edit"))
        self.pushdatapos_combo = QtGui.QComboBox(self.frame_2)
        self.pushdatapos_combo.setGeometry(QtCore.QRect(320, 180, 131, 27))
        self.pushdatapos_combo.setObjectName(_fromUtf8("pushdatapos_combo"))
        self.pushdatapos_combo.addItem(_fromUtf8(""))
        self.pushdatapos_combo.addItem(_fromUtf8(""))
        self.pushdatapos_combo.addItem(_fromUtf8(""))
        self.label_11 = QtGui.QLabel(self.frame_2)
        self.label_11.setGeometry(QtCore.QRect(320, 150, 141, 17))
        self.label_11.setObjectName(_fromUtf8("label_11"))
        self.label_10 = QtGui.QLabel(self.frame_2)
        self.label_10.setGeometry(QtCore.QRect(10, 10, 131, 21))
        font = QtGui.QFont()
        font.setPointSize(13)
        font.setBold(True)
        font.setItalic(True)
        font.setWeight(75)
        self.label_10.setFont(font)
        self.label_10.setObjectName(_fromUtf8("label_10"))
        self.frame_3 = QtGui.QFrame(self.emptycan_gbox)
        self.frame_3.setGeometry(QtCore.QRect(10, 280, 741, 201))
        self.frame_3.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame_3.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_3.setObjectName(_fromUtf8("frame_3"))
        self.label_7 = QtGui.QLabel(self.frame_3)
        self.label_7.setGeometry(QtCore.QRect(320, 80, 169, 39))
        self.label_7.setObjectName(_fromUtf8("label_7"))
        self.label_8 = QtGui.QLabel(self.frame_3)
        self.label_8.setGeometry(QtCore.QRect(180, 80, 102, 39))
        self.label_8.setObjectName(_fromUtf8("label_8"))
        self.label_9 = QtGui.QLabel(self.frame_3)
        self.label_9.setGeometry(QtCore.QRect(50, 80, 81, 39))
        self.label_9.setObjectName(_fromUtf8("label_9"))
        self.vanpeakfwhm_edit = QtGui.QLineEdit(self.frame_3)
        self.vanpeakfwhm_edit.setGeometry(QtCore.QRect(50, 130, 91, 27))
        self.vanpeakfwhm_edit.setObjectName(_fromUtf8("vanpeakfwhm_edit"))
        self.vanpeaktol_edit = QtGui.QLineEdit(self.frame_3)
        self.vanpeaktol_edit.setGeometry(QtCore.QRect(180, 130, 101, 27))
        self.vanpeaktol_edit.setObjectName(_fromUtf8("vanpeaktol_edit"))
        self.stripvanpeaks_chkbox = QtGui.QCheckBox(self.frame_3)
        self.stripvanpeaks_chkbox.setGeometry(QtCore.QRect(320, 20, 181, 22))
        self.stripvanpeaks_chkbox.setObjectName(_fromUtf8("stripvanpeaks_chkbox"))
        self.vansmoothpar_edit = QtGui.QLineEdit(self.frame_3)
        self.vansmoothpar_edit.setGeometry(QtCore.QRect(320, 130, 201, 27))
        self.vansmoothpar_edit.setObjectName(_fromUtf8("vansmoothpar_edit"))
        self.label = QtGui.QLabel(self.frame_3)
        self.label.setGeometry(QtCore.QRect(10, 10, 201, 41))
        font = QtGui.QFont()
        font.setPointSize(14)
        font.setBold(True)
        font.setItalic(True)
        font.setWeight(75)
        self.label.setFont(font)
        self.label.setObjectName(_fromUtf8("label"))
        self.verticalLayout_3.addWidget(self.emptycan_gbox)

        self.retranslateUi(Frame)
        QtCore.QMetaObject.connectSlotsByName(Frame)

    def retranslateUi(self, Frame):
        Frame.setWindowTitle(QtGui.QApplication.translate("Frame", "Frame", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setText(QtGui.QApplication.translate("Frame", "Maximum Chunk Size", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("Frame", "Remove Prompt Pulse Width", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("Frame", "Minimum Cropped Wavelength", None, QtGui.QApplication.UnicodeUTF8))
        self.filterbadpulses_chkbox.setText(QtGui.QApplication.translate("Frame", "Filter Bad Pulses", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("Frame", "Low Resolution Ref", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("Frame", "Unwrap Ref", None, QtGui.QApplication.UnicodeUTF8))
        self.pushdatapos_combo.setItemText(0, QtGui.QApplication.translate("Frame", "None", None, QtGui.QApplication.UnicodeUTF8))
        self.pushdatapos_combo.setItemText(1, QtGui.QApplication.translate("Frame", "ResetToZero", None, QtGui.QApplication.UnicodeUTF8))
        self.pushdatapos_combo.setItemText(2, QtGui.QApplication.translate("Frame", "AddMinimum", None, QtGui.QApplication.UnicodeUTF8))
        self.label_11.setText(QtGui.QApplication.translate("Frame", "Push Data Postive", None, QtGui.QApplication.UnicodeUTF8))
        self.label_10.setText(QtGui.QApplication.translate("Frame", "Advanced Setup", None, QtGui.QApplication.UnicodeUTF8))
        self.label_7.setText(QtGui.QApplication.translate("Frame", "Peak Smooth Parameters", None, QtGui.QApplication.UnicodeUTF8))
        self.label_8.setText(QtGui.QApplication.translate("Frame", "Peak Tolerance", None, QtGui.QApplication.UnicodeUTF8))
        self.label_9.setText(QtGui.QApplication.translate("Frame", "Peak FWHM", None, QtGui.QApplication.UnicodeUTF8))
        self.stripvanpeaks_chkbox.setText(QtGui.QApplication.translate("Frame", "Strip Vanadium Peaks", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("Frame", "Strip Vanadium Peaks", None, QtGui.QApplication.UnicodeUTF8))

