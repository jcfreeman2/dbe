#ifndef SORTING_H
#define SORTING_H

#include "config/Schema.hpp"
#include "dbcontroller.h"
#include <string>

namespace dbe
{

class SortObjects
{
public:
  bool operator() ( std::string const & L, std::string const & R )
  {
    return L.compare ( R ) < 0;
  }

  bool operator() ( dbe::tref const & L, dbe::tref const & R )
  {
    return ( L.UID() ).compare ( ( R.UID() ) ) < 0;
  }

  bool operator () ( const dunedaq::config::attribute_t & L, const dunedaq::config::attribute_t & R )
  {
    int LString = L.p_type;
    int RString = R.p_type;

    return LString > RString;
  }
};

}  // namespace dbe
#endif // SORTING_H
