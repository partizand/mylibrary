/*
 * Copyright (C) 2022-2023 Yury Bobylev <bobilev_yury@mail.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef INCLUDE_CREATELEFTGRID_H_
#define INCLUDE_CREATELEFTGRID_H_

#include <string>
#include <vector>

struct genre_item
{
  std::string code;
  std::string name;
};

struct genres
{
  std::string header;
  std::vector<genre_item> items;
};

#include "MainWindow.h"
#ifndef ML_GTK_OLD
#include "ModelBoxes.h"
#endif

class MainWindow;

class CreateLeftGrid
{
public:
  CreateLeftGrid(MainWindow *mw);
  virtual
  ~CreateLeftGrid();

  Gtk::Grid*
  formLeftGrid();

#ifdef ML_GTK_OLD
  void
  formCollCombo(Gtk::ComboBoxText *combo);
#endif

#ifndef ML_GTK_OLD
  Glib::RefPtr<Gio::ListStore<ModelBoxes>>
  formCollCombo();
#endif

  void
  formGenreVect(std::vector<genres> *genrev);
private:
  MainWindow *mw = nullptr;
};

#endif /* INCLUDE_CREATELEFTGRID_H_ */
