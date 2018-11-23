# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
#     NScD Oak Ridge National Laboratory, European Spallation Source
#     & Institut Laue - Langevin
# SPDX - License - Identifier: GPL - 3.0 +
#  This file is part of the mantid workbench.
#
#
from __future__ import absolute_import, division, print_function

from qtpy.QtWidgets import QTableWidgetItem

from mantid.simpleapi import DeleteTableRows
from mantidqt.widgets.common.table_copying import copy_cells, show_no_selection_to_copy_toast
from .model import TableWorkspaceDisplayModel
from .view import TableWorkspaceDisplayView


class TableWorkspaceDisplay(object):
    A_LOT_OF_THINGS_TO_PLOT_MESSAGE = "You selected {} spectra to plot. Are you sure you want to plot that many?"
    NUM_SELECTED_FOR_CONFIRMATION = 10

    def __init__(self, ws, plot=None, parent=None, model=None, view=None):
        # Create model and view, or accept mocked versions
        self.model = model if model else TableWorkspaceDisplayModel(ws)
        self.view = view if view else TableWorkspaceDisplayView(self, parent, self.model.get_name())
        self.plot = plot
        self.view.set_context_menu_actions(self.view)
        column_headers = self.model.get_column_headers()
        # self.editable_columns = self.model.get_editable_columns(column_headers)
        self.view.setColumnCount(len(column_headers))
        self.view.setHorizontalHeaderLabels(column_headers)
        # self.view.setHorizontalHeaderLabels(["{}[Y]".format(x) for x in column_headers])
        self.load_data(self.view)

    def load_data(self, table):
        num_rows = self.model.get_number_of_rows()
        table.setRowCount(num_rows)

        num_cols = self.model.get_number_of_columns()
        for col in range(num_cols):
            column_data = self.model.get_column(col)
            # editable = False
            # if peaks_workspace and col in self.editable_columns:
            #     editable = True
            for row in range(num_rows):
                item = QTableWidgetItem(str(column_data[row]))
                # if not editable:
                #     item.setFlags(item.flags() & ~Qt.ItemIsEditable)
                table.setItem(row, col, item)

    def action_copy_cells(self, table):
        copy_cells(table)

    def action_copy_bin_values(self, table):
        copy_cells(table)

    def action_copy_spectrum_values(self, table):
        copy_cells(table)

    def action_keypress_copy(self, table):
        copy_cells(table)

    def action_delete_row(self):
        selection_model = self.view.selectionModel()
        if not selection_model.hasSelection():
            show_no_selection_to_copy_toast()
            return

        selected_rows = selection_model.selectedRows()
        selected_rows_list = [index.row() for index in selected_rows]
        selected_rows_str = ",".join([str(row) for row in selected_rows_list])

        DeleteTableRows(self.model.ws, selected_rows_str)
        # Reverse the list so that we delete in order from bottom -> top
        # this prevents the row index from shifting up when deleting rows above
        for row in reversed(selected_rows_list):
            self.view.removeRow(row)

    def action_statistics_on_rows(self):
        selection_model = self.view.selectionModel()
        if not selection_model.hasSelection():
            show_no_selection_to_copy_toast()
            return

        selected_rows = selection_model.selectedRows()
        selected_rows_list = [index.row() for index in selected_rows]
        num_cols = self.model.get_number_of_columns()

    # def _do_action_plot(self, table, axis, get_index, plot_errors=False):
    #     if self.plot is None:
    #         raise ValueError("Trying to do a plot, but no plotting class dependency was injected in the constructor")
    #     selection_model = table.selectionModel()
    #     if not selection_model.hasSelection():
    #         self.show_no_selection_to_copy_toast()
    #         return
    #
    #     if axis == MantidAxType.SPECTRUM:
    #         selected = selection_model.selectedRows()  # type: list
    #     else:
    #         selected = selection_model.selectedColumns()  # type: list
    #
    #     if len(selected) > self.NUM_SELECTED_FOR_CONFIRMATION and not self.view.ask_confirmation(
    #             self.A_LOT_OF_THINGS_TO_PLOT_MESSAGE.format(len(selected))):
    #         return
    #
    #     plot_kwargs = {"capsize": 3} if plot_errors else {}
    #     plot_kwargs["axis"] = axis
    #
    #     ws_list = [self.model._ws]
    #     self.plot(ws_list, wksp_indices=[get_index(index) for index in selected], errors=plot_errors,
    #               plot_kwargs=plot_kwargs)
    #
    # def action_plot_spectrum(self, table):
    #     self._do_action_plot(table, MantidAxType.SPECTRUM, lambda index: index.row())
    #
    # def action_plot_spectrum_with_errors(self, table):
    #     self._do_action_plot(table, MantidAxType.SPECTRUM, lambda index: index.row(), plot_errors=True)
    #
    # def action_plot_bin(self, table):
    #     self._do_action_plot(table, MantidAxType.BIN, lambda index: index.column())
    #
    # def action_plot_bin_with_errors(self, table):
    #     self._do_action_plot(table, MantidAxType.BIN, lambda index: index.column(), plot_errors=True)

    # def _get_ws_read_from_type(self, type):
    #     if type == TableWorkspaceTableViewModelType.y:
    #         return self.model._ws.readY
    #     elif type == TableWorkspaceTableViewModelType.x:
    #         return self.model._ws.readX
    #     elif type == TableWorkspaceTableViewModelType.e:
    #         return self.model._ws.readE
    #     else:
    #         raise ValueError("Unknown TableViewModel type {}".format(type))
