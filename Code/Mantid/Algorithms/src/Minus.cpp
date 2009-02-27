//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include <cmath>
#include "MantidAlgorithms/Minus.h"
#include "MantidAPI/WorkspaceIterator.h"

using namespace Mantid::API;
using namespace Mantid::Kernel;

namespace Mantid
{
  namespace Algorithms
  {
    // Register the class into the algorithm factory
    DECLARE_ALGORITHM(Minus)

    // Get a reference to the logger
    Logger& Minus::g_log = Logger::get("Minus");

    /** Performs the minus operation using Iterators and the std::tranform function.
    * @param it_in1 The const iterator to the lhs data item
    * @param it_in2 The const iterator to the rhs data item
    * @param it_out The output iterator to the new workspace
    */
    void Minus::performBinaryOperation(API::MatrixWorkspace::const_iterator it_in1, API::MatrixWorkspace::const_iterator it_in2,
        API::MatrixWorkspace::iterator it_out)
    {
      std::transform(it_in1.begin(),it_in1.end(),it_in2.begin(),it_out.begin(),Minus_fn(this,it_in1.end() - it_in1.begin()));
    }

    const bool Minus::checkCompatibility(const API::MatrixWorkspace_const_sptr lhs,const API::MatrixWorkspace_const_sptr rhs) const
    {
      if ( lhs->YUnit() != rhs->YUnit() )
      {
        g_log.error("The two workspace are not compatible because they have different units for the data (Y).");
        return false;
      }
      if ( lhs->isDistribution() != rhs->isDistribution() )
      {
        g_log.error("The two workspace are not compatible because one is flagged as a distribution.");
        return false;
      }
      
      return BinaryOperation::checkCompatibility(lhs,rhs);
    }

    /** Performs the addition with Gausian errors within the transform function
    * @param a The LocatedData ref of the first workspace data item
    * @param b The LocatedData ref of the second workspace data item
    * @returns A LocatedData ref of the result with Gausian errors
    */
    LocatedDataValue&
      Minus::Minus_fn::operator() (const ILocatedData& a,const ILocatedData& b)
    {
      //copy the values from lhs
      result = a;
      //use the error helper to correct the changed values in result
      a.ErrorHelper()->minus(a,b,result);
      if (m_progress++ % m_progress_step == 0) report_progress();
      return result;
    }
  }
}
