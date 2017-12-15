#ifndef MANTID_ALGORITHMS_POLARIZATIONEFFICIENCYCORTEST_H_
#define MANTID_ALGORITHMS_POLARIZATIONEFFICIENCYCORTEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidAlgorithms/PolarizationEfficiencyCor.h"

#include "MantidAPI/AnalysisDataService.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAPI/TextAxis.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidAPI/WorkspaceGroup.h"
#include "MantidDataObjects/Workspace2D.h"
#include "MantidDataObjects/WorkspaceCreation.h"

#include <Eigen/Dense>

using Mantid::Algorithms::PolarizationEfficiencyCor;

class PolarizationEfficiencyCorTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static PolarizationEfficiencyCorTest *createSuite() { return new PolarizationEfficiencyCorTest(); }
  static void destroySuite( PolarizationEfficiencyCorTest *suite ) { delete suite; }

  void tearDown() override {
    using namespace Mantid::API;
    AnalysisDataService::Instance().clear();
  }

  void test_Init() {
    PolarizationEfficiencyCor alg;
    TS_ASSERT_THROWS_NOTHING( alg.initialize() )
    TS_ASSERT( alg.isInitialized() )
  }

  void test_IdealCaseFullCorrections() {
    using namespace Mantid::API;
    using namespace Mantid::DataObjects;
    using namespace Mantid::HistogramData;
    using namespace Mantid::Kernel;
    constexpr size_t nBins{3};
    constexpr size_t nHist{2};
    BinEdges edges{0.3, 0.6, 0.9, 1.2};
    const double yVal = 2.3;
    Counts counts{yVal, yVal, yVal};
    MatrixWorkspace_sptr ws00 = create<Workspace2D>(nHist, Histogram(edges, counts));
    MatrixWorkspace_sptr ws01 = ws00->clone();
    MatrixWorkspace_sptr ws10 = ws00->clone();
    MatrixWorkspace_sptr ws11 = ws00->clone();
    WorkspaceGroup_sptr inputWS = boost::make_shared<WorkspaceGroup>();
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws00));
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws01));
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws10));
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws11));
    for (size_t i = 0; i != 4; ++i) {
      MatrixWorkspace_sptr ws = boost::dynamic_pointer_cast<MatrixWorkspace>(inputWS->getItem(i));
      for (size_t j = 0; j != nHist; ++j) {
        ws->mutableY(j) *= static_cast<double>(i + 1);
        ws->mutableE(j) *= static_cast<double>(i + 1);
      }
    }
    auto effWS = idealEfficiencies(edges);
    PolarizationEfficiencyCor alg;
    alg.setChild(true);
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.initialize())
    TS_ASSERT(alg.isInitialized())
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", inputWS))
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("OutputWorkspace", m_outputWSName))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Efficiencies", effWS))
    TS_ASSERT_THROWS_NOTHING(alg.execute())
    TS_ASSERT(alg.isExecuted())
    WorkspaceGroup_sptr outputWS = alg.getProperty("OutputWorkspace");
    TS_ASSERT(outputWS)
    TS_ASSERT_EQUALS(outputWS->getNumberOfEntries(), 4)
    const std::array<std::string, 4> POL_DIRS{{"++", "+-", "-+", "--"}};
    for (size_t i = 0; i != 4; ++i) {
      const std::string wsName = m_outputWSName + std::string("_") + POL_DIRS[i];
      MatrixWorkspace_sptr ws = boost::dynamic_pointer_cast<MatrixWorkspace>(outputWS->getItem(wsName));
      TS_ASSERT(ws)
      TS_ASSERT_EQUALS(ws->getNumberHistograms(), nHist)
      for (size_t j = 0; j != nHist; ++j) {
        const auto &xs = ws->x(j);
        const auto &ys = ws->y(j);
        const auto &es = ws->e(j);
        TS_ASSERT_EQUALS(ys.size(), nBins)
        for (size_t k = 0; k != nBins; ++k) {
          TS_ASSERT_EQUALS(xs[k], edges[k])
          TS_ASSERT_EQUALS(ys[k], yVal * static_cast<double>(i + 1))
          TS_ASSERT_EQUALS(es[k], std::sqrt(yVal) * static_cast<double>(i + 1))
        }
      }
    }
  }

  void test_IdealCaseThreeInputs10Missing() {
    idealThreeInputsTest("10");
  }

  void test_IdealCaseThreeInputs01Missing() {
    idealThreeInputsTest("01");
  }

  void test_IdealCaseTwoInputsWithAnalyzer() {
    using namespace Mantid::API;
    using namespace Mantid::DataObjects;
    using namespace Mantid::HistogramData;
    using namespace Mantid::Kernel;
    constexpr size_t nBins{3};
    constexpr size_t nHist{2};
    BinEdges edges{0.3, 0.6, 0.9, 1.2};
    const double yVal = 2.3;
    Counts counts{yVal, yVal, yVal};
    MatrixWorkspace_sptr ws00 = create<Workspace2D>(nHist, Histogram(edges, counts));
    MatrixWorkspace_sptr ws11 = ws00->clone();
    WorkspaceGroup_sptr inputWS = boost::make_shared<WorkspaceGroup>();
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws00));
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws11));
    for (size_t i = 0; i != nHist; ++i) {
      ws11->mutableY(i) *= 2.;
      ws11->mutableE(i) *= 2.;
    }

    auto effWS = idealEfficiencies(edges);
    PolarizationEfficiencyCor alg;
    alg.setChild(true);
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.initialize())
    TS_ASSERT(alg.isInitialized())
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", inputWS))
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("OutputWorkspace", m_outputWSName))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Efficiencies", effWS))
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("Flippers", "00, 11"))
    TS_ASSERT_THROWS_NOTHING(alg.execute())
    TS_ASSERT(alg.isExecuted())
    WorkspaceGroup_sptr outputWS = alg.getProperty("OutputWorkspace");
    TS_ASSERT(outputWS)
    TS_ASSERT_EQUALS(outputWS->getNumberOfEntries(), 4)
    const std::array<std::string, 4> POL_DIRS{{"++", "+-", "-+", "--"}};
    for (size_t i = 0; i != 4; ++i) {
      const auto &dir = POL_DIRS[i];
      const std::string wsName = m_outputWSName + std::string("_") + dir;
      MatrixWorkspace_sptr ws = boost::dynamic_pointer_cast<MatrixWorkspace>(outputWS->getItem(wsName));
      TS_ASSERT(ws)
      const double expected = [yVal, &dir]() {
        if (dir == "++") {
          return yVal;
        } else if (dir == "--") {
          return 2. * yVal;
        } else {
          return 0.;
        }
      }();
      const double expectedError = [yVal, &dir]() {
        if (dir == "++") {
          return std::sqrt(yVal);
        } else if (dir == "--") {
          return 2. * std::sqrt(yVal);
        } else {
            return 0.;
        }
      }();
      TS_ASSERT_EQUALS(ws->getNumberHistograms(), nHist)
      for (size_t j = 0; j != nHist; ++j) {
        const auto &xs = ws->x(j);
        const auto &ys = ws->y(j);
        const auto &es = ws->e(j);
        TS_ASSERT_EQUALS(ys.size(), nBins)
        for (size_t k = 0; k != nBins; ++k) {
          TS_ASSERT_EQUALS(xs[k], edges[k])
          TS_ASSERT_EQUALS(ys[k], expected)
          TS_ASSERT_EQUALS(es[k], expectedError)
        }
      }
    }
  }

  void test_IdealCaseTwoInputsNoAnalyzer() {
    using namespace Mantid::API;
    using namespace Mantid::DataObjects;
    using namespace Mantid::HistogramData;
    using namespace Mantid::Kernel;
    constexpr size_t nBins{3};
    constexpr size_t nHist{2};
    BinEdges edges{0.3, 0.6, 0.9, 1.2};
    const double yVal = 2.3;
    Counts counts{yVal, yVal, yVal};
    MatrixWorkspace_sptr ws00 = create<Workspace2D>(nHist, Histogram(edges, counts));
    MatrixWorkspace_sptr ws11 = ws00->clone();
    WorkspaceGroup_sptr inputWS = boost::make_shared<WorkspaceGroup>();
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws00));
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws11));
    for (size_t i = 0; i != nHist; ++i) {
      ws11->mutableY(i) *= 2.;
      ws11->mutableE(i) *= 2.;
    }
    auto effWS = idealEfficiencies(edges);
    PolarizationEfficiencyCor alg;
    alg.setChild(true);
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.initialize())
    TS_ASSERT(alg.isInitialized())
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", inputWS))
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("OutputWorkspace", m_outputWSName))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Efficiencies", effWS))
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("Flippers", "00, 11"))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Analyzer", false))
    TS_ASSERT_THROWS_NOTHING(alg.execute())
    TS_ASSERT(alg.isExecuted())
    WorkspaceGroup_sptr outputWS = alg.getProperty("OutputWorkspace");
    TS_ASSERT(outputWS)
    TS_ASSERT_EQUALS(outputWS->getNumberOfEntries(), 2)
    const std::array<std::string, 2> POL_DIRS{{"++", "--"}};
    for (size_t i = 0; i != 2; ++i) {
      const auto &dir = POL_DIRS[i];
      const std::string wsName = m_outputWSName + std::string("_") + dir;
      MatrixWorkspace_sptr ws = boost::dynamic_pointer_cast<MatrixWorkspace>(outputWS->getItem(wsName));
      TS_ASSERT(ws)
      TS_ASSERT_EQUALS(ws->getNumberHistograms(), nHist)
      for (size_t j = 0; j != nHist; ++j) {
        const auto &xs = ws->x(j);
        const auto &ys = ws->y(j);
        const auto &es = ws->e(j);
        TS_ASSERT_EQUALS(ys.size(), nBins)
        for (size_t k = 0; k != nBins; ++k) {
          TS_ASSERT_EQUALS(xs[k], edges[k])
          TS_ASSERT_EQUALS(ys[k], yVal * static_cast<double>(i + 1))
          TS_ASSERT_EQUALS(es[k], std::sqrt(yVal) * static_cast<double>(i + 1))
        }
      }
    }
  }

  void test_IdealCaseDirectBeamCorrections() {
    using namespace Mantid::API;
    using namespace Mantid::DataObjects;
    using namespace Mantid::HistogramData;
    using namespace Mantid::Kernel;
    constexpr size_t nBins{3};
    constexpr size_t nHist{2};
    BinEdges edges{0.3, 0.6, 0.9, 1.2};
    const double yVal = 2.3;
    Counts counts{yVal, yVal, yVal};
    MatrixWorkspace_sptr ws00 = create<Workspace2D>(nHist, Histogram(edges, counts));
    WorkspaceGroup_sptr inputWS = boost::make_shared<WorkspaceGroup>();
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws00));
    auto effWS = idealEfficiencies(edges);
    PolarizationEfficiencyCor alg;
    alg.setChild(true);
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.initialize())
    TS_ASSERT(alg.isInitialized())
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", inputWS))
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("OutputWorkspace", m_outputWSName))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Efficiencies", effWS))
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("Flippers", "00"))
    TS_ASSERT_THROWS_NOTHING(alg.execute())
    TS_ASSERT(alg.isExecuted())
    WorkspaceGroup_sptr outputWS = alg.getProperty("OutputWorkspace");
    TS_ASSERT(outputWS)
    TS_ASSERT_EQUALS(outputWS->getNumberOfEntries(), 1)
    MatrixWorkspace_sptr ws = boost::dynamic_pointer_cast<MatrixWorkspace>(outputWS->getItem(m_outputWSName + std::string("_++")));
    TS_ASSERT(ws)
    TS_ASSERT_EQUALS(ws->getNumberHistograms(), nHist)
    for (size_t i = 0; i != nHist; ++i) {
      const auto &xs = ws->x(i);
      const auto &ys = ws->y(i);
      const auto &es = ws->e(i);
      TS_ASSERT_EQUALS(ys.size(), nBins)
      for (size_t j = 0; j != nBins; ++j) {
        TS_ASSERT_EQUALS(xs[j], edges[j])
        TS_ASSERT_EQUALS(ys[j], yVal)
        TS_ASSERT_EQUALS(es[j], std::sqrt(yVal))
      }
    }
  }

  void test_FullCorrections() {
    using namespace Mantid::API;
    using namespace Mantid::DataObjects;
    using namespace Mantid::HistogramData;
    using namespace Mantid::Kernel;
    constexpr size_t nHist{2};
    BinEdges edges{0.3, 0.6, 0.9, 1.2};
    const double yVal = 2.3;
    Counts counts{yVal, yVal, yVal};
    MatrixWorkspace_sptr ws00 = create<Workspace2D>(nHist, Histogram(edges, counts));
    MatrixWorkspace_sptr ws01 = ws00->clone();
    MatrixWorkspace_sptr ws10 = ws00->clone();
    MatrixWorkspace_sptr ws11 = ws00->clone();
    WorkspaceGroup_sptr inputWS = boost::make_shared<WorkspaceGroup>();
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws00));
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws01));
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws10));
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws11));
    for (size_t i = 0; i != 4; ++i) {
      MatrixWorkspace_sptr ws = boost::dynamic_pointer_cast<MatrixWorkspace>(inputWS->getItem(i));
      for (size_t j = 0; j != nHist; ++j) {
        ws->mutableY(j) *= static_cast<double>(i + 1);
        ws->mutableE(j) *= static_cast<double>(i + 1);
      }
    }
    auto effWS = efficiencies(edges);
    PolarizationEfficiencyCor alg;
    alg.setChild(true);
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.initialize())
    TS_ASSERT(alg.isInitialized())
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", inputWS))
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("OutputWorkspace", m_outputWSName))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Efficiencies", effWS))
    TS_ASSERT_THROWS_NOTHING(alg.execute())
    TS_ASSERT(alg.isExecuted())
    WorkspaceGroup_sptr outputWS = alg.getProperty("OutputWorkspace");
    TS_ASSERT(outputWS)
    TS_ASSERT_EQUALS(outputWS->getNumberOfEntries(), 4)
    fullFourInputsResultsCheck(outputWS, ws00, ws01, ws10, ws11, effWS);
  }

  void test_ThreeInputsWithMissing01FlipperConfiguration() {
    threeInputsTest("01");
  }

  void test_ThreeInputsWithMissing10FlipperConfiguration() {
    threeInputsTest("10");
  }

  void test_TwoInputsWithAnalyzer() {
    using namespace Mantid::API;
    using namespace Mantid::DataObjects;
    using namespace Mantid::HistogramData;
    using namespace Mantid::Kernel;
    constexpr size_t nHist{2};
    constexpr size_t nBins{3};
    BinEdges edges{0.3, 0.6, 0.9, 1.2};
    const double yVal = 2.3;
    Counts counts{yVal, yVal, yVal};
    MatrixWorkspace_sptr ws00 = create<Workspace2D>(nHist, Histogram(edges, counts));
    MatrixWorkspace_sptr ws01 = nullptr;
    MatrixWorkspace_sptr ws10 = nullptr;
    MatrixWorkspace_sptr ws11 = ws00->clone();
    WorkspaceGroup_sptr inputWS = boost::make_shared<WorkspaceGroup>();
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws00));
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws11));
    for (size_t i = 0; i != 2; ++i) {
      MatrixWorkspace_sptr ws = boost::dynamic_pointer_cast<MatrixWorkspace>(inputWS->getItem(i));
      for (size_t j = 0; j != nHist; ++j) {
        ws->mutableY(j) *= static_cast<double>(i + 1);
        ws->mutableE(j) *= static_cast<double>(i + 1);
      }
    }
    auto effWS = efficiencies(edges);
    PolarizationEfficiencyCor alg;
    alg.setChild(true);
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.initialize())
    TS_ASSERT(alg.isInitialized())
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", inputWS))
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("OutputWorkspace", m_outputWSName))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Efficiencies", effWS))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Flippers", "00, 11"))
    TS_ASSERT_THROWS_NOTHING(alg.execute())
    TS_ASSERT(alg.isExecuted())
    WorkspaceGroup_sptr outputWS = alg.getProperty("OutputWorkspace");
    TS_ASSERT(outputWS)
    TS_ASSERT_EQUALS(outputWS->getNumberOfEntries(), 4)
    solveMissingIntensities(ws00, ws01, ws10, ws11, effWS);
    using namespace Mantid::API;
    const double F1 = effWS->y(0).front();
    const double F1e = effWS->e(0).front();
    const double F2 = effWS->y(1).front();
    const double F2e = effWS->e(1).front();
    const double P1 = effWS->y(2).front();
    const double P1e = effWS->e(2).front();
    const double P2 = effWS->y(3).front();
    const double P2e = effWS->e(3).front();
    const Eigen::Vector4d y{ws00->y(0).front(), ws01->y(0).front(), ws10->y(0).front(), ws11->y(0).front()};
    const auto expected = correction(y, F1, F2, P1, P2);
    const Eigen::Vector4d e{ws00->e(0).front(), ws01->e(0).front(), ws10->e(0).front(), ws11->e(0).front()};
    const auto expectedError = error(y, e, F1, F1e, F2, F2e, P1, P1e, P2, P2e);
    MatrixWorkspace_sptr ppWS = boost::dynamic_pointer_cast<MatrixWorkspace>(outputWS->getItem(m_outputWSName + std::string("_++")));
    MatrixWorkspace_sptr pmWS = boost::dynamic_pointer_cast<MatrixWorkspace>(outputWS->getItem(m_outputWSName + std::string("_+-")));
    MatrixWorkspace_sptr mpWS = boost::dynamic_pointer_cast<MatrixWorkspace>(outputWS->getItem(m_outputWSName + std::string("_-+")));
    MatrixWorkspace_sptr mmWS = boost::dynamic_pointer_cast<MatrixWorkspace>(outputWS->getItem(m_outputWSName + std::string("_--")));
    TS_ASSERT(ppWS)
    TS_ASSERT(pmWS)
    TS_ASSERT(mpWS)
    TS_ASSERT(mmWS)
    TS_ASSERT_EQUALS(ppWS->getNumberHistograms(), nHist)
    TS_ASSERT_EQUALS(pmWS->getNumberHistograms(), nHist)
    TS_ASSERT_EQUALS(mpWS->getNumberHistograms(), nHist)
    TS_ASSERT_EQUALS(mmWS->getNumberHistograms(), nHist)
    for (size_t j = 0; j != nHist; ++j) {
      const auto &ppX = ppWS->x(j);
      const auto &ppY = ppWS->y(j);
      const auto &ppE = ppWS->e(j);
      const auto &pmX = pmWS->x(j);
      const auto &pmY = pmWS->y(j);
      const auto &pmE = pmWS->e(j);
      const auto &mpX = mpWS->x(j);
      const auto &mpY = mpWS->y(j);
      const auto &mpE = mpWS->e(j);
      const auto &mmX = mmWS->x(j);
      const auto &mmY = mmWS->y(j);
      const auto &mmE = mmWS->e(j);
      TS_ASSERT_EQUALS(ppY.size(), nBins)
      TS_ASSERT_EQUALS(pmY.size(), nBins)
      TS_ASSERT_EQUALS(mpY.size(), nBins)
      TS_ASSERT_EQUALS(mmY.size(), nBins)
      for (size_t k = 0; k != nBins; ++k) {
        TS_ASSERT_EQUALS(ppX[k], edges[k])
        TS_ASSERT_EQUALS(pmX[k], edges[k])
        TS_ASSERT_EQUALS(mpX[k], edges[k])
        TS_ASSERT_EQUALS(mmX[k], edges[k])
        TS_ASSERT_DELTA(ppY[k], expected[0], 1e-12)
        TS_ASSERT_DELTA(pmY[k], expected[1], 1e-12)
        TS_ASSERT_DELTA(mpY[k], expected[2], 1e-12)
        TS_ASSERT_DELTA(mmY[k], expected[3], 1e-12)
        // This test constructs the expected missing I01 and I10 intensities
        // slightly different from what the algorithm does: I10 is solved
        // first and then I01 is solved using all I00, I10 and I11. This
        // results in slightly larger errors estimates for I01 and thus for
        // the final corrected expected intensities.
        TS_ASSERT_DELTA(ppE[k], expectedError[0], 1e-6)
        TS_ASSERT_LESS_THAN(ppE[k], expectedError[0])
        TS_ASSERT_DELTA(pmE[k], expectedError[1], 1e-2)
        TS_ASSERT_LESS_THAN(pmE[k], expectedError[1])
        TS_ASSERT_DELTA(mpE[k], expectedError[2], 1e-7)
        TS_ASSERT_LESS_THAN(mpE[k], expectedError[2])
        TS_ASSERT_DELTA(mmE[k], expectedError[3], 1e-5)
        TS_ASSERT_LESS_THAN(mmE[k], expectedError[3])
      }
    }
  }

  void test_FailureWhenEfficiencyHistogramIsMissing() {
    using namespace Mantid::API;
    using namespace Mantid::DataObjects;
    using namespace Mantid::HistogramData;
    using namespace Mantid::Kernel;
    BinEdges edges{0.3, 0.6, 0.9, 1.2};
    Counts counts{0., 0., 0.};
    MatrixWorkspace_sptr ws00 = create<Workspace2D>(1, Histogram(edges, counts));
    WorkspaceGroup_sptr inputWS = boost::make_shared<WorkspaceGroup>();
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws00));
    auto effWS = idealEfficiencies(edges);
    // Rename F1 to something else.
    auto axis = make_unique<TextAxis>(4);
    axis->setLabel(0, "__wrong_histogram_label");
    axis->setLabel(1, "F2");
    axis->setLabel(2, "P1");
    axis->setLabel(3, "P2");
    effWS->replaceAxis(1, axis.release());
    PolarizationEfficiencyCor alg;
    alg.setChild(true);
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.initialize())
    TS_ASSERT(alg.isInitialized())
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", inputWS))
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("OutputWorkspace", m_outputWSName))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Efficiencies", effWS))
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("Flippers", "00"))
    TS_ASSERT_THROWS(alg.execute(), std::runtime_error)
    TS_ASSERT(!alg.isExecuted())
  }

  void test_FailureWhenEfficiencyXDataMismatches() {
    using namespace Mantid::API;
    using namespace Mantid::DataObjects;
    using namespace Mantid::HistogramData;
    using namespace Mantid::Kernel;
    BinEdges edges{0.3, 0.6, 0.9, 1.2};
    Counts counts{0., 0., 0.};
    MatrixWorkspace_sptr ws00 = create<Workspace2D>(1, Histogram(edges, counts));
    WorkspaceGroup_sptr inputWS = boost::make_shared<WorkspaceGroup>();
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws00));
    auto effWS = idealEfficiencies(edges);
    // Change a bin edge of one of the histograms.
    auto &xs = effWS->mutableX(0);
    xs[xs.size() / 2] *= 1.01;
    PolarizationEfficiencyCor alg;
    alg.setChild(true);
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.initialize())
    TS_ASSERT(alg.isInitialized())
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", inputWS))
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("OutputWorkspace", m_outputWSName))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Efficiencies", effWS))
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("Flippers", "00"))
    TS_ASSERT_THROWS(alg.execute(), std::runtime_error)
    TS_ASSERT(!alg.isExecuted())
  }

  void test_FailureWhenNumberOfHistogramsInInputWorkspacesMismatch() {
    using namespace Mantid::API;
    using namespace Mantid::DataObjects;
    using namespace Mantid::HistogramData;
    using namespace Mantid::Kernel;
    constexpr size_t nHist{2};
    BinEdges edges{0.3, 0.6, 0.9, 1.2};
    Counts counts{0., 0., 0.};
    MatrixWorkspace_sptr ws00 = create<Workspace2D>(nHist, Histogram(edges, counts));
    MatrixWorkspace_sptr ws01 = ws00->clone();
    MatrixWorkspace_sptr ws10 = create<Workspace2D>(nHist + 1, Histogram(edges, counts));
    MatrixWorkspace_sptr ws11 = ws00->clone();
    WorkspaceGroup_sptr inputWS = boost::make_shared<WorkspaceGroup>();
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws00));
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws01));
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws10));
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws11));
    auto effWS = idealEfficiencies(edges);
    PolarizationEfficiencyCor alg;
    alg.setChild(true);
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.initialize())
    TS_ASSERT(alg.isInitialized())
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", inputWS))
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("OutputWorkspace", m_outputWSName))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Efficiencies", effWS))
    TS_ASSERT_THROWS(alg.execute(), std::runtime_error)
    TS_ASSERT(!alg.isExecuted())
  }

  void test_FailureWhenAnInputWorkspaceIsMissing() {
    using namespace Mantid::API;
    using namespace Mantid::DataObjects;
    using namespace Mantid::HistogramData;
    using namespace Mantid::Kernel;
    constexpr size_t nHist{2};
    BinEdges edges{0.3, 0.6, 0.9, 1.2};
    Counts counts{0., 0., 0.};
    MatrixWorkspace_sptr ws00 = create<Workspace2D>(nHist, Histogram(edges, counts));
    MatrixWorkspace_sptr ws01 = ws00->clone();
    MatrixWorkspace_sptr ws11 = ws00->clone();
    WorkspaceGroup_sptr inputWS = boost::make_shared<WorkspaceGroup>();
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws00));
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws01));
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws11));
    auto effWS = idealEfficiencies(edges);
    PolarizationEfficiencyCor alg;
    alg.setChild(true);
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.initialize())
    TS_ASSERT(alg.isInitialized())
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", inputWS))
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("OutputWorkspace", m_outputWSName))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Efficiencies", effWS))
    TS_ASSERT_THROWS(alg.execute(), std::runtime_error)
    TS_ASSERT(!alg.isExecuted())
  }

private:
  std::string m_outputWSName{"output"};

  Mantid::API::MatrixWorkspace_sptr efficiencies(const Mantid::HistogramData::BinEdges &edges) {
    using namespace Mantid::API;
    using namespace Mantid::DataObjects;
    using namespace Mantid::HistogramData;
    using namespace Mantid::Kernel;
    const auto nBins = edges.size() - 1;
    constexpr size_t nHist{4};
    Counts counts(nBins, 0.0);
    MatrixWorkspace_sptr ws = create<Workspace2D>(nHist, Histogram(edges, counts));
    ws->mutableY(0) = 0.95;
    ws->mutableE(0) = 0.01;
    ws->mutableY(1) = 0.92;
    ws->mutableE(1) = 0.02;
    ws->mutableY(2) = 0.05;
    ws->mutableE(2) = 0.015;
    ws->mutableY(3) = 0.04;
    ws->mutableE(3) = 0.03;
    auto axis = make_unique<TextAxis>(4);
    axis->setLabel(0, "F1");
    axis->setLabel(1, "F2");
    axis->setLabel(2, "P1");
    axis->setLabel(3, "P2");
    ws->replaceAxis(1, axis.release());
    return ws;
  }

  Mantid::API::MatrixWorkspace_sptr idealEfficiencies(const Mantid::HistogramData::BinEdges &edges) {
    using namespace Mantid::API;
    using namespace Mantid::DataObjects;
    using namespace Mantid::HistogramData;
    using namespace Mantid::Kernel;
    const auto nBins = edges.size() - 1;
    constexpr size_t nHist{4};
    Counts counts(nBins, 0.0);
    MatrixWorkspace_sptr ws = create<Workspace2D>(nHist, Histogram(edges, counts));
    ws->mutableY(0) = 1.;
    ws->mutableY(1) = 1.;
    auto axis = make_unique<TextAxis>(4);
    axis->setLabel(0, "F1");
    axis->setLabel(1, "F2");
    axis->setLabel(2, "P1");
    axis->setLabel(3, "P2");
    ws->replaceAxis(1, axis.release());
    return ws;
  }

  void idealThreeInputsTest(const std::string &missingFlipperConf) {
    using namespace Mantid::API;
    using namespace Mantid::DataObjects;
    using namespace Mantid::HistogramData;
    using namespace Mantid::Kernel;
    constexpr size_t nBins{3};
    constexpr size_t nHist{2};
    BinEdges edges{0.3, 0.6, 0.9, 1.2};
    const double yVal = 2.3;
    Counts counts{yVal, yVal, yVal};
    MatrixWorkspace_sptr ws00 = create<Workspace2D>(nHist, Histogram(edges, counts));
    MatrixWorkspace_sptr wsXX = ws00->clone();
    MatrixWorkspace_sptr ws11 = ws00->clone();
    WorkspaceGroup_sptr inputWS = boost::make_shared<WorkspaceGroup>();
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws00));
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(wsXX));
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws11));
    for (size_t i = 0; i != 3; ++i) {
      MatrixWorkspace_sptr ws = boost::dynamic_pointer_cast<MatrixWorkspace>(inputWS->getItem(i));
      for (size_t j = 0; j != nHist; ++j) {
        ws->mutableY(j) *= static_cast<double>(i + 1);
        ws->mutableE(j) *= static_cast<double>(i + 1);
      }
    }
    auto effWS = idealEfficiencies(edges);
    constexpr char *OUTWS_NAME{"output"};
    PolarizationEfficiencyCor alg;
    alg.setChild(true);
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.initialize())
    TS_ASSERT(alg.isInitialized())
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", inputWS))
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("OutputWorkspace", OUTWS_NAME))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Efficiencies", effWS))
    const std::string presentFlipperConf = missingFlipperConf == "01" ? "10" : "01";
    const std::string flipperConf = "00, " + presentFlipperConf + ", 11";
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("Flippers", flipperConf))
    TS_ASSERT_THROWS_NOTHING(alg.execute())
    TS_ASSERT(alg.isExecuted())
    WorkspaceGroup_sptr outputWS = alg.getProperty("OutputWorkspace");
    TS_ASSERT(outputWS)
    TS_ASSERT_EQUALS(outputWS->getNumberOfEntries(), 4)
    const std::array<std::string, 4> POL_DIRS{{"++", "+-", "-+", "--"}};
    for (size_t i = 0; i != 4; ++i) {
      const auto &dir = POL_DIRS[i];
      const std::string wsName = OUTWS_NAME + std::string("_") + dir;
      MatrixWorkspace_sptr ws = boost::dynamic_pointer_cast<MatrixWorkspace>(outputWS->getItem(wsName));
      TS_ASSERT(ws)
      const double expected = [yVal, &dir]() {
        if (dir == "++") {
          return yVal;
        } else if (dir == "--") {
          return 3. * yVal;
        } else {
          return 2. * yVal;
        }
      }();
      const double expectedError = [yVal, &dir, &missingFlipperConf]() {
        if (dir == "++") {
          return std::sqrt(yVal);
        } else if (dir == "--") {
          return 3. * std::sqrt(yVal);
        } else {
          std::string conf = std::string(dir.front() == '+' ? "0" : "1") + std::string(dir.back() == '+' ? "0" : "1");
          if (conf != missingFlipperConf) {
            return 2. * std::sqrt(yVal);
          } else {
            return 0.;
          }
        }
      }();
      TS_ASSERT_EQUALS(ws->getNumberHistograms(), nHist)
      for (size_t j = 0; j != nHist; ++j) {
        const auto &xs = ws->x(j);
        const auto &ys = ws->y(j);
        const auto &es = ws->e(j);
        TS_ASSERT_EQUALS(ys.size(), nBins)
        for (size_t k = 0; k != nBins; ++k) {
          TS_ASSERT_EQUALS(xs[k], edges[k])
          TS_ASSERT_EQUALS(ys[k], expected)
          TS_ASSERT_EQUALS(es[k], expectedError)
        }
      }
    }
  }

  void threeInputsTest(const std::string &missingFlipperConf) {
    using namespace Mantid::API;
    using namespace Mantid::DataObjects;
    using namespace Mantid::HistogramData;
    using namespace Mantid::Kernel;
    constexpr size_t nHist{2};
    BinEdges edges{0.3, 0.6, 0.9, 1.2};
    const double yVal = 2.3;
    Counts counts{yVal, yVal, yVal};
    MatrixWorkspace_sptr ws00 = create<Workspace2D>(nHist, Histogram(edges, counts));
    MatrixWorkspace_sptr ws01 = missingFlipperConf == "01" ? nullptr : ws00->clone();
    MatrixWorkspace_sptr ws10 = missingFlipperConf == "10" ? nullptr : ws00->clone();
    MatrixWorkspace_sptr ws11 = ws00->clone();
    WorkspaceGroup_sptr inputWS = boost::make_shared<WorkspaceGroup>();
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws00));
    if (ws01) {
      inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws01));
    } else {
      inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws10));
    }
    inputWS->addWorkspace(boost::dynamic_pointer_cast<Workspace>(ws11));
    for (size_t i = 0; i != 3; ++i) {
      MatrixWorkspace_sptr ws = boost::dynamic_pointer_cast<MatrixWorkspace>(inputWS->getItem(i));
      for (size_t j = 0; j != nHist; ++j) {
        ws->mutableY(j) *= static_cast<double>(i + 1);
        ws->mutableE(j) *= static_cast<double>(i + 1);
      }
    }
    auto effWS = efficiencies(edges);
    PolarizationEfficiencyCor alg;
    alg.setChild(true);
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.initialize())
    TS_ASSERT(alg.isInitialized())
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", inputWS))
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("OutputWorkspace", m_outputWSName))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Efficiencies", effWS))
    const std::string presentFlipperConf = missingFlipperConf == "01" ? "10" : "01";
    const std::string flipperConf = "00, " + presentFlipperConf + ", 11";
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Flippers", flipperConf))
    TS_ASSERT_THROWS_NOTHING(alg.execute())
    TS_ASSERT(alg.isExecuted())
    WorkspaceGroup_sptr outputWS = alg.getProperty("OutputWorkspace");
    TS_ASSERT(outputWS)
    TS_ASSERT_EQUALS(outputWS->getNumberOfEntries(), 4)
    solveMissingIntensity(ws00, ws01, ws10, ws11, effWS);
    fullFourInputsResultsCheck(outputWS, ws00, ws01, ws10, ws11, effWS);
  }

  void fullFourInputsResultsCheck(Mantid::API::WorkspaceGroup_sptr &outputWS, Mantid::API::MatrixWorkspace_sptr &ws00, Mantid::API::MatrixWorkspace_sptr &ws01, Mantid::API::MatrixWorkspace_sptr &ws10, Mantid::API::MatrixWorkspace_sptr &ws11, Mantid::API::MatrixWorkspace_sptr &effWS) {
    using namespace Mantid::API;
    const auto nHist = ws00->getNumberHistograms();
    const auto nBins = ws00->y(0).size();
    const auto edges = ws00->binEdges(0);
    const double F1 = effWS->y(0).front();
    const double F1e = effWS->e(0).front();
    const double F2 = effWS->y(1).front();
    const double F2e = effWS->e(1).front();
    const double P1 = effWS->y(2).front();
    const double P1e = effWS->e(2).front();
    const double P2 = effWS->y(3).front();
    const double P2e = effWS->e(3).front();
    const Eigen::Vector4d y{ws00->y(0).front(), ws01->y(0).front(), ws10->y(0).front(), ws11->y(0).front()};
    const auto expected = correction(y, F1, F2, P1, P2);
    const Eigen::Vector4d e{ws00->e(0).front(), ws01->e(0).front(), ws10->e(0).front(), ws11->e(0).front()};
    const auto expectedError = error(y, e, F1, F1e, F2, F2e, P1, P1e, P2, P2e);
    MatrixWorkspace_sptr ppWS = boost::dynamic_pointer_cast<MatrixWorkspace>(outputWS->getItem(m_outputWSName + std::string("_++")));
    MatrixWorkspace_sptr pmWS = boost::dynamic_pointer_cast<MatrixWorkspace>(outputWS->getItem(m_outputWSName + std::string("_+-")));
    MatrixWorkspace_sptr mpWS = boost::dynamic_pointer_cast<MatrixWorkspace>(outputWS->getItem(m_outputWSName + std::string("_-+")));
    MatrixWorkspace_sptr mmWS = boost::dynamic_pointer_cast<MatrixWorkspace>(outputWS->getItem(m_outputWSName + std::string("_--")));
    TS_ASSERT(ppWS)
    TS_ASSERT(pmWS)
    TS_ASSERT(mpWS)
    TS_ASSERT(mmWS)
    TS_ASSERT_EQUALS(ppWS->getNumberHistograms(), nHist)
    TS_ASSERT_EQUALS(pmWS->getNumberHistograms(), nHist)
    TS_ASSERT_EQUALS(mpWS->getNumberHistograms(), nHist)
    TS_ASSERT_EQUALS(mmWS->getNumberHistograms(), nHist)
    for (size_t j = 0; j != nHist; ++j) {
      const auto &ppX = ppWS->x(j);
      const auto &ppY = ppWS->y(j);
      const auto &ppE = ppWS->e(j);
      const auto &pmX = pmWS->x(j);
      const auto &pmY = pmWS->y(j);
      const auto &pmE = pmWS->e(j);
      const auto &mpX = mpWS->x(j);
      const auto &mpY = mpWS->y(j);
      const auto &mpE = mpWS->e(j);
      const auto &mmX = mmWS->x(j);
      const auto &mmY = mmWS->y(j);
      const auto &mmE = mmWS->e(j);
      TS_ASSERT_EQUALS(ppY.size(), nBins)
      TS_ASSERT_EQUALS(pmY.size(), nBins)
      TS_ASSERT_EQUALS(mpY.size(), nBins)
      TS_ASSERT_EQUALS(mmY.size(), nBins)
      for (size_t k = 0; k != nBins; ++k) {
        TS_ASSERT_EQUALS(ppX[k], edges[k])
        TS_ASSERT_EQUALS(pmX[k], edges[k])
        TS_ASSERT_EQUALS(mpX[k], edges[k])
        TS_ASSERT_EQUALS(mmX[k], edges[k])
        TS_ASSERT_DELTA(ppY[k], expected[0], 1e-12)
        TS_ASSERT_DELTA(pmY[k], expected[1], 1e-12)
        TS_ASSERT_DELTA(mpY[k], expected[2], 1e-12)
        TS_ASSERT_DELTA(mmY[k], expected[3], 1e-12)
        TS_ASSERT_DELTA(ppE[k], expectedError[0], 1e-12)
        TS_ASSERT_DELTA(pmE[k], expectedError[1], 1e-12)
        TS_ASSERT_DELTA(mpE[k], expectedError[2], 1e-12)
        TS_ASSERT_DELTA(mmE[k], expectedError[3], 1e-12)
      }
    }
  }
  void invertedF1(Eigen::Matrix4d &m, const double f1) {
    m << f1, 0., 0., 0.,
         0., f1, 0., 0.,
         f1 - 1., 0., 1., 0.,
         0., f1 - 1., 0., 1.;
    m *= 1. / f1;
  }

  void invertedF1Derivative(Eigen::Matrix4d &m, const double f1) {
    m << 0., 0., 0., 0.,
         0., 0., 0., 0.,
         1., 0., -1., 0.,
         0., 1., 0., -1.;
    m *= 1. / (f1 * f1);
  }

  void invertedF2(Eigen::Matrix4d &m, const double f2) {
    m << f2, 0., 0., 0.,
         f2 - 1., 1., 0., 0.,
         0., 0., f2, 0.,
         0., 0., f2 - 1., 1.;
    m *= 1. / f2;
  }

  void invertedF2Derivative(Eigen::Matrix4d &m, const double f2) {
    m << 0., 0., 0., 0.,
         1., -1., 0., 0.,
         0., 0., 0., 0.,
         0., 0., 1., -1.;
    m *= 1. / (f2 * f2);
  }

  void invertedP1(Eigen::Matrix4d &m, const double p1) {
    m << p1 - 1., 0., p1, 0.,
         0., p1 - 1., 0., p1,
         p1, 0., p1 - 1., 0.,
         0., p1, 0., p1 - 1.;
    m *= 1. / (2. * p1 - 1.);
  }

  void invertedP1Derivative(Eigen::Matrix4d &m, const double p1) {
    m << 1., 0., -1., 0.,
         0., 1., 0., -1.,
         -1., 0., 1., 0.,
         0., -1., 0., 1.;
    m *= 1. / (2. * p1 - 1.) / (2. * p1 - 1.);
  }

  void invertedP2(Eigen::Matrix4d &m, const double p2) {
    m << p2 - 1., p2, 0., 0.,
         p2, p2 - 1., 0., 0.,
         0., 0., p2 - 1., p2,
         0., 0., p2, p2 - 1.;
    m *= 1. / (2. * p2 - 1.);
  }

  void invertedP2Derivative(Eigen::Matrix4d &m, const double p2) {
    m << 1., -1., 0., 0.,
         -1., 1., 0., 0.,
         0., 0., 1., -1.,
         0., 0., -1., 1.;
    m *= 1. / (2. * p2 - 1.) / (2. * p2 - 1.);
  }

  Eigen::Vector4d correction(const Eigen::Vector4d &y, const double f1, const double f2, const double p1, const double p2) {
    Eigen::Matrix4d F1;
    invertedF1(F1, f1);
    Eigen::Matrix4d F2;
    invertedF2(F2, f2);
    Eigen::Matrix4d P1;
    invertedP1(P1, p1);
    Eigen::Matrix4d P2;
    invertedP2(P2, p2);
    const auto inverted = P2 * P1 * F2 * F1;
    return static_cast<Eigen::Vector4d>(inverted * y);
  }

  Eigen::Vector4d error(const Eigen::Vector4d &y, const Eigen::Vector4d &e, const double f1, const double f1e, const double f2, const double f2e, const double p1, const double p1e, const double p2, const double p2e) {
    Eigen::Matrix4d F1;
    invertedF1(F1, f1);
    Eigen::Matrix4d dF1;
    invertedF1Derivative(dF1, f1);
    dF1 *= f1e;
    Eigen::Matrix4d F2;
    invertedF2(F2, f2);
    Eigen::Matrix4d dF2;
    invertedF2Derivative(dF2, f2);
    dF2 *= f2e;
    Eigen::Matrix4d P1;
    invertedP1(P1, p1);
    Eigen::Matrix4d dP1;
    invertedP1Derivative(dP1, p1);
    dP1 *= p1e;
    Eigen::Matrix4d P2;
    invertedP2(P2, p2);
    Eigen::Matrix4d dP2;
    invertedP2Derivative(dP2, p2);
    dP2 *= p2e;
    const auto p2Error = (dP2 * P1 * F2 * F1 * y).array();
    const auto p1Error = (P2 * dP1 * F2 * F1 * y).array();
    const auto f2Error = (P2 * P1 * dF2 * F1 * y).array();
    const auto f1Error = (P2 * P1 * F2 * dF1 * y).array();
    const auto inverted = (P2 * P1 * F2 * F1).array();
    const auto yError = ((inverted * inverted).matrix() * (e.array() * e.array()).matrix()).array();
    return (p2Error * p2Error + p1Error * p1Error + f2Error * f2Error + f1Error * f1Error + yError).sqrt().matrix();
  }

  void solveMissingIntensity(const Mantid::API::MatrixWorkspace_sptr &ppWS, Mantid::API::MatrixWorkspace_sptr &pmWS, Mantid::API::MatrixWorkspace_sptr &mpWS, const Mantid::API::MatrixWorkspace_sptr &mmWS, const Mantid::API::MatrixWorkspace_sptr &effWS) {
    const auto &F1 = effWS->y(0);
    const auto &F2 = effWS->y(1);
    const auto &P1 = effWS->y(2);
    const auto &P2 = effWS->y(3);
    if (!pmWS) {
      pmWS = mpWS->clone();
      for (size_t wsIndex = 0; wsIndex != pmWS->getNumberHistograms(); ++wsIndex) {
        const auto &ppY = ppWS->y(wsIndex);
        auto &pmY = pmWS->mutableY(wsIndex);
        auto &pmE = pmWS->mutableE(wsIndex);
        const auto &mpY = mpWS->y(wsIndex);
        const auto &mmY = mmWS->y(wsIndex);
        for (size_t binIndex = 0; binIndex != mpY.size(); ++binIndex) {
          // mI01[j] = -(2*MI00[j]*f2L[j]*P2L[j]-P2L[j]*MI11[j]-2*MI10[j]*f2L[j]*P2L[j]+MI10[j]*P2L[j]-MI00[j]*P2L[j]+P1L[j]*MI11[j]-2*MI00[j]*f1L[j]*P1L[j]+MI00[j]*P1L[j]-P1L[j]*MI10[j]+MI00[j]*f1L[j]+MI10[j]*f2L[j]-MI00[j]*f2L[j])/(P2L[j]-P1L[j]+2*f1L[j]*P1L[j]-f1L[j])
          pmY[binIndex] = -(2*ppY[binIndex]*F2[binIndex]*P2[binIndex]-P2[binIndex]*mmY[binIndex]-2*mpY[binIndex]*F2[binIndex]*P2[binIndex]+mpY[binIndex]*P2[binIndex]-ppY[binIndex]*P2[binIndex]+P1[binIndex]*mmY[binIndex]-2*ppY[binIndex]*F1[binIndex]*P1[binIndex]+ppY[binIndex]*P1[binIndex]-P1[binIndex]*mpY[binIndex]+ppY[binIndex]*F1[binIndex]+mpY[binIndex]*F2[binIndex]-ppY[binIndex]*F2[binIndex])/(P2[binIndex]-P1[binIndex]+2*F1[binIndex]*P1[binIndex]-F1[binIndex]);
          // Error propagation is not implemented in the algorithm.
          pmE[binIndex] = 0.;
        }
      }
    } else {
      mpWS = pmWS->clone();
      for (size_t wsIndex = 0; wsIndex != mpWS->getNumberHistograms(); ++wsIndex) {
        const auto &ppY = ppWS->y(wsIndex);
        const auto &pmY = pmWS->y(wsIndex);
        auto &mpY = mpWS->mutableY(wsIndex);
        auto &mpE = mpWS->mutableE(wsIndex);
        const auto &mmY = mmWS->y(wsIndex);
        for (size_t binIndex = 0; binIndex != mpY.size(); ++binIndex) {
          // mI10[j] = (-MI00[j]*P2L[j]+P2L[j]*MI01[j]-P2L[j]*MI11[j]+2*MI00[j]*f2L[j]*P2L[j]-MI01[j]*P1L[j]+P1L[j]*MI11[j]+MI00[j]*P1L[j]-2*MI00[j]*f1L[j]*P1L[j]+2*MI01[j]*f1L[j]*P1L[j]+MI00[j]*f1L[j]-MI00[j]*f2L[j]-MI01[j]*f1L[j])/(-P2L[j]+2*f2L[j]*P2L[j]+P1L[j]-f2L[j])
          mpY[binIndex] = (-ppY[binIndex]*P2[binIndex]+P2[binIndex]*pmY[binIndex]-P2[binIndex]*mmY[binIndex]+2*ppY[binIndex]*F2[binIndex]*P2[binIndex]-pmY[binIndex]*P1[binIndex]+P1[binIndex]*mmY[binIndex]+ppY[binIndex]*P1[binIndex]-2*ppY[binIndex]*F1[binIndex]*P1[binIndex]+2*pmY[binIndex]*F1[binIndex]*P1[binIndex]+ppY[binIndex]*F1[binIndex]-ppY[binIndex]*F2[binIndex]-pmY[binIndex]*F1[binIndex])/(-P2[binIndex]+2*F2[binIndex]*P2[binIndex]+P1[binIndex]-F2[binIndex]);
          // Error propagation is not implemented in the algorithm.
          mpE[binIndex] = 0.;
        }
      }
    }
  }

  void solveMissingIntensities(const Mantid::API::MatrixWorkspace_sptr &ppWS, Mantid::API::MatrixWorkspace_sptr &pmWS, Mantid::API::MatrixWorkspace_sptr &mpWS, const Mantid::API::MatrixWorkspace_sptr &mmWS, const Mantid::API::MatrixWorkspace_sptr &effWS) {
    const auto &F1 = effWS->y(0);
    const auto &F1E = effWS->e(0);
    const auto &F2 = effWS->y(1);
    const auto &F2E = effWS->e(1);
    const auto &P1 = effWS->y(2);
    const auto &P1E = effWS->e(2);
    const auto &P2 = effWS->y(3);
    const auto &P2E = effWS->e(3);
    pmWS = ppWS->clone();
    mpWS = ppWS->clone();
    for (size_t wsIndex = 0; wsIndex != ppWS->getNumberHistograms(); ++wsIndex) {
      const auto &ppY = ppWS->y(wsIndex);
      const auto &ppE = ppWS->e(wsIndex);
      auto &pmY = pmWS->mutableY(wsIndex);
      auto &pmE = pmWS->mutableE(wsIndex);
      auto &mpY = mpWS->mutableY(wsIndex);
      auto &mpE = mpWS->mutableE(wsIndex);
      const auto &mmY = mmWS->y(wsIndex);
      const auto &mmE = mmWS->e(wsIndex);
      for (size_t binIndex = 0; binIndex != mpY.size(); ++binIndex) {
        const double P12 = P1[binIndex] * P1[binIndex];
        const double P13 = P1[binIndex] * P12;
        const double P14 = P1[binIndex] * P13;
        const double P22 = P2[binIndex] * P2[binIndex];
        const double P23 = P2[binIndex] * P22;
        const double F12 = F1[binIndex] * F1[binIndex];
        {
          mpY[binIndex] = -(-mmY[binIndex]*P22*F1[binIndex]+2*F1[binIndex]*P1[binIndex]*mmY[binIndex]*P22-2*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P2[binIndex]-8*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P12*P2[binIndex]+2*ppY[binIndex]*F2[binIndex]*P12*P2[binIndex]+8*ppY[binIndex]*F12*F2[binIndex]*P12*P2[binIndex]+2*ppY[binIndex]*F12*F2[binIndex]*P2[binIndex]-8*ppY[binIndex]*F12*F2[binIndex]*P2[binIndex]*P1[binIndex]-2*F1[binIndex]*P1[binIndex]*mmY[binIndex]*P2[binIndex]-2*ppY[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]+8*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]+mmY[binIndex]*P2[binIndex]*F1[binIndex]+ppY[binIndex]*F1[binIndex]*F2[binIndex]-ppY[binIndex]*F2[binIndex]*P12+4*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P12+4*ppY[binIndex]*F12*F2[binIndex]*P1[binIndex]-4*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]+ppY[binIndex]*F2[binIndex]*P1[binIndex]-4*ppY[binIndex]*F12*F2[binIndex]*P12-ppY[binIndex]*F12*F2[binIndex])/(-F1[binIndex]*F2[binIndex]+2*F2[binIndex]*P1[binIndex]*P2[binIndex]+3*F1[binIndex]*F2[binIndex]*P1[binIndex]-2*F1[binIndex]*F2[binIndex]*P22-2*P22*F1[binIndex]*P1[binIndex]+2*P2[binIndex]*F1[binIndex]*P1[binIndex]+3*F1[binIndex]*F2[binIndex]*P2[binIndex]-P2[binIndex]*F1[binIndex]+P22*F1[binIndex]+F2[binIndex]*P12-2*F2[binIndex]*P12*P2[binIndex]-2*F1[binIndex]*F2[binIndex]*P12-F2[binIndex]*P1[binIndex]-8*F1[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]+4*F1[binIndex]*F2[binIndex]*P1[binIndex]*P22+4*F1[binIndex]*F2[binIndex]*P12*P2[binIndex]);
          const double dI00 = -F2[binIndex]*(-2*P2[binIndex]*F1[binIndex]+2*P12*P2[binIndex]+8*P2[binIndex]*F1[binIndex]*P1[binIndex]-2*P1[binIndex]*P2[binIndex]+2*P2[binIndex]*F12-8*P2[binIndex]*F12*P1[binIndex]-8*P2[binIndex]*F1[binIndex]*P12+8*P2[binIndex]*F12*P12-4*F1[binIndex]*P1[binIndex]-F12+4*F12*P1[binIndex]+P1[binIndex]+F1[binIndex]-P12+4*F1[binIndex]*P12-4*F12*P12)/(-P2[binIndex]*F1[binIndex]+3*F1[binIndex]*F2[binIndex]*P2[binIndex]-2*P22*F1[binIndex]*P1[binIndex]-2*F1[binIndex]*F2[binIndex]*P22-2*F2[binIndex]*P12*P2[binIndex]-2*F1[binIndex]*F2[binIndex]*P12+2*P2[binIndex]*F1[binIndex]*P1[binIndex]+P22*F1[binIndex]+F2[binIndex]*P12+3*F1[binIndex]*F2[binIndex]*P1[binIndex]+2*F2[binIndex]*P1[binIndex]*P2[binIndex]-F1[binIndex]*F2[binIndex]-F2[binIndex]*P1[binIndex]-8*F1[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]+4*F1[binIndex]*F2[binIndex]*P1[binIndex]*P22+4*F1[binIndex]*F2[binIndex]*P12*P2[binIndex]);
          const double dI11 = -P2[binIndex]*F1[binIndex]*(1-2*P1[binIndex]-P2[binIndex]+2*P1[binIndex]*P2[binIndex])/(-P2[binIndex]*F1[binIndex]+3*F1[binIndex]*F2[binIndex]*P2[binIndex]-2*P22*F1[binIndex]*P1[binIndex]-2*F1[binIndex]*F2[binIndex]*P22-2*F2[binIndex]*P12*P2[binIndex]-2*F1[binIndex]*F2[binIndex]*P12+2*P2[binIndex]*F1[binIndex]*P1[binIndex]+P22*F1[binIndex]+F2[binIndex]*P12+3*F1[binIndex]*F2[binIndex]*P1[binIndex]+2*F2[binIndex]*P1[binIndex]*P2[binIndex]-F1[binIndex]*F2[binIndex]-F2[binIndex]*P1[binIndex]-8*F1[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]+4*F1[binIndex]*F2[binIndex]*P1[binIndex]*P22+4*F1[binIndex]*F2[binIndex]*P12*P2[binIndex]);
          const double divisor1 = (-P2[binIndex]*F1[binIndex]+3*F1[binIndex]*F2[binIndex]*P2[binIndex]-2*P22*F1[binIndex]*P1[binIndex]-2*F1[binIndex]*F2[binIndex]*P22-2*F2[binIndex]*P12*P2[binIndex]-2*F1[binIndex]*F2[binIndex]*P12+2*P2[binIndex]*F1[binIndex]*P1[binIndex]+P22*F1[binIndex]+F2[binIndex]*P12+3*F1[binIndex]*F2[binIndex]*P1[binIndex]+2*F2[binIndex]*P1[binIndex]*P2[binIndex]-F1[binIndex]*F2[binIndex]-F2[binIndex]*P1[binIndex]-8*F1[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]+4*F1[binIndex]*F2[binIndex]*P1[binIndex]*P22+4*F1[binIndex]*F2[binIndex]*P12*P2[binIndex]);
          const double dF1 = -F2[binIndex]*(-P1[binIndex]*mmY[binIndex]*P2[binIndex]+4*ppY[binIndex]*F2[binIndex]*P1[binIndex]*P22-ppY[binIndex]*F2[binIndex]*P12*P2[binIndex]-10*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P12-8*ppY[binIndex]*F2[binIndex]*P12*P22+2*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]-ppY[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]-32*ppY[binIndex]*F12*F2[binIndex]*P14*P2[binIndex]+32*ppY[binIndex]*F2[binIndex]*P14*P2[binIndex]*F1[binIndex]-32*ppY[binIndex]*F2[binIndex]*P14*P22*F1[binIndex]+32*ppY[binIndex]*F12*F2[binIndex]*P14*P22+32*ppY[binIndex]*F12*F2[binIndex]*P13*P23+2*ppY[binIndex]*F2[binIndex]*P14+4*ppY[binIndex]*P13*P23-4*P13*mmY[binIndex]*P23-8*ppY[binIndex]*F2[binIndex]*P13*P23-16*ppY[binIndex]*P23*F12*P13+8*ppY[binIndex]*F12*F2[binIndex]*P14-8*ppY[binIndex]*F2[binIndex]*P14*P2[binIndex]+8*ppY[binIndex]*F2[binIndex]*P14*P22-8*ppY[binIndex]*F2[binIndex]*P14*F1[binIndex]+10*ppY[binIndex]*F2[binIndex]*P13*P2[binIndex]-4*ppY[binIndex]*F2[binIndex]*P13*P22+16*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P13-4*ppY[binIndex]*F2[binIndex]*P1[binIndex]*P23+12*ppY[binIndex]*F2[binIndex]*P12*P23+18*ppY[binIndex]*P22*F12*P1[binIndex]-20*ppY[binIndex]*F12*F2[binIndex]*P13-36*ppY[binIndex]*P22*F12*P12+24*ppY[binIndex]*P22*F12*P13-6*ppY[binIndex]*P2[binIndex]*F12*P1[binIndex]-5*ppY[binIndex]*F12*F2[binIndex]*P2[binIndex]+8*ppY[binIndex]*F12*F2[binIndex]*P22-8*ppY[binIndex]*P2[binIndex]*F12*P13+12*ppY[binIndex]*P2[binIndex]*F12*P12+18*ppY[binIndex]*F12*F2[binIndex]*P12-7*ppY[binIndex]*F12*F2[binIndex]*P1[binIndex]-12*ppY[binIndex]*P23*F12*P1[binIndex]+24*ppY[binIndex]*P23*F12*P12-4*ppY[binIndex]*F12*F2[binIndex]*P23-3*ppY[binIndex]*P1[binIndex]*P22+ppY[binIndex]*F2[binIndex]*P12-3*ppY[binIndex]*P12*P2[binIndex]+3*P12*mmY[binIndex]*P2[binIndex]-9*P12*mmY[binIndex]*P22+9*ppY[binIndex]*P12*P22+ppY[binIndex]*P1[binIndex]*P2[binIndex]+3*P1[binIndex]*mmY[binIndex]*P22-8*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]+8*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]*P22+40*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P12*P2[binIndex]-40*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P12*P22-64*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P13*P2[binIndex]+64*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P13*P22+34*ppY[binIndex]*F12*F2[binIndex]*P2[binIndex]*P1[binIndex]-52*ppY[binIndex]*F12*F2[binIndex]*P22*P1[binIndex]-84*ppY[binIndex]*F12*F2[binIndex]*P12*P2[binIndex]+120*ppY[binIndex]*F12*F2[binIndex]*P12*P22+88*ppY[binIndex]*F12*F2[binIndex]*P13*P2[binIndex]-112*ppY[binIndex]*F12*F2[binIndex]*P13*P22+24*ppY[binIndex]*F12*F2[binIndex]*P23*P1[binIndex]-48*ppY[binIndex]*F12*F2[binIndex]*P12*P23+2*ppY[binIndex]*P13*P2[binIndex]-6*ppY[binIndex]*P13*P22-3*ppY[binIndex]*F2[binIndex]*P13+2*ppY[binIndex]*P1[binIndex]*P23-6*ppY[binIndex]*P12*P23+ppY[binIndex]*P2[binIndex]*F12-3*ppY[binIndex]*P22*F12+ppY[binIndex]*F12*F2[binIndex]+2*ppY[binIndex]*P23*F12-2*P13*mmY[binIndex]*P2[binIndex]+6*P13*mmY[binIndex]*P22+6*P12*mmY[binIndex]*P23-2*P1[binIndex]*mmY[binIndex]*P23)/(divisor1 * divisor1);
          const double divisor2 = (-P2[binIndex]*F1[binIndex]+3*F1[binIndex]*F2[binIndex]*P2[binIndex]-2*P22*F1[binIndex]*P1[binIndex]-2*F1[binIndex]*F2[binIndex]*P22-2*F2[binIndex]*P12*P2[binIndex]-2*F1[binIndex]*F2[binIndex]*P12+2*P2[binIndex]*F1[binIndex]*P1[binIndex]+P22*F1[binIndex]+F2[binIndex]*P12+3*F1[binIndex]*F2[binIndex]*P1[binIndex]+2*F2[binIndex]*P1[binIndex]*P2[binIndex]-F1[binIndex]*F2[binIndex]-F2[binIndex]*P1[binIndex]-8*F1[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]+4*F1[binIndex]*F2[binIndex]*P1[binIndex]*P22+4*F1[binIndex]*F2[binIndex]*P12*P2[binIndex]);
          const double dF2 = P2[binIndex]*F1[binIndex]*(3*P1[binIndex]*mmY[binIndex]*P2[binIndex]-12*ppY[binIndex]*P22*F1[binIndex]*P1[binIndex]-36*ppY[binIndex]*P2[binIndex]*F1[binIndex]*P12+24*ppY[binIndex]*P22*F1[binIndex]*P12+18*ppY[binIndex]*P2[binIndex]*F1[binIndex]*P1[binIndex]+12*ppY[binIndex]*F1[binIndex]*P12+24*ppY[binIndex]*P2[binIndex]*F1[binIndex]*P13-16*ppY[binIndex]*P22*F1[binIndex]*P13+12*ppY[binIndex]*P22*F12*P1[binIndex]-24*ppY[binIndex]*P22*F12*P12+16*ppY[binIndex]*P22*F12*P13-18*ppY[binIndex]*P2[binIndex]*F12*P1[binIndex]-24*ppY[binIndex]*P2[binIndex]*F12*P13+36*ppY[binIndex]*P2[binIndex]*F12*P12-19*F1[binIndex]*P1[binIndex]*mmY[binIndex]*P2[binIndex]+28*F1[binIndex]*P12*mmY[binIndex]*P2[binIndex]-12*F1[binIndex]*P13*mmY[binIndex]*P2[binIndex]+22*F1[binIndex]*P1[binIndex]*mmY[binIndex]*P22-28*F1[binIndex]*P12*mmY[binIndex]*P22+8*F1[binIndex]*P13*mmY[binIndex]*P22-8*F1[binIndex]*P1[binIndex]*mmY[binIndex]*P23+8*F1[binIndex]*P12*mmY[binIndex]*P23-ppY[binIndex]*F12+2*ppY[binIndex]*P13-2*P13*mmY[binIndex]-mmY[binIndex]*F1[binIndex]+2*ppY[binIndex]*P1[binIndex]*P22+9*ppY[binIndex]*P12*P2[binIndex]-9*P12*mmY[binIndex]*P2[binIndex]+6*P12*mmY[binIndex]*P22-6*ppY[binIndex]*P12*P22-3*ppY[binIndex]*P1[binIndex]*P2[binIndex]-2*P1[binIndex]*mmY[binIndex]*P22-6*ppY[binIndex]*F1[binIndex]*P1[binIndex]+2*ppY[binIndex]*P22*F1[binIndex]-3*ppY[binIndex]*P2[binIndex]*F1[binIndex]-P1[binIndex]*mmY[binIndex]+ppY[binIndex]*P1[binIndex]-3*ppY[binIndex]*P12+ppY[binIndex]*F1[binIndex]+3*P12*mmY[binIndex]-6*ppY[binIndex]*P13*P2[binIndex]+4*ppY[binIndex]*P13*P22+3*ppY[binIndex]*P2[binIndex]*F12-2*ppY[binIndex]*P22*F12+5*F1[binIndex]*P1[binIndex]*mmY[binIndex]+6*ppY[binIndex]*F12*P1[binIndex]-8*F1[binIndex]*P12*mmY[binIndex]-12*F12*P12*ppY[binIndex]-8*ppY[binIndex]*F1[binIndex]*P13+6*P13*mmY[binIndex]*P2[binIndex]+4*F1[binIndex]*P13*mmY[binIndex]+8*F12*P13*ppY[binIndex]-4*P13*mmY[binIndex]*P22-5*mmY[binIndex]*P22*F1[binIndex]+2*mmY[binIndex]*P23*F1[binIndex]+4*mmY[binIndex]*P2[binIndex]*F1[binIndex])/(divisor2 * divisor2);
          const double divisor3 = (-P2[binIndex]*F1[binIndex]+3*F1[binIndex]*F2[binIndex]*P2[binIndex]-2*P22*F1[binIndex]*P1[binIndex]-2*F1[binIndex]*F2[binIndex]*P22-2*F2[binIndex]*P12*P2[binIndex]-2*F1[binIndex]*F2[binIndex]*P12+2*P2[binIndex]*F1[binIndex]*P1[binIndex]+P22*F1[binIndex]+F2[binIndex]*P12+3*F1[binIndex]*F2[binIndex]*P1[binIndex]+2*F2[binIndex]*P1[binIndex]*P2[binIndex]-F1[binIndex]*F2[binIndex]-F2[binIndex]*P1[binIndex]-8*F1[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]+4*F1[binIndex]*F2[binIndex]*P1[binIndex]*P22+4*F1[binIndex]*F2[binIndex]*P12*P2[binIndex]);
          const double dP1 = -F1[binIndex]*F2[binIndex]*(-2*P1[binIndex]*mmY[binIndex]*P2[binIndex]-2*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P2[binIndex]+8*ppY[binIndex]*F2[binIndex]*P1[binIndex]*P22+24*ppY[binIndex]*P22*F1[binIndex]*P1[binIndex]+8*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P22+8*ppY[binIndex]*P2[binIndex]*F1[binIndex]*P12+6*ppY[binIndex]*F2[binIndex]*P12*P2[binIndex]+4*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P12-24*ppY[binIndex]*P22*F1[binIndex]*P12-12*ppY[binIndex]*F2[binIndex]*P12*P22-8*ppY[binIndex]*P2[binIndex]*F1[binIndex]*P1[binIndex]-2*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]-2*ppY[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]+ppY[binIndex]*F2[binIndex]*P2[binIndex]-4*ppY[binIndex]*F2[binIndex]*P22-8*ppY[binIndex]*F2[binIndex]*P1[binIndex]*P23-16*ppY[binIndex]*P23*F1[binIndex]*P1[binIndex]-8*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P23+16*ppY[binIndex]*P23*F1[binIndex]*P12+8*ppY[binIndex]*F2[binIndex]*P12*P23-24*ppY[binIndex]*P22*F12*P1[binIndex]+24*ppY[binIndex]*P22*F12*P12+8*ppY[binIndex]*P2[binIndex]*F12*P1[binIndex]+6*ppY[binIndex]*F12*F2[binIndex]*P2[binIndex]-12*ppY[binIndex]*F12*F2[binIndex]*P22-8*ppY[binIndex]*P2[binIndex]*F12*P12-4*ppY[binIndex]*F12*F2[binIndex]*P12+4*ppY[binIndex]*F12*F2[binIndex]*P1[binIndex]+16*ppY[binIndex]*P23*F12*P1[binIndex]-16*ppY[binIndex]*P23*F12*P12+8*ppY[binIndex]*F12*F2[binIndex]*P23+4*F1[binIndex]*P1[binIndex]*mmY[binIndex]*P2[binIndex]-4*F1[binIndex]*P12*mmY[binIndex]*P2[binIndex]-12*F1[binIndex]*P1[binIndex]*mmY[binIndex]*P22+12*F1[binIndex]*P12*mmY[binIndex]*P22+8*F1[binIndex]*P1[binIndex]*mmY[binIndex]*P23-8*F1[binIndex]*P12*mmY[binIndex]*P23+2*mmY[binIndex]*P23-2*ppY[binIndex]*P23+4*ppY[binIndex]*F2[binIndex]*P23-6*ppY[binIndex]*P1[binIndex]*P22-ppY[binIndex]*F2[binIndex]*P12-2*ppY[binIndex]*P12*P2[binIndex]+2*P12*mmY[binIndex]*P2[binIndex]-6*P12*mmY[binIndex]*P22+6*ppY[binIndex]*P12*P22+2*ppY[binIndex]*P1[binIndex]*P2[binIndex]-ppY[binIndex]*P2[binIndex]+6*P1[binIndex]*mmY[binIndex]*P22-6*ppY[binIndex]*P22*F1[binIndex]+2*ppY[binIndex]*P2[binIndex]*F1[binIndex]+3*ppY[binIndex]*P22+16*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]-40*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]*P22-24*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P12*P2[binIndex]+48*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P12*P22+mmY[binIndex]*P2[binIndex]-3*mmY[binIndex]*P22+32*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]*P23-32*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P12*P23-24*ppY[binIndex]*F12*F2[binIndex]*P2[binIndex]*P1[binIndex]+48*ppY[binIndex]*F12*F2[binIndex]*P22*P1[binIndex]+24*ppY[binIndex]*F12*F2[binIndex]*P12*P2[binIndex]-48*ppY[binIndex]*F12*F2[binIndex]*P12*P22-32*ppY[binIndex]*F12*F2[binIndex]*P23*P1[binIndex]+32*ppY[binIndex]*F12*F2[binIndex]*P12*P23+4*ppY[binIndex]*P1[binIndex]*P23+4*ppY[binIndex]*P23*F1[binIndex]-4*ppY[binIndex]*P12*P23-2*ppY[binIndex]*P2[binIndex]*F12+6*ppY[binIndex]*P22*F12-ppY[binIndex]*F12*F2[binIndex]-4*ppY[binIndex]*P23*F12+4*P12*mmY[binIndex]*P23-4*P1[binIndex]*mmY[binIndex]*P23+3*mmY[binIndex]*P22*F1[binIndex]-2*mmY[binIndex]*P23*F1[binIndex]-mmY[binIndex]*P2[binIndex]*F1[binIndex])/(divisor3 * divisor3);
          const double divisor4 = (-P2[binIndex]*F1[binIndex]+3*F1[binIndex]*F2[binIndex]*P2[binIndex]-2*P22*F1[binIndex]*P1[binIndex]-2*F1[binIndex]*F2[binIndex]*P22-2*F2[binIndex]*P12*P2[binIndex]-2*F1[binIndex]*F2[binIndex]*P12+2*P2[binIndex]*F1[binIndex]*P1[binIndex]+P22*F1[binIndex]+F2[binIndex]*P12+3*F1[binIndex]*F2[binIndex]*P1[binIndex]+2*F2[binIndex]*P1[binIndex]*P2[binIndex]-F1[binIndex]*F2[binIndex]-F2[binIndex]*P1[binIndex]-8*F1[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]+4*F1[binIndex]*F2[binIndex]*P1[binIndex]*P22+4*F1[binIndex]*F2[binIndex]*P12*P2[binIndex]);
          const double dP2 = F1[binIndex]*F2[binIndex]*(-2*P1[binIndex]*mmY[binIndex]*P2[binIndex]-4*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P2[binIndex]+4*ppY[binIndex]*F2[binIndex]*P1[binIndex]*P22+12*ppY[binIndex]*P22*F1[binIndex]*P1[binIndex]+4*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P22+24*ppY[binIndex]*P2[binIndex]*F1[binIndex]*P12+12*ppY[binIndex]*F2[binIndex]*P12*P2[binIndex]+12*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P12-24*ppY[binIndex]*P22*F1[binIndex]*P12-12*ppY[binIndex]*F2[binIndex]*P12*P22-12*ppY[binIndex]*P2[binIndex]*F1[binIndex]*P1[binIndex]-6*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]-4*ppY[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]-12*ppY[binIndex]*F1[binIndex]*P12-16*ppY[binIndex]*P2[binIndex]*F1[binIndex]*P13+16*ppY[binIndex]*P22*F1[binIndex]*P13-8*ppY[binIndex]*F2[binIndex]*P13*P2[binIndex]+8*ppY[binIndex]*F2[binIndex]*P13*P22-8*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P13-12*ppY[binIndex]*P22*F12*P1[binIndex]+8*ppY[binIndex]*F12*F2[binIndex]*P13+24*ppY[binIndex]*P22*F12*P12-16*ppY[binIndex]*P22*F12*P13+12*ppY[binIndex]*P2[binIndex]*F12*P1[binIndex]+4*ppY[binIndex]*F12*F2[binIndex]*P2[binIndex]-4*ppY[binIndex]*F12*F2[binIndex]*P22+16*ppY[binIndex]*P2[binIndex]*F12*P13-24*ppY[binIndex]*P2[binIndex]*F12*P12-12*ppY[binIndex]*F12*F2[binIndex]*P12+6*ppY[binIndex]*F12*F2[binIndex]*P1[binIndex]+10*F1[binIndex]*P1[binIndex]*mmY[binIndex]*P2[binIndex]-16*F1[binIndex]*P12*mmY[binIndex]*P2[binIndex]+8*F1[binIndex]*P13*mmY[binIndex]*P2[binIndex]-6*F1[binIndex]*P1[binIndex]*mmY[binIndex]*P22+12*F1[binIndex]*P12*mmY[binIndex]*P22-8*F1[binIndex]*P13*mmY[binIndex]*P22+ppY[binIndex]*F12-2*ppY[binIndex]*P13+2*P13*mmY[binIndex]+mmY[binIndex]*F1[binIndex]-2*ppY[binIndex]*P1[binIndex]*P22+ppY[binIndex]*F2[binIndex]*P1[binIndex]-3*ppY[binIndex]*F2[binIndex]*P12-6*ppY[binIndex]*P12*P2[binIndex]+6*P12*mmY[binIndex]*P2[binIndex]-6*P12*mmY[binIndex]*P22+6*ppY[binIndex]*P12*P22+2*ppY[binIndex]*P1[binIndex]*P2[binIndex]+ppY[binIndex]*F1[binIndex]*F2[binIndex]+2*P1[binIndex]*mmY[binIndex]*P22+6*ppY[binIndex]*F1[binIndex]*P1[binIndex]-2*ppY[binIndex]*P22*F1[binIndex]+2*ppY[binIndex]*P2[binIndex]*F1[binIndex]+24*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]-24*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]*P22-48*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P12*P2[binIndex]+48*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P12*P22+P1[binIndex]*mmY[binIndex]-ppY[binIndex]*P1[binIndex]+3*ppY[binIndex]*P12-ppY[binIndex]*F1[binIndex]-3*P12*mmY[binIndex]+32*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P13*P2[binIndex]-32*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P13*P22-24*ppY[binIndex]*F12*F2[binIndex]*P2[binIndex]*P1[binIndex]+24*ppY[binIndex]*F12*F2[binIndex]*P22*P1[binIndex]+48*ppY[binIndex]*F12*F2[binIndex]*P12*P2[binIndex]-48*ppY[binIndex]*F12*F2[binIndex]*P12*P22-32*ppY[binIndex]*F12*F2[binIndex]*P13*P2[binIndex]+32*ppY[binIndex]*F12*F2[binIndex]*P13*P22+4*ppY[binIndex]*P13*P2[binIndex]-4*ppY[binIndex]*P13*P22+2*ppY[binIndex]*F2[binIndex]*P13-2*ppY[binIndex]*P2[binIndex]*F12+2*ppY[binIndex]*P22*F12-ppY[binIndex]*F12*F2[binIndex]-5*F1[binIndex]*P1[binIndex]*mmY[binIndex]-6*ppY[binIndex]*F12*P1[binIndex]+8*F1[binIndex]*P12*mmY[binIndex]+12*F12*P12*ppY[binIndex]+8*ppY[binIndex]*F1[binIndex]*P13-4*P13*mmY[binIndex]*P2[binIndex]-4*F1[binIndex]*P13*mmY[binIndex]-8*F12*P13*ppY[binIndex]+4*P13*mmY[binIndex]*P22+mmY[binIndex]*P22*F1[binIndex]-2*mmY[binIndex]*P2[binIndex]*F1[binIndex])/(divisor4 * divisor4);
          const double e1 = dI00 * ppE[binIndex];
          const double e2 = dI11 * mmE[binIndex];
          const double e3 = dF1 * F1E[binIndex];
          const double e4 = dF2 * F2E[binIndex];
          const double e5 = dP1 * P1E[binIndex];
          const double e6 = dP2 * P2E[binIndex];
          mpE[binIndex] = std::sqrt(e1 * e1 + e2 * e2 + e3 * e3 + e4 * e4 + e5 * e5 + e6 * e6);
        }
        {
          pmY[binIndex] = -(ppY[binIndex]*P2[binIndex]*F1[binIndex]-2*ppY[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]-2*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P2[binIndex]-2*ppY[binIndex]*P2[binIndex]*F1[binIndex]*P1[binIndex]+2*P1[binIndex]*mpY[binIndex]*F2[binIndex]*P2[binIndex]+ppY[binIndex]*P1[binIndex]*P2[binIndex]-P1[binIndex]*mpY[binIndex]*P2[binIndex]+4*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]+P1[binIndex]*mmY[binIndex]*P2[binIndex]-ppY[binIndex]*F1[binIndex]+2*ppY[binIndex]*F1[binIndex]*P1[binIndex]-P1[binIndex]*mmY[binIndex]-P1[binIndex]*mpY[binIndex]*F2[binIndex]+ppY[binIndex]*F2[binIndex]*P1[binIndex]+ppY[binIndex]*F1[binIndex]*F2[binIndex]-2*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]+P1[binIndex]*mpY[binIndex]-ppY[binIndex]*P1[binIndex])/((-P1[binIndex]+2*F1[binIndex]*P1[binIndex]-F1[binIndex])*(-1+P2[binIndex]));
          const double dI00 = -(-P1[binIndex]+P1[binIndex]*P2[binIndex]+F2[binIndex]*P1[binIndex]-2*F2[binIndex]*P1[binIndex]*P2[binIndex]+2*F1[binIndex]*P1[binIndex]-2*P2[binIndex]*F1[binIndex]*P1[binIndex]-2*F1[binIndex]*F2[binIndex]*P1[binIndex]+4*F1[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]+F1[binIndex]*F2[binIndex]-F1[binIndex]+P2[binIndex]*F1[binIndex]-2*F1[binIndex]*F2[binIndex]*P2[binIndex])/((-P1[binIndex]+2*F1[binIndex]*P1[binIndex]-F1[binIndex])*(-1+P2[binIndex]));
          const double dI11 = -(P1[binIndex]*P2[binIndex]-P1[binIndex])/((-P1[binIndex]+2*F1[binIndex]*P1[binIndex]-F1[binIndex])*(-1+P2[binIndex]));
          const double dI10 = -(P1[binIndex]-P1[binIndex]*P2[binIndex]-F2[binIndex]*P1[binIndex]+2*F2[binIndex]*P1[binIndex]*P2[binIndex])/((-P1[binIndex]+2*F1[binIndex]*P1[binIndex]-F1[binIndex])*(-1+P2[binIndex]));
          const double factor1 = (-P1[binIndex]+2*F1[binIndex]*P1[binIndex]-F1[binIndex]);
          const double dF1 = -(ppY[binIndex]*P2[binIndex]-2*ppY[binIndex]*F2[binIndex]*P2[binIndex]-2*ppY[binIndex]*P1[binIndex]*P2[binIndex]+4*ppY[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]-ppY[binIndex]+2*ppY[binIndex]*P1[binIndex]+ppY[binIndex]*F2[binIndex]-2*ppY[binIndex]*F2[binIndex]*P1[binIndex])/((-P1[binIndex]+2*F1[binIndex]*P1[binIndex]-F1[binIndex])*(-1+P2[binIndex]))+(ppY[binIndex]*P2[binIndex]*F1[binIndex]-2*ppY[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]-2*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P2[binIndex]-2*ppY[binIndex]*P2[binIndex]*F1[binIndex]*P1[binIndex]+2*P1[binIndex]*mpY[binIndex]*F2[binIndex]*P2[binIndex]+ppY[binIndex]*P1[binIndex]*P2[binIndex]-P1[binIndex]*mpY[binIndex]*P2[binIndex]+4*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]+P1[binIndex]*mmY[binIndex]*P2[binIndex]-ppY[binIndex]*F1[binIndex]+2*ppY[binIndex]*F1[binIndex]*P1[binIndex]-P1[binIndex]*mmY[binIndex]-P1[binIndex]*mpY[binIndex]*F2[binIndex]+ppY[binIndex]*F2[binIndex]*P1[binIndex]+ppY[binIndex]*F1[binIndex]*F2[binIndex]-2*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]+P1[binIndex]*mpY[binIndex]-ppY[binIndex]*P1[binIndex])*(-1+2*P1[binIndex])/((factor1 * factor1)*(-1+P2[binIndex]));
          const double dF2 = -(-2*ppY[binIndex]*P1[binIndex]*P2[binIndex]-2*ppY[binIndex]*P2[binIndex]*F1[binIndex]+2*P1[binIndex]*mpY[binIndex]*P2[binIndex]+4*ppY[binIndex]*P2[binIndex]*F1[binIndex]*P1[binIndex]-P1[binIndex]*mpY[binIndex]+ppY[binIndex]*P1[binIndex]+ppY[binIndex]*F1[binIndex]-2*ppY[binIndex]*F1[binIndex]*P1[binIndex])/((-P1[binIndex]+2*F1[binIndex]*P1[binIndex]-F1[binIndex])*(-1+P2[binIndex]));
          const double factor2 = (-P1[binIndex]+2*F1[binIndex]*P1[binIndex]-F1[binIndex]);
          const double dP1 = -(-2*ppY[binIndex]*F2[binIndex]*P2[binIndex]-2*ppY[binIndex]*P2[binIndex]*F1[binIndex]+2*mpY[binIndex]*F2[binIndex]*P2[binIndex]+ppY[binIndex]*P2[binIndex]-mpY[binIndex]*P2[binIndex]+4*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P2[binIndex]+mmY[binIndex]*P2[binIndex]+2*ppY[binIndex]*F1[binIndex]-mmY[binIndex]-mpY[binIndex]*F2[binIndex]+ppY[binIndex]*F2[binIndex]-2*ppY[binIndex]*F1[binIndex]*F2[binIndex]+mpY[binIndex]-ppY[binIndex])/((-P1[binIndex]+2*F1[binIndex]*P1[binIndex]-F1[binIndex])*(-1+P2[binIndex]))+(ppY[binIndex]*P2[binIndex]*F1[binIndex]-2*ppY[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]-2*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P2[binIndex]-2*ppY[binIndex]*P2[binIndex]*F1[binIndex]*P1[binIndex]+2*P1[binIndex]*mpY[binIndex]*F2[binIndex]*P2[binIndex]+ppY[binIndex]*P1[binIndex]*P2[binIndex]-P1[binIndex]*mpY[binIndex]*P2[binIndex]+4*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]+P1[binIndex]*mmY[binIndex]*P2[binIndex]-ppY[binIndex]*F1[binIndex]+2*ppY[binIndex]*F1[binIndex]*P1[binIndex]-P1[binIndex]*mmY[binIndex]-P1[binIndex]*mpY[binIndex]*F2[binIndex]+ppY[binIndex]*F2[binIndex]*P1[binIndex]+ppY[binIndex]*F1[binIndex]*F2[binIndex]-2*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]+P1[binIndex]*mpY[binIndex]-ppY[binIndex]*P1[binIndex])*(-1+2*F1[binIndex])/((factor2*factor2)*(-1+P2[binIndex]));
          const double factor3 = (-1+P2[binIndex]);
          const double dP2 = -(ppY[binIndex]*F1[binIndex]-2*ppY[binIndex]*F2[binIndex]*P1[binIndex]-2*ppY[binIndex]*F1[binIndex]*F2[binIndex]-2*ppY[binIndex]*F1[binIndex]*P1[binIndex]+2*P1[binIndex]*mpY[binIndex]*F2[binIndex]+ppY[binIndex]*P1[binIndex]-P1[binIndex]*mpY[binIndex]+4*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]+P1[binIndex]*mmY[binIndex])/((-P1[binIndex]+2*F1[binIndex]*P1[binIndex]-F1[binIndex])*(-1+P2[binIndex]))+(ppY[binIndex]*P2[binIndex]*F1[binIndex]-2*ppY[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]-2*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P2[binIndex]-2*ppY[binIndex]*P2[binIndex]*F1[binIndex]*P1[binIndex]+2*P1[binIndex]*mpY[binIndex]*F2[binIndex]*P2[binIndex]+ppY[binIndex]*P1[binIndex]*P2[binIndex]-P1[binIndex]*mpY[binIndex]*P2[binIndex]+4*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]*P2[binIndex]+P1[binIndex]*mmY[binIndex]*P2[binIndex]-ppY[binIndex]*F1[binIndex]+2*ppY[binIndex]*F1[binIndex]*P1[binIndex]-P1[binIndex]*mmY[binIndex]-P1[binIndex]*mpY[binIndex]*F2[binIndex]+ppY[binIndex]*F2[binIndex]*P1[binIndex]+ppY[binIndex]*F1[binIndex]*F2[binIndex]-2*ppY[binIndex]*F1[binIndex]*F2[binIndex]*P1[binIndex]+P1[binIndex]*mpY[binIndex]-ppY[binIndex]*P1[binIndex])/((-P1[binIndex]+2*F1[binIndex]*P1[binIndex]-F1[binIndex])*(factor3 * factor3));
          const double e1 = dI00 * ppE[binIndex];
          const double e2 = dI11 * mmE[binIndex];
          const double e3 = dI10 * mpE[binIndex];
          const double e4 = dF1 * F1E[binIndex];
          const double e5 = dF2 * F2E[binIndex];
          const double e6 = dP1 * P1E[binIndex];
          const double e7 = dP2 * P2E[binIndex];
          pmE[binIndex] = std::sqrt(e1 * e1 + e2 * e2 + e3 * e3 + e4 * e4 + e5 * e5 + e6 * e6 + e7 * e7);

        }
      }
    }
  }
};


#endif /* MANTID_ALGORITHMS_POLARIZATIONEFFICIENCYCORTEST_H_ */
