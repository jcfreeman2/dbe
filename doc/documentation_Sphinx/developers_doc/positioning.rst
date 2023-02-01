###############################
Positioning of widgets in "dbe"
###############################

   * in ``dbe/object/attrandreleditors.h``, the ``m_pos`` variables store the position (QPoint coordinates) of the widget. Then the getPoint() and setPoint() functions handle the value. And those are then called by the tableView delegate (``src/structure/tabledelegate``) to set the position of the widget.

