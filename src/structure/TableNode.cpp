/// Including DBE
#include "dbe/TableNode.hpp"

dbe::TableNode::TableNode ( const QStringList & NodeData )
  : Data ( NodeData )
{
}

dbe::TableNode::~TableNode()
{
}

QStringList dbe::TableNode::GetData() const
{
  return Data;
}

dbe::TableAttributeNode::TableAttributeNode ( dunedaq::oksdbinterfaces::attribute_t Attribute,
                                              const QStringList & NodeData )
  : TableNode ( NodeData ),
    AttributeData ( Attribute )
{
}

dbe::TableAttributeNode::~TableAttributeNode() = default;

QStringList dbe::TableAttributeNode::GetData() const
{
  return Data;
}

dunedaq::oksdbinterfaces::attribute_t dbe::TableAttributeNode::GetAttribute() const
{
  return AttributeData;
}

dbe::TableRelationshipNode::TableRelationshipNode ( dunedaq::oksdbinterfaces::relationship_t
                                                    Relationship,
                                                    const QStringList & NodeData )
  : TableNode ( NodeData ),
    RelationshipData ( Relationship )
{
}

dbe::TableRelationshipNode::~TableRelationshipNode() = default;

QStringList dbe::TableRelationshipNode::GetData() const
{
  return Data;
}

dunedaq::oksdbinterfaces::relationship_t dbe::TableRelationshipNode::GetRelationship() const
{
  return RelationshipData;
}

void dbe::TableNode::resetdata ( QStringList const & newdata )
{
  Data = newdata;
}
