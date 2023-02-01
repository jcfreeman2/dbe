#ifndef KERNELWRAPPER_H
#define KERNELWRAPPER_H

/// Including Qt
#include <QStringList>
#include <QObject>
/// Including c++
#include <vector>
#include <set>
/// Including Schema
#include "SchemaCommand.h"

class OksKernel;
class OksClass;

namespace dbse
{

class KernelWrapper: public QObject
{
  friend class SchemaMainWindow;
  friend class SchemaClassEditor;
  friend class SchemaAttributeEditor;
  friend class SchemaMethodEditor;
  friend class SchemaRelationshipEditor;

  Q_OBJECT
public:
  static KernelWrapper & GetInstance();
  OksKernel * GetKernel();
  void SetActiveSchema ( const std::string & ActiveSchema );
  void GetClassList ( std::vector<OksClass *> & ClassList ) const;
  void GetClassListString ( QStringList & ClassListString ) const;
  void GetSchemaFiles ( std::vector<std::string> & SchemaFiles );
  void GetIncludedList ( const std::string & FileName,
                         std::set<std::string> & IncludedFiles );
  bool IsFileWritable ( std::string & FileName ) const;
  bool IsActive() const;
  OksClass * FindClass ( std::string ClassName ) const;
  void LoadSchema ( const std::string & SchemaName ) const;
  void SaveAllSchema() const;
  void CloseAllSchema() const;
  void CreateNewSchema ( const std::string & SchemaName ) const;
  bool AnyClassReferenceThis ( OksClass * SchemaClass );
  QString GetCardinalityStringRelationship ( OksRelationship * SchemaRelationship ) const;
  /// Editor Functions
  void SetInheritanceMode ( bool Mode );
  bool GetInheritanceMode() const;
  /// Stack Functions
  QUndoStack * GetUndoStack();
  /// Commands Functions
  /// Class commands
  void PushSetAbstractClassCommand ( OksClass * Class, bool Value );
  void PushSetDescriptionClassCommand ( OksClass * Class, std::string Description );
  void PushAddSuperClassCommand ( OksClass * Class, std::string SuperClass );
  void PushRemoveSuperClassCommand ( OksClass * Class, std::string SuperClass );
  void PushCreateClassCommand ( std::string ClassName, std::string ClassDescription,
                                bool Abstract );
  void PushRemoveClassCommand ( OksClass * Class, std::string ClassName,
                                std::string ClassDescription, bool Abstract );
  /// Relationship commands
  void PushSetNameRelationshipCommand ( OksClass* Class, OksRelationship * Relationship, std::string Name );
  void PushSetClassTypeRelationshipCommand ( OksClass* Class, OksRelationship * Relationship,
                                             std::string ClassType );
  void PushSetDescriptionRelationshipCommand ( OksClass* Class, OksRelationship * Relationship,
                                               std::string Description );
  void PushSetLowCcRelationshipCommand (
    OksClass* Class, OksRelationship * Relationship, OksRelationship::CardinalityConstraint NewCardinality );
  void PushSetHighCcRelationshipCommand (
    OksClass* Class, OksRelationship * Relationship, OksRelationship::CardinalityConstraint NewCardinality );
  void PushSetIsCompositeRelationshipCommand ( OksClass* Class, OksRelationship * Relationship, bool Value );
  void PushSetIsDependentRelationshipCommand ( OksClass* Class, OksRelationship * Relationship, bool Value );
  void PushSetIsExclusiveRelationshipCommand ( OksClass* Class, OksRelationship * Relationship, bool Value );
  void PushAddRelationship ( OksClass * Class, std::string Name, std::string Description,
                             std::string Type, bool Composite, bool Exclusive, bool Dependent,
                             OksRelationship::CardinalityConstraint LowCc,
                             OksRelationship::CardinalityConstraint HighCc );
  void PushRemoveRelationship ( OksClass * Class, OksRelationship * Relationship,
                                std::string Name, std::string Description, std::string Type,
                                bool Composite, bool Exclusive, bool Dependent,
                                OksRelationship::CardinalityConstraint LowCc,
                                OksRelationship::CardinalityConstraint HighCc );
  /// Method implementation commands
  void PushSetMethodImplementationLanguage ( OksClass * Class, OksMethod * Method, OksMethodImplementation * Implementation,
                                             std::string Language );
  void PushSetMethodImplementationPrototype ( OksClass * Class, OksMethod * Method, OksMethodImplementation * Implementation,
                                              std::string Prototype );
  void PushSetMethodImplementationBody ( OksClass * Class, OksMethod * Method, OksMethodImplementation * Implementation,
                                         std::string Body );
  void PushAddMethodImplementationComand ( OksClass * Class, OksMethod * Method, std::string Language,
                                           std::string Prototype, std::string Body );
  void PushRemoveMethodImplementationComand ( OksClass * Class, OksMethod * Method, std::string Language,
                                              std::string Prototype, std::string Body );
  /// Method commands
  void PushSetNameMethodCommand ( OksClass * Class, OksMethod * Method, std::string name );
  void PushSetDescriptionMethodCommand ( OksClass * Class, OksMethod * Method, std::string description );
  void PushAddMethodCommand ( OksClass * Class, std::string name, std::string description );
  void PushRemoveMethodCommand ( OksClass * Class, OksMethod * Method, std::string name,
                                 std::string description );
  /// Attribute commands
  void PushSetAttributeNameCommand ( OksClass * Class, OksAttribute * Attribute, std::string NewName );
  void PushSetAttributeDescriptionCommand ( OksClass * Class,
                                            OksAttribute * Attribute,
                                            std::string NewDescription );
  void PushSetAttributeTypeCommand ( OksClass * Class, OksAttribute * Attribute, std::string NewType );
  void PushSetAttributeRangeCommand ( OksClass * Class, OksAttribute * Attribute, std::string NewRange );
  void PushSetAttributeFormatCommand ( OksClass * Class, OksAttribute * Attribute,
                                       OksAttribute::Format NewFormat );
  void PushSetAttributeMultiCommand ( OksClass * Class, OksAttribute * Attribute, bool NewIsMulti );
  void PushSetAttributeIsNullCommand ( OksClass * Class,OksAttribute * Attribute, bool NewIsNull );
  void PushSetAttributeInitialValuesCommand ( OksClass * Class, OksAttribute * Attribute,
                                              std::string NewValues );
  void PushAddAttributeCommand ( OksClass * Class, std::string name, std::string type,
                                 bool is_mv, std::string range, std::string init_values,
                                 std::string description, bool is_null,
                                 OksAttribute::Format format = OksAttribute::Format::Dec );
  void PushRemoveAttributeCommand ( OksClass * Class, OksAttribute * Attribute,
                                    std::string name,
                                    std::string type, bool is_mv, std::string range,
                                    std::string init_values, std::string description,
                                    bool is_null, OksAttribute::Format format =
                                      OksAttribute::Format::Dec );
private:
  KernelWrapper ( QObject * parent = nullptr );
  OksKernel * Kernel;
  QUndoStack * CommandStack;
  bool InheritanceMode;
signals:
  void ClassCreated();
  void ClassRemoved ( QString ClassName );
  void ClassUpdated ( QString ClassName );
  void RebuildAttributeModel();
};

}  // namespace dbse
#endif // KERNELWRAPPER_H
