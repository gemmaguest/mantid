#ifndef MANTID_PYTHONINTERFACE_IFUNCTIONADAPTER_H_
#define MANTID_PYTHONINTERFACE_IFUNCTIONADAPTER_H_
/**
    Copyright &copy; 2011 ISIS Rutherford Appleton Laboratory & NScD Oak Ridge National Laboratory

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

    File change history is stored at: <https://github.com/mantidproject/mantid>.
    Code Documentation is available at: <http://doxygen.mantidproject.org>
 */
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "MantidAPI/IFunction.h"
#include "MantidKernel/ClassMacros.h"

#include <boost/python/object.hpp>

namespace Mantid
{
  namespace PythonInterface
  {
    /**
     * Provides a layer to hook into the protected functions
     * of IFunction
     */
    class IFunctionAdapter : public virtual API::IFunction
    {
    public:
      /// A constructor that looks like a Python __init__ method
      IFunctionAdapter(PyObject* self);

      /// Returns the name of the function
      std::string name() const;

      /// Declare an attribute with an initial value
      void declareAttribute(const std::string &name, const boost::python::object &defaultValue);
      /// Get a named attribute value
      PyObject * getAttributeValue(const std::string & name);

    private:
      /// The PyObject must be supplied to construct the object
      DISABLE_DEFAULT_CONSTRUCT(IFunctionAdapter);
      DISABLE_COPY_AND_ASSIGN(IFunctionAdapter);

      /// The name of the function
      std::string m_name;
    };
  }
}


#endif /* MANTID_PYTHONINTERFACE_IFUNCTIONADAPTER_H_ */
