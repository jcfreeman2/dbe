#ifndef TABLENODE_H
#define TABLENODE_H

/// Including DBE
#include <QStringList>
/// Including config headers
#include "config/Schema.hpp"

namespace dbe
{

class TableNode
{
public:
  virtual ~TableNode();
  TableNode ( QStringList const & NodeData );
  virtual QStringList GetData() const;
  void resetdata ( QStringList const & );
protected:
  QStringList Data;
};

class TableAttributeNode: public TableNode
{
public:
  TableAttributeNode ( dunedaq::config::attribute_t Attribute, const QStringList & NodeData );
  ~TableAttributeNode();
  QStringList GetData() const;
  dunedaq::config::attribute_t GetAttribute() const;
private:
  dunedaq::config::attribute_t AttributeData;
};

class TableRelationshipNode: public TableNode
{
public:
  TableRelationshipNode ( dunedaq::config::relationship_t Relationship,
                          const QStringList & NodeData );
  ~TableRelationshipNode();
  QStringList GetData() const;
  dunedaq::config::relationship_t GetRelationship() const;
private:
  dunedaq::config::relationship_t RelationshipData;
};

}  // namespace dbe
#endif // TABLENODE_H
