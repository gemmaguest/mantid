#ifndef MANTID_VATES_vtkSplatterPlotFactory_H_
#define MANTID_VATES_vtkSplatterPlotFactory_H_

#include "MantidAPI/IMDEventWorkspace.h"
#include "MantidMDEvents/MDEventFactory.h"
#include "MantidMDEvents/MDEventWorkspace.h"
#include "MantidVatesAPI/ThresholdRange.h"
#include "MantidVatesAPI/vtkDataSetFactory.h"
#include <boost/shared_ptr.hpp>

using Mantid::MDEvents::MDEventWorkspace;

namespace Mantid
{
namespace VATES
{

/**
 * Factory that creates a simple "splatter plot" data set
 * composed of points of a selection of the events in a MDEventWorkspace.
 *

 @author Janik Zikovsky
 @date August 16, 2011

 Copyright &copy; 2010 ISIS Rutherford Appleton Laboratory & NScD Oak Ridge National Laboratory

 This file is part of Mantid.

 Mantid is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 Mantid is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 File change history is stored at: <https://svn.mantidproject.org/mantid/trunk/Code/Mantid>
 Code Documentation is available at: <http://doxygen.mantidproject.org>
 */

class DLLExport vtkSplatterPlotFactory : public vtkDataSetFactory
{

public:

  /// Constructor
  vtkSplatterPlotFactory(ThresholdRange_scptr thresholdRange, const std::string& scalarName, const size_t numPoints = 1000);

  /// Destructor
  virtual ~vtkSplatterPlotFactory();

  /// Factory Method. Should also handle delegation to successors.
  virtual vtkDataSet* create() const;
  
  /// Create as a mesh only.
  virtual vtkDataSet* createMeshOnly() const;

  /// Create the scalar array only.
  virtual vtkFloatArray* createScalarArray() const;

  /// Initalize with a target workspace.
  virtual void initialize(Mantid::API::Workspace_sptr);

  /// Get the name of the type.
  virtual std::string getFactoryTypeName() const
  {
    return "vtkSplatterPlotFactory";
  }

  virtual void SetNumberOfPoints(size_t points);

private:

  template<typename MDE, size_t nd>
  void doCreate(typename MDEventWorkspace<MDE, nd>::sptr ws) const;

  /// Template Method pattern to validate the factory before use.
  virtual void validate() const;

  /// Threshold range strategy.
  ThresholdRange_scptr m_thresholdRange;

  /// Scalar name to provide on dataset.
  const std::string m_scalarName;

  /// Member workspace to generate vtkdataset from.
  Mantid::API::IMDEventWorkspace_sptr m_workspace;

  /// Approximate number of points to plot
  size_t m_numPoints;

  /// Data set that will be generated
  mutable vtkDataSet * dataSet;

  /// We are slicing down from > 3 dimensions
  mutable bool slice;

  /// Mask for choosing along which dimensions to slice
  mutable bool * sliceMask;

  /// Implicit function to define which boxes to render.
  mutable Mantid::Geometry::MDImplicitFunction * sliceImplicitFunction;

};


}
}


#endif
