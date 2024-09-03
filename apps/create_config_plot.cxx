/************************************************************
 * create_config_plot.cpp
 *
 * JCF, May-7-2024
 *
 * Main file of create_config_plot used to generate GraphViz dot
 * files of DUNE DAQ configurations. The latter can
 * be used to generate graphs that visualize the database
 * patterns
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 *
 *************************************************************/
#include "GraphBuilder.hpp"
#include "dbe/confaccessor.hpp"

#include "logging/Logging.hpp"
#include "appmodel/appmodelIssues.hpp"

#include <boost/program_options.hpp>

#include <map>
#include <numeric>
#include <sstream>
#include <stdexcept>

namespace bpo = boost::program_options;

int main ( int argc, char * argv[] )
{
  // Setting language variable to English (otherwise "," is interpreted as "." in numbers)
  setenv ( "LC_ALL", "C", 1 );

  std::string oksfilename = "";
  std::string outputfilename = "";
  std::string level = "";
  std::string object_uid = "";

  bpo::options_description options_description (
    "Allowed options", 128 );

  options_description.add_options()

  ( "help,h", "Provide help message" )

  ( "file,f", bpo::value<std::string> ( &oksfilename ), "OKS database file name" )

  ( "level,l", bpo::value<std::string>(&level), "base level (session, segment, application, module)")

  ( "object,b", bpo::value<std::string>(&object_uid), "base object UID")

  ( "output,o", bpo::value<std::string> ( &outputfilename ),
    "Output DOT file which can be used as input to GraphViz" );

  bpo::variables_map args;

  auto display_help_message = [&options_description]()
  {
    TLOG() 
        << "DBE create_config_plot : Generate dot graphs from database files"
        << std::endl
        << std::endl
        << "Usage: create_config_plot -f/--file <input OKS file> -b/--object <base object UID for a session or application> -l/--level <session, segment, application or module> (-o/--output <output DOT file>)"
        << "\nIf no output file is specified the result is sent to stdout"
        << std::endl
        << std::endl
        << options_description
        << std::endl;
  };

  try
  {
    bpo::store ( bpo::command_line_parser ( argc, argv ).options ( options_description ).run(),
                 args );
    bpo::notify ( args );

    const std::map<std::string, dbe::GraphBuilder::ObjectKind> level_as_enum {
      {"session", dbe::GraphBuilder::ObjectKind::kSession },
      {"segment", dbe::GraphBuilder::ObjectKind::kSegment },
      {"application", dbe::GraphBuilder::ObjectKind::kApplication },
      {"module", dbe::GraphBuilder::ObjectKind::kModule }
    };
    
    if ( args.count ( "help" ) || ! args.count ( "file" ) || ! args.count("level") || ! level_as_enum.count(level) || ! args.count("object") )
    {
      display_help_message();
      return EXIT_FAILURE;
    }
    
    dbe::GraphBuilder graphbuilder(oksfilename);
    graphbuilder.construct_graph( level_as_enum.at( level ), object_uid );

    graphbuilder.write_graph(outputfilename);
    
  } catch (const bpo::error& e) {

    display_help_message();
    
    std::stringstream errmsgstr;
    errmsgstr << "Incorrect command line argument: " << e.what();
    ers::fatal(dbe::GeneralGraphToolError(ERS_HERE, errmsgstr.str()));

  } catch (dunedaq::appmodel::BadConf& exc) {
    std::stringstream errmsgstr;
    errmsgstr << "Caught BadConf exception: " << exc;
    ers::fatal(dbe::GeneralGraphToolError(ERS_HERE, errmsgstr.str()));

  } catch(const dbe::GeneralGraphToolError& e) {

    ers::fatal(e);

  }

  return 0;

}

