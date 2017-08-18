/*!
 * @file DigitalLogic.h
 * @brief Representation of digital logic pulses and basic boolean algebra.
 * @author Segev BenZvi
 * @date 17 Feb 2012
 * @version $Id: DigitalLogic.h 13185 2012-11-10 15:46:03Z sybenzvi $
 */

#ifndef DATACLASSES_MATH_DIGITALLOGIC_H_INCLUDED
#define DATACLASSES_MATH_DIGITALLOGIC_H_INCLUDED

#include <algorithm>
#include <iostream>
#include <vector>

namespace DigitalLogic {

  /*!
   * @class Edge
   * @author Segev BenZvi
   * @date 17 Feb 2012
   * @ingroup math
   * @brief Representation of a state transition in a digital circuit element
   *
   * This class is useful for basic simulations of the Milagro/HAWC digital
   * electronics.
   *
   * Logic transitions contain information about the type of transition:
   * low-high, or "rising", and high-low, or "falling."  The transitions also
   * have a timestamp and can be sorted in time.
   */
  class Edge {

    public:

      enum Type {
        RISING,
        FALLING,
        UNDEFINED
      };

      Edge() : type_(UNDEFINED), time_(0) { }

      Edge(const Type& type, const double t) : type_(type), time_(t) { }

      double GetTime() const { return time_; }

      const Type& GetType() const { return type_; }

      bool IsRising() const { return type_ == RISING; }

      bool operator<(const Edge& tr) const { return time_ < tr.time_; }

    private:

      Type type_;
      double time_;

    friend std::ostream& operator<<(std::ostream& os, const Edge& tr);

  };

  /*!
   * @class Pulse
   * @author Segev BenZvi
   * @date 17 Feb 2012
   * @ingroup math
   * @brief A collection of rising and falling edges in a digital component
   */
  class Pulse {
    
    public:

      typedef std::vector<Edge> EdgeList;
      typedef EdgeList::iterator EdgeIterator;
      typedef EdgeList::const_iterator ConstEdgeIterator;

      Pulse() { }

      void AddEdge(const Edge& tr)
      { edges_.push_back(tr); }

      size_t GetNEdges() const
      { return edges_.size(); }

      void Sort()
      { std::sort(edges_.begin(), edges_.end()); }

      void Clear()
      { edges_.clear(); }

      const Edge& GetFirstEdge() const { return edges_.front(); }

      const Edge& GetLastEdge() const { return edges_.back(); }

      void DeleteLastEdge();

      EdgeIterator EdgesBegin()
      { return edges_.begin(); }

      EdgeIterator EdgesEnd()
      { return edges_.end(); }

      ConstEdgeIterator EdgesBegin() const
      { return edges_.begin(); }

      ConstEdgeIterator EdgesEnd() const
      { return edges_.end(); }

    private:

      EdgeList edges_;

    friend std::ostream& operator<<(std::ostream& os, const Pulse& p);

  };

  /*!
   * @brief Logical AND between two pulses
   */
  Pulse AND(const Pulse& a, const Pulse& b);

  /*!
   * @brief Logical OR between two pulses
   */
  Pulse OR(const Pulse& a, const Pulse& b);

  /*!
   * @brief Logical negation of a pulse
   */
  Pulse NOT(const Pulse& a);

  /*!
   * @brief Delay of a pulse
   */
  Pulse DELAY(const Pulse& p, const double dt);

}

#endif // DATACLASSES_MATH_DIGITALLOGIC_H_INCLUDED

