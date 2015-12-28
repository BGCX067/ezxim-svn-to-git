#include "include/im_threadctl.h"


QThreadSocketWait::QThreadSocketWait(QObject *parent)
 : QObject(parent, "QThreadSocketWait")
{
  reset();
  needTimeout = false;
}

void QThreadSocketWait::setTimeout(int sec, int usec)
{
  if (sec<0)
    needTimeout = false;
  else
  {
    needTimeout = true;
    timeout.tv_sec = sec;
    timeout.tv_usec = usec;
  }
}

bool QThreadSocketWait::checkExcept(int fd)
{
  return FD_ISSET(fd, &exceptset);
}

bool QThreadSocketWait::checkWrite(int fd)
{
  return FD_ISSET(fd, &writeset);
}

bool QThreadSocketWait::checkRead(int fd)
{
  return FD_ISSET(fd, &readset);
}


void QThreadSocketWait::removeExcept(int fd)
{
  FD_CLR(fd, &exceptset);
}


void QThreadSocketWait::removeWrite(int fd)
{
  FD_CLR(fd, &writeset);
}


void QThreadSocketWait::removeRead(int fd)
{
  FD_CLR(fd, &readset);
}


void QThreadSocketWait::addExcept(int fd)
{
  if (fd>maxn)
    maxn = fd;
  FD_SET(fd, &exceptset);
}

void QThreadSocketWait::addWrite(int fd)
{
  if (fd>maxn)
    maxn = fd;
  FD_SET(fd, &writeset);
}

void QThreadSocketWait::addRead(int fd)
{
  if (fd>maxn)
    maxn = fd;
  FD_SET(fd, &readset);
}

int QThreadSocketWait::block()
{
  return select(maxn+1, &readset, &writeset, &exceptset, (needTimeout)?(&timeout):NULL);
}

void QThreadSocketWait::reset()
{
  maxn = 0;
  FD_ZERO(&readset);
  FD_ZERO(&writeset);
  FD_ZERO(&exceptset);
}
