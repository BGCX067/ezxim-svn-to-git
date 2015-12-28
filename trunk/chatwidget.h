#ifndef _CHATWIDGET_H_
#define _CHATWIDGET_H_

#include <qwidget.h>
#include <qtextview.h>
#include <qvector.h>
#include <qcombobox.h>
#include <ZMultiLineEdit.h>
#include <ZScrollView.h>
#include <ZMainWidget.h>
#include <UTIL_PushButton.h>
#include <qpopupmenu.h>
#include <UTIL_CST.h>

/**
  Виджет чата. Использует QTextView для отображения лога чата.
  @todo Подумать, есть ли возможность заменить QTextView на что-то более разумное.
*/
class IMChatWidget: public ZMainWidget
{
  Q_OBJECT
public:

  /**
    Тип сообщения. Влияет на форматирование и группировку.
  */
  enum MessageType
  {
    Undefined = -1,
    PrivateOut = 0,
    PrivateIn = 1,
    GroupChat = 2
  };

  /**
    Структура состояния текущей сессии в чате.
  */
  struct ChatSession
  {
    /**
      Идентификация пользователя - протокол.
    */
    int protoId;
    /**
      Идентификация пользователя - ID пользователя в протоколе.
    */
    QString userId;
    /**
      Отображаемое имя другого пользователя.
    */
    QString userName;
    /**
      Отображаемое свое имя.
    */
    QString selfName;
    /**
      Текстовое представление лога.
    */
    QString savedChatLog;
    /**
      Содержимое поля ввода.
    */
    QString currentMessage;
    /**
      Положение в виджете лога.
    */
    int logOffset;
    /**
      Хитрость для обхода бага QTextView - при вызове append() если текста нет,
      то перед добавляемыми строками появляются несколько пустых, вместо
      append() в этом случае надо использовать setText()
    */
    bool needReset;
    /**
      Тип последнего сообщения для группировки сообщений.
    */
    MessageType lastMessageType;
  };

  IMChatWidget(QWidget *parent = 0);
  virtual ~IMChatWidget();

  /**
    Добавление сообщения в лог текущей сессии.
  */
  void addMessage(int protoId, const QString &userId, MessageType type, QString text, QTime timestamp);
public slots:
  /**
    Запуск сессии для контакта.
  */
  void startSession(int protoId, const QString &id);
  /**
    Переход на следующую активную сессию.
  */
  void nextSession();
  /**
    Переход на предыдущую активную сессию.
  */
  void prevSession();
  /**
    Отправка сообщения, введенного в поле ввода.
  */
  void sendMessage();
  /**
    Очистка лога текущей сессии.
  */
  void clearLog();
protected slots:
  void kbStateChanged(bool show, const QUuid& imName, int kbh);
protected:
  virtual void showEvent(QShowEvent *);
  virtual void hideEvent(QHideEvent *);
private:
  void startSession(int sId);
  int findSession(int protoId, const QString &id);
  int createSession(int protoId, const QString &userId);

  UTIL_PushButton *fullscreenButton;

  UTIL_PushButton *clearButton;
  UTIL_PushButton *historyButton;
  UTIL_PushButton *sendButton;

  UTIL_PushButton *menuButton;
  UTIL_PushButton *quitButton;

  QTextView *chatLog;
  ZMultiLineEdit *inputField;
  ZScrollView *inputScrollView;

  QVector<ChatSession> sessions;
  QComboBox *sessionList;
  int currentSession;
  UTIL_PushButton *nextSessionButton;
  UTIL_PushButton *prevSessionButton;

  QPopupMenu *popupMenu;
};

#endif
