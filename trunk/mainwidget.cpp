#include <qdir.h>
#include <qpopupmenu.h>
#include <qlist.h>
#include <cstdlib>
#include <ZMainWidget.h>
#include <ZLayout.h>
#include <ZApplication.h>

#include "mainwidget.h"
#include "im_global.h"

IMWindowSwitcher::IMWindowSwitcher()
    : QWidgetStack(0, "EZXIM Window Switcher")
{
  IMGlobal::windowSwitcher = this;

  isFullScreen = false;
  setGeometry(ZGlobal::getHomeR());

  connect(this, SIGNAL(aboutToShow(QWidget *)), this, SLOT(switchHook(QWidget *)));
  connect(qApp, SIGNAL(signalKbStateChanged(const QRect &, int, bool, bool)), this, SLOT(slotKbStateChanged(const QRect &, int, bool, bool)));

  IMGlobal::mainContactListContainerWidget = new IMContactListContainerWidget(this);
  addWidget(IMGlobal::mainContactListContainerWidget, 0);
  raiseWidget(IMGlobal::mainContactListContainerWidget);
  connect(IMGlobal::mainContactListWidget, SIGNAL(contactActivated(int)), this, SLOT(showChat(int)));

  IMGlobal::eventListener = new IMEventListener(this);
  qApp->installEventFilter(IMGlobal::eventListener);
}

IMWindowSwitcher::~IMWindowSwitcher()
{
}

void IMWindowSwitcher::showChat(int id)
{
  printf("Opening chat for #%d\n", id);
  IMContact *contact = IMGlobal::mainContactList->getContact(id);
  if (!contact)
    return;
  if (!IMGlobal::mainChatWidget)
  {
    IMGlobal::mainChatWidget = new IMChatWidget(this);
    addWidget(IMGlobal::mainChatWidget, 1);
  }
  IMGlobal::mainChatWidget->startSession(contact->protoId(), contact->userId());
  raiseWidget(IMGlobal::mainChatWidget);
}

void IMWindowSwitcher::showContactList()
{
  raiseWidget(IMGlobal::mainContactListContainerWidget);
}

void IMWindowSwitcher::toggleFullScreen()
{
  isFullScreen = !isFullScreen;
  if (isFullScreen)
    setGeometry(ZGlobal::getScreenR());
  else
    setGeometry(ZGlobal::getHomeR());
}

void IMWindowSwitcher::switchHook(QWidget *widget)
{
  //ZMainWidget *newTopWidget = (ZMainWidget *)widget;
  //if (isFullScreen!=newTopWidget->getFullScreenMode())
  //  newTopWidget->setFullScreenMode(isFullScreen);
}

bool IMWindowSwitcher::getFullScreenMode() const
{
  return isFullScreen;
}

void IMWindowSwitcher::slotKbStateChanged(const QRect & resizeR, int topLevelWinId, bool show, bool changed)
{
  //printf("slotKbStateChanged((%d, %d, %d, %d), %d, %d, %d)\n",
  //       resizeR.left(), resizeR.top(), resizeR.right(), resizeR.bottom(), topLevelWinId, show, changed);
}

