#ifndef MANTID_MDEVENTS_MDBOX_NEXUSS_IO_H
#define MANTID_MDEVENTS_MDBOX_NEXUSS_IO_H

#include "MantidAPI/IBoxControllerIO.h"

namespace Mantid
{
namespace MDEvents
{

  //===============================================================================================
  /** The class responsible for saving events into nexus file using generic box controller interface

      @date March 15, 2013

      Copyright &copy; 2008-2010 ISIS Rutherford Appleton Laboratory & NScD Oak Ridge National Laboratory

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
    class DLLExport MDBoxNxSIO : public IBoxControllerIO
    {

      virtual bool openFile(const std::string &fileName)=0;
      virtual bool isOpened()const=0;
      virtual const std::string &getFileName()const=0;

      virtual void saveBlock(void const * const /* Block */, const uint64_t /*blockPosition*/,const size_t /*blockSize*/)=0;
      virtual void loadBlock(void  * const  /* Block */, const uint64_t /*blockPosition*/,const size_t /*blockSize*/)=0;
      virtual void flushData()=0;
      virtual void closeFile()=0;

      virtual ~IBoxControllerIO(){}

    };


#endif