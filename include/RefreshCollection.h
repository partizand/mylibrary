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

#ifndef REFRESHCOLLECTION_H_
#define REFRESHCOLLECTION_H_

#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <sstream>
#include <fstream>
#include <tuple>
#include <algorithm>
#include <functional>
#include <thread>
#include <mutex>
#include <unistd.h>
#include "CreateCollection.h"
#include "AuxFunc.h"

class RefreshCollection
{
public:
  RefreshCollection(std::string collname, unsigned int thr_num, int *cancel);
  virtual
  ~RefreshCollection();
  std::function<void
  ()> refresh_canceled;
  std::function<void
  ()> refresh_finished;
  std::function<void
  (int)> total_files;
  std::function<void
  (uint64_t)> total_hash;
  std::function<void
  (uint64_t)> byte_hashed;
  std::function<void
  (int)> files_added;
  void
  startRefreshing();
  void
  removeBook(std::string book_str);
  void
  addBook(std::string book_path);
private:
  std::string
  readList();
  void
  readColl(std::string bookpath);
  void
  collRefresh();
  void
  fb2ThrFunc(std::filesystem::path p);
  void
  epubThrFunc(std::filesystem::path p);
  void
  zipThrFunc(
      std::tuple<std::filesystem::path,
	  std::vector<std::tuple<int, int, std::string>>> ziptup);

  std::string collname;
  int *cancel = nullptr;
  std::vector<std::tuple<std::filesystem::path, std::string>> saved_hashes;
  std::vector<std::filesystem::path> fb2parse;
  std::mutex fb2parsemtx;
  std::vector<std::filesystem::path> epubparse;
  std::mutex epubparsemtx;
  std::vector<
      std::tuple<std::filesystem::path,
	  std::vector<std::tuple<int, int, std::string>>>> zipparse;
  std::mutex zipparsemtx;
  std::vector<std::filesystem::path> fb2remove;
  std::vector<std::filesystem::path> zipremove;
  std::vector<std::filesystem::path> epubremove;
  unsigned int thr_num = 1;
  unsigned int run_thr = 0;
  std::mutex run_thrmtx;
  std::mutex cmtx;
};

#endif /* REFRESHCOLLECTION_H_ */