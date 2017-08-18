/*!
 * @file TestEBLModels.cc
 * @brief Unit test for extragalactic background light absorption models.
 * @author Segev BenZvi
 * @date 17 Feb 2014
 * @ingroup unit_test
 * @version $Id: TestEBLModels.cc 19516 2014-04-16 17:14:36Z sybenzvi $
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/output_test_stream.hpp>
#include <boost/format.hpp>

#include <hawcnest/HAWCNest.h>
#include <hawcnest/HAWCUnits.h>
#include <hawcnest/test/OutputConfig.h>

#include <grmodel-services/ebl/EBLAbsorptionService.h>

#include <cmath>

using boost::test_tools::output_test_stream;
using namespace HAWCUnits;
using namespace std;

BOOST_AUTO_TEST_SUITE(EBLAbsorptionModels)

  // Global logger settings.
  // Note: this will affect *all* unit tests in the detector-services project.
  BOOST_GLOBAL_FIXTURE(OutputConfig);

  // ___________________________________________________________________________
  // Check the Gilmore EBL model
  BOOST_AUTO_TEST_CASE(Gilmore09EBLModel)
  {
    HAWCNest nest;
    nest.Service("Gilmore09EBLModel", "gilmore");
    nest.Configure();

    const EBLAbsorptionService& ebl =
      GetService<EBLAbsorptionService>("gilmore");

    const double z[] = { 0., 0.01, 0.05, 0.1, 0.5, 1.0 };
    const double E[] = { 100*GeV, 250*GeV, 1*TeV, 6*TeV, 30*TeV };
    const double od[][5] = {
      {0.0000000, 0.0000000,  0.0000000,  0.0000000,   0.0000000},
      {0.0015061, 0.0115418,  0.0963389,  0.2434142,   1.9280328},
      {0.0080495, 0.0608863,  0.4882726,  1.2446042,  10.1892544},
      {0.0174848, 0.1300117,  1.0017150,  2.5990502,  22.2235828},
      {0.1561811, 0.9948679,  5.6699960, 18.3416342, 210.0651262},
      {0.5283835, 2.7292792, 11.7764700, 53.9886634, 721.0563294}
    };

    for (int i = 0; i < 6; ++i) {
      for (int j = 0; j < 5; ++j) {
        BOOST_CHECK_CLOSE(ebl.GetOpticalDepth(E[j], z[i]), od[i][j], 0.01);
        BOOST_CHECK_CLOSE(ebl.GetAttenuation(E[j], z[i]), exp(-od[i][j]), 1e-4);
      }
    }
  }

BOOST_AUTO_TEST_SUITE_END()
