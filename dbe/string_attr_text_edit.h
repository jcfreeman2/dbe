#ifndef STRING_ATTR_TEXT_EDIT_H
#define STRING_ATTR_TEXT_EDIT_H

#include <QTextEdit>
#include <QValidator>

namespace dbe
{
class string_attr_text_edit: public QTextEdit
{
  Q_OBJECT
public:
  explicit string_attr_text_edit ( QWidget * parent = 0, bool IsNull = true, bool IsMulti = false );
  ~string_attr_text_edit();
  void SetNullCheck ( bool IsNull );
  void SetMultiCheck ( bool IsMulti );
  bool IsNullCheck();
  bool IsValid();
  void SetCheckDefault ( bool Default );
  void SetDefaultValue ( QString ValueDefault );
  void ValidateText();
  void SetLoadedDefaultFlag ( bool Loaded );
  void SetValidator ( QValidator * ValidatorSet );
  void SetPopupMenu();
  void CreateActions();
signals:
  void StringValidated();
  void ValueChanged();
  void DecChange();
  void HexChange();
  void OctChange();
public slots:
  void TryValidate ();
  void EmitDecSlot();
  void EmitOctSlot();
  void EmitHexSlot();
protected:
  void contextMenuEvent ( QContextMenuEvent * Event );
private:
  bool Valid;
  bool NullCheck;
  bool CheckDefault;
  bool PopupMenu;
  bool IsMultiValue;
  QMenu * ContextMenu;
  QAction * Dec;
  QAction * Oct;
  QAction * Hex;
  QValidator * Validator;
  QString DefaultValue;
};
} // end namespace dbe

#endif // STRING_ATTR_TEXT_EDIT_H
