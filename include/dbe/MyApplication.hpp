#ifndef MYAPPLICATION_H
#define MYAPPLICATION_H

#include <QApplication>
#include <QMessageBox>

extern char const * const dbe_lib_core_version;

namespace dbe
{
class MyApplication: public QApplication
{
  Q_OBJECT
public:
  MyApplication ( int & argc, char ** argv )
    : QApplication ( argc, argv )
  {
  }
  virtual ~MyApplication()
  {
  }
  virtual bool notify ( QObject * rec, QEvent * ev );
};
}  // namespace dbe

#endif // MYAPPLICATION_H
