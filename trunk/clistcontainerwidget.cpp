
#include <qlayout.h>
#include <qframe.h>
#include <UTIL_CST.h>
#include <ZLayout.h>
#include <UTIL_PushButton.h>
#include "clistcontainerwidget.h"
#include "im_global.h"

#define DEBUG_FAKE_CONTACTS

IMContactListContainerWidget::IMContactListContainerWidget(QWidget *parent)
  : ZMainWidget(true, parent, "ContactListContainerWidget")
{
  QFrame *content = new QFrame(this);
  content->setFrameStyle(QFrame::NoFrame);
  setContentWidget(content);

  UTIL_CST *cst = new UTIL_CST(this, "Select");
  setCSTWidget(cst);

  QVBoxLayout *contentLayout = new QVBoxLayout(content);

  QFrame *topToolBar = new QFrame(content);
  topToolBar->setFrameStyle(QFrame::NoFrame);
  //topToolBar->setFixedHeight(27);
  QHBoxLayout *topToolBarLayout = new QHBoxLayout(topToolBar);

  UTIL_PushButton *fullscreenButton = new UTIL_PushButton("", "X", topToolBar, 0);
  fullscreenButton->setFixedSize(24, 24);
  connect(fullscreenButton, SIGNAL(clicked()), IMGlobal::windowSwitcher, SLOT(toggleFullScreen()));
  topToolBarLayout->addWidget(fullscreenButton);
  topToolBarLayout->addStretch();

  contentLayout->addWidget(topToolBar);

  IMGlobal::mainContactListWidget = new IMContactListWidget(NULL, content);
  IMGlobal::mainContactList = IMGlobal::mainContactListWidget->contactList();

  contentLayout->addWidget(IMGlobal::mainContactListWidget);

#ifdef DEBUG_FAKE_CONTACTS
  QFile clfile("clist.xml");
  IMGlobal::mainContactList->import(QXmlInputSource(clfile));
#endif
}

IMContactListContainerWidget::~IMContactListContainerWidget()
{

}

