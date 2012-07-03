#ifndef RUNTEST_H_
#define RUNTEST_H_

#include "MantidAPI/Run.h"
#include "MantidKernel/Exception.h"
#include "MantidKernel/Matrix.h"
#include "MantidKernel/Property.h"
#include "MantidKernel/TimeSeriesProperty.h"
#include "MantidKernel/V3D.h"
#include <cxxtest/TestSuite.h>
#include "MantidKernel/NexusTestHelper.h"

using namespace Mantid::Kernel;
using namespace Mantid::API;
using namespace Mantid::Geometry;
using namespace Mantid;
using Mantid::Kernel::NexusTestHelper;

// Helper class
namespace
{
  class ConcreteProperty : public Property
  {
  public:
    ConcreteProperty() : Property( "Test", typeid( int ) ) {}
    Property* clone() { return new ConcreteProperty(*this); }
    bool isDefault() const { return true; }
    std::string getDefault() const { return "getDefault() is not implemented in this class"; }
    std::string value() const { return "Nothing"; }
    std::string setValue( const std::string& ) { return ""; }
    std::string setValueFromProperty( const Property& ) { return ""; }
    std::string setDataItem(const boost::shared_ptr<DataItem>) { return ""; } 
    Property& operator+=( Property const * ) { return *this; }
  };
}


class RunTest : public CxxTest::TestSuite
{
 
public:
  void testAddGetData()
  {
    Run runInfo;

    Property *p = new ConcreteProperty();
    TS_ASSERT_THROWS_NOTHING( runInfo.addProperty(p) );

    Property *pp = NULL;
    TS_ASSERT_THROWS_NOTHING( pp = runInfo.getProperty("Test") );
    TS_ASSERT_EQUALS( p, pp );
    TS_ASSERT( ! pp->name().compare("Test") );
    TS_ASSERT( dynamic_cast<ConcreteProperty*>(pp) );
    TS_ASSERT_THROWS( pp = runInfo.getProperty("NotThere"), Exception::NotFoundError );

    std::vector< Property* > props = runInfo.getProperties();
    TS_ASSERT( ! props.empty() );
    TS_ASSERT_EQUALS( props.size(), 1 );
    TS_ASSERT( ! props[0]->name().compare("Test") );
    TS_ASSERT( dynamic_cast<ConcreteProperty*>(props[0]) );
  }

  void testRemoveLogData()
  {
    Run runInfo;
    
    Property *p = new ConcreteProperty();
    TS_ASSERT_THROWS_NOTHING( runInfo.addProperty(p) );
    TS_ASSERT_THROWS_NOTHING( runInfo.removeProperty("Test") );
    TS_ASSERT_EQUALS( runInfo.getProperties().size(), 0 );
  }

  void testGetSetProtonCharge()
  {
    Run runInfo;
    TS_ASSERT_THROWS(runInfo.getProtonCharge(), Exception::NotFoundError);
    TS_ASSERT_THROWS_NOTHING( runInfo.setProtonCharge(10.0) );
    TS_ASSERT_EQUALS( runInfo.getProtonCharge(), 10.0 );
  }

  void testCopyAndAssignment()
  {
    Run runInfo;
    runInfo.setProtonCharge(10.0);
    Property *p = new ConcreteProperty();
    TS_ASSERT_THROWS_NOTHING( runInfo.addProperty(p) );
    TS_ASSERT_EQUALS( runInfo.getProperties().size(), 2);
    
    //Copy constructor
    Run runInfo_2(runInfo);
    TS_ASSERT_EQUALS( runInfo_2.getProperties().size(), 2);
    TS_ASSERT_DELTA( runInfo_2.getProtonCharge(), 10.0, 1e-8);
    TS_ASSERT_EQUALS( runInfo_2.getLogData("Test")->value(), "Nothing" );    


    // Now assignment
    runInfo.setProtonCharge(15.0);
    runInfo.removeProperty("Test");
    runInfo_2 = runInfo;
    TS_ASSERT_EQUALS( runInfo_2.getProperties().size(), 1);
    TS_ASSERT_DELTA( runInfo_2.getProtonCharge(), 15.0, 1e-8);
  }

  void testMemory()
  {
    Run runInfo;
    TS_ASSERT_EQUALS( runInfo.getMemorySize(), 0);
    
    Property *p = new ConcreteProperty();
    runInfo.addProperty(p);
    TS_ASSERT_EQUALS( runInfo.getMemorySize(), sizeof(ConcreteProperty) + sizeof( void *));
  }

  void test_GetTimeSeriesProperty_Returns_TSP_When_Log_Exists()
  {
    Run runInfo;
    const std::string & name = "double_time_series";
    const double value = 10.9;
    addTimeSeriesEntry(runInfo, name, value);

    TimeSeriesProperty<double> * tsp(NULL);
    TS_ASSERT_THROWS_NOTHING(tsp = runInfo.getTimeSeriesProperty<double>(name));
    TS_ASSERT_DELTA(tsp->firstValue(), value, 1e-12);
  }

  void test_GetTimeSeriesProperty_Throws_When_Log_Does_Not_Exist()
  {
    Run runInfo;
    TS_ASSERT_THROWS(runInfo.getTimeSeriesProperty<double>("not_a_log"), Exception::NotFoundError);
  }

  void test_GetTimeSeriesProperty_Throws_When_Log_Exists_But_Is_Not_Correct_Type()
  {
    Run runInfo;
    const std::string & name = "double_prop";
    runInfo.addProperty(name, 5.6); // Standard double property

    TS_ASSERT_THROWS(runInfo.getTimeSeriesProperty<double>(name), std::invalid_argument);
  }

  void test_GetPropertyAsType_Throws_When_Property_Does_Not_Exist()
  {
    Run runInfo;
    TS_ASSERT_THROWS(runInfo.getPropertyValueAsType<double>("not_a_log"), Exception::NotFoundError);
  }

  void test_GetPropertyAsType_Returns_Expected_Value_When_Type_Is_Correct()
  {
    Run runInfo;
    const std::string & name = "double_prop";
    const double value = 5.6;
    runInfo.addProperty(name, value); // Standard double property

    double retrieved(0.0);
    TS_ASSERT_THROWS_NOTHING(retrieved = runInfo.getPropertyValueAsType<double>(name));
    TS_ASSERT_DELTA(retrieved, value, 1e-12);
  }

  void test_GetPropertyAsType_Throws_When_Requested_Type_Does_Not_Match()
  {
    Run runInfo;
    runInfo.addProperty("double_prop", 6.7); // Standard double property

    TS_ASSERT_THROWS(runInfo.getPropertyValueAsType<int>("double_prop"), std::invalid_argument);
  }


  void test_getGoniometer()
  {
    Run runInfo;
    TS_ASSERT_THROWS_NOTHING( runInfo.getGoniometer() );
    // No axes by default
    TS_ASSERT_EQUALS( runInfo.getGoniometer().getNumberAxes(), 0 );
    // Now does copy work?
    runInfo.getGoniometer().makeUniversalGoniometer();
    TS_ASSERT_EQUALS( runInfo.getGoniometer().getNumberAxes(), 3 );
    Run runCopy(runInfo);
    TS_ASSERT_EQUALS( runCopy.getGoniometer().getNumberAxes(), 3 );
    runCopy = runInfo;
    TS_ASSERT_EQUALS( runCopy.getGoniometer().getNumberAxes(), 3 );
  }


  void addTimeSeriesEntry(Run & runInfo, std::string name, double val)
  {
    TimeSeriesProperty<double> * tsp;
    tsp = new TimeSeriesProperty<double>(name);
    tsp->addValue("2011-05-24T00:00:00", val);
    runInfo.addProperty(tsp);
  }


  /** Setting up a goniometer and the angles to feed it
   * using sample logs, then getting the right rotation matrix out.
   */
  void test_getGoniometerMatrix()
  {
    Run runInfo;
    addTimeSeriesEntry(runInfo, "phi", 90.0);
    addTimeSeriesEntry(runInfo, "chi", 90.0);
    addTimeSeriesEntry(runInfo, "omega", 90.0);
    runInfo.getGoniometer().makeUniversalGoniometer();
    DblMatrix r = runInfo.getGoniometerMatrix();
    V3D rot = r * V3D(-1,0,0);
    TS_ASSERT_EQUALS( rot, V3D(1, 0, 0));
    rot = r * V3D(0,0,1);
    TS_ASSERT_EQUALS( rot, V3D(0, 1, 0));
  }

  void test_getGoniometerMatrix2()
  {
    Run runInfo;
    addTimeSeriesEntry(runInfo, "phi", 45.0);
    addTimeSeriesEntry(runInfo, "chi", 90.0);
    addTimeSeriesEntry(runInfo, "omega", 0.0);
    runInfo.getGoniometer().makeUniversalGoniometer();
    DblMatrix r = runInfo.getGoniometerMatrix();
    V3D rot = r * V3D(-1,0,0);
    TS_ASSERT_EQUALS( rot, V3D(0, -sqrt(0.5), sqrt(0.5)));
  }


  /** Save and load to NXS file */
  void test_nexus()
  {
    NexusTestHelper th(true);
    th.createFile("RunTest.nxs");

    Run run1;
    run1.getGoniometer().makeUniversalGoniometer();
    addTimeSeriesEntry(run1, "double_series", 45.0);
    run1.addProperty( new PropertyWithValue<int>("int_val", 1234) );
    run1.addProperty( new PropertyWithValue<std::string>("string_val", "help_im_stuck_in_a_log_file") );
    run1.addProperty( new PropertyWithValue<double>("double_val", 5678.9) );
    addTimeSeriesEntry(run1, "phi", 12.3);
    addTimeSeriesEntry(run1, "chi", 45.6);
    addTimeSeriesEntry(run1, "omega", 78.9);
    addTimeSeriesEntry(run1, "proton_charge", 78.9);

    run1.saveNexus(th.file, "logs");
    th.file->openGroup("logs", "NXgroup");
    th.file->makeGroup("junk_to_ignore", "NXmaterial");
    th.file->makeGroup("more_junk_to_ignore", "NXsample");

    // ---- Now re-load the same and compare ------
    th.reopenFile();
    Run run2;
    run2.loadNexus(th.file, "logs");
    TS_ASSERT( run2.hasProperty("double_series") );
    TS_ASSERT( run2.hasProperty("int_val") );
    TS_ASSERT( run2.hasProperty("string_val") );
    TS_ASSERT( run2.hasProperty("double_val") );
    // This test both uses the goniometer axes AND looks up some values.
    TS_ASSERT_EQUALS( run2.getGoniometerMatrix(), run1.getGoniometerMatrix() );

    // Reload without opening the group (for backwards-compatible reading of old files)
    Run run3;
    th.file->openGroup("logs", "NXgroup");
    run3.loadNexus(th.file, "");
    TS_ASSERT( run3.hasProperty("double_series") );
    TS_ASSERT( run3.hasProperty("int_val") );
    TS_ASSERT( run3.hasProperty("string_val") );
    TS_ASSERT( run3.hasProperty("double_val") );
  }

  /** Check for loading the old way of saving proton_charge */
  void test_legacy_nexus()
  {
    NexusTestHelper th(true);
    th.createFile("RunTest.nxs");
    th.file->makeGroup("sample", "NXsample", 1);
    th.file->writeData("proton_charge", 1.234);
    th.reopenFile();
    th.file->openGroup("sample", "NXsample");
    Run run3;
    run3.loadNexus(th.file, "");

    TS_ASSERT_DELTA( run3.getProtonCharge(), 1.234, 1e-5 );
  }


};



#endif
