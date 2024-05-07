
#include "GraphBuilder.hpp"

#include "dbe/confaccessor.hpp"

#include <QFileInfo>

namespace dbe {

  GraphBuilder::GraphBuilder(const std::string& oksfilename) :
    m_oksfilename(oksfilename),
    m_included_classes {
		       { TopGraphLevel::kSession, {"Segment", "Application", "Network"} },
		       { TopGraphLevel::kSegment, {"Segment", "Application", "Network"} },
		       { TopGraphLevel::kApplication, {"Module", "Queue"} },
		       { TopGraphLevel::kModule, {""} }
    }
  {

    QFileInfo database_file(QString::fromStdString(oksfilename));

    if (database_file.exists()) {
      QString path_to_database = QString(database_file.absoluteFilePath());

      {
	confaccessor::setdbinfo( path_to_database, dbe::dbinfo::oks);
	
	if (confaccessor::load()) {
	  TLOG () << "Database initialized from \"" << oksfilename << "\"";
	} else {
	  ers::fatal(dbe::GeneralGraphToolError(ERS_HERE, "Could not load database. Check environment variable DUNEDAQ_DB_PATH"));
	}
      }
    } else {
      std::stringstream errmsg;
      errmsg << "Cannot open database. File error for file \"" << oksfilename << "\"";
      ers::fatal(dbe::GeneralGraphToolError(ERS_HERE, errmsg.str()));
    }
  }
		       
  void GraphBuilder::construct_graph(const TopGraphLevel level) {
    auto oksclasslist = m_included_classes[ level ];

    for (const auto& oksclass : oksclasslist) {
      TLOG() << "ALLOWING " << oksclass;
    }
  }

} // namespace dbe

