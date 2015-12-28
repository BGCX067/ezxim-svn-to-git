#ifndef _IM_GLOBAL_
#define _IM_GLOBAL_

#include "clistutils.h"
#include "clistwidget.h"
#include "chatwidget.h"
#include "clistcontainerwidget.h"
#include "mainwidget.h"
#include "im_eventlistener.h"

class IMGlobal // Static global variables
{
public:
  static IMContactList                *mainContactList;
  static IMContactListWidget          *mainContactListWidget;
  static IMContactListContainerWidget *mainContactListContainerWidget;
  static IMChatWidget                 *mainChatWidget;
  static IMWindowSwitcher             *windowSwitcher;
  static IMEventListener              *eventListener;
};

#endif
