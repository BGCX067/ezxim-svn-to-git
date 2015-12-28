#ifndef _CLISTCONTAINERWIDGET_
#define _CLISTCONTAINERWIDGET_

#include <ZMainWidget.h>
#include "clistwidget.h"

/**
  Виджет-контейнер, содержащий сам контакт-лист и всякие кнопочки.
*/
class IMContactListContainerWidget: public ZMainWidget
{
  public:
    IMContactListContainerWidget(QWidget *parent);
    virtual ~IMContactListContainerWidget();
};

#endif

