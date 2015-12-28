#ifndef _MAINWIDGET_H_
#define _MAINWIDGET_H_

#include <qtimer.h>
#include <ZMainWidget.h>
#include <UTIL_CST.h>
#include <qlabel.h>
#include <qwidgetstack.h>

#include "chatwidget.h"
#include "clistwidget.h"

/*
  Переключатель окон. Позволяет переключаться между диалогами и устанавливает их геометрию.
*/

class IMWindowSwitcher : public QWidgetStack
{
  Q_OBJECT
public:
  IMWindowSwitcher();
  virtual ~IMWindowSwitcher();

  bool getFullScreenMode() const;

public slots:
  void showChat(int id);
  void showContactList();
  void toggleFullScreen();
  void slotKbStateChanged(const QRect & resizeR, int topLevelWinId, bool show, bool changed);

private slots:
  void switchHook(QWidget *widget);

private:
  bool isFullScreen;
};

#endif
