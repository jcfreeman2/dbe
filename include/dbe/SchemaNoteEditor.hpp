#ifndef SCHEMANOTEEDITOR_HPP
#define SCHEMANOTEEDITOR_HPP
#include <QDialog>

#include "dbe/SchemaGraphicNote.hpp"

namespace dbse {
  namespace Ui {
    class SchemaNoteEditor;
  } // namespace Ui

  class SchemaNoteEditor : public QDialog {
    Q_OBJECT
  public:
    SchemaNoteEditor(SchemaGraphicNote* note, QWidget* parent=0);
    ~SchemaNoteEditor();
  private:
    SchemaGraphicNote* m_note;
    Ui::SchemaNoteEditor* m_ui;
  private slots:
    void update_text();
  };
} //namespace dbse
#endif // SCHEMANOTEEDITOR_HPP
