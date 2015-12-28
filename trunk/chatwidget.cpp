#include <cstdio>
#include <qlayout.h>
#include <qdatetime.h>
#include <qdir.h>
#include <ZMainWidget.h>
#include <ZLayout.h>
#include <ZApplication.h>
#include <qrect.h>
#include "im_global.h"
#include "clistutils.h"
#include "chatwidget.h"

IMChatWidget::IMChatWidget(QWidget *parent)
    : ZMainWidget(true, parent, "IMChatWidget")
{
  QFrame *content = new QFrame(this);
  content->setFrameStyle(QFrame::NoFrame);
  setContentWidget(content);

  QWidget *fakeCST = new QWidget(this, "FakeCST");
  fakeCST->setFixedSize(0, 0);
  setCSTWidget(fakeCST);

  QVBoxLayout *mainLayout = new QVBoxLayout(content, 0, 0);

  QHBoxLayout *sessionListLayout = new QHBoxLayout();
  fullscreenButton = new UTIL_PushButton("", "X", content, UTIL_PushButton::JoinRight);
  fullscreenButton->setFixedSize(24, 24);
  prevSessionButton = new UTIL_PushButton("", "<", content, UTIL_PushButton::JoinLeft | UTIL_PushButton::JoinRight);
  prevSessionButton->setFixedSize(32, 24);
  nextSessionButton = new UTIL_PushButton("", ">", content, UTIL_PushButton::JoinLeft | UTIL_PushButton::JoinRight);
  nextSessionButton->setFixedSize(32, 24);
  quitButton = new UTIL_PushButton("", "Q", content, UTIL_PushButton::JoinLeft);
  quitButton->setFixedSize(24, 24);
  sessionList = new QComboBox(false, content);
  sessionList->setFixedHeight(24);
  currentSession = -1;
  sessions.resize(16);
  sessionListLayout->addWidget(fullscreenButton);
  sessionListLayout->addWidget(prevSessionButton);
  sessionListLayout->addWidget(sessionList);
  sessionListLayout->addWidget(nextSessionButton);
  sessionListLayout->addWidget(quitButton);
  mainLayout->addLayout(sessionListLayout);
  connect(sessionList, SIGNAL(activated(int)), SLOT(startSession(int)));
  connect(prevSessionButton, SIGNAL(clicked()), SLOT(prevSession()));
  connect(nextSessionButton, SIGNAL(clicked()), SLOT(nextSession()));

  QColorGroup *chatColorGroup = new QColorGroup();
  chatColorGroup->setColor(QColorGroup::Background, white);
  chatColorGroup->setColor(QColorGroup::Foreground, black);
  chatColorGroup->setColor(QColorGroup::Base, white);
  chatColorGroup->setColor(QColorGroup::Text, black);
  chatColorGroup->setColor(QColorGroup::Highlight, blue);
  chatColorGroup->setColor(QColorGroup::HighlightedText, white);

  chatLog = new QTextView("", QDir::current().absPath(), content);
  chatLog->setMinimumHeight(24);
  chatLog->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));
  chatLog->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  chatLog->setLineWidth(1);
  chatLog->setHScrollBarMode(QScrollView::AlwaysOff);
  chatLog->setTextFormat(RichText);
  chatLog->setPaperColorGroup(*chatColorGroup);
  mainLayout->addWidget(chatLog);

  QFrame *controlButtonBar = new QFrame(content);
  controlButtonBar->setFixedHeight(27);
  controlButtonBar->setMargin(1);
  controlButtonBar->setLineWidth(0);
  QHBoxLayout *controlButtonLayout = new QHBoxLayout(controlButtonBar, 0, 0);

  menuButton = new UTIL_PushButton("", "M", controlButtonBar, UTIL_PushButton::JoinRight);
  menuButton->setFixedSize(24, 24);
  clearButton = new UTIL_PushButton("", "C", controlButtonBar, UTIL_PushButton::JoinRight | UTIL_PushButton::JoinLeft);
  clearButton->setFixedSize(24, 24);
  historyButton = new UTIL_PushButton("", "H", controlButtonBar, UTIL_PushButton::JoinLeft);
  historyButton->setFixedSize(24, 24);
  sendButton = new UTIL_PushButton("", "=>", controlButtonBar);
  sendButton->setFixedSize(64, 24);

  connect(clearButton, SIGNAL(clicked()), SLOT(clearLog()));
  connect(sendButton, SIGNAL(clicked()), SLOT(sendMessage()));
  setInputMethod(sendButton);

  mainLayout->addWidget(controlButtonBar);
  controlButtonLayout->addWidget(menuButton);
  controlButtonLayout->addWidget(clearButton);
  controlButtonLayout->addWidget(historyButton);
  controlButtonLayout->addStretch();
  controlButtonLayout->addWidget(sendButton);

  inputScrollView = new ZScrollView(content);
  inputField = new ZMultiLineEdit(inputScrollView->viewport(), true, 2);
  inputField->setMinimumWidth(238);
  inputField->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  inputScrollView->addChild(inputField, 0, 0, true);
  inputScrollView->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
  mainLayout->addWidget(inputScrollView);

  connect(quitButton, SIGNAL(clicked()), IMGlobal::windowSwitcher, SLOT(showContactList()));
  connect(fullscreenButton, SIGNAL(clicked()), IMGlobal::windowSwitcher, SLOT(toggleFullScreen()));
  popupMenu = new QPopupMenu(this);
  menuButton->setPopup(popupMenu);

  popupMenu->insertItem("Menu");
  popupMenu->insertSeparator();
  popupMenu->insertItem("But");
  popupMenu->insertItem("nothing");
  popupMenu->insertItem("is");
  popupMenu->insertItem("here.");

  connect(this, SIGNAL(kbStateChanged(bool, const QUuid&, int)), this, SLOT(kbStateChanged(bool, const QUuid&, int)));
}

IMChatWidget::~IMChatWidget()
{
}

void IMChatWidget::showEvent(QShowEvent *)
{
  getCSTWidget(TRUE)->hide();
  ZApplication *zApp = (ZApplication *)qApp;
  zApp->setAutoHideKb(false);
}

void IMChatWidget::hideEvent(QHideEvent *)
{
  ZApplication *zApp = (ZApplication *)qApp;
  zApp->setAutoHideKb(true);
}

void IMChatWidget::sendMessage()
{
  QString msg = inputField->text();
  if (msg.isEmpty()) return;

  inputField->clear();

  addMessage(sessions[currentSession]->protoId, sessions[currentSession]->userId, PrivateOut, msg, QTime::currentTime());
}

int IMChatWidget::findSession(int protoId, const QString &id)
{
  if (currentSession < 0 || currentSession >= sessions.size())
    return -1;
  if (sessions[currentSession] && sessions[currentSession]->protoId == protoId &&
      sessions[currentSession]->userId == id)
    return currentSession;

  for (uint i = 0; i < sessions.count(); i++)
    if (sessions[i] && sessions[i]->protoId == protoId && sessions[i]->userId == id)
      return i;
  return -1;
}

void IMChatWidget::startSession(int protoId, const QString &userId)
{
  int sId = findSession(protoId, userId);
  if (sId < 0)
    sId = createSession(protoId, userId);
  startSession(sId);
}

void IMChatWidget::startSession(int sId)
{
  if (sId < 0)
    return;
  if (currentSession == sId)
    return;
  if (sessions[sId])
  {
    if (currentSession >= 0)
    {
      sessions[currentSession]->currentMessage = inputField->text();
      sessions[currentSession]->logOffset = chatLog->contentsY();
    }
    currentSession = sId;
    chatLog->setText(sessions[sId]->savedChatLog);
    chatLog->setContentsPos(0, sessions[sId]->logOffset);
    inputField->setText(sessions[sId]->currentMessage);
    if (sessionList->currentItem() != currentSession)
      sessionList->setCurrentItem(currentSession);
  }
}

int IMChatWidget::createSession(int protoId, const QString &userId)
{
  if (sessions.size() == sessions.count())
    sessions.resize(sessions.size()*2);
  int sId = sessions.count();
  sessions.insert(sId, new ChatSession);
  sessions[sId]->protoId = protoId;
  sessions[sId]->userId = userId;
  sessions[sId]->userName = "WTF?!";
  IMContact *contact = IMGlobal::mainContactList->getContact(protoId, userId);
  if (contact)
    sessions[sId]->userName = contact->nick();
  sessions[sId]->selfName = "MyNick";
  sessions[sId]->savedChatLog = "";
  sessions[sId]->logOffset = 0;
  sessions[sId]->currentMessage = "";
  sessions[sId]->needReset = TRUE;
  sessions[sId]->lastMessageType = Undefined;
  sessionList->insertItem(sessions[sId]->userName, sId);
  return sId;
}

void IMChatWidget::nextSession()
{
  if (currentSession < 0)
    return;
  uint sId = currentSession + 1;
  if (sId >= sessions.count())
    sId -= sessions.count();
  startSession(sId);
}

void IMChatWidget::prevSession()
{
  if (currentSession < 0)
    return;
  int sId = currentSession - 1;
  if (sId < 0)
    sId += sessions.count();
  startSession(sId);
}

void IMChatWidget::addMessage(int protoId, const QString &userId, MessageType type, QString text, QTime timestamp)
{
  int sId = findSession(protoId, userId);
  if (sId<0)
    sId = createSession(protoId, userId);
  QString logMsg;
  if (type != sessions[currentSession]->lastMessageType)
  {
    switch (type)
    {
    case PrivateIn:
      logMsg = QString("<big><font color=\"#0000FF\">") + sessions[currentSession]->userName + QString(":</font></big><br />");
      break;
    case PrivateOut:
      logMsg = QString("<big><font color=\"#FF0000\">") + sessions[currentSession]->selfName + QString(":</font></big><br />");
      break;
    }
  }
  logMsg += QString("<small><font color=\"#888888\">%1</font></small>  %2<br />").arg(timestamp.toString()).arg(text);
  if (sId == currentSession)
  {
    if (sessions[currentSession]->needReset)
    {
      sessions[currentSession]->needReset = FALSE;
      chatLog->setText(logMsg);
    }
    else
      chatLog->append(logMsg);
  }
  sessions[sId]->savedChatLog += logMsg;
  sessions[sId]->lastMessageType = type;
}

void IMChatWidget::clearLog()
{
  chatLog->setText("");
  sessions[currentSession]->needReset = TRUE;
}

void IMChatWidget::kbStateChanged(bool show, const QUuid&, int kbh)
{
  printf("FullScreenMode is %d, kb height is %d\n", IMGlobal::windowSwitcher->getFullScreenMode(), kbh);
  //if (show && !getFullScreenMode())
  //  IMGlobal::windowSwitcher->toggleFullScreen();
  fullscreenButton->setEnabled(!show);
  if (show && !IMGlobal::windowSwitcher->getFullScreenMode())
    chatLog->setVScrollBarMode(QScrollView::AlwaysOff);
  if (!show && !IMGlobal::windowSwitcher->getFullScreenMode())
    chatLog->setVScrollBarMode(QScrollView::Auto);
  if (!0)
  {
    QRect newGeometry;
    if (IMGlobal::windowSwitcher->getFullScreenMode())
      newGeometry = ZGlobal::getScreenR();
    else
      newGeometry = ZGlobal::getHomeR();
      newGeometry.setHeight(newGeometry.height()-kbh);
    IMGlobal::windowSwitcher->setGeometry(newGeometry);
    ZSetLayout(this, newGeometry);
  }
  getCSTWidget(TRUE)->hide();
}
