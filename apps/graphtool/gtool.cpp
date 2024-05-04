/************************************************************
 * gtool.cpp
 *
 *  Created on: Jun 10, 2016
 *      Author: Leonidas Georgopoulos
 ************************************************************/

#include "dbe/graphtool.hpp"
#include "dbe/confaccessor.hpp"
#include "dbe/gtool.hpp"
#include "dbe/messenger.hpp"
#include "dbe/config_api_info.hpp"
#include "dbe/config_reference.hpp"
#include "dbe/config_api.hpp"

#include "logging/Logging.hpp"


#include <QFileInfo>

#include <boost/graph/graphviz.hpp>

#include <vector>
#include <string>
#include <algorithm>

namespace dbe
{
namespace tool
{
namespace graph
{

//------------------------------------------------------------------------------------------
gtool::gtool ( std::string const & rl, dbinfo dbtype )
  :
  this_src ( rl )
{
  QFileInfo DatabaseFile ( QString::fromStdString ( rl ) );

  if ( DatabaseFile.exists() )
  {
    QString path_to_database = QString ( DatabaseFile.absoluteFilePath() );
    {
      confaccessor::setdbinfo ( path_to_database, dbtype );
      TLOG() << "Database location set";

      if ( confaccessor::load() )
      {
        TLOG () << "Database initialized";
      }
      else
      {
	ers::error(dbe::GeneralGraphToolError(ERS_HERE, "Could not load database. Check environment variable DUNEDAQ_DB_PATH"));
	throw std::string ( "Could not load database" );
      }
    }
  }
  else
  {
    std::stringstream errmsg;
    errmsg << "Cannot open database. File error for file " << rl;
    ers::error(dbe::GeneralGraphToolError(ERS_HERE, errmsg.str()));
  }
}

gtool::t_graph const & gtool::getgraph() const
{
  return this_graph;
}

void gtool::load_all()
{
  TLOG () << "Start load objects into internal cache" ;

  std::vector<std::string> all_classes =
  { config::api::info::onclass::allnames<std::vector<std::string>>() };
  
  for ( auto const & x : all_classes )
  {
    load_all_class_objects ( x );
  }

  TLOG () << "All objects loaded into internal cache. #objects:" << std::to_string ( this_all_objects.size() ) ;
}

void gtool::load_all_class_objects ( std::string const & cname )
{
  std::vector<dbe::tref> all_objects = config::api::info::onclass::objects<false> ( cname,
                                                                                    false );

  TLOG () << "Class to load: " << cname ;

  for ( dbe::tref const & x : all_objects )
  {
    this_all_objects.push_back ( x );
  }

  TLOG () << "Class load success: #objects:" << std::to_string ( all_objects.size() ) ;
}

void gtool::create_graph()
{
  TLOG () << "Database graph generation initiated";
  already_processed.clear();

  for ( auto const & x : this_all_objects )
  {
    add_object_and_friends ( this_graph, x, already_processed );
  }

  TLOG () << "Graph generation completed successfully" ;
}

gtool::t_vertex gtool::add_object ( gtool::t_graph & g, tref const & x, bool uniqueness )
{
  vertex_label xl
  { x.UID(), x.class_name(), x.full_name() };

  if ( not uniqueness )
  {
    // Multiple vertices with the same properties can be added to this graph
    return boost::add_vertex ( xl, g );
  }
  else
  {
    bool result;
    boost::graph_traits<t_graph>::vertex_iterator viter;
    std::tie ( result, viter ) = lookup ( g, xl );

    if ( result )
    {
      return *viter;
    }
    else
    {
      return boost::add_vertex ( xl, g );

    }
  }
}

gtool::t_vertex gtool::add_object ( gtool::t_graph & g, tref const & x,
                                    t_registry & registry )
{
  t_registry::iterator vat = registry.find ( x.full_name() );

  if ( std::end ( registry ) == vat )
  {
    TLOG () << "Adding object " << x.full_name() ;
    // not in the registry implies it is not in the graph , i.e. its user responsibility
    t_registry::iterator v;
    std::tie ( v, std::ignore ) = registry.emplace ( x.full_name(), add_object ( g, x,
                                                                                 false ) );
    return v->second;
  }
  else
  {
    return vat->second;
  }
}

gtool::t_vertex gtool::add_object_and_friends ( t_graph & g, tref const & o,
                                                t_registry & registry )
{
  TLOG() << "Processing object " << o.full_name() ;
  t_vertex ov = add_object ( g, o, registry );

  std::vector<tref> friends
  { config::api::graph::linked::by::object<tref> ( o ) }; // get all neighbors
  TLOG () <<  "Processing object " << o.full_name() << ", # friends: " << friends.size() ;

  for ( tref const & x : friends )
  {
    t_vertex xv = add_object ( g, x, registry );
    boost::add_edge ( ov, xv, g );
    TLOG() << "Add edge " << o.full_name() << " -> " << x.full_name();
  }

  return ov;
}

gtool::t_lookup_ret gtool::lookup ( t_graph const & g, vertex_label const & l )
{
  auto const & vertices = boost::vertices ( g );

  for ( auto v = vertices.first; v != vertices.second; ++v )
  {
    if ( l.label == boost::get ( &vertex_label::label, g, *v ) )
    {
      return t_lookup_ret ( true, v );
    }
  }

  return t_lookup_ret ( false, vertices.second );
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
writegraph::writegraph ( std::string const & s )
  :
  this_dest ( s )
{
}

int writegraph::operator() ( gtool const & x ) const
{
  TLOG () << "Saving result" << this_dest ;
  write ( x.getgraph() );
  TLOG () << "Result sent to output" << this_dest ;
  return EXIT_SUCCESS;
}

void writegraph::write ( gtool::t_graph const & g ) const
{
  graph::write ( g, this_dest );
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
void write ( gtool::t_graph const & g, std::string const & ofn )
{
  if ( ofn.empty() )
  {
    write_to_cout ( g );
  }
  else
  {
    write_to_file ( g, ofn );
  }
}

void write_to_cout ( gtool::t_graph const & g )
{
  TLOG () << "Sending output to stdout";
  boost::write_graphviz (
			 std::cout, g, boost::make_label_writer ( boost::get ( &gtool::vertex_label::label, g ) ) ) ;
}

void write_to_file ( gtool::t_graph const & g, std::string const & ofn )
{
  std::ofstream of;
  of.open ( ofn );

  if ( of.is_open() )
  {
    TLOG () << "Sending output to file. File name:" << ofn ;
    boost::write_graphviz (
      of, g, boost::make_label_writer ( boost::get ( &gtool::vertex_label::label, g ) ) );

    of.close();

    if ( of.fail() )
    {
      std::stringstream errmsg;
      errmsg << "Could not close file " << ofn;
      ers::error(dbe::GeneralGraphToolError(ERS_HERE, errmsg.str()));
    }
  } else {
    std::stringstream errmsg;
    errmsg << "Output could not be written to file, stream could not be opened: " << ofn;
    ers::error(dbe::GeneralGraphToolError(ERS_HERE, errmsg.str()));
  }
}
//------------------------------------------------------------------------------------------

} /* namespace graph */
} /* namespace tool */
} /* namespace dbe */
