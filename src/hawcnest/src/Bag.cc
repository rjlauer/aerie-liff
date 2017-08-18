/*!
 * @file Bag.cc 
 * @brief Implementation of the Bag object dictionary.
 * @author John Pretz
 * @date 23 Jul 2009 
 * @version $Id: Bag.cc 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#include <hawcnest/processing/Bag.h>

using namespace std;

void
Bag::Delete(const string& name)
{
  BagType::iterator iObj = bag_.find(name);
  if (iObj != bag_.end())
    bag_.erase(iObj);

  KeyType::iterator iKey = bagTypes_.find(name);
  if (iKey != bagTypes_.end())
    bagTypes_.erase(iKey);
}

void
Bag::dump(ostream& os)
  const
{
  os << "bag members:\n";
  for (ConstTypeIterator iT = bagTypes_.begin(); iT != bagTypes_.end(); ++iT) {
    os << "  \"" << iT->first << "\" => <" << iT->second << ">";
    if (distance(iT, bagTypes_.end()) > 1)
      os << '\n';
  }
}

