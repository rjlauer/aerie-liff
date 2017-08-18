/*!
 * @file drawBField.cc
 * @author Segev BenZvi
 * @date 14 Jan 2013
 * @brief Draw a geomagnetic magnetic field model implemented in
 *        astro-service.
 * @version $Id: drawBField.cc 27911 2015-11-21 14:24:35Z sybenzvi $
 */

#include <astro-service/GeomagneticField.h>
#include <astro-service/GeoDipoleService.h>
#include <astro-service/IGRFService.h>

#include <data-structures/geometry/Point.h>
#include <data-structures/geometry/Vector.h>
#include <data-structures/geometry/Ellipsoid.h>
#include <data-structures/time/ModifiedJulianDate.h>

#include <hawcnest/HAWCNest.h>
#include <hawcnest/HAWCUnits.h>
#include <hawcnest/Logging.h>
#include <hawcnest/CommandLineConfigurator.h>

#include <TApplication.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TStyle.h>

#include <iostream>

using namespace std;
using namespace HAWCUnits;

void setRootStyles();

int main(int argc, char* argv[])
{
  // Parse the command line
  CommandLineConfigurator cl(
    "Draw the strength of the geomagnetic field using one of several models.");

  cl.AddOption<int>("model,m", 1, "Magnetic field model: Dipole=1, IGRF=2");
  if (!cl.ParseCommandLine(argc, argv))
    return 1;

  int model = cl.GetArgument<int>("model");
  bool useIGRF = false;
  switch (model) {
    case 1:
      useIGRF = false;
      break;
    case 2:
      useIGRF = true;
      break;
    default:
      log_fatal("Unrecognized field model type " << model
                << "; use 1=Dipole or 2=IGRF");
      break;
  }

  // Set up an interactive ROOT application
  TApplication app("app", &argc, argv);

  setRootStyles();

  // Set up 2D histograms for plotting
  int N = 500;
  double cmin = -20.;
  double cmax =  20.;
  double dc = (cmax - cmin)/(N - 1);
  const double Re = Ellipsoid::GetEquatorialRadius();

  TH2D* h = new TH2D("hField", ";x/R_{E};z/R_{E};B_{int} [nT]",
                     N, cmin, cmax, N, cmin, cmax);
  h->SetStats(kFALSE);
  h->GetXaxis()->CenterTitle();
  h->GetYaxis()->CenterTitle();
  h->GetZaxis()->CenterTitle();
  h->GetZaxis()->SetTitleOffset(1.3);
  
  // Set up the field calculation
  Point p;
  ModifiedJulianDate mjd(56171*day);
  Vector B;

  HAWCNest nest;

  if (useIGRF) {
#ifdef COMPILE_FORTRAN_COMPONENTS
    nest.Service<IGRFService>("bfield");
    h->SetTitle("IGRF");
#else
    log_warn("IGRF field model not available. Using geomagnetic dipole");
    nest.Service<GeoDipoleService>("bfield");
    h->SetTitle("GeoDipole Field");
#endif
  }
  else {
    nest.Service<GeoDipoleService>("bfield");
    h->SetTitle("GeoDipole Field");
  }

  nest.Configure();

  const GeomagneticField& bfield = GetService<GeomagneticField>("bfield");

  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      p.SetXYZ((cmin + i*dc)*Re,
               0.*Re,
               (cmin + j*dc)*Re);

      if (p.GetMag() < Re)
        h->Fill(p.GetX()/Re, p.GetZ()/Re, 0.);
      else {
        bfield.GetField(p, mjd, B);
        h->Fill(p.GetX()/Re, p.GetZ()/Re, B.GetMag()/(HAWCUnits::nano*tesla));
      }
    }
  }

  nest.Finish();

  // Draw the field
  TCanvas* c = new TCanvas("c", "c", 750, 675);
  c->SetRightMargin(0.18);
  c->SetLogz();
  c->SetGridx();
  c->SetGridy();
  c->cd();
  h->Draw("colz");
  c->Update();

  app.Run();

  return 0;
}

void
setRootStyles()
{
  // Canvas printing details: white bg, no borders.
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasBorderSize(0);

  // Canvas frame printing details: white bg, no borders.
  gStyle->SetFrameFillColor(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);

  // Plot title details: centered, no bg, no border, nice font.
  gStyle->SetTitleX(0.5);
  gStyle->SetTitleW(0.8);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);

  // Font details for titles and labels.
  gStyle->SetTitleFont(132, "xyz");
  gStyle->SetTitleFont(132, "pad");
  gStyle->SetLabelFont(132, "xyz");
  gStyle->SetLabelFont(132, "pad");

  // Details for stat box.
  gStyle->SetStatColor(0);
  gStyle->SetStatFont(132);
  gStyle->SetStatBorderSize(1);
  gStyle->SetStatX(0.975);
  gStyle->SetStatY(0.9);

  // Color palette
  const int NRGBs = 5;
 const int NCont = 255;

  double stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  double red[NRGBs]   = { 1.00, 0.95, 0.89, 0.50, 0.00 };
  double green[NRGBs] = { 1.00, 0.71, 0.27, 0.00, 0.00 };
  double blue[NRGBs]  = { 1.00, 0.40, 0.00, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(50);
}

