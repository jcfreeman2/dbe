/// Including QT Headers
#include <QKeyEvent>
/// Including DBE
#include "SearchComboBox.h"

dbe::SearchComboBox::SearchComboBox ( QWidget * parent )
  : QComboBox ( parent ),
    UserText ( QString ( "" ) )
{
  connect ( this, SIGNAL ( currentIndexChanged ( const QString & ) ), this,
            SLOT ( ChangeToolTip ( const QString & ) ) );
}

void dbe::SearchComboBox::focusInEvent ( QFocusEvent * Event )
{
  UserText = currentText();
  QComboBox::focusInEvent ( Event );
}

void dbe::SearchComboBox::keyPressEvent ( QKeyEvent * Event )
{
  switch ( Event->key() )
  {
  case Qt::Key_Return:
    emit ReturnPressed();
    return;

  default:
    break;
  }

  QComboBox::keyPressEvent ( Event );

  if ( currentText().compare ( UserText ) != 0 )
  {
    UserText = currentText();
    emit TextModified ( currentText() );
  }
}

void dbe::SearchComboBox::ChangeToolTip ( const QString & Text )
{
  setToolTip ( QString ( "Search mode: %1" ).arg ( Text ) );
}
