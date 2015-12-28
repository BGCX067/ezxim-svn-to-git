#ifndef _IM_EVENTLISTENER_
#define _IM_EVENTLISTENER_

#include <qobject.h>
#include "include/im_events.h"

class IMEventListener: public QObject
{
  Q_OBJECT
public:
  IMEventListener(QObject *parent = NULL);
  ~IMEventListener();
  virtual bool eventFilter(QObject *, QEvent *);

  void contactListChangedEvent(IMContactListChangedEvent *event);
  void messageReceivedEvent(IMMessageReceivedEvent *event);
  void messageDeliveredEvent(IMMessageDeliveredEvent *event);
  void chatStateChangedEvent(IMChatStateChangedEvent *event);
  void presenceReceivedEvent(IMPresenceReceivedEvent *event);
  void contactDetailsEvent(IMContactDetailsEvent *event);
};

#endif
