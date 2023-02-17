#ifndef TABLENODE_H
#define TABLENODE_H

/// Including DBE
#include <QStringList>
/// Including config headers
#include "oksdbinterfaces/Schema.hpp"

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
  TableAttributeNode ( dunedaq::oksdbinterfaces::attribute_t Attribute, const QStringList & NodeData );
  ~TableAttributeNode();
  QStringList GetData() const;
  dunedaq::oksdbinterfaces::attribute_t GetAttribute() const;
private:
  dunedaq::oksdbinterfaces::attribute_t AttributeData;
};

class TableRelationshipNode: public TableNode
{
public:
  TableRelationshipNode ( dunedaq::oksdbinterfaces::relationship_t Relationship,
                          const QStringList & NodeData );
  ~TableRelationshipNode();
  QStringList GetData() const;
  dunedaq::oksdbinterfaces::relationship_t GetRelationship() const;
private:
  dunedaq::oksdbinterfaces::relationship_t RelationshipData;
};

}  // namespace dbe
#endif // TABLENODE_H
