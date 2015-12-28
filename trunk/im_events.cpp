#include "include/im_events.h"

IMChatStateChangedEvent::~IMChatStateChangedEvent()
{
}
IMChatStateChangedEvent::IMChatStateChangedEvent()
    : QCustomEvent(ChatStateChanged)
{

}

IMMessageDeliveredEvent::~IMMessageDeliveredEvent()
{
}
IMMessageDeliveredEvent::IMMessageDeliveredEvent()
    : QCustomEvent(MessageDelivered)
{

}

IMMessageReceivedEvent::~IMMessageReceivedEvent()
{
}
IMMessageReceivedEvent::IMMessageReceivedEvent()
    : QCustomEvent(MessageReceived)
{

}

IMContactListChangedEvent::~IMContactListChangedEvent()
{
}
IMContactListChangedEvent::IMContactListChangedEvent()
    : QCustomEvent(ContactListChanged)
{

}

IMPresenceReceivedEvent::~IMPresenceReceivedEvent()
{
}
IMPresenceReceivedEvent::IMPresenceReceivedEvent()
    : QCustomEvent(PresenceReceived)
{

}

IMContactDetailsEvent::~IMContactDetailsEvent()
{
}
IMContactDetailsEvent::IMContactDetailsEvent()
    : QCustomEvent(ContactDetails)
{

}


