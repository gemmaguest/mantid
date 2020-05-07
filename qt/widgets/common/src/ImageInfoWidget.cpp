// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2020 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +

#include "MantidQtWidgets/Common/ImageInfoWidget.h"
#include "MantidAPI/AnalysisDataService.h"

#include <QAbstractScrollArea>
#include <QHeaderView>
#include <QTableWidget>

namespace MantidQt {
namespace MantidWidgets {

/**
 * Constructor
 */
ImageInfoWidget::ImageInfoWidget(std::string &wsName, DisplayType *type,
                                 QWidget *parent)
    : QTableWidget(2, 0, parent) {
  auto workspace =
      Mantid::API::AnalysisDataService::Instance().retrieve(wsName);
  m_model = std::make_unique<ImageInfoModel>(workspace, type);
  horizontalHeader()->hide();
  verticalHeader()->hide();

  int height = 20;
  for (int i = 0; i < rowCount(); i++)
    height += rowHeight(i);

  setFixedHeight(height);
}

/**
 * Destructor
 */
ImageInfoWidget::~ImageInfoWidget() {}

void ImageInfoWidget::updateTable(const double x, const double y,
                                  const double z) {
  auto info = m_model->getInfoList(x, y, z);

  if (info.empty())
    return;

  setColumnCount(static_cast<int>(info.size() / 2));
  setRowCount(2);

  auto row = 0;
  auto column = 0;
  for (const auto &item : info) {
    auto cell = new QTableWidgetItem(QString::fromStdString(item));
    setItem(row, column, cell);
    cell->setFlags(Qt::ItemIsSelectable);
    row++;
    if (row == 2) {
      row = 0;
      column++;
    }

    resizeColumnsToContents();
    int table_width = 2;
    for (int i = 0; i < static_cast<int>(info.size()) / 2; i++)
      table_width += columnWidth(i);
    setMaximumWidth(table_width);
  }
}

} // namespace MantidWidgets
} // namespace MantidQt
