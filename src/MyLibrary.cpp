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

#include <iostream>
#include <gtkmm.h>
#include <string>
#include <filesystem>
#include <libintl.h>
#include "AuxFunc.h"
#include "MyLibraryApplication.h"

int
main(int argc, char *argv[])
{
  AuxFunc af;
  std::string Sharepath;
  std::filesystem::path p(std::filesystem::u8path(af.get_selfpath()));
  Sharepath = p.parent_path().u8string() + "/../share/locale";
  bindtextdomain("MyLibrary", Sharepath.c_str());
  bind_textdomain_codeset("MyLibrary", "UTF-8");
  textdomain("MyLibrary");
  gcry_check_version(NULL);
  gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);
  auto app = MyLibraryApplication::create();
  return app->run(argc, argv);
}
