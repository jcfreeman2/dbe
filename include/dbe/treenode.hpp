#ifndef NODE_H
#define NODE_H

#include "dbe/config_reference.hpp"

#include"oksdbinterfaces/ConfigObject.hpp"
#include"oksdbinterfaces/Configuration.hpp"
#include"oksdbinterfaces/Schema.hpp"
#include "dbe/datahandler.hpp"

namespace dbe
{

class treenode
{
public:
  treenode ( treenode * ParentNode = nullptr );
  treenode ( QString const & Datum, treenode * ParentNode = nullptr );
  treenode ( QStringList const & DataList, treenode * ParentNode = nullptr );

  virtual ~treenode();

  virtual QVariant GetData ( const int Column, int role = Qt::DisplayRole ) const;

  virtual tref GetObject() const;

  int GetRow() const;

  virtual void AddChild ( treenode * Child );

  virtual void RemoveChild ( treenode * Child );

  treenode * GetChild ( const int Row ) const;

  QList<treenode *> GetChildren() const;

  treenode * GetParent() const;

  int ChildCount() const;
  int ColumnCount() const;

  void SetHasStructure ( bool Structure );
  bool GetHasStructure() const;
  void SetWasFetched ( bool Fetched );
  bool GetWasFetched() const;

  void rename ( QString const & );

protected:
  treenode * Parent;
  QList<treenode *> Children;
  QList<QVariant> Data;
  bool HasStructure;
  bool WasFetched;
};

class ClassNode: public treenode
{
public:
  ClassNode ( const dunedaq::oksdbinterfaces::class_t & Info, treenode * ParentNode );
  ~ClassNode();
  virtual QVariant GetData ( const int Column, int role = Qt::DisplayRole ) const;
  dunedaq::oksdbinterfaces::class_t GetClassInfo() const;
  void AddChild ( treenode * Child ) override;
  void RemoveChild ( treenode * Child ) override;

protected:
  void updateData(bool addition);

private:
  dunedaq::oksdbinterfaces::class_t ClassInfo;
  unsigned int numObjects;
};

class ObjectNode: public treenode
{
public:
  ObjectNode ( dref ObjectData, bool IsCopy, treenode * ParentNode );
  ~ObjectNode();
  virtual QVariant GetData ( const int Column, int role = Qt::DisplayRole ) const;
  tref GetObject() const;
private:
  dref configdata;
};

class AttributeNode: public treenode
{
public:
  AttributeNode ( const dunedaq::oksdbinterfaces::attribute_t & AttributeData, treenode * ParentNode );
  ~AttributeNode();
  virtual QVariant GetData ( const int Column, int role = Qt::DisplayRole ) const;
  dunedaq::oksdbinterfaces::attribute_t attribute_t() const;
private:
  dunedaq::oksdbinterfaces::attribute_t attribute_t_definition;
};

class RelationshipNode: public treenode
{
public:
  RelationshipNode ( const dunedaq::oksdbinterfaces::relationship_t & RelationshipData,
                     treenode * ParentNode );
  ~RelationshipNode();
  virtual QVariant GetData ( const int Column, int role = Qt::DisplayRole ) const;
  dunedaq::oksdbinterfaces::relationship_t relation_t() const;
private:
  dunedaq::oksdbinterfaces::relationship_t relation_t_definition;
};

}  // namespace dbe
#endif // NODE_H
