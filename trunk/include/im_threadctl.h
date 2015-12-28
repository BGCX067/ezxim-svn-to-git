#ifndef _IM_THREADCTL_H_
#define _IM_THREADCTL_H_
#include <qobject.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/**
  Блокировка процесса при использовании неблокирующих сокетов.
  Предоставляет интерфейс, аналогичный использованию select().
  Передд блокировкой дескрипторы, подлежащие слежению,
  должны быть добавлены в соответствующие контрольные множества.
  После выхода из блокировки контрольные множества содержат
  дескрипторы, в которых случились события.
*/
class QThreadSocketWait: public QObject
{
  Q_OBJECT
  public:
    QThreadSocketWait(QObject *parent);
    virtual ~QThreadSocketWait() {};

    /**
      Сброс всех множеств.
    */
    void reset();
    /**
      Блокировка процесса с использованием контрольных множеств.
    */
    int block();

    /**
      Задает таймаут для блокировки.
      @param sec если <0, то таймаут не используется, иначе задает время таймаута в секундах.
      @param usec дополняет параметр sec значением в микросекндах.
    */
    void setTimeout(int sec, int usec = 0);

    /**
      Добавляет дескриптор в контрольное множество чтения.
    */
    void addRead(int fd);
    /**
      Добавляет дескриптор в контрольное множество записи.
    */
    void addWrite(int fd);
    /**
      Добавляет дескриптор в контрольное множество ошибок.
    */
    void addExcept(int fd);

    /**
      Удаляет дескриптор из контрольного множества чтения.
    */
    void removeRead(int fd);
    /**
      Удаляет дескриптор из контрольного множества записи.
    */
    void removeWrite(int fd);
    /**
      Удаляет дескриптор из контрольного множества ошибок.
    */
    void removeExcept(int fd);

    /**
      Проверяет наличие дескриптора в контрольном множестве чтения.
    */
    bool checkRead(int fd);
    /**
      Проверяет наличие дескриптора в контрольном множестве записи.
    */
    bool checkWrite(int fd);
    /**
      Проверяет наличие дескриптора в контрольном множестве ошибок.
    */
    bool checkExcept(int fd);
  private:
    fd_set readset;
    fd_set writeset;
    fd_set exceptset;

    timeval timeout;
    bool needTimeout;

    int maxn;
};

#endif
