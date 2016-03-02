#ifndef MANTID_DATAHANDLING_DATABLOCK_H_
#define MANTID_DATAHANDLING_DATABLOCK_H_

#include "MantidDataHandling/DllConfig.h"
#include "MantidNexus/NexusClasses.h"

namespace Mantid {
namespace DataHandling {

class DataBlockGenerator;

class DLLExport DataBlock {
public:
  DataBlock();
  DataBlock(const Mantid::NeXus::NXInt &data);
  DataBlock(int numberOfperiods, size_t numberOfChannels, size_t numberOfSpectra);

  virtual ~DataBlock();

  virtual int64_t getMinSpectrumID() const;
  virtual void setMinSpectrumID(int64_t minSpecID);

  virtual int64_t getMaxSpectrumID() const;
  virtual void setMaxSpectrumID(int64_t minSpecID);

  virtual size_t getNumberOfSpectra() const;
  int getNumberOfPeriods() const;
  size_t getNumberOfChannels() const;

  virtual std::unique_ptr<DataBlockGenerator> getGenerator() const;

protected:
  int m_numberOfPeriods;
  // The number of spectra
  size_t m_numberOfSpectra;
  // The number of time channels per spectrum (N histogram bins -1)
  std::size_t m_numberOfChannels;

  // minimal spectra Id (by default 1, undefined -- max_value)
  int64_t m_minSpectraID;
  // maximal spectra Id (by default 1, undefined  -- 0)
  int64_t m_maxSpectraID;
};

} // namespace DataHandling
} // namespace Mantid

#endif /* MANTID_DATAHANDLING_DATABLOCK_H_ */