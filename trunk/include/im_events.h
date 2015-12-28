#ifndef _IM_EVENTS_H_
#define _IM_EVENTS_H_

// EZX Instant Messenger API
// Messaging events

#include <qevent.h>
#include <qdatetime.h>

#define IMEVENT_BASE_NUMBER (QEvent::User + 43)

enum IMEventType
{
  ContactListChanged = IMEVENT_BASE_NUMBER + 1,
  MessageReceived = IMEVENT_BASE_NUMBER + 2,
  MessageDelivered = IMEVENT_BASE_NUMBER + 3,
  ChatStateChanged = IMEVENT_BASE_NUMBER + 4,
  PresenceReceived = IMEVENT_BASE_NUMBER + 5,
  ContactDetails = IMEVENT_BASE_NUMBER + 6,
};

class IMContactListChangedEvent: public QCustomEvent
{
public:

  enum SubType
  {
    ContactAdded,
    ContactRemoved,
    ContactGroupChange,
    GroupAdded,
    GroupRemoved,
  };

  IMContactListChangedEvent();
  virtual ~IMContactListChangedEvent();

  SubType subType;

  int protoId;
  QString userId;
  QString groupName;
};

class IMMessageReceivedEvent: public QCustomEvent
{
public:
  IMMessageReceivedEvent();
  virtual ~IMMessageReceivedEvent();

  int protoId;
  QString userId;
  QTime timestamp;
  QString message;
};

class IMMessageDeliveredEvent: public QCustomEvent
{
public:
  IMMessageDeliveredEvent();
  virtual ~IMMessageDeliveredEvent();

  int protoId;
  QString userId;
  int messageId;
};

class IMChatStateChangedEvent: public QCustomEvent
{
public:

  enum ChatState
  {
    Typing,
    StopTyping,
    MessageWinOpen,
    MessageWinClose,
  };

  IMChatStateChangedEvent();
  virtual ~IMChatStateChangedEvent();

  int protoId;
  QString userId;
  ChatState state;
};

class IMPresenceReceivedEvent: public QCustomEvent
{
public:

  enum PresenceType
  {
    Status,
    XStatus
  };

  IMPresenceReceivedEvent();
  virtual ~IMPresenceReceivedEvent();

  int protoId;
  QString userId;
  PresenceType type;
  int status;
  QString statusMessage;
  int xStatus;
  QString xStatusName;
  QString xStatusMessage;
};

class IMContactDetailsEvent: public QCustomEvent
{
public:
  IMContactDetailsEvent();
  virtual ~IMContactDetailsEvent();

  int protoId;
  QString userId;
  QString nick;
};

#endif // _IM_EVENTS_H_
