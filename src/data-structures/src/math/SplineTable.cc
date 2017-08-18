/*!
 * @file SplineTable.cc
 * @brief Implementation of N-dimensional B-spline coefficient table.
 * @author Segev BenZvi
 * @date 10 Jul 2012
 * @version $Id: SplineTable.cc 13185 2012-11-10 15:46:03Z sybenzvi $
 */

#include <data-structures/math/SplineTable.h>

#include <hawcnest/Logging.h>

#include <photospline/core/bspline.h>

using namespace std;

SplineTable::SplineTable(const std::string& fitsFile)
{
  if (readsplinefitstable(fitsFile.c_str(), &table_) != 0)
    log_fatal("Could not read spline table in " << fitsFile);
}

SplineTable::~SplineTable()
{
  splinetable_free(&table_);
}

int
SplineTable::GetOrder(const int i)
  const
{
  if (i < 0 || i >= table_.ndim)
    log_fatal("Index " << i << "; must be between 0 and " << table_.ndim);
  return table_.order[i];
}

void 
SplineTable::GetExtent(const int i, double& emin, double& emax)
  const
{
  if (i < 0 || i >= table_.ndim)
    log_fatal("Index " << i << "; must be between 0 and " << table_.ndim);
  emin = table_.extents[i][0];
  emax = table_.extents[i][1];
}

int
SplineTable::Eval(double* x, double* result)
{
  int centers[table_.ndim];

  if (tablesearchcenters(&table_, x, centers) == 0)
    *result = ndsplineeval(&table_, x, centers, 0);
  else
    return EINVAL;

  return 0;
}

double
SplineTable::Eval(double* x)
{
  double result;
  if (Eval(x, &result))
    log_fatal("Could not evaluate spline function");

  return result;
}

