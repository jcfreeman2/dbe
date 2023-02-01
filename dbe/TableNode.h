#ifndef TABLENODE_H
#define TABLENODE_H

/// Including DBE
#include <QStringList>
/// Including config headers
#include <config/Schema.h>

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
  TableAttributeNode ( daq::config::attribute_t Attribute, const QStringList & NodeData );
  ~TableAttributeNode();
  QStringList GetData() const;
  daq::config::attribute_t GetAttribute() const;
private:
  daq::config::attribute_t AttributeData;
};

class TableRelationshipNode: public TableNode
{
public:
  TableRelationshipNode ( daq::config::relationship_t Relationship,
                          const QStringList & NodeData );
  ~TableRelationshipNode();
  QStringList GetData() const;
  daq::config::relationship_t GetRelationship() const;
private:
  daq::config::relationship_t RelationshipData;
};

}  // namespace dbe
#endif // TABLENODE_H
