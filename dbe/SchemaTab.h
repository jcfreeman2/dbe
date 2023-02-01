#ifndef SCHEMATAB_H
#define SCHEMATAB_H

/// Include QT Headers
#include <QWidget>
#include <QGraphicsView>
/// Include Schema Editor
#include "SchemaGraphicsScene.h"

namespace dbse
{

class SchemaTab: public QWidget
{
  Q_OBJECT
public:
  explicit SchemaTab ( QWidget * parent = nullptr );
  ~SchemaTab();
  SchemaGraphicsScene * GetScene() const;
  QGraphicsView * GetView() const;
private:
  QGraphicsView * GraphView;
  SchemaGraphicsScene * GraphScene;
};

}  // namespace dbse
#endif // SCHEMATAB_H
