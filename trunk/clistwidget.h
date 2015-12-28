#ifndef _CLISTWIDGET_H_
#define _CLISTWIDGET_H_

#include <qwidget.h>
#include <qvector.h>
#include <qfont.h>
#include <qscrollview.h>
#include <qpixmap.h>
#include <qintdict.h>
#include <qimage.h>
#include <qhbox.h>
#include <qpopupmenu.h>
#include <UTIL_CST.h>
#include <UTIL_PushButton.h>
#include <qtimer.h>

#include "clistutils.h"

/**
  Виджет контакт-листа. Отвечает за его отбражение и непосредственное взаимодействие с пользователем.

  @todo Оптимизация рисования при помощи RSQ
*/
class IMContactListWidget: public QScrollView
{
  Q_OBJECT
public:
  /**
    Конструктор виджета.
    @param clist Экземпляр контакт-листа для виджета. Если NULL, то создается новый.
    @see QWidget::QWidget(QWidget *, QString)
  */
  IMContactListWidget(IMContactList *clist = NULL, QWidget *parent = NULL, QString name = "IMContactListWidget");
  virtual ~IMContactListWidget();
  IMContactList *contactList() const;
public slots:
  /**
    Необходим пересчет размера контактлиста и обновление изображения.
  */
  void contactListUpdated();
signals:
  /**
    Активация контакта по двойному клику или как-то, как пока еще не реализовано.
  */
  void contactActivated(int id);
protected slots:
  /** @internal
    Связывается с таймером двойного клика.
  */
  void doubleTapTimeout();
protected:
  virtual void drawContents(QPainter *realpainter, int cx, int cy, int cw, int ch);
  virtual void contentsMousePressEvent(QMouseEvent *ev);
  virtual void viewportResizeEvent(QResizeEvent *ev);
  /** @internal
    Получение контакта по координате.
  */
  int getContactAtY(int y);
  /** @internal
    Получение координаты верхней границы контакта.
  */
  int getContactY(int id);
  /** @internal
    Проверяет уиловия видимости контакта.
  */
  bool getContactVisibility(int id);
  /** @internal
    Обработка клика по контакту.
  */
  void itemActivated(int id);
private:
  /** @internal
    Буфер для буферизованного вывода.
    Значительно ускоряет перерисовку.
  */
  QPixmap renderBuffer;
  IMContactList *m_contactList;

  int smallIconHeight;
  int smallIconWidth;
  int bigIconHeight;
  int bigIconWidth;
  int normalLineHeight;
  int cursorLineHeight;
  int contactOffset;

  QFont firstLineFont;
  QFont secondLineFont;

  QBrush lineColorA;
  QBrush lineColorB;
  QBrush bgColor;
  QBrush cursorColor;
  QPen   firstLineFontColor;
  QPen groupHLinePen;

  int cursorAt;

  int quickSelection;
  QTimer doubleTapTimer;
};

#endif
