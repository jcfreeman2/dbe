/// Including DBE
#include "dbe/CommitDialog.hpp"

dbe::CommitDialog::~CommitDialog() = default;

dbe::CommitDialog::CommitDialog ( QWidget * parent )
  : QDialog ( parent ),
    ui ( new Ui::CommitDialog )
{
  /// Setting Ui
  setupUi ( this );

  /// Setting initial values
  setModal ( true );
  setSizePolicy ( QSizePolicy::Preferred, QSizePolicy::Preferred );
  setToolTip ( "Enter the commit message and press the ok button." );
  show();
  OkButton->setDisabled ( true );

  /// Setting Controller
  SetController();
}

void dbe::CommitDialog::SetController()
{
  connect ( OkButton, SIGNAL ( clicked() ), this, SLOT ( OkCommitMessage() ),
            Qt::UniqueConnection );
  connect ( CancelButton, SIGNAL ( clicked() ), this, SLOT ( CancelCommitMessage() ),
            Qt::UniqueConnection );
  connect ( CommitMessageLine, SIGNAL ( textEdited ( QString ) ), this,
            SLOT ( CommitMessageEdited ( QString ) ), Qt::UniqueConnection );
}

QString dbe::CommitDialog::GetCommitMessage() const
{
  return CommitMessageLine->text();
}

void dbe::CommitDialog::CommitMessageEdited ( QString TextEdited )
{
  if ( !CommitMessageLine->text().isEmpty() )
  {
    OkButton->setEnabled ( true );
  }
  else
  {
    OkButton->setEnabled ( false );
  }
}

void dbe::CommitDialog::OkCommitMessage()
{
  accept();
}

void dbe::CommitDialog::CancelCommitMessage()
{
  reject();
}
