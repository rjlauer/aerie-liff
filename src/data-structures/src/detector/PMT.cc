/*!
 * @file PMT.cc 
 * @brief Geometrical and optical response properties of a photomultiplier.
 * @author Segev BenZvi 
 * @date 25 Dec 2010 
 * @version $Id: PMT.cc 29796 2016-02-12 17:40:35Z sybenzvi $
 */

#include <data-structures/detector/PMT.h>

#include <hawcnest/HAWCUnits.h>

using namespace det;
using namespace HAWCUnits;

namespace {

  TabulatedFunction<float>
  InitializeQE_R5912()
  {
    TabulatedFunction<float> qeVsLambda;

    qeVsLambda.PushBack(279.677000 * nm, 0.054127 * perCent);
    qeVsLambda.PushBack(283.911000 * nm, 0.054127 * perCent);
    qeVsLambda.PushBack(287.375000 * nm, 0.054127 * perCent);
    qeVsLambda.PushBack(291.224000 * nm, 0.324763 * perCent);
    qeVsLambda.PushBack(294.688000 * nm, 0.920162 * perCent);
    qeVsLambda.PushBack(297.383000 * nm, 1.407310 * perCent);
    qeVsLambda.PushBack(300.077000 * nm, 2.165090 * perCent);
    qeVsLambda.PushBack(302.386000 * nm, 2.977000 * perCent);
    qeVsLambda.PushBack(304.311000 * nm, 3.734780 * perCent);
    qeVsLambda.PushBack(305.851000 * nm, 4.438430 * perCent);
    qeVsLambda.PushBack(307.775000 * nm, 5.412720 * perCent);
    qeVsLambda.PushBack(309.700000 * nm, 6.441140 * perCent);
    qeVsLambda.PushBack(312.009000 * nm, 7.523680 * perCent);
    qeVsLambda.PushBack(314.704000 * nm, 8.985120 * perCent);
    qeVsLambda.PushBack(318.168000 * nm, 10.933700 * perCent);
    qeVsLambda.PushBack(322.787000 * nm, 13.423500 * perCent);
    qeVsLambda.PushBack(326.251000 * nm, 15.372100 * perCent);
    qeVsLambda.PushBack(329.715000 * nm, 17.158300 * perCent);
    qeVsLambda.PushBack(333.949000 * nm, 18.457400 * perCent);
    qeVsLambda.PushBack(337.028000 * nm, 19.702300 * perCent);
    qeVsLambda.PushBack(341.647000 * nm, 20.893100 * perCent);
    qeVsLambda.PushBack(345.497000 * nm, 21.705000 * perCent);
    qeVsLambda.PushBack(351.270000 * nm, 22.679300 * perCent);
    qeVsLambda.PushBack(356.659000 * nm, 23.274700 * perCent);
    qeVsLambda.PushBack(362.433000 * nm, 23.816000 * perCent);
    qeVsLambda.PushBack(368.206000 * nm, 24.357200 * perCent);
    qeVsLambda.PushBack(373.980000 * nm, 24.573700 * perCent);
    qeVsLambda.PushBack(379.369000 * nm, 24.682000 * perCent);
    qeVsLambda.PushBack(385.142000 * nm, 24.898500 * perCent);
    qeVsLambda.PushBack(390.531000 * nm, 25.006800 * perCent);
    qeVsLambda.PushBack(396.690000 * nm, 25.115000 * perCent);
    qeVsLambda.PushBack(402.463000 * nm, 25.060900 * perCent);
    qeVsLambda.PushBack(407.467000 * nm, 24.952600 * perCent);
    qeVsLambda.PushBack(410.931000 * nm, 24.790300 * perCent);
    qeVsLambda.PushBack(415.165000 * nm, 24.736100 * perCent);
    qeVsLambda.PushBack(420.554000 * nm, 24.627900 * perCent);
    qeVsLambda.PushBack(424.788000 * nm, 24.303100 * perCent);
    qeVsLambda.PushBack(430.562000 * nm, 23.978300 * perCent);
    qeVsLambda.PushBack(437.490000 * nm, 23.545300 * perCent);
    qeVsLambda.PushBack(444.804000 * nm, 23.004100 * perCent);
    qeVsLambda.PushBack(449.423000 * nm, 22.733400 * perCent);
    qeVsLambda.PushBack(455.196000 * nm, 22.138000 * perCent);
    qeVsLambda.PushBack(465.204000 * nm, 20.893100 * perCent);
    qeVsLambda.PushBack(472.902000 * nm, 19.756400 * perCent);
    qeVsLambda.PushBack(481.370000 * nm, 18.728000 * perCent);
    qeVsLambda.PushBack(486.374000 * nm, 18.078500 * perCent);
    qeVsLambda.PushBack(491.763000 * nm, 17.374800 * perCent);
    qeVsLambda.PushBack(500.616000 * nm, 16.346400 * perCent);
    qeVsLambda.PushBack(506.774000 * nm, 15.480400 * perCent);
    qeVsLambda.PushBack(513.703000 * nm, 14.073100 * perCent);
    qeVsLambda.PushBack(519.861000 * nm, 12.882300 * perCent);
    qeVsLambda.PushBack(523.711000 * nm, 11.962100 * perCent);
    qeVsLambda.PushBack(528.714000 * nm, 10.771300 * perCent);
    qeVsLambda.PushBack(534.488000 * nm, 9.580510 * perCent);
    qeVsLambda.PushBack(541.801000 * nm, 8.064950 * perCent);
    qeVsLambda.PushBack(549.500000 * nm, 7.198920 * perCent);
    qeVsLambda.PushBack(558.737000 * nm, 6.278760 * perCent);
    qeVsLambda.PushBack(566.436000 * nm, 5.683360 * perCent);
    qeVsLambda.PushBack(572.979000 * nm, 4.979700 * perCent);
    qeVsLambda.PushBack(578.753000 * nm, 4.330180 * perCent);
    qeVsLambda.PushBack(584.527000 * nm, 3.897160 * perCent);
    qeVsLambda.PushBack(591.455000 * nm, 3.193500 * perCent);
    qeVsLambda.PushBack(600.693000 * nm, 2.435720 * perCent);
    qeVsLambda.PushBack(608.776000 * nm, 1.732070 * perCent);
    qeVsLambda.PushBack(619.169000 * nm, 1.136670 * perCent);
    qeVsLambda.PushBack(628.406000 * nm, 0.703654 * perCent);
    qeVsLambda.PushBack(637.644000 * nm, 0.487145 * perCent);
    qeVsLambda.PushBack(643.803000 * nm, 0.324763 * perCent);
    qeVsLambda.PushBack(653.041000 * nm, 0.108254 * perCent);
    qeVsLambda.PushBack(658.814000 * nm, 0.108254 * perCent);

    return qeVsLambda;
  }

  TabulatedFunction<float>
  InitializeQE_R7081HQE()
  {
    TabulatedFunction<float> qeVsLambda;

    qeVsLambda.PushBack(270.449000 * nm, 0.098280 * perCent);
    qeVsLambda.PushBack(279.156000 * nm, 0.196560 * perCent);
    qeVsLambda.PushBack(284.697000 * nm, 0.786241 * perCent);
    qeVsLambda.PushBack(289.446000 * nm, 1.375920 * perCent);
    qeVsLambda.PushBack(291.821000 * nm, 2.555280 * perCent);
    qeVsLambda.PushBack(294.195000 * nm, 3.832920 * perCent);
    qeVsLambda.PushBack(297.361000 * nm, 5.798530 * perCent);
    qeVsLambda.PushBack(301.319000 * nm, 8.648650 * perCent);
    qeVsLambda.PushBack(302.902000 * nm, 10.221100 * perCent);
    qeVsLambda.PushBack(305.277000 * nm, 12.579900 * perCent);
    qeVsLambda.PushBack(308.443000 * nm, 16.019700 * perCent);
    qeVsLambda.PushBack(311.609000 * nm, 18.869800 * perCent);
    qeVsLambda.PushBack(317.150000 * nm, 22.014700 * perCent);
    qeVsLambda.PushBack(321.900000 * nm, 24.373500 * perCent);
    qeVsLambda.PushBack(328.232000 * nm, 26.928700 * perCent);
    qeVsLambda.PushBack(333.773000 * nm, 28.501200 * perCent);
    qeVsLambda.PushBack(339.314000 * nm, 29.484000 * perCent);
    qeVsLambda.PushBack(342.480000 * nm, 30.368600 * perCent);
    qeVsLambda.PushBack(346.438000 * nm, 31.351400 * perCent);
    qeVsLambda.PushBack(350.396000 * nm, 31.941000 * perCent);
    qeVsLambda.PushBack(357.520000 * nm, 32.235900 * perCent);
    qeVsLambda.PushBack(363.061000 * nm, 32.825600 * perCent);
    qeVsLambda.PushBack(367.810000 * nm, 33.611800 * perCent);
    qeVsLambda.PushBack(373.351000 * nm, 34.201500 * perCent);
    qeVsLambda.PushBack(378.892000 * nm, 34.398000 * perCent);
    qeVsLambda.PushBack(386.807000 * nm, 34.299800 * perCent);
    qeVsLambda.PushBack(393.931000 * nm, 34.103200 * perCent);
    qeVsLambda.PushBack(400.264000 * nm, 33.906600 * perCent);
    qeVsLambda.PushBack(405.013000 * nm, 33.317000 * perCent);
    qeVsLambda.PushBack(410.554000 * nm, 32.727300 * perCent);
    qeVsLambda.PushBack(416.887000 * nm, 32.432400 * perCent);
    qeVsLambda.PushBack(421.636000 * nm, 31.941000 * perCent);
    qeVsLambda.PushBack(428.760000 * nm, 31.154800 * perCent);
    qeVsLambda.PushBack(435.092000 * nm, 30.368600 * perCent);
    qeVsLambda.PushBack(444.591000 * nm, 29.385700 * perCent);
    qeVsLambda.PushBack(454.881000 * nm, 27.813300 * perCent);
    qeVsLambda.PushBack(465.963000 * nm, 25.945900 * perCent);
    qeVsLambda.PushBack(474.670000 * nm, 24.471700 * perCent);
    qeVsLambda.PushBack(482.586000 * nm, 22.997500 * perCent);
    qeVsLambda.PushBack(489.710000 * nm, 21.130200 * perCent);
    qeVsLambda.PushBack(500.000000 * nm, 19.656000 * perCent);
    qeVsLambda.PushBack(507.124000 * nm, 18.476700 * perCent);
    qeVsLambda.PushBack(512.665000 * nm, 17.788700 * perCent);
    qeVsLambda.PushBack(515.040000 * nm, 16.707600 * perCent);
    qeVsLambda.PushBack(522.164000 * nm, 14.938600 * perCent);
    qeVsLambda.PushBack(528.496000 * nm, 13.267800 * perCent);
    qeVsLambda.PushBack(535.620000 * nm, 11.695300 * perCent);
    qeVsLambda.PushBack(541.161000 * nm, 10.319400 * perCent);
    qeVsLambda.PushBack(546.702000 * nm, 9.336610 * perCent);
    qeVsLambda.PushBack(557.784000 * nm, 7.960690 * perCent);
    qeVsLambda.PushBack(569.657000 * nm, 6.486490 * perCent);
    qeVsLambda.PushBack(584.697000 * nm, 5.307130 * perCent);
    qeVsLambda.PushBack(598.945000 * nm, 4.029480 * perCent);
    qeVsLambda.PushBack(613.193000 * nm, 2.850120 * perCent);
    qeVsLambda.PushBack(623.483000 * nm, 2.162160 * perCent);
    qeVsLambda.PushBack(632.190000 * nm, 1.474200 * perCent);
    qeVsLambda.PushBack(644.063000 * nm, 0.982801 * perCent);
    qeVsLambda.PushBack(655.145000 * nm, 0.687961 * perCent);
    qeVsLambda.PushBack(663.852000 * nm, 0.393120 * perCent);
    qeVsLambda.PushBack(673.351000 * nm, 0.294840 * perCent);
    qeVsLambda.PushBack(683.641000 * nm, 0.098280 * perCent);
    qeVsLambda.PushBack(697.098000 * nm, 0.000000 * perCent);

    return qeVsLambda;
  }

  TabulatedFunction<float>
  InitializeQE_R12199()
  {
    TabulatedFunction<float> qeVsLambda;

    qeVsLambda.PushBack(275.000 * nm,  0.000 * perCent);
    qeVsLambda.PushBack(280.290 * nm,  0.173 * perCent);
    qeVsLambda.PushBack(290.294 * nm,  1.489 * perCent);
    qeVsLambda.PushBack(300.413 * nm,  6.209 * perCent);
    qeVsLambda.PushBack(309.984 * nm, 12.834 * perCent);
    qeVsLambda.PushBack(320.309 * nm, 18.166 * perCent);
    qeVsLambda.PushBack(329.742 * nm, 21.505 * perCent);
    qeVsLambda.PushBack(339.838 * nm, 22.957 * perCent);
    qeVsLambda.PushBack(349.620 * nm, 23.729 * perCent);
    qeVsLambda.PushBack(360.246 * nm, 24.281 * perCent);
    qeVsLambda.PushBack(370.921 * nm, 24.744 * perCent);
    qeVsLambda.PushBack(380.847 * nm, 24.979 * perCent);
    qeVsLambda.PushBack(390.023 * nm, 24.979 * perCent);
    qeVsLambda.PushBack(400.443 * nm, 24.627 * perCent);
    qeVsLambda.PushBack(409.920 * nm, 24.396 * perCent);
    qeVsLambda.PushBack(420.245 * nm, 24.067 * perCent);
    qeVsLambda.PushBack(430.472 * nm, 23.506 * perCent);
    qeVsLambda.PushBack(440.204 * nm, 22.741 * perCent);
    qeVsLambda.PushBack(450.036 * nm, 21.809 * perCent);
    qeVsLambda.PushBack(459.814 * nm, 20.804 * perCent);
    qeVsLambda.PushBack(470.090 * nm, 19.588 * perCent);
    qeVsLambda.PushBack(480.320 * nm, 18.249 * perCent);
    qeVsLambda.PushBack(490.897 * nm, 17.102 * perCent);
    qeVsLambda.PushBack(500.823 * nm, 16.469 * perCent);
    qeVsLambda.PushBack(509.950 * nm, 15.433 * perCent);
    qeVsLambda.PushBack(520.226 * nm, 13.160 * perCent);
    qeVsLambda.PushBack(529.752 * nm, 10.320 * perCent);
    qeVsLambda.PushBack(540.077 * nm,  8.248 * perCent);
    qeVsLambda.PushBack(550.753 * nm,  7.000 * perCent);
    qeVsLambda.PushBack(559.929 * nm,  6.306 * perCent);
    qeVsLambda.PushBack(570.999 * nm,  5.305 * perCent);
    qeVsLambda.PushBack(580.481 * nm,  4.481 * perCent);
    qeVsLambda.PushBack(590.756 * nm,  3.628 * perCent);
    qeVsLambda.PushBack(600.682 * nm,  2.937 * perCent);
    qeVsLambda.PushBack(610.159 * nm,  2.289 * perCent);
    qeVsLambda.PushBack(620.085 * nm,  1.672 * perCent);
    qeVsLambda.PushBack(629.863 * nm,  1.117 * perCent);
    qeVsLambda.PushBack(639.744 * nm,  0.723 * perCent);
    qeVsLambda.PushBack(650.670 * nm,  0.423 * perCent);
    qeVsLambda.PushBack(660.152 * nm,  0.233 * perCent);
    qeVsLambda.PushBack(669.580 * nm,  0.123 * perCent);
    qeVsLambda.PushBack(680.000 * nm,  0.000 * perCent);

    return qeVsLambda;
  }

  TabulatedFunction<float>
  InitializeQE_FlatPMT()
  {
    TabulatedFunction<float> qeVsLambda;

    qeVsLambda.PushBack(275.000 * nm,  0.000 * perCent);
    qeVsLambda.PushBack(280.290 * nm,  0.173 * perCent);
    qeVsLambda.PushBack(290.294 * nm,  1.489 * perCent);
    qeVsLambda.PushBack(300.413 * nm,  6.209 * perCent);
    qeVsLambda.PushBack(309.984 * nm, 12.834 * perCent);
    qeVsLambda.PushBack(320.309 * nm, 18.166 * perCent);
    qeVsLambda.PushBack(329.742 * nm, 21.505 * perCent);
    qeVsLambda.PushBack(339.838 * nm, 22.957 * perCent);
    qeVsLambda.PushBack(349.620 * nm, 23.729 * perCent);
    qeVsLambda.PushBack(360.246 * nm, 24.281 * perCent);
    qeVsLambda.PushBack(370.921 * nm, 24.744 * perCent);
    qeVsLambda.PushBack(380.847 * nm, 24.979 * perCent);
    qeVsLambda.PushBack(390.023 * nm, 24.979 * perCent);
    qeVsLambda.PushBack(400.443 * nm, 24.627 * perCent);
    qeVsLambda.PushBack(409.920 * nm, 24.396 * perCent);
    qeVsLambda.PushBack(420.245 * nm, 24.067 * perCent);
    qeVsLambda.PushBack(430.472 * nm, 23.506 * perCent);
    qeVsLambda.PushBack(440.204 * nm, 22.741 * perCent);
    qeVsLambda.PushBack(450.036 * nm, 21.809 * perCent);
    qeVsLambda.PushBack(459.814 * nm, 20.804 * perCent);
    qeVsLambda.PushBack(470.090 * nm, 19.588 * perCent);
    qeVsLambda.PushBack(480.320 * nm, 18.249 * perCent);
    qeVsLambda.PushBack(490.897 * nm, 17.102 * perCent);
    qeVsLambda.PushBack(500.823 * nm, 16.469 * perCent);
    qeVsLambda.PushBack(509.950 * nm, 15.433 * perCent);
    qeVsLambda.PushBack(520.226 * nm, 13.160 * perCent);
    qeVsLambda.PushBack(529.752 * nm, 10.320 * perCent);
    qeVsLambda.PushBack(540.077 * nm,  8.248 * perCent);
    qeVsLambda.PushBack(550.753 * nm,  7.000 * perCent);
    qeVsLambda.PushBack(559.929 * nm,  6.306 * perCent);
    qeVsLambda.PushBack(570.999 * nm,  5.305 * perCent);
    qeVsLambda.PushBack(580.481 * nm,  4.481 * perCent);
    qeVsLambda.PushBack(590.756 * nm,  3.628 * perCent);
    qeVsLambda.PushBack(600.682 * nm,  2.937 * perCent);
    qeVsLambda.PushBack(610.159 * nm,  2.289 * perCent);
    qeVsLambda.PushBack(620.085 * nm,  1.672 * perCent);
    qeVsLambda.PushBack(629.863 * nm,  1.117 * perCent);
    qeVsLambda.PushBack(639.744 * nm,  0.723 * perCent);
    qeVsLambda.PushBack(650.670 * nm,  0.423 * perCent);
    qeVsLambda.PushBack(660.152 * nm,  0.233 * perCent);
    qeVsLambda.PushBack(669.580 * nm,  0.123 * perCent);
    qeVsLambda.PushBack(680.000 * nm,  0.000 * perCent);

    return qeVsLambda;
  }


}

TabulatedFunction<float> PMT::qeVsLambdaR5912_(InitializeQE_R5912());
TabulatedFunction<float> PMT::qeVsLambdaR7081HQE_(InitializeQE_R7081HQE());
TabulatedFunction<float> PMT::qeVsLambdaR12199_(InitializeQE_R12199());
//TabulatedFunction<float> PMT::qeVsLambdaFlatPMT_(InitializeQE_FlatPMT());

PMT::PMT(const PMTType& pmtType) :
  pmtId_(0),
  pmtType_(pmtType),
  gain_(1.4e7),
  gainErr_(35.7 * perCent),
  ampl_(13.17 * mV),
  sig1_(2 * ns),
  sig2_(3 * ns),
  sigErr_(8 * perCent),
  Zout_(75 * ohm)
{
  InitializeProperties();
}

void
PMT::InitializeProperties()
{
  switch (pmtType_) {
    case R5912:
    {
      diameter_ = 202 * mm;
      cathDiameter_ = 190 * mm;
      baseDiameter_ = 84.5 * mm;
      baseHeight_ = 275 * mm;
      qeVsLambda_ = &qeVsLambdaR5912_;
      chargeFactor_ = 1.;
      break;
    }
    case R7081HQE:
    {
      diameter_ = 253 * mm;
      cathDiameter_ = 220 * mm;
      baseDiameter_ = 84.5 * mm;
      baseHeight_ = 300 * mm;
      qeVsLambda_ = &qeVsLambdaR7081HQE_;
      chargeFactor_ = 0.4554;
      break;
    }
    case R12199:
    {
      diameter_ = 80 * mm;
      cathDiameter_ = 72 * mm;
      baseDiameter_ = 40 * mm;
      baseHeight_ = 250 * mm;
      qeVsLambda_ = &qeVsLambdaR12199_;
      chargeFactor_ = 1.;
      break;
    }
//    case FlatPMT:
//    {
//      diameter_ = 1000 * mm;
//      baseDiameter_ = 1000 * mm;
//      baseHeight_ = 0 * mm;
//      qeVsLambda_ = &qeVsLambdaFlatPMT_;
//      chargeFactor_ = 1.;
//      break;
//    }
    default:
      log_fatal("Inializing PMT " << pmtId_ << " with unknown PMT type.");
      break;
  }
}

