#ifndef TABLENODE_H
#define TABLENODE_H

/// Including DBE
#include <QStringList>
/// Including config headers
#include "conffwk/Schema.hpp"

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
  TableAttributeNode ( dunedaq::conffwk::attribute_t Attribute, const QStringList & NodeData );
  ~TableAttributeNode();
  QStringList GetData() const;
  dunedaq::conffwk::attribute_t GetAttribute() const;
private:
  dunedaq::conffwk::attribute_t AttributeData;
};

class TableRelationshipNode: public TableNode
{
public:
  TableRelationshipNode ( dunedaq::conffwk::relationship_t Relationship,
                          const QStringList & NodeData );
  ~TableRelationshipNode();
  QStringList GetData() const;
  dunedaq::conffwk::relationship_t GetRelationship() const;
private:
  dunedaq::conffwk::relationship_t RelationshipData;
};

}  // namespace dbe
#endif // TABLENODE_H
