/*!
 * @file DigitalLogic.cc
 * @brief Implementation of digital logic pulses and boolean algebra.
 * @author Segev BenZvi
 * @date 17 Feb 2012
 * @version $Id: DigitalLogic.cc 13185 2012-11-10 15:46:03Z sybenzvi $
 */

#include <data-structures/math/DigitalLogic.h>

using namespace std;

namespace DigitalLogic {

  void
  Pulse::DeleteLastEdge()
  {
    if (!edges_.empty()) {
      edges_.pop_back();
    }
  }

  ostream&
  operator<<(ostream& os, const Edge& tr)
  {
    os << tr.time_ << (tr.IsRising() ? " (1)" : " (0)");
    return os;
  }

  ostream&
  operator<<(ostream& os, const Pulse& p)
  {
    for (Pulse::ConstEdgeIterator iE = p.EdgesBegin();
         iE != p.EdgesEnd(); ++iE)
    {
      os << *iE;
      if (distance(iE, p.EdgesEnd()) > 1)
        os << " | ";
    }
    return os;
  }

  Pulse
  AND(const Pulse& a, const Pulse& b)
  {
    Pulse::EdgeList edges;
    edges.resize(a.GetNEdges() + b.GetNEdges());
    Pulse::EdgeIterator iE = edges.begin();

    iE = copy(a.EdgesBegin(), a.EdgesEnd(), edges.begin());
    copy(b.EdgesBegin(), b.EdgesEnd(), iE);
    sort(edges.begin(), edges.end());

    int nUp = 0;
    Pulse result;

    for (iE = edges.begin(); iE != edges.end(); ++iE) {
      // AND rising edge: both inputs are high
      if (nUp == 1 && iE->GetType() == Edge::RISING)
        result.AddEdge(*iE);

      // AND falling edge: one input drops low
      if (nUp == 2 && iE->GetType() == Edge::FALLING)
        result.AddEdge(*iE);

      switch (iE->GetType()) {
        case Edge::RISING:
          ++nUp;
          break;
        case Edge::FALLING:
          if (nUp > 0) --nUp;
          break;
        default:
          break;
      }
    }

    return result;
  }

  Pulse
  OR(const Pulse& a, const Pulse& b)
  {
    Pulse::EdgeList edges;
    edges.resize(a.GetNEdges() + b.GetNEdges());
    Pulse::EdgeIterator iE = edges.begin();

    iE = copy(a.EdgesBegin(), a.EdgesEnd(), edges.begin());
    copy(b.EdgesBegin(), b.EdgesEnd(), iE);
    sort(edges.begin(), edges.end());

    int nUp = 0;
    Pulse result;

    for (iE = edges.begin(); iE != edges.end(); ++iE) {
      // OR rising edge: one input goes high 
      if (nUp == 0 && iE->GetType() == Edge::RISING)
        result.AddEdge(*iE);

      // OR falling edge: both inputs drop low
      if (nUp == 1 && iE->GetType() == Edge::FALLING)
        result.AddEdge(*iE);

      switch (iE->GetType()) {
        case Edge::RISING:
          ++nUp;
          break;
        case Edge::FALLING:
          if (nUp > 0) --nUp;
          break;
        default:
          break;
      }
    }

    return result;
  }

  Pulse
  NOT(const Pulse& a)
  {
    Pulse result;
    for (Pulse::ConstEdgeIterator iE = a.EdgesBegin(); iE != a.EdgesEnd(); ++iE)
    {
      Edge::Type type = Edge::RISING;
      if (iE->GetType() == Edge::RISING)
        type = Edge::FALLING;

      result.AddEdge(Edge(type, iE->GetTime()));
    }

    return result;
  }

  Pulse
  DELAY(const Pulse& p, const double dt)
  {
    Pulse result;
    for (Pulse::ConstEdgeIterator iE = p.EdgesBegin(); iE != p.EdgesEnd(); ++iE)
      result.AddEdge(Edge(iE->GetType(), iE->GetTime() + dt));
    return result;
  }

}

