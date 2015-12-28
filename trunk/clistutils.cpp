#include <qapplication.h>
#include <cstdio>
#include "clistutils.h"

IMContact::IMContact(IMContactList *parent, const QString &name)
{
  m_type = GroupHeader;

  m_protoId = -1;
  m_userId = "";
  m_nick = name;

  m_status = 0;
  m_xStatus = 0;

  m_id = 0;
  m_groupId = 0;
  if (parent)
    parent->addContact(this);
}

IMContact::IMContact(IMContactList *parent, int protoId,const QString &userId, const QString &nick)
    : QObject(parent)
{
  m_type = NormalContact;

  m_protoId = protoId;
  m_userId = userId;
  m_nick = nick;

  m_status = 0;
  m_xStatus = 0;

  m_id = 0;
  m_groupId = -1;
  if (parent)
    parent->addContact(this);
}

IMContact::~IMContact()
{

}

IMContact::ContactType IMContact::type() const
{
  return m_type;
}

QString IMContact::userId() const
{
  return m_userId;
}

int IMContact::protoId() const
{
  return m_protoId;
}

int IMContact::id() const
{
  return m_id;
}

int IMContact::groupId() const
{
  return m_groupId;
}

void IMContact::setGroupId(int groupId)
{
  m_groupId = groupId;
  notifyUpdated();
}

QString IMContact::nick() const
{
  return m_nick;
}

void IMContact::setNick(const QString &nick)
{
  m_nick = nick;
  notifyUpdated();
}

int IMContact::status() const
{
  return m_status;
}

QString IMContact::statusMessage() const
{
  return m_statusMessage;
}

void IMContact::setStatus(int status, const QString &statusMessage)
{
  m_status = status;
  m_statusMessage = statusMessage;
  notifyUpdated();
}

int IMContact::xStatus() const
{
  return m_xStatus;
}

QString IMContact::xStatusName() const
{
  return m_xStatusName;
}

QString IMContact::xStatusMessage() const
{
  return m_xStatusMessage;
}

void IMContact::setXStatus(int xStatus, const QString &xStatusName, const QString &xStatusMessage)
{
  m_xStatus = xStatus;
  m_xStatusName = xStatusName;
  m_xStatusMessage = xStatusMessage;
  notifyUpdated();
}

void IMContact::notifyUpdated()
{
  IMContactList *cl = (IMContactList *)parent();
  if (cl)
    cl->notifyContactUpdated(m_id);
}

//------------------------------------

IMContactGroup::IMContactGroup(IMContactList *parent, const QString &name)
    : QObject(parent)
{
  m_name = name;
  m_id = 0;
  m_order = 0;
  if (parent)
    parent->addGroup(this);
}

IMContactGroup::~IMContactGroup()
{
}

int IMContactGroup::id() const
{
  return m_id;
}

int IMContactGroup::order() const
{
  return m_order;
}

QString IMContactGroup::name() const
{
  return m_name;
}

void IMContactGroup::setName(const QString &name)
{
  m_name = name;
  notifyUpdated();
}

void IMContactGroup::notifyUpdated()
{
  IMContactList *cl = (IMContactList *)parent();
  if (cl)
    cl->notifyGroupUpdated(m_id);
}

bool IMContactGroup::isCollapsed() const
{
  return m_isCollapsed;
}

void IMContactGroup::open()
{
  if (m_isCollapsed)
  {
    m_isCollapsed = false;
    notifyUpdated();
  }
}

void IMContactGroup::collapse()
{
  if (!m_isCollapsed)
  {
    m_isCollapsed = true;
    notifyUpdated();
  }
}


//------------------

IMContactList::IMContactList(QObject *parent)
    : QObject(parent),
    groups(), items(groups), items_index()
{
  items.resize(32);
  items.setAutoDelete(true);
  items_index.resize(32);
  groups.resize(32);
  groups.setAutoDelete(true);
  groups_index.resize(32);

  allowSorting = true;

  contactsCount = 0;
  groupsCount = 0;

  IMContactGroup *noGroup = new IMContactGroup(this, qApp->tr("No Group"));
}

IMContactList::~IMContactList()
{
  items.clear();
  groups.clear();
}

IMContact *IMContactList::getContact(int id)
{
  if (id < 0 || id >= items_index.size())
    return NULL;
  return items[items_index[id]];
}

IMContact *IMContactList::getContact(int protoId, const QString &userId)
{
  /**
    @todo Заменить линейный поиск на более быстрый
  */
  for (int i = 0; i < items.size(); i++)
  {
    IMContact *cont = items[i];
    if (cont && cont->protoId() == protoId && cont->userId() == userId)
      return cont;
  }
  return NULL;
}

IMContactGroup *IMContactList::getGroup(int id)
{
  return groups[id];
}

IMContactGroup *IMContactList::getGroup(const QString &name)
{
  /**
    @todo Заменить линейный поиск на более быстрый
  */
  if (name.isEmpty())
    return groups[0];
  for (int i = 0; i < groups.size(); i++)
  {
    IMContactGroup *cg = groups[i];
    if (cg && cg->name() == name)
      return cg;
  }
  return NULL;
}

void IMContactList::removeContact(int id)
{
  items.remove(items_index[id]);
  items_index[id] = -1;
  sortContacts();

  emit updated();
}

void IMContactList::removeGroup(int id)
{
  // Уводим все контакты из группы
  disableSorting();
  int gheaderId = -1;
  for (int i = 0; i < items.count(); i++)
  {
    if (items[i]->groupId() == id)
    {
      if (items[i]->type() == IMContact::GroupHeader)
        gheaderId = items[i]->id();
      else
        items[i]->setGroupId(0);
    }
  }

  if (gheaderId != -1)
    removeContact(gheaderId);
  enableSorting();
  sortGroups();

  emit updated();
}

void IMContactList::addContact(IMContact *contact)
{
  if (items.count() == items.size())
    items.resize(items.size() + 16);
  if (contactsCount == items_index.size())
    items_index.resize(items_index.size() + 16);
  contact->m_id = contactsCount;
  contact->m_groupId = 0; //->[No Group]
  items_index[contactsCount] = items.count();
  items.insert(items.count(), contact);

  contactsCount++;

  sortContacts();

  printf("[IMContactList]: Adding contact (uid=\"%s\" id=%d index=%d nick=\"%s\")\n", contact->userId().latin1(), contact->id(), items_index[contact->id()], contact->nick().latin1());

  emit updated();
}

void IMContactList::addGroup(IMContactGroup *group)
{
  disableSorting();
  if (groupsCount == groups.size())
    groups.resize(groups.size() + 16);
  group->m_id = groupsCount;
  groups.insert(groupsCount, group);

  IMContact *gheader = new IMContact(this, group->name());
  gheader->setGroupId(group->id());

  if (groupsCount == groups_index.size())
    groups_index.resize(groups_index.size() + 16);
  groups_index[groupsCount] = gheader->id();

  groupsCount++;
  enableSorting();

  printf("[IMContactList]: Adding group (id=%d index=%d name=\"%s\")\n", group->id(), group->order(), group->name().latin1());

  sortGroups();

  emit updated();
}

void IMContactList::notifyContactUpdated(int id)
{
  sortContacts();

  emit updated();
}

void IMContactList::notifyGroupUpdated(int id)
{
  IMContact *gheader = items[items_index[groups_index[id]]];
  gheader->setNick(groups[id]->name());
  sortGroups();

  emit updated();
}

void IMContactList::sortContacts()
{
  if (!allowSorting) return;
  items.sort();
  for (int i = 0; i < items.count(); i++)
    items_index[items[i]->id()] = i;
}

void IMContactList::sortGroups()
{
  if (!allowSorting) return;
  GroupCollection tgroups(groups);
  tgroups.sort();
  for (int i = 0; i < tgroups.count(); i++)
  {
    if (tgroups[i]->m_order != -1)
      tgroups[i]->m_order = i;
  }
  sortContacts();
}

int IMContactList::GroupCollection::compareItems(QCollection::Item pa, QCollection::Item pb)
{
  if (pa == NULL)
    return 1;
  if (pb == NULL)
    return -1;

  IMContactGroup *a = (IMContactGroup *)pa;
  IMContactGroup *b = (IMContactGroup *)pb;
  if (a->order() < 0)
    return 1;
  if (b->order() < 0)
    return -1;
  return qstrcmp(a->name(), b->name());
}

IMContactList::ItemCollection::ItemCollection(GroupCollection &v_groups)
    : QVector<IMContact>(),
    groups(v_groups)
{
}

int IMContactList::ItemCollection::compareItems(QCollection::Item pa, QCollection::Item pb)
{
  if (pa == NULL)
    return 1;
  if (pb == NULL)
    return -1;

  IMContact *a = (IMContact *)pa;
  IMContact *b = (IMContact *)pb;

  if (groups[b->groupId()]->order() < 0)
    return -1;
  if (groups[a->groupId()]->order() < 0)
    return 1;

  if (groups[a->groupId()]->order() < groups[b->groupId()]->order())
    return -1;
  if (groups[a->groupId()]->order() > groups[b->groupId()]->order())
    return 1;

  if (a->type() < b->type())
    return -1;
  if (a->type() > b->type())
    return 1;

  if (a->status() > b->status())
    return -1;
  if (a->status() < b->status())
    return 1;

  if (a->nick() < b->nick())
    return -1;
  if (a->nick() > b->nick())
    return 1;

  return 0;
}

void IMContactList::disableSorting()
{
  allowSorting = false;
}

void IMContactList::enableSorting()
{
  allowSorting = true;
}

// XML input

void IMContactList::import(const QXmlInputSource &src)
{
  QXmlSimpleReader reader;
  MyXmlHandler handler(this);
  reader.setContentHandler(&handler);
  if (!reader.parse(src))
    printf("[IMContactList] Error importing XMLContactList\n");
}

IMContactList::MyXmlHandler::MyXmlHandler(IMContactList *owner)
    : QXmlDefaultHandler()
{
  currentGroupId = 0;
  contactList = owner;
}

IMContactList::MyXmlHandler::~MyXmlHandler()
{

}

bool IMContactList::MyXmlHandler::startElement(const QString &, const QString &, const QString & qName, const QXmlAttributes & atts)
{
  printf("SAX Element %s\n", qName.latin1());
  if (!contactList)
    return false;
  if (qName == "group")
  {
    int attId = atts.index("name");
    if (attId < 0)
      currentGroupId = 0;
    else
    {
      QString groupName = atts.value(attId).stripWhiteSpace();
      if (groupName.isEmpty())
        currentGroupId = 0;
      else
      {
        IMContactGroup *gr = contactList->getGroup(atts.value(attId));
        if (!gr)
          gr = new IMContactGroup(contactList, groupName);
        currentGroupId = gr->id();
      }
    }
    return true;
  }
  if (qName == "contact")
  {
    int attUserId = atts.index("id");
    int attProtoId = atts.index("protocol");
    int attNick = atts.index("nick");
    QString userId;
    if (attUserId < 0)
    {
      printf("no UserId\n");
      return false;
    }
    else
      userId = atts.value(attUserId);
    int protoId;
    if (attProtoId < 0)
      protoId = -1;
    else
    {
      bool result;
      protoId = atts.value(attProtoId).stripWhiteSpace().toInt(&result);
      if (!result)
      {
        printf("bad ProtoId\n");
        return false;
      }
    }
    QString nick;
    if (attNick >= 0)
      nick = atts.value(attNick).stripWhiteSpace();
    IMContact *contact = new IMContact(contactList, protoId, userId, nick);
    contact->setGroupId(currentGroupId);
    printf("groupId set to %d\n", currentGroupId);
    return true;
  }
  return true;
}


