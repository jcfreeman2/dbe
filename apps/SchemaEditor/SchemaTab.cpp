/// Including QT Headers
#include <QVBoxLayout>
/// Including Schema Editor
#include "dbe/SchemaTab.hpp"

dbse::SchemaTab::SchemaTab ( QWidget * parent )
  : QWidget ( parent )
{
  /// Generating view/schema
  GraphView = new QGraphicsView();
  GraphScene = new SchemaGraphicsScene();
  GraphView->setScene ( GraphScene );
  GraphView->centerOn ( 0, 0 );
  /// Adjusting layout
  QVBoxLayout * TabLayout = new QVBoxLayout ( this );
  TabLayout->addWidget ( GraphView );
}

dbse::SchemaGraphicsScene * dbse::SchemaTab::GetScene() const
{
  return GraphScene;
}

QGraphicsView * dbse::SchemaTab::GetView() const
{
  return GraphView;
}

dbse::SchemaTab::~SchemaTab() = default;
