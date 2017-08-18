/*!
 * @file Math.cc 
 * @brief Python bindings to math-based data structures.
 * @author Segev BenZvi 
 * @date 19 Feb 2012 
 * @version $Id: Math.cc 27607 2015-11-07 22:31:17Z sybenzvi $
 */

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>

#include <data-structures/math/PowerLaw.h>
#include <data-structures/math/BrokenPowerLaw.h>
#include <data-structures/math/CutoffPowerLaw.h>
#include <data-structures/math/DoubleBrokenPowerLaw.h>
#include <data-structures/math/LogParabola.h>
#include <data-structures/math/DigitalLogic.h>
#include <data-structures/math/SplineTable.h>
#include <data-structures/math/PeriodicFunction.h>
#include <data-structures/math/PnPoly.h>
#include <data-structures/math/ConvexHull.h>

using namespace boost::python;
using namespace std;



// Function to convert boost::python lists and tuples to std::vector
namespace {

  inline
  std::vector<double>
  listToVector(const boost::python::object& l)
  {
    return std::vector<double>(stl_input_iterator<double>(l),
                               stl_input_iterator<double>());
  }
}

// -----------------------------------------------------------------------------
/// Expose PowerLaw to boost::python
void
pybind_math_PowerLaw()
{
  class_<PowerLaw, bases<Baggable>, boost::shared_ptr<PowerLaw> >
    ("PowerLaw",
     "Define a power law A (x/xN)^a on [x0, x1).",
     init<const double, const double,
          const double, const double,
          const double>(args("x0", "x1", "A", "xN", "a"),
          "Evaluate a power law on [x0, x1].\n\nArgs:\n"
          "    x0: Lower limit of power law range\n"
          "    x1: Upper limit of power law range (can be infinite)\n"
          "    A:  Power law normalization\n"
          "    xN: Normalization point in [x0,x1]\n"
          "    a:  Spectral index (can be >0 or <0)\n")
     )

    .add_property("normalization", &PowerLaw::GetNormalization,
                  "Normalization constant A of the power law.")
    .add_property("xnorm", &PowerLaw::GetNormX,
                  "Location where the normalization constant A is defined.")
    .add_property("xmin", &PowerLaw::GetMinX,
                  "Minimum valid input of the power law.")
    .add_property("xmax", &PowerLaw::GetMaxX,
                  "Maximum valid input of the power law.")

    .def("spectral_index", &PowerLaw::GetSpectralIndex, args("x"),
         "Return the power law index as a function of x.")
    .def("evaluate", &PowerLaw::Evaluate, args("x"),
         "Evaluate the power law at some value x.")
    .def("integrate", &PowerLaw::Integrate, args("x0", "x1"),
         "Integrate the power law between two values.\n\nArgs:\n"
         "   x0: Lower limit of integration\n"
         "   x1: Upper limit of integration (can be infinity)\n")
    .def("normweight", &PowerLaw::GetNormWeight, args("x0", "x1"),
         "Normalization factor of the power law between two values.\n\nArgs:\n"
         "   x0: Lower limit of range\n"
         "   x1: Upper limit of range (can be infinity)\n")
    .def("reweight", &PowerLaw::Reweight, args("pl", "x"),
         "Reweighting factor of another power law into this one.\n\nArgs:\n"
         "   pl: a different PowerLaw\n"
         "   x:  value where we compute the weight")
    .def("prob_to_keep", &PowerLaw::GetProbToKeep, args("pl", "x"),
         "Get the probability to keep an event sampled from another PowerLaw"
         "\n\nArgs:\n"
         "   pl: a different PowerLaw\n"
         "   x:  value where we compute the probability")
    .def("invert_integral", &PowerLaw::InvertIntegral, args("frac"),
         "Get the value of x between x0 and x1 at which the PowerLaw\n"
         "integral obtains some fraction of its total value\n\nArgs:\n"
         "   frac: Fraction of PowerLaw integral")
    ;
}

// -----------------------------------------------------------------------------
/// Expose BrokenPowerLaw to boost::python
void
pybind_math_BrokenPowerLaw()
{
  class_<BrokenPowerLaw, bases<PowerLaw>, boost::shared_ptr<BrokenPowerLaw> >
    ("BrokenPowerLaw",
     "Define a power law with two spectral indices and a break point.",
     init<const double, const double,
          const double, const double,
          const double, const double, const double>(
          args("x0", "x1", "A", "xN", "a", "xB", "b"),
          "Evaluate a broken power law on [x0, x1].\n\nArgs:\n"
          "    x0: Lower limit of power law range\n"
          "    x1: Upper limit of power law range (can be infinite)\n"
          "    A:  Power law normalization\n"
          "    xN: Normalization point in [x0,x1]\n"
          "    a:  Spectral index for x < xB\n"
          "    xB: Breakpoint in spectrum\n"
          "    b:  Spectral index for x > xB\n")
     )
    ;
}

// -----------------------------------------------------------------------------
/// Expose CutoffPowerLaw to boost::python
void
pybind_math_CutoffPowerLaw()
{
  class_<CutoffPowerLaw, bases<PowerLaw>, boost::shared_ptr<CutoffPowerLaw> >
    ("CutoffPowerLaw",
     "Define a power law with an exponential cutoff.",
     init<const double, const double,
          const double, const double,
          const double, const double>(
          args("x0", "x1", "A", "xN", "a", "xC"),
          "Evaluate a cutoff power law on [x0, x1].\n\nArgs:\n"
          "    x0: Lower limit of power law range\n"
          "    x1: Upper limit of power law range (can be infinite)\n"
          "    A:  Power law normalization\n"
          "    xN: Normalization point in [x0,x1]\n"
          "    a:  Spectral index for x < xB\n"
          "    xC: Exponential cutoff location\n")
    )

    .add_property("cutoff", &CutoffPowerLaw::GetCutoffX,
                  "Exponential cutoff point of the power law.")
    ;
}

// -----------------------------------------------------------------------------
/// Expose DoubleBrokenPowerLaw to boost::python
void
pybind_math_DoubleBrokenPowerLaw()
{
  class_<DoubleBrokenPowerLaw, bases<BrokenPowerLaw>,
                               boost::shared_ptr<DoubleBrokenPowerLaw> >
    ("DoubleBrokenPowerLaw",
     "Define a power law with three spectral indices and two break points.",
     init<const double, const double,
          const double, const double,
          const double, const double,
          const double, const double,
          const double>(
          args("x0", "x1", "A", "xN", "a", "xB1", "b", "xB2", "c"),
          "Evaluate a double-broken power law on [x0, x1].\n\nArgs:\n"
          "    x0:  Lower limit of power law range\n"
          "    x1:  Upper limit of power law range (can be infinite)\n"
          "    A:   Power law normalization\n"
          "    xN:  Normalization point in [x0,x1]\n"
          "    a:   Spectral index for x < xB1\n"
          "    xB1: First breakpoint in spectrum\n"
          "    b:   Spectral index for x > xB1 and x < xB2\n"
          "    xB2: Second breakpoint in spectrum\n"
          "    c:   Spectral index for x > xB2\n")
    )
    ;
}

// -----------------------------------------------------------------------------
/// Expose LogParabola to boost::python
void
pybind_math_LogParabola()
{
  class_<LogParabola, bases<PowerLaw>, boost::shared_ptr<LogParabola> >
    ("LogParabola",
     "Define a log-parabola power law.",
     init<const double, const double,
          const double, const double,
          const double, const double>(
          args("x0", "x1", "A", "xN", "a", "b"),
          "Evaluate a broken power law on [x0, x1].\n\nArgs:\n"
          "    x0: Lower limit of power law range\n"
          "    x1: Upper limit of power law range (can be infinite)\n"
          "    A:  Power law normalization\n"
          "    xN: Normalization point in [x0,x1]\n"
          "    a:  Spectral index\n"
          "    b:  Curvature in spectral index\n")
    )
    ;
}

// -----------------------------------------------------------------------------
/// Expose DigitalLogic::Edge to boost::python
void
pybind_math_DigitalLogicEdge()
{
  class_<DigitalLogic::Edge>
    ("DigitalEdge",
     "Representation of a state transition in a digital circuit element.")

    .add_property("time",
                  &DigitalLogic::Edge::GetTime,
                  "Edge time.")
    .add_property("is_rising",
                  &DigitalLogic::Edge::IsRising,
                  "True if edge is a low-to-high (rising) transition.")

    .def(self < self)
    .def(self_ns::str(self))
  ;
}

// -----------------------------------------------------------------------------
// Pointer to const iterators in Pulse class
typedef DigitalLogic::Pulse::ConstEdgeIterator (DigitalLogic::Pulse::*EdgeIter)() const;

/// Expose DigitalLogic::Pulse to boost::python
void
pybind_math_DigitalLogicPulse()
{
  class_<DigitalLogic::Pulse>
    ("DigitalPulse",
     "A collection of transitions (edges) in a digital circuit element.")

    .add_property("nedges",
                  &DigitalLogic::Pulse::GetNEdges,
                  "Number of edges in the pulse series.")
    .def("edges",
         range<return_value_policy<copy_const_reference> >(
            EdgeIter(&DigitalLogic::Pulse::EdgesBegin),
            EdgeIter(&DigitalLogic::Pulse::EdgesEnd)),
         "Edge generator, e.g., 'for edge in pulse.edges(): ...'")

    .add_property("first_edge",
                  make_function(&DigitalLogic::Pulse::GetFirstEdge,
                    return_value_policy<copy_const_reference>()),
                  "Return the first edge in the pulse (presumably rising).")

    .add_property("last_edge",
                  make_function(&DigitalLogic::Pulse::GetLastEdge,
                    return_value_policy<copy_const_reference>()),
                  "Return the last edge in the pulse (presumably rising).")

    .def("__len__",
         &DigitalLogic::Pulse::GetNEdges,
         "Number of edges in the pulse series.")
    .def("__iter__",
         range<return_value_policy<copy_const_reference> >(
            EdgeIter(&DigitalLogic::Pulse::EdgesBegin),
            EdgeIter(&DigitalLogic::Pulse::EdgesEnd)),
         "Edge iterator, e.g., 'for edge in pulse: ...'")

    .def(self_ns::str(self))
    ;
}

// -----------------------------------------------------------------------------
// Helper functions for tuple/SplineTable conversions
namespace {

  double
  spline_evaluate(SplineTable& t, const boost::python::tuple& c)
  {
    int n = len(c);
    vector<double> coord(n, 0.);
    for (int i = 0; i < n; ++i)
      coord[i] = extract<double>(c[i]);

    return t.Eval(&coord[0]);
  }

  boost::python::tuple
  spline_extent(const SplineTable& t, const int i)
  {
    double xmin, xmax;
    t.GetExtent(i, xmin, xmax);
    return boost::python::make_tuple(xmin, xmax);
  }

}

/// Expose SplineTable to boost::python
void
pybind_math_SplineTable()
{
  class_<SplineTable>
    ("SplineTable",
     "A b-spline coefficient table with evaluation interface.",
     init<const std::string&>())

    .add_property("ndimensions",
                  &SplineTable::GetNDimensions,
                  "Number of dimensions (axes) in the spline table.")

    .def("GetOrder",
         &SplineTable::GetOrder,
         "Return the spline order of the i^th axis.")

    .def("GetExtent",
         spline_extent,
         "Get the extent of data on the i^th axis.")

    .def("Eval",
         spline_evaluate,
         "Evaluate the spline at an n-D coordinate (given by a tuple).")
    ;
}

// -----------------------------------------------------------------------------
/// Expose Periodic function to boost::python
namespace {

  // Wrapper of PeriodicFunction needed so that python constructor which
  // accepts lists and tuples calls C++ constructor which expects std::vector
  class PeriodicWrapper {
    public:
      static boost::shared_ptr<PeriodicFunction>
      init(double t0, double period, const object& fphase, const object& fnfunc)
      {
        vector<double> fp = listToVector(fphase);
        vector<double> fn = listToVector(fnfunc);
        return boost::make_shared<PeriodicFunction>(t0, period, fp, fn);
      }

      static double Evaluate(const PeriodicFunction& pf, const double t)
      { return pf.Evaluate(t); } 
  };

}

void
pybind_math_PeriodicFunction()
{
  class_<PeriodicFunction, boost::shared_ptr<PeriodicFunction> >
    ("PeriodicFunction",
     "A non-parametric periodic function defined by an offset, a period length, and a table that defines the function values within a period.",
     no_init)

    .def("__init__",
         make_constructor(&PeriodicWrapper::init))

    .def("Evaluate",
         &PeriodicWrapper::Evaluate, (boost::python::arg("t")),
         "Calculate the function at some value.")

    .def("__call__",
         &PeriodicWrapper::Evaluate, (boost::python::arg("t")),
         "Calculate the function at some value.")
    ;
}


// -----------------------------------------------------------------------------
/// Expose PnPoly function to boost::python
namespace {

  // Wrapper of PnPoly needed so that python constructor which
  // accepts lists and tuples calls C++ constructor which expects std::vector
  class PnPolyWrapper {
    public:
      static boost::shared_ptr<PnPoly>
      init(const object& x, const object& y)
      {
        vector<double> vertx = listToVector(x);
        vector<double> verty = listToVector(y);
        return boost::make_shared<PnPoly>(vertx, verty);
      }
  };

}

// -----------------------------------------------------------------------------
/// Expose PnPoly to boost::python
void
pybind_math_PnPoly()
{
  class_<PnPoly, boost::shared_ptr<PnPoly> >
    ("PnPoly",
     "Define a polygon for PnPoly inclusion test.",
     no_init)

    .def("__init__",
         make_constructor(&PnPolyWrapper::init))

    .def("evaluate", &PnPoly::Eval, 
         "Evaluate whether test point (x,y) is inside polygon.")
    ;
}


// -----------------------------------------------------------------------------
/// Expose ConvexHull function to boost::python
namespace {

  //boost::python::tuple
  //get_centroid(const ConvexHull& t)
  //{
  //  double x, y;
  //  t.GetCentroid(x, y);
  //  return boost::python::make_tuple(x, y);
  //}

  // Wrapper of ConvexHull needed so that python constructor which
  // accepts lists and tuples calls C++ constructor which expects std::vector
  class ConvexHullWrapper {
    public:
      static boost::shared_ptr<ConvexHull>
      init(const object& x, const object& y)
      {
        vector<double> vertx = listToVector(x);
        vector<double> verty = listToVector(y);
        return boost::make_shared<ConvexHull>(vertx, verty);
      }
      
      static boost::python::tuple
      get_hull(ConvexHull& t)
      {
        vector<double> x;
        vector<double> y;
        t.GetHull(x,y);
        return boost::python::make_tuple(x,y);
      }
      
      static boost::python::tuple
      get_centroid(ConvexHull& t)
      {
        double x, y;
        t.GetCentroid(x,y);
        return boost::python::make_tuple(x,y);
      }

      static boost::python::tuple
      scale_hull(ConvexHull& t, const double scale)
      {
        vector<double> x;
        vector<double> y;
        t.ScaleHull(scale,x,y);
        return boost::python::make_tuple(x,y);
      }
  };

}

// -----------------------------------------------------------------------------
/// Expose PnPoly to boost::python
void
pybind_math_ConvexHull()
{
  class_<ConvexHull, boost::shared_ptr<ConvexHull> >
    ("ConvexHull",
     "Define convex hull object from an ordered set of input points.",
     no_init)

    .def("__init__",
         make_constructor(&ConvexHullWrapper::init))

    .def("scale_hull", &ConvexHullWrapper::scale_hull,
         "Scale the vertices by factor >0 of the hull found from input points.")

    .def("get_hull", &ConvexHullWrapper::get_hull,
         "Get the vertices of the hull found from input points.")

    .def("get_centroid", &ConvexHullWrapper::get_centroid,
         "Get the centroid of the convex hull. Returns tuple (x,y) of centroid coordinates.")

    .def("area", &ConvexHull::Area,
         "Get area defined by the convex hull (m^2).")
    ;
}
