// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#pragma once

#include "DllOption.h"
#include "MantidQtWidgets/Common/MantidDialog.h"
#include <QGridLayout>
#include <QHash>
#include <QSignalMapper>

namespace MantidQt {
namespace MantidWidgets {

class EXPORT_OPT_MANTIDQT_COMMON DiagResults : public API::MantidDialog {
  Q_OBJECT

public:
  DiagResults(QWidget *parent);
  void updateResults(const QString &testSummary);

signals:
  /// is emitted just before the window dies to let the window that created this
  /// know the pointer it has is invalid
  void died();

private:
  void updateRow(int row, const QString &text);
  int addRow(const QString &firstColumn, const QString &secondColumn);
  void closeEvent(QCloseEvent *event) override;

private:
  /// the layout that widgets are added to
  QGridLayout *m_Grid;
};
} // namespace MantidWidgets
} // namespace MantidQt
