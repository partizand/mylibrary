/*
 Copyright 2022 Yury Bobylev <bobilev_yury@mail.ru>

 This file is part of MyLibrary.
 MyLibrary is free software: you can redistribute it and/or
 modify it under the terms of the GNU General Public License as
 published by the Free Software Foundation, either version 3 of
 the License, or (at your option) any later version.
 MyLibrary is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with MyLibrary. If not,
 see <https://www.gnu.org/licenses/>.
 */

#ifndef MYLIBRARYAPPLICATION_H
#define MYLIBRARYAPPLICATION_H

#include <gtkmm.h>
#include "MainWindow.h"


class MyLibraryApplication : public Gtk::Application
{
public:
  MyLibraryApplication();
  virtual ~MyLibraryApplication();
  static Glib::RefPtr<MyLibraryApplication>
  create();
private:
  MainWindow *
  create_appwindow();
protected:
  void
  on_activate() override;
};

#endif // MYLIBRARYAPPLICATION_H
