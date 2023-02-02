/// Including DBE
#include "MyApplication.h"
#include "Exceptions.h"
/// Including TDAq
#include "config/Configuration.hpp"
#include "ers/ers.hpp"
#include "messenger.h"
#include "version.h"

char const * const dbe_lib_core_version = dbe_compiled_version;

bool dbe::MyApplication::notify ( QObject * rec, QEvent * ev )
{
  HERE_AUTO_DEF ( notify );

  try
  {
    return QApplication::notify ( rec, ev );
  }
  catch ( daq::config::Exception const & err )
  {
    ERS_LOG ("MyApplication: daq::config::Exception: " << err );
    ERROR ( "Unexpected error occurred", dbe::config::errors::unwind(err), "daq::config::Exception caught at", HERE );
    return false;
  }
  catch ( ers::Issue const & err )
  {
    ERS_LOG ("MyApplication: ers::Issue: " << err );
    ERROR ( "ers::Issue occurred", dbe::config::errors::unwind(err), "\n\nCaught at:", HERE );
    return false;
  }
  catch ( std::exception const & err )
  {
    ERS_LOG ( "MyApplication: std::exception: " );
    ERROR ( "Error sending event", err.what(), "for object", typeid ( *ev ).name(), "Receiver",
            typeid ( *rec ).name() );
    return false;
  }
  catch ( char const * str )
  {
    ERS_LOG ( "MyApplication: EXCEPTION: " << str << std::endl );
    ERROR ( "Unknown exception", str, "\n\nCaught at: ", HERE );
    return false;
  }
  catch ( ... )
  {
    ERS_DEBUG ( 0, "MyApplication: Unknown exception!" );
    ERROR ( "Unknown exception", "\n\nCaught at: ", HERE );
    return false;
  }

  ERS_LOG ( "MyApplication: outside catch..." );
  return false;
}
