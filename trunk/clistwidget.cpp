#include "clistwidget.h"
#include <qpainter.h>
#include <qapplication.h>
#include <qfontmetrics.h>
#include <qlayout.h>
#include <ZMainWidget.h>
#include <qdatetime.h>
#include <algorithm>
#include <cstdio>

#define CONTACTLIST_BUFFERING

IMContactListWidget::IMContactListWidget(IMContactList *clist, QWidget *parent, QString name)
    : QScrollView(parent, name)
{
  if (clist == NULL)
    m_contactList = new IMContactList(this);
  else
    m_contactList = clist;
  connect(m_contactList, SIGNAL(updated()), this, SLOT(contactListUpdated()));
  setHScrollBarMode(AlwaysOff);
  setFrameStyle(NoFrame);
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  viewport()->setBackgroundMode(NoBackground);

  cursorAt = 0;

  smallIconHeight = 24;
  smallIconWidth = 24;
  bigIconHeight = 32;
  bigIconWidth = 32;
  firstLineFont.setPointSize(18);
  QFontMetrics firstLineFontMetrics(firstLineFont);
  secondLineFont.setPointSize(15);
  QFontMetrics secondLineFontMetrics(secondLineFont);
  normalLineHeight = std::max(smallIconHeight + 2,
                              firstLineFontMetrics.height() + 2);
  cursorLineHeight = std::max(bigIconHeight + 2,
                              1 + firstLineFontMetrics.height() + 1 + secondLineFontMetrics.height() + 1);
  contactOffset = 5;

  lineColorA = QColor(255, 255, 255);
  lineColorB = QColor(225, 225, 225);
  bgColor    = QColor(255, 255, 255);
  cursorColor = QColor(180, 208, 237);
  firstLineFontColor = QPen(QColor(0, 0, 0));
  groupHLinePen = QPen(QColor(50, 50, 50), 3);

  quickSelection = -1;
  connect(&doubleTapTimer, SIGNAL(timeout()), SLOT(doubleTapTimeout()));
}

IMContactListWidget::~IMContactListWidget()
{
}

IMContactList *IMContactListWidget::contactList() const
{
  return m_contactList;
}

void IMContactListWidget::contactListUpdated()
{
  int size = cursorLineHeight - normalLineHeight;
  for (int i = 0; i < m_contactList->items.count(); i++)
  {
    int id = m_contactList->items[i]->id();
    if (getContactVisibility(id))
    {
      size += normalLineHeight;
    }
  }
  resizeContents(visibleWidth(), size);
  printf("ContactList resized to %d (%dx%d/%dx%d) [%dx%d]\n", size, viewport()->width(), viewport()->height(), width(), height(), renderBuffer.width(), renderBuffer.height());
}

void IMContactListWidget::drawContents (QPainter *realpainter, int clipx, int clipy, int clipw, int cliph)
{
  //printf("IMContactListWidget::drawContents(%d, %d, %d, %d);\n", clipx, clipy, clipw, cliph);

  QPainter *painter;
#ifdef CONTACTLIST_BUFFERING
  painter = new QPainter(&renderBuffer);
  painter->translate(-clipx, -clipy);
#else
  painter = realpainter;
#endif

  int i = getContactAtY(clipy);
  //printf("[starting at %d]\n", i);
  if (i < 0)
    return;
  int cy = getContactY(i);
  i = m_contactList->items_index[i];
  while (i < m_contactList->items.count() && cy < clipy + cliph)
  {
    IMContact *currentContact = m_contactList->items[i];
    QRect brect;
    if (!getContactVisibility(currentContact->id()))
    {
      i++;
      continue;
    }
    if (currentContact->id() == cursorAt)
    {
      painter->fillRect(0, cy, viewport()->width(), cursorLineHeight, cursorColor);
      //painter->fillRect(0, cy, viewport()->width(), cursorLineHeight, (i%2==0)?lineColorA:lineColorB);

      switch (currentContact->type())
      {
      case IMContact::NormalContact:
        // Icon
        painter->fillRect(contactOffset, cy + (cursorLineHeight - bigIconHeight) / 2, bigIconWidth, bigIconHeight, QColor(0, 0, 255));
        // Text: First Line
        painter->setPen(firstLineFontColor);
        painter->drawText(contactOffset + bigIconWidth + 2, cy + 1, viewport()->width() - (2 + 2 + bigIconWidth + contactOffset), cursorLineHeight - 2,
                          Qt::SingleLine | Qt::AlignLeft | Qt::AlignTop, currentContact->nick());
        // Text: Second Line
        //...
        break;
      case IMContact::GroupHeader:
        painter->setPen(firstLineFontColor);
        painter->drawText(4, cy + 1, viewport()->width() - 8, cursorLineHeight - 2,
                          Qt::SingleLine | Qt::AlignHCenter | Qt::AlignTop, currentContact->nick(), -1, &brect);
        painter->setPen(groupHLinePen);
        int midline = (brect.top() + brect.bottom()) / 2;
        if (4 < brect.left() - 4)
          painter->drawLine(4, midline, brect.left() - 4, midline);
        if (brect.right() + 4 < viewport()->width() - 1 - 4)
          painter->drawLine(brect.right() + 4, midline, viewport()->width() - 1 - 4, midline);
        break;
      }

      cy += cursorLineHeight;
    }
    else
    {
      painter->fillRect(0, cy, viewport()->width(), normalLineHeight, (i % 2 == 0) ? lineColorA : lineColorB);

      switch (currentContact->type())
      {
      case IMContact::NormalContact:
        // Icon
        painter->fillRect(contactOffset, cy + (normalLineHeight - smallIconHeight) / 2, smallIconWidth, smallIconHeight, QColor(0, 0, 255));
        // Text: Single Line
        painter->setPen(firstLineFontColor);
        painter->drawText(contactOffset + smallIconWidth + 2, cy + 1, viewport()->width() - (2 + 2 + smallIconWidth + contactOffset), normalLineHeight - 2,
                          Qt::SingleLine | Qt::AlignLeft | Qt::AlignVCenter, currentContact->nick());
        break;
      case IMContact::GroupHeader:
        painter->setPen(firstLineFontColor);
        painter->drawText(4, cy + 1, viewport()->width() - 8, normalLineHeight - 2,
                          Qt::SingleLine | Qt::AlignCenter, currentContact->nick(), -1, &brect);
        painter->setPen(groupHLinePen);
        int midline = (brect.top() + brect.bottom()) / 2;
        if (4 < brect.left() - 4)
          painter->drawLine(4, midline, brect.left() - 4, midline);
        if (brect.right() + 4 < viewport()->width() - 1 - 4)
          painter->drawLine(brect.right() + 4, midline, viewport()->width() - 1 - 4, midline);
        break;
      }

      cy += normalLineHeight;
    }
    i++;
  }
  if (cy < clipy + cliph)
    painter->fillRect(0, cy, viewport()->width(), clipy + cliph - cy, bgColor);
#ifdef CONTACTLIST_BUFFERING
  delete painter;
  realpainter->drawPixmap(clipx, clipy, renderBuffer, 0, 0, clipw, cliph);
#endif
}

void IMContactListWidget::contentsMousePressEvent(QMouseEvent *ev)
{
  int newCursorAt = getContactAtY(ev->y());
  if (newCursorAt==-1)
    return;
  printf("select contact %d->%d\n", cursorAt, newCursorAt);
  cursorAt = newCursorAt;
  viewport()->update();
  if (cursorAt == quickSelection)
  {
    itemActivated(quickSelection);
    doubleTapTimer.stop();
  }
  else
  {
    quickSelection = cursorAt;
    doubleTapTimer.start(1000, true);
  }
}

void IMContactListWidget::viewportResizeEvent(QResizeEvent *ev)
{
  renderBuffer.resize(std::max(renderBuffer.width(), ev->size().width()),
                      std::max(renderBuffer.height(), ev->size().height()));
  QScrollView::viewportResizeEvent(ev);
}

int IMContactListWidget::getContactAtY(int y)
{
  int i = -1;
  int reali = -1;
  int cy = 0;
  while (i < (int)m_contactList->items.count() && cy <= y)
  {
    i++;
    if (i >= (int)m_contactList->items.count())
      break;
    reali = m_contactList->items[i]->id();
    if (getContactVisibility(reali))
      if (cursorAt == reali)
        cy += cursorLineHeight;
      else
        cy += normalLineHeight;
  }
  if (i >= m_contactList->items.count())
    reali = -1;
  return reali;
}

int IMContactListWidget::getContactY(int id)
{
  int cy = 0;
  for (int i = 0; i < m_contactList->items_index[id]; i++)
  {
    int id = m_contactList->items[i]->id();
    if (getContactVisibility(id))
    {
      if (cursorAt == id)
        cy += cursorLineHeight;
      else
        cy += normalLineHeight;
    }
  }
  return cy;
}

bool IMContactListWidget::getContactVisibility(int id)
{
  IMContact *cont = m_contactList->getContact(id);
  if (!cont)
  {
    printf("[getContactVisibility] Contact %d not found!\n", id);
    return false;
  }
  IMContactGroup *cgroup = m_contactList->getGroup(cont->groupId());
  if (!cgroup)
  {
    printf("[getContactVisibility] Group %d not found!\n", cont->groupId());
    return false;
  }
  if (cgroup->isCollapsed() && cont->type() != IMContact::GroupHeader)
    return false;
  return true;
}

void IMContactListWidget::doubleTapTimeout()
{
  printf("Double tap timer expired\n");
  quickSelection = -1;
}

void IMContactListWidget::itemActivated(int id)
{
  printf("itemActivated(%d)\n", id);
  IMContact *contact = contactList()->getContact(id);
  if (!contact)
    return;
  if (contact->type() == IMContact::GroupHeader)
  {
    IMContactGroup *group = contactList()->getGroup(contact->groupId());
    if (!group)
      return;
    if (group->isCollapsed())
      group->open();
    else
      group->collapse();
    return;
  }
  emit contactActivated(id);
}
