/// Including QT Headers
#include <QApplication>
#include <QCursor>
#include <QEvent>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsView>
#include <QPainter>

#include <QToolTip>

#include "dbe/SchemaGraphicNote.hpp"
#include "dbe/SchemaGraphicsScene.hpp"
#include "dbe/SchemaNoteEditor.hpp"

#include <iostream>

namespace dbse {

SchemaGraphicNote::SchemaGraphicNote (const QString& name,
                                      const QString& text,
                                      QGraphicsObject* parent)
  : QGraphicsObject (parent),
    m_name(name),
    m_text(text) {

  setAcceptHoverEvents(true);
  m_font = QFont( "Helvetica [Cronyx]", 9);
  m_bold_font = QFont( "Helvetica [Cronyx]", 9, QFont::DemiBold);
  m_default_color = QColor ( 0x1e1b18 );
  m_highlight_color = QColor ( 0x14aaff );
  m_background_color = QColor ( 0xf8f8ff );

  setFlag ( ItemIsMovable );
  setFlag ( ItemSendsGeometryChanges, true );
  setFlag ( ItemSendsScenePositionChanges, true );

}

void SchemaGraphicNote::open_editor() {
  std::cout << "Open note editor\n";
  bool editorFound = false;

  for ( auto widget : QApplication::allWidgets() ) {
    auto editor = dynamic_cast<SchemaNoteEditor*> (widget);
    if ( editor != nullptr ) {
      if ( (editor->objectName() ).compare (m_name) == 0 ) {
        editor->raise();
        editor->setVisible ( true );
        editor->activateWindow();
        editorFound = true;
      }
    }
  }

  if ( !editorFound ) {
    auto editor = new SchemaNoteEditor ( this );
    editor->show();
  }
}

void SchemaGraphicNote::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent* ) {
  open_editor();
}


void SchemaGraphicNote::update_note ( ) {
  update();
}

void SchemaGraphicNote::remove_note ( QString name )
{
  if ( name != m_name ) {
    return;
  }

  /// Updating object representation
  SchemaGraphicsScene* this_scene = dynamic_cast<SchemaGraphicsScene *> ( scene() );

  if ( this_scene ) {
    this_scene->remove_note_object ( this );
  }
}

QRectF SchemaGraphicNote::boundingRect() const
{
  double height = 10;
  int max_len = 0;
  QFontMetrics font_metrics(m_font);
  QRectF font_rect;
  for (auto line: m_text.split("\n")) {
    height += font_metrics.height();
    if (line.size()>max_len) {
      max_len = line.size();
      font_rect = font_metrics.boundingRect(line);
    }
  }
  double width=font_rect.width() + 10;

  return QRectF(0, 0, width, height);
  //return font_metrics.boundingRect(m_text);
}

void SchemaGraphicNote::paint (QPainter* painter,
                               const QStyleOptionGraphicsItem* /*option*/,
                               QWidget* /*widget*/ ) {

  const QPen pen(m_default_color, 0.5);
  QBrush brush(m_background_color);
  painter->setFont ( m_font );
  painter->setBrush (brush);//m_background_color);
//  painter->setBackgroundMode(Qt::OpaqueMode);
  painter->setPen ( pen );
  auto rect = boundingRect();
  painter->drawRect ( rect );
  painter->drawText(rect.adjusted(5,5,-2,-2), Qt::AlignJustify, m_text);
}

QPainterPath SchemaGraphicNote::shape() const {
  QPainterPath path;
  path.addRect ( boundingRect() );
  return path;
}
} //namespace dbse
