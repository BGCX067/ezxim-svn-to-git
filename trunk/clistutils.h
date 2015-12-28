#ifndef _CLISTUTILS_H_
#define _CLISTUTILS_H_

#include <qobject.h>
#include <qvector.h>
#include <qxml.h>

class IMContactList;

/**
  Любой видимый элемент контакт-диста - как настоящий контакт,
  так и виртуальный (например, заголовок группы).
  Контакт добавляется в контакт-лист в момент создания.
*/
class IMContact: public QObject
{
  Q_OBJECT
public:
  enum ContactType
  {
    GroupHeader = 0,
    NormalContact = 1
  };

  /**
    Конструктор заголовка группы.
  */
  IMContact(IMContactList *parent, const QString &name);
  /**
    Конструктор обычного контакта.
  */
  IMContact(IMContactList *parent, int protoId, const QString &userId, const QString &nick = "");
  virtual ~IMContact();

  /**
    Тип контакта
  */
  ContactType type() const;

  /**
    Идентификатор пользователя с точки зрения протокола.
  */
  QString userId() const;
  /**
    Идентификатор протоколоа, которому принадлежит пользователь.
  */
  int protoId() const;

  /**
    Идентификатор с точки зрения контакт-листа.
  */
  int id() const;

  /**
    Идентификатор группы, в которую входит контакт.
    @see IMContactGroup::id()
  */
  int groupId() const;
  /**
    Установка идентификатора группы для контакта.
    @see IMContactGroup::id()
  */
  void setGroupId(int groupId);

  /**
    Ник контакта.
  */
  QString nick() const;
  /**
    Установка ника контакта.
  */
  void setNick(const QString &nick);

  /**
    Код статуса контакта. Расшифровка кода статуса выполняется протоколом.
    Положительные коды считаются онлайн статусами, отрицательные - оффлайн.
    Сортировка списка выполняется по убыванию кода статуса.
  */
  int status() const;
  /**
    Сообщение статуса.
  */
  QString statusMessage() const;
  /**
    Установка статуса.
  */
  void setStatus(int status, const QString &statusMessage);

  /**
    Код дополнительного статуса.
    Не влияет на сортировку контакт-листа.
  */
  int xStatus() const;
  /**
    Название доп. статуса.
  */
  QString xStatusName() const;
  /**
    Описание доп. статуса.
  */
  QString xStatusMessage() const;
  /**
    Установка доп. статуса.
  */
  void setXStatus(int xStatus, const QString &xStatusName, const QString &xStatusMessage);

private:
  void notifyUpdated();

  ContactType m_type;

  QString m_userId;
  int m_protoId;
  int m_id;
  int m_groupId;

  QString m_nick;

  int m_status;
  QString m_statusMessage;

  int m_xStatus;
  QString m_xStatusName;
  QString m_xStatusMessage;

  friend class IMContactList;
};

/**
  Группа контактов. Представляет собой свойство контакта, не отображается
  в контакт-листе, но влияет на его сортировку.
  Группа добавляется в контакт-лист в моммент создания.
*/
class IMContactGroup: public QObject
{
  Q_OBJECT
public:
  /**
    Создание группы
    @param parent Экземпляр контакт-листа.
    @param name Имя группы.
  */
  IMContactGroup(IMContactList *parent, const QString &name);
  virtual ~IMContactGroup();

  /**
    Идентификатор группы. Используется для адресации.
  */
  int id() const;

  /**
    Индекс группы при сортировке. Может меняться.
  */
  int order() const;

  /**
    Показывает, свернута ли группа.
  */
  bool isCollapsed() const;
  /**
    Разворачивает группу.
  */
  void open();
  /**
    Сворачивает группу.
  */
  void collapse();

  uint childContacts;
  uint onlineContacts;

  /**
    Имя группы.
  */
  QString name() const;
  /**
    Установка имени группы.
  */
  void setName(const QString &name);

private:
  void notifyUpdated();

  QString m_name;
  int m_id;
  int m_order;
  bool m_isCollapsed;

  friend class IMContactList;
};

/**
  Список контактов. Отвечает за сортировку и организацию списка.
*/
class IMContactList: public QObject
{
  Q_OBJECT
public:
  /**
    Получение контакта по его идентификатору.
    @see IMContact::id()
  */
  IMContact *getContact(int id);
  /**
    Поиск контакта по его идентификатору в IM сети.
  */
  IMContact *getContact(int protoId, const QString &userId);

  /**
    Получение группы по идентификатору.
  */
  IMContactGroup *getGroup(int id);
  /**
    Поиск группы по имени.
  */
  IMContactGroup *getGroup(const QString &name);

  /**
    Удаление контакта.
  */
  void removeContact(int id);
  /**
    Удаление группы. Помещает всех контактов этой группы в группу "No Group".
  */
  void removeGroup(int id);

  /**
    Импорт контакт-листа из XML-данных
    Пример формата в файле clist.xml
  */
  void import(const QXmlInputSource &src);

  IMContactList(QObject *parent);
  virtual ~IMContactList();
signals:
  /**
    Что-то случилось в контакт-листе. По этому сигналу можно, к примеру, обновить виджет.
  */
  void updated();
protected:
  void addContact(IMContact *contact);
  void addGroup(IMContactGroup *group);
  void notifyContactUpdated(int id);
  void notifyGroupUpdated(int id);

  void disableSorting();
  void enableSorting();

  friend class IMContact;
  friend class IMContactGroup;
  friend class IMContactListWidget;
  friend class MyXmlHandler;

private:
class MyXmlHandler: public QXmlDefaultHandler
  {
  public:
    MyXmlHandler(IMContactList *owner);
    virtual ~MyXmlHandler();
    virtual bool startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts);
  private:
    int currentGroupId;
    IMContactList *contactList;
  };

  void sortContacts();
  void sortGroups();

class GroupCollection: public QVector<IMContactGroup>
  {
  public:
    virtual int compareItems(QCollection::Item pa, QCollection::Item pb); // реализует сортировку списка групп
  };
class ItemCollection: public QVector<IMContact>
  {
  public:
    ItemCollection(GroupCollection &v_groups);
    virtual int compareItems(QCollection::Item pa, QCollection::Item pb); // реализует сортировку списска контактов
  private:
    GroupCollection &groups;
  };

  uint contactsCount;
  uint groupsCount;

  GroupCollection groups; // список групп
  ItemCollection items; // список контактов
  QArray<int> items_index; // таблица глобальных индексов контактов
  QArray<int> groups_index; // соответствия между группами и их заголовками.
  // Группе соответствует items[items_index[groups_index[group_id]]]. И не говорите мне, что это слишком громоздко ;-)

  bool allowSorting; // Разрешает или запрещает пересортировку списка. По умолчанию true
};

#endif

