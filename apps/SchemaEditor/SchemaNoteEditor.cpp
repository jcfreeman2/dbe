

#include "dbe/SchemaNoteEditor.hpp"
#include "dbe/SchemaGraphicNote.hpp"
#include "ui_SchemaNoteEditor.h"

namespace dbse {
  SchemaNoteEditor::SchemaNoteEditor(SchemaGraphicNote* note, QWidget* parent)
    : QDialog(parent), m_note(note),  m_ui(new Ui::SchemaNoteEditor) {

    m_ui->setupUi(this);
    m_ui->text->setPlainText(m_note->text());
    connect(m_ui->buttons, SIGNAL(accepted()), this, SLOT(update_text()));
  }

  SchemaNoteEditor::~SchemaNoteEditor() {
    delete m_ui;
  }

  void SchemaNoteEditor::update_text() {
    m_note->set(m_ui->text->toPlainText());
    m_note->update_note();
  }
} //namespace dbse
