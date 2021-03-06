/*
 * EZX Instant Messenger
 * Copyright (C) 2007 Mikhail Borisov aka borman <megaborman@gmail.com>
  *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * Further, this software is distributed without any warranty that it is
 * free of the rightful claim of any third person regarding infringement
 * or the like.  Any license provided herein, whether implied or
 * otherwise, applies only to this software file.  Patent licenses, if
 * any, provided herein do not apply to combinations of this program with
 * other software, or any other product whatsoever.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <ZApplication.h>

#include "mainwidget.h"
#include "im_global.h"

IMContactList *IMGlobal::mainContactList = NULL;
IMContactListWidget *IMGlobal::mainContactListWidget = NULL;
IMContactListContainerWidget *IMGlobal::mainContactListContainerWidget = NULL;
IMChatWidget *IMGlobal::mainChatWidget = NULL;
IMWindowSwitcher *IMGlobal::windowSwitcher = NULL;
IMEventListener *IMGlobal::eventListener = NULL;

int main(int argc, char** argv)
{
  ZApplication app(argc, argv, QApplication::GuiClient);

  IMGlobal::windowSwitcher = new IMWindowSwitcher();
  app.showMainWidget(IMGlobal::windowSwitcher);
  int result = app.exec();
  return result;
};

