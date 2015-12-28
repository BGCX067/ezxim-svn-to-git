#include "im_eventlistener.h"
#include "im_global.h"
#include "clistutils.h"
#include "chatwidget.h"

 IMEventListener::IMEventListener(QObject *parent)
 : QObject(parent, "IMEventListener")
{

}

 IMEventListener::~IMEventListener()
{
}

/**
  Фильтрация и обработка событий.
*/
bool IMEventListener::eventFilter(QObject *, QEvent *event)
{
  switch (event->type())
  {
    case ContactListChanged:
    {
      contactListChangedEvent((IMContactListChangedEvent *)event);
    }
    break;
    case MessageReceived:
    {
      messageReceivedEvent((IMMessageReceivedEvent *)event);
    }
    break;
    case MessageDelivered:
    {
      messageDeliveredEvent((IMMessageDeliveredEvent *)event);
    }
    break;
    case ChatStateChanged:
    {
      chatStateChangedEvent((IMChatStateChangedEvent *)event);
    }
    break;
    case PresenceReceived:
    {
      presenceReceivedEvent((IMPresenceReceivedEvent *)event);
    }
    break;
    case ContactDetails:
    {
      contactDetailsEvent((IMContactDetailsEvent *)event);
    }
    default:
      return false;
  }
  return true;
}

/**
  Что-то случилось с контакт-листом.
*/
void IMEventListener::contactListChangedEvent(IMContactListChangedEvent *event)
{
  switch (event->subType)
  {
    case IMContactListChangedEvent::ContactAdded:
    {
      IMContact *contact = IMGlobal::mainContactList->getContact(event->protoId, event->userId);
      if (!contact)
      {
        contact = new IMContact(IMGlobal::mainContactList,
                                           event->protoId, event->userId, "");
        IMContactGroup *group = IMGlobal::mainContactList->getGroup(event->groupName);
        if (!group)
          group = new IMContactGroup(IMGlobal::mainContactList, event->groupName);
        contact->setGroupId(group->id());
      }
    }
    break;
    case IMContactListChangedEvent::ContactRemoved:
    {
      IMContact *contact = IMGlobal::mainContactList->getContact(event->protoId, event->userId);
      if (contact)
        IMGlobal::mainContactList->removeContact(contact->id());
    }
    break;
    case IMContactListChangedEvent::ContactGroupChange:
    {
      IMContact *contact = IMGlobal::mainContactList->getContact(event->protoId, event->userId);
      if (contact)
      {
        IMContactGroup *group = IMGlobal::mainContactList->getGroup(event->groupName);
        if (!group)
          group = new IMContactGroup(IMGlobal::mainContactList, event->groupName);
        contact->setGroupId(group->id());
      }
    }
    break;
    case IMContactListChangedEvent::GroupAdded:
    {
      IMContactGroup *group = IMGlobal::mainContactList->getGroup(event->groupName);
      if (!group)
        group = new IMContactGroup(IMGlobal::mainContactList, event->groupName);
    }
    break;
    case IMContactListChangedEvent::GroupRemoved:
    {
      IMContactGroup *group = IMGlobal::mainContactList->getGroup(event->groupName);
      if (group)
        IMGlobal::mainContactList->removeGroup(group->id());
    }
    break;
  }
}

/**
  Пришло сообщение.
*/
void IMEventListener::messageReceivedEvent(IMMessageReceivedEvent *event)
{
  IMGlobal::mainChatWidget->addMessage(event->protoId, event->userId, IMChatWidget::PrivateIn, event->message, event->timestamp);
}

/**
  Статус отправки сообщения.
*/
void IMEventListener::messageDeliveredEvent(IMMessageDeliveredEvent *event)
{

}

/**
  Оповещение о состоянии чата.
*/
void IMEventListener::chatStateChangedEvent(IMChatStateChangedEvent *event)
{

}

/**
  Смена статуса.
*/
void IMEventListener::presenceReceivedEvent(IMPresenceReceivedEvent *event)
{
  IMContact *contact = IMGlobal::mainContactList->getContact(event->protoId, event->userId);
  if (contact)
  {
    switch(event->type)
    {
      case IMPresenceReceivedEvent::Status:
      {
        contact->setStatus(event->status, event->statusMessage);
      }
      break;
      case IMPresenceReceivedEvent::XStatus:
      {
        contact->setXStatus(event->xStatus, event->xStatusName, event->xStatusMessage);
      }
      break;
    }
  }
}

/**
  Специфичные для протокола свойства контакта.
*/
void IMEventListener::contactDetailsEvent(IMContactDetailsEvent *event)
{

}


