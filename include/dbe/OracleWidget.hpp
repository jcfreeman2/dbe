#ifndef ORACLEWIDGET_H
#define ORACLEWIDGET_H

#include <memory>
#include <QWidget>

namespace dbe
{
namespace Ui
{
class OracleWidget;
}  // namespace Ui

class OracleWidget: public QWidget
{
  Q_OBJECT
public:
  ~OracleWidget();
  explicit OracleWidget ( QWidget * parent = 0 );

private:
  void SetController();

  std::unique_ptr<dbe::Ui::OracleWidget> ui;

private slots:
  void ProcessOracleCommand();

signals:
  void OpenOracleConfig ( const QString & ConfigStream );
};

}  // namespace dbe
#endif // ORACLEWIDGET_H
