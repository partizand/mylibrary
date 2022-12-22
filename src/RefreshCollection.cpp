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

#include "RefreshCollection.h"

RefreshCollection::RefreshCollection(std::string collname, unsigned int thr_num,
				     int *cancel)
{
  this->collname = collname;
  this->cancel = cancel;
  this->thr_num = thr_num;

}

RefreshCollection::~RefreshCollection()
{
  // TODO Auto-generated destructor stub
}

void
RefreshCollection::startRefreshing()
{
  std::string bookpath = readList();
  readColl(bookpath);
  for(;;)
    {
      run_thrmtx.lock();
      if(run_thr == 0)
	{
	  break;
	}
      run_thrmtx.unlock();
      usleep(100);
    }
  if(*cancel != 1)
    {
      collRefresh();
    }
  else
    {
      if(refresh_canceled)
	{
	  refresh_canceled();
	}
    }
}

std::string
RefreshCollection::readList()
{
  std::string filename;
  std::string bookpath;
  AuxFunc af;
  af.homePath(&filename);
  filename = filename + "/.MyLibrary/Collections/" + collname;
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  if(std::filesystem::exists(filepath))
    {
      filename = filename + "/fb2hash";
      filepath = std::filesystem::u8path(filename);
      std::fstream f;
      f.open(filepath, std::ios_base::in);
      if(!f.is_open())
	{
	  std::cerr << "RefreshCollecttion: fb2hash file not opened"
	      << std::endl;
	}
      else
	{
	  int count = 0;
	  while(!f.eof())
	    {
	      std::string line;
	      getline(f, line);
	      if(!line.empty())
		{
		  if(count == 0)
		    {
		      bookpath = line;
		    }
		  else
		    {
		      std::tuple<std::filesystem::path, std::string> ttup;
		      std::string p_str = line;
		      p_str = p_str.substr(0, p_str.find("<?>"));
		      p_str = bookpath + p_str;
		      std::get<0>(ttup) = std::filesystem::u8path(p_str);
		      p_str = line;
		      p_str.erase(
			  0, p_str.find("<?>") + std::string("<?>").size());
		      std::get<1>(ttup) = p_str;
		      saved_hashes.push_back(ttup);
		    }
		}
	      count++;
	    }
	  f.close();
	}
      filename = filepath.parent_path().u8string();
      filename = filename + "/ziphash";
      filepath = std::filesystem::u8path(filename);
      f.open(filepath, std::ios_base::in);
      if(!f.is_open())
	{
	  std::cerr << "RefreshCollection: ziphash file not opened"
	      << std::endl;
	}
      else
	{
	  int count = 0;
	  while(!f.eof())
	    {
	      std::string line;
	      getline(f, line);
	      if(!line.empty())
		{
		  if(count == 0)
		    {
		      bookpath = line;
		    }
		  else
		    {
		      std::tuple<std::filesystem::path, std::string> ttup;
		      std::string p_str = line;
		      p_str = p_str.substr(0, p_str.find("<?>"));
		      p_str = bookpath + p_str;
		      std::get<0>(ttup) = std::filesystem::u8path(p_str);
		      p_str = line;
		      p_str.erase(
			  0, p_str.find("<?>") + std::string("<?>").size());
		      std::get<1>(ttup) = p_str;
		      saved_hashes.push_back(ttup);
		    }
		}
	      count++;
	    }
	  f.close();
	}
      filename = filepath.parent_path().u8string();
      filename = filename + "/epubhash";
      filepath = std::filesystem::u8path(filename);
      f.open(filepath, std::ios_base::in);
      if(!f.is_open())
	{
	  std::cerr << "RefreshCollection: ziphash file not opened"
	      << std::endl;
	}
      else
	{
	  int count = 0;
	  while(!f.eof())
	    {
	      std::string line;
	      getline(f, line);
	      if(!line.empty())
		{
		  if(count == 0)
		    {
		      bookpath = line;
		    }
		  else
		    {
		      std::tuple<std::filesystem::path, std::string> ttup;
		      std::string p_str = line;
		      p_str = p_str.substr(0, p_str.find("<?>"));
		      p_str = bookpath + p_str;
		      std::get<0>(ttup) = std::filesystem::u8path(p_str);
		      p_str = line;
		      p_str.erase(
			  0, p_str.find("<?>") + std::string("<?>").size());
		      std::get<1>(ttup) = p_str;
		      saved_hashes.push_back(ttup);
		    }
		}
	      count++;
	    }
	  f.close();
	}
    }
  return bookpath;
}

void
RefreshCollection::readColl(std::string bookpath)
{
  std::filesystem::path filepath = std::filesystem::u8path(bookpath);
  AuxFunc af;
  std::vector<std::filesystem::path> fb2;
  std::vector<std::filesystem::path> epub;
  std::vector<
      std::tuple<std::filesystem::path,
	  std::vector<std::tuple<int, int, std::string>>>> zip;
  if(std::filesystem::exists(filepath))
    {
      for(auto &dirit : std::filesystem::recursive_directory_iterator(filepath))
	{
	  if(*cancel == 1)
	    {
	      return void();
	    }
	  std::filesystem::path p = dirit.path();
	  if(p.extension().u8string() == ".fb2"
	      && !std::filesystem::is_directory(p))
	    {
	      fb2.push_back(p);
	    }
	  if(p.extension().u8string() == ".epub"
	      && !std::filesystem::is_directory(p))
	    {
	      epub.push_back(p);
	    }
	  if(p.extension().u8string() == ".zip"
	      && !std::filesystem::is_directory(p))
	    {
	      std::vector<std::tuple<int, int, std::string>> filenames;
	      af.fileNames(p.u8string(), filenames);
	      filenames.erase(
		  std::remove_if(
		      filenames.begin(),
		      filenames.end(),
		      []
		      (auto &el)
			{
			  std::filesystem::path p = std::filesystem::u8path(std::get<2>(el));
			  if(p.extension().u8string() != ".fb2" &&
			      p.extension().u8string() != ".epub")
			    {
			      return true;
			    }
			  else
			    {
			      return false;
			    }
			}),
		  filenames.end());
	      if(filenames.size() > 0)
		{
		  zip.push_back(std::make_tuple(p, filenames));
		}
	    }
	}
    }
  if(total_hash)
    {
      uint64_t sz = 0;
      for(size_t i = 0; i < fb2.size(); i++)
	{
	  sz = sz + std::filesystem::file_size(fb2[i]);
	}
      for(size_t i = 0; i < epub.size(); i++)
	{
	  sz = sz + std::filesystem::file_size(epub[i]);
	}
      for(size_t i = 0; i < zip.size(); i++)
	{
	  sz = sz + std::filesystem::file_size(std::get<0>(zip[i]));
	}
      total_hash(sz);
    }

  for(size_t i = 0; i < fb2.size(); i++)
    {
      if(*cancel == 1)
	{
	  return void();
	}
      std::filesystem::path p = fb2[i];
      cmtx.lock();
      std::thread *thr = new std::thread(
	  std::bind(&RefreshCollection::fb2ThrFunc, this, p));
      run_thrmtx.lock();
      run_thr++;
      thr->detach();
      delete thr;
      if(run_thr < thr_num)
	{
	  cmtx.try_lock();
	  cmtx.unlock();
	}
      run_thrmtx.unlock();
    }

  for(size_t i = 0; i < epub.size(); i++)
    {
      if(*cancel == 1)
	{
	  return void();
	}
      std::filesystem::path p = epub[i];
      cmtx.lock();
      std::thread *thr = new std::thread(
	  std::bind(&RefreshCollection::epubThrFunc, this, p));
      run_thrmtx.lock();
      run_thr++;
      thr->detach();
      delete thr;
      if(run_thr < thr_num)
	{
	  cmtx.try_lock();
	  cmtx.unlock();
	}
      run_thrmtx.unlock();
    }

  for(size_t i = 0; i < zip.size(); i++)
    {
      if(*cancel == 1)
	{
	  return void();
	}
      std::tuple<std::filesystem::path,
	  std::vector<std::tuple<int, int, std::string>>> ziptup;
      ziptup = zip[i];
      cmtx.lock();
      std::thread *thr = new std::thread(
	  std::bind(&RefreshCollection::zipThrFunc, this, ziptup));
      run_thrmtx.lock();
      run_thr++;
      thr->detach();
      delete thr;
      if(run_thr < thr_num)
	{
	  cmtx.try_lock();
	  cmtx.unlock();
	}
      run_thrmtx.unlock();
    }
  for(size_t i = 0; i < saved_hashes.size(); i++)
    {
      if(*cancel == 1)
	{
	  return void();
	}
      std::filesystem::path p = std::get<0>(saved_hashes[i]);
      if(p.extension().u8string() == ".fb2")
	{
	  auto itfb2 = std::find_if(fb2.begin(), fb2.end(), [p]
	  (auto &el)
	    {
	      return p == el;
	    });
	  if(itfb2 == fb2.end())
	    {
	      fb2remove.push_back(p);
	    }
	}
      if(p.extension().u8string() == ".epub")
	{
	  auto itepub = std::find_if(epub.begin(), epub.end(), [p]
	  (auto &el)
	    {
	      return p == el;
	    });
	  if(itepub == epub.end())
	    {
	      epubremove.push_back(p);
	    }
	}
      if(p.extension().u8string() == ".zip")
	{
	  auto itzip = std::find_if(zip.begin(), zip.end(), [p]
	  (auto &el)
	    {
	      return p == std::get<0>(el);
	    });
	  if(itzip == zip.end())
	    {
	      zipremove.push_back(p);
	    }
	}
    }
}

void
RefreshCollection::fb2ThrFunc(std::filesystem::path p)
{
  AuxFunc af;
  std::vector<char> hash;
  if(byte_hashed)
    {
      hash = af.filehash(p, byte_hashed);
    }
  else
    {
      hash = af.filehash(p);
    }
  std::string hex_hash = af.to_hex(&hash);
  auto itsh = std::find_if(
      saved_hashes.begin(), saved_hashes.end(), [p, hex_hash]
      (auto &el)
	{
	  if(p == std::get<0>(el) && std::get<1>(el) == hex_hash)
	    {
	      return true;
	    }
	  else
	    {
	      return false;
	    }
	});
  if(itsh == saved_hashes.end())
    {
      fb2parsemtx.lock();
      fb2parse.push_back(p);
      fb2parsemtx.unlock();
    }
  run_thrmtx.lock();
  run_thr = run_thr - 1;
  if(run_thr < thr_num)
    {
      cmtx.try_lock();
      cmtx.unlock();
    }
  run_thrmtx.unlock();
}

void
RefreshCollection::epubThrFunc(std::filesystem::path p)
{
  AuxFunc af;
  std::vector<char> hash;
  if(byte_hashed)
    {
      hash = af.filehash(p, byte_hashed);
    }
  else
    {
      hash = af.filehash(p);
    }
  std::string hex_hash = af.to_hex(&hash);
  auto itsh = std::find_if(
      saved_hashes.begin(), saved_hashes.end(), [p, hex_hash]
      (auto &el)
	{
	  if(p == std::get<0>(el) && std::get<1>(el) == hex_hash)
	    {
	      return true;
	    }
	  else
	    {
	      return false;
	    }
	});
  if(itsh == saved_hashes.end())
    {
      epubparsemtx.lock();
      epubparse.push_back(p);
      epubparsemtx.unlock();
    }
  run_thrmtx.lock();
  run_thr = run_thr - 1;
  if(run_thr < thr_num)
    {
      cmtx.try_lock();
      cmtx.unlock();
    }
  run_thrmtx.unlock();
}

void
RefreshCollection::zipThrFunc(
    std::tuple<std::filesystem::path,
	std::vector<std::tuple<int, int, std::string>>> ziptup)
{
  AuxFunc af;
  std::vector<char> hash;
  std::filesystem::path p = std::get<0>(ziptup);
  if(byte_hashed)
    {
      hash = af.filehash(p, byte_hashed);
    }
  else
    {
      hash = af.filehash(p);
    }
  std::string hex_hash = af.to_hex(&hash);
  auto itsh = std::find_if(
      saved_hashes.begin(), saved_hashes.end(), [p, hex_hash]
      (auto &el)
	{
	  if(p == std::get<0>(el) && std::get<1>(el) == hex_hash)
	    {
	      return true;
	    }
	  else
	    {
	      return false;
	    }
	});
  if(itsh == saved_hashes.end())
    {
      zipparsemtx.lock();
      zipparse.push_back(std::make_tuple(p, std::get<1>(ziptup)));
      zipparsemtx.unlock();
    }
  run_thrmtx.lock();
  run_thr = run_thr - 1;
  if(run_thr < thr_num)
    {
      cmtx.try_lock();
      cmtx.unlock();
    }
  run_thrmtx.unlock();
}

void
RefreshCollection::removeBook(std::string book_str)
{
  std::string loc_p = book_str;
  std::string::size_type n;
  n = loc_p.find("<zip>");
  if(n == std::string::npos)
    {
      std::filesystem::path filepath = std::filesystem::u8path(loc_p);
      std::filesystem::remove(filepath);
      if(filepath.extension().u8string() == ".fb2")
	{
	  fb2remove.push_back(std::filesystem::u8path(loc_p));
	}
      if(filepath.extension().u8string() == ".epub")
	{
	  epubremove.push_back(std::filesystem::u8path(loc_p));
	}
    }
  else
    {
      std::string archpath = loc_p;
      archpath.erase(
	  0, archpath.find("<archpath>") + std::string("<archpath>").size());
      archpath = archpath.substr(0, archpath.find("</archpath>"));
      std::string ind_str = loc_p;
      ind_str.erase(0, ind_str.find("<index>") + std::string("<index>").size());
      ind_str = ind_str.substr(0, ind_str.find("</index>"));
      std::stringstream strm;
      std::locale loc("C");
      strm.imbue(loc);
      strm << ind_str;
      uint64_t index;
      strm >> index;
      AuxFunc af;
      af.removeFmArch(archpath, index);
      std::filesystem::path filepath = std::filesystem::u8path(archpath);
      if(std::filesystem::exists(filepath))
	{
	  std::vector<std::tuple<int, int, std::string>> tv;
	  af.fileNames(archpath, tv);
	  tv.erase(
	      std::remove_if(
		  tv.begin(),
		  tv.end(),
		  []
		  (auto &el)
		    {
		      std::filesystem::path p = std::filesystem::u8path(std::get<2>(el));
		      if(p.extension().u8string() == ".fb2" ||
			  p.extension().u8string() == ".epub")
			{
			  return false;
			}
		      else
			{
			  return true;
			}
		    }),
	      tv.end());
	  zipparse.push_back(std::make_tuple(filepath, tv));
	}
      else
	{
	  zipremove.push_back(filepath);
	}
    }
  collRefresh();
}

void
RefreshCollection::addBook(std::string book_path)
{
  std::string filename;
  AuxFunc af;
  af.homePath(&filename);
  filename = filename + "/.MyLibrary/Collections/" + collname;
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  std::string bookpath;
  if(std::filesystem::exists(filepath))
    {
      for(auto &dirit : std::filesystem::directory_iterator(filepath))
	{
	  std::filesystem::path p = dirit.path();
	  if(!std::filesystem::is_directory(p))
	    {
	      if(p.filename().u8string() == "fb2base"
		  || p.filename().u8string() == "zipbase"
		  || p.filename().u8string() == "epubbase")
		{
		  bookpath.clear();
		  std::fstream f;
		  f.open(p, std::ios_base::in | std::ios_base::binary);
		  if(f.is_open())
		    {
		      bookpath.resize(std::filesystem::file_size(p));
		      f.read(&bookpath[0], bookpath.size());
		      bookpath.erase(0, std::string("<bp>").size());
		      bookpath = bookpath.substr(0, bookpath.find("</bp>"));
		      f.close();
		    }
		}
	      else
		{
		  bookpath.clear();
		  std::fstream f;
		  f.open(p, std::ios_base::in);
		  if(f.is_open())
		    {
		      getline(f, bookpath);
		      f.close();
		    }
		}
	    }
	  if(bookpath.size() > 0)
	    {
	      break;
	    }
	}
    }

  std::filesystem::path copy_fm = std::filesystem::u8path(book_path);
  if(std::filesystem::exists(copy_fm))
    {
      filename = bookpath + "/" + copy_fm.filename().u8string();
      filepath = std::filesystem::u8path(filename);
      if(!std::filesystem::exists(filepath))
	{
	  std::filesystem::copy(copy_fm, filepath);
	}
      if(filepath.extension().u8string() == ".fb2")
	{
	  fb2parse.push_back(filepath);
	}
      if(filepath.extension().u8string() == ".epub")
	{
	  epubparse.push_back(filepath);
	}
      if(filepath.extension().u8string() == ".zip")
	{
	  std::vector<std::tuple<int, int, std::string>> tv;
	  AuxFunc af;
	  af.fileNames(filepath.u8string(), tv);
	  tv.erase(
	      std::remove_if(
		  tv.begin(),
		  tv.end(),
		  []
		  (auto &el)
		    {
		      std::filesystem::path p = std::filesystem::u8path(std::get<2>(el));
		      if(p.extension().u8string() == ".fb2" || p.extension().u8string() == ".epub")
			{
			  return false;
			}
		      else
			{
			  return true;
			}
		    }),
	      tv.end());
	  zipparse.push_back(std::make_tuple(filepath, tv));
	}

    }
  collRefresh();
}

void
RefreshCollection::collRefresh()
{
  std::string filename;
  std::string bookpath;
  AuxFunc af;
  std::string rand = af.randomFileName();

  af.homePath(&filename);
  filename = filename + "/.MyLibrary/Collections/" + collname + "/fb2base";
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  filename = filepath.parent_path().u8string();
  filename = filename + "/fb2hash";
  std::filesystem::path hashpath = std::filesystem::u8path(filename);
  std::vector<std::string> fb2base;
  std::vector<std::string> hash;
  std::fstream f;
  f.open(filepath, std::ios_base::in | std::ios_base::binary);
  if(!f.is_open())
    {
      std::cerr << "RefreshCollection: fb2base file not openned" << std::endl;
    }
  else
    {
      std::string file_str;
      file_str.resize(std::filesystem::file_size(filepath));
      f.read(&file_str[0], file_str.size());
      f.close();
      bookpath = file_str.substr(
	  0, file_str.find("</bp>") + std::string("</bp>").size());
      file_str.erase(0, bookpath.size());
      bookpath.erase(0, std::string("<bp>").size());
      bookpath = bookpath.substr(0, bookpath.find("</bp>"));
      while(!file_str.empty())
	{
	  std::string line = file_str.substr(
	      0, file_str.find("<?L>") + std::string("<?L>").size());
	  file_str.erase(0, line.size());
	  line = line.substr(0, line.find("<?L>"));
	  fb2base.push_back(line);
	}
    }

  std::filesystem::remove(filepath);

  f.open(hashpath, std::ios_base::in);
  if(!f.is_open())
    {
      std::cerr << "RefreshCollection: fb2hash file not openned" << std::endl;
    }
  else
    {
      int count = 0;
      while(!f.eof())
	{
	  std::string line;
	  getline(f, line);
	  if(!line.empty())
	    {
	      if(count == 0)
		{
		  bookpath = line;
		}
	      else
		{
		  hash.push_back(line);
		}
	    }
	  count++;
	}
      f.close();
    }
  std::filesystem::remove(hashpath);

  for(size_t i = 0; i < fb2remove.size(); i++)
    {
      std::filesystem::path p = fb2remove[i];
      std::string p_str = p.u8string();
      p_str.erase(0, bookpath.size());
      fb2base.erase(std::remove_if(fb2base.begin(), fb2base.end(), [p_str]
      (auto &el)
	{
	  std::string::size_type n;
	  n = el.find(p_str);
	  if(n != std::string::npos)
	    {
	      return true;
	    }
	  else
	    {
	      return false;
	    }
	}),
		    fb2base.end());

      hash.erase(std::remove_if(hash.begin(), hash.end(), [p_str]
      (auto &el)
	{
	  std::string::size_type n;
	  n = el.find(p_str);
	  if(n != std::string::npos)
	    {
	      return true;
	    }
	  else
	    {
	      return false;
	    }
	}),
		 hash.end());
    }
  for(size_t i = 0; i < fb2parse.size(); i++)
    {
      std::filesystem::path p = fb2parse[i];
      std::string p_str = p.u8string();
      p_str.erase(0, bookpath.size());
      fb2base.erase(std::remove_if(fb2base.begin(), fb2base.end(), [p_str]
      (auto &el)
	{
	  std::string::size_type n;
	  n = el.find(p_str);
	  if(n != std::string::npos)
	    {
	      return true;
	    }
	  else
	    {
	      return false;
	    }
	}),
		    fb2base.end());

      hash.erase(std::remove_if(hash.begin(), hash.end(), [p_str]
      (auto &el)
	{
	  std::string::size_type n;
	  n = el.find(p_str);
	  if(n != std::string::npos)
	    {
	      return true;
	    }
	  else
	    {
	      return false;
	    }
	}),
		 hash.end());
    }
  filename = filepath.parent_path().parent_path().u8string();
  filename = filename + "/" + rand;
  std::filesystem::path t_c_p = std::filesystem::u8path(filename);
  if(std::filesystem::exists(t_c_p))
    {
      std::filesystem::remove_all(t_c_p);
    }
  std::filesystem::create_directories(t_c_p);
  CreateCollection cc(rand, std::filesystem::u8path(bookpath), thr_num, cancel);
  if(total_files)
    {
      cc.total_files = total_files;
    }
  if(files_added)
    {
      cc.files_added = files_added;
    }
  cc.createFileList(&fb2parse, &epubparse, &zipparse);
  filename = t_c_p.u8string();
  filename = filename + "/fb2base";
  t_c_p = std::filesystem::u8path(filename);
  f.open(t_c_p, std::ios_base::in | std::ios_base::binary);
  if(f.is_open())
    {
      std::string file_str;
      file_str.resize(std::filesystem::file_size(t_c_p));
      f.read(&file_str[0], file_str.size());
      f.close();
      bookpath = file_str.substr(
	  0, file_str.find("</bp>") + std::string("</bp>").size());
      file_str.erase(0, bookpath.size());
      bookpath.erase(0, std::string("<bp>").size());
      bookpath = bookpath.substr(0, bookpath.find("</bp>"));
      while(!file_str.empty())
	{
	  std::string line = file_str.substr(
	      0, file_str.find("<?L>") + std::string("<?L>").size());
	  file_str.erase(0, line.size());
	  line = line.substr(0, line.find("<?L>"));
	  fb2base.push_back(line);
	}
    }

  filename = t_c_p.parent_path().u8string();
  filename = filename + "/fb2hash";
  t_c_p = std::filesystem::u8path(filename);
  f.open(t_c_p, std::ios_base::in);
  if(f.is_open())
    {
      int count = 0;
      while(!f.eof())
	{
	  std::string line;
	  getline(f, line);
	  if(!line.empty())
	    {
	      if(count > 0)
		{
		  hash.push_back(line);
		}
	    }
	  count++;
	}
      f.close();
    }

  f.open(filepath, std::ios_base::out | std::ios_base::binary);
  if(f.is_open())
    {
      std::string line = "<bp>" + bookpath + "</bp>";
      f.write(line.c_str(), line.size());
      for(size_t i = 0; i < fb2base.size(); i++)
	{
	  line = fb2base[i] + "<?L>";
	  f.write(line.c_str(), line.size());
	}
      f.close();
    }

  f.open(hashpath, std::ios_base::out | std::ios_base::binary);
  if(f.is_open())
    {
      std::string line = bookpath + "\n";
      f.write(line.c_str(), line.size());
      for(size_t i = 0; i < hash.size(); i++)
	{
	  line = hash[i] + "\n";
	  f.write(line.c_str(), line.size());
	}
      f.close();
    }

  fb2base.clear();
  hash.clear();

  std::vector<std::tuple<std::string, std::vector<std::string>>> zipbase;
  filename = filepath.parent_path().u8string();
  filename = filename + "/zipbase";
  filepath = std::filesystem::u8path(filename);
  filename = filepath.parent_path().u8string();
  filename = filename + "/ziphash";
  hashpath = std::filesystem::u8path(filename);
  f.open(filepath, std::ios_base::in | std::ios_base::binary);
  if(f.is_open())
    {
      std::string file_str;
      file_str.resize(std::filesystem::file_size(filepath));
      f.read(&file_str[0], file_str.size());
      f.close();
      bookpath = file_str.substr(
	  0, file_str.find("</bp>") + std::string("</bp>").size());
      file_str.erase(0, bookpath.size());
      bookpath.erase(0, std::string("<bp>").size());
      bookpath = bookpath.substr(0, bookpath.find("</bp>"));
      while(!file_str.empty())
	{
	  std::string archpath = file_str.substr(
	      0, file_str.find("<?e>") + std::string("<?e>").size());
	  file_str.erase(0, archpath.size());
	  archpath.erase(0, std::string("<?a>").size());
	  archpath = archpath.substr(0, archpath.find("<?e>"));
	  std::string archgr = file_str.substr(0, file_str.find("<?a>"));
	  file_str.erase(0, archgr.size());
	  std::vector<std::string> tv;
	  while(!archgr.empty())
	    {
	      std::string line = archgr.substr(
		  0, archgr.find("<?L>") + std::string("<?L>").size());
	      archgr.erase(0, line.size());
	      line = line.substr(0, line.find("<?L>"));
	      tv.push_back(line);
	    }
	  std::tuple<std::string, std::vector<std::string>> ttup;
	  std::get<0>(ttup) = archpath;
	  std::get<1>(ttup) = tv;
	  zipbase.push_back(ttup);
	}
    }

  std::filesystem::remove(filepath);

  f.open(hashpath, std::ios_base::in);
  if(f.is_open())
    {
      int count = 0;
      while(!f.eof())
	{
	  std::string line;
	  getline(f, line);
	  if(!line.empty())
	    {
	      if(count == 0)
		{
		  bookpath = line;
		}
	      else
		{
		  hash.push_back(line);
		}
	    }
	  count++;
	}
      f.close();
    }
  std::filesystem::remove(hashpath);

  for(size_t i = 0; i < zipremove.size(); i++)
    {
      std::filesystem::path p = zipremove[i];
      std::string p_str = p.u8string();
      p_str.erase(0, bookpath.size());
      zipbase.erase(std::remove_if(zipbase.begin(), zipbase.end(), [p_str]
      (auto &el)
	{
	  return std::get<0>(el) == p_str;
	}),
		    zipbase.end());
      hash.erase(std::remove_if(hash.begin(), hash.end(), [p_str]
      (auto &el)
	{
	  std::string::size_type n;
	  n = el.find(p_str);
	  if(n != std::string::npos)
	    {
	      return true;
	    }
	  else
	    {
	      return false;
	    }
	}),
		 hash.end());
    }

  for(size_t i = 0; i < zipparse.size(); i++)
    {
      std::filesystem::path p = std::get<0>(zipparse[i]);
      std::string p_str = p.u8string();
      p_str.erase(0, bookpath.size());
      zipbase.erase(std::remove_if(zipbase.begin(), zipbase.end(), [p_str]
      (auto &el)
	{
	  return std::get<0>(el) == p_str;
	}),
		    zipbase.end());
      hash.erase(std::remove_if(hash.begin(), hash.end(), [p_str]
      (auto &el)
	{
	  std::string::size_type n;
	  n = el.find(p_str);
	  if(n != std::string::npos)
	    {
	      return true;
	    }
	  else
	    {
	      return false;
	    }
	}),
		 hash.end());
    }
  filename = t_c_p.parent_path().u8string();
  filename = filename + "/zipbase";
  t_c_p = std::filesystem::u8path(filename);
  f.open(t_c_p, std::ios_base::in | std::ios_base::binary);
  if(f.is_open())
    {
      std::string file_str;
      file_str.resize(std::filesystem::file_size(t_c_p));
      f.read(&file_str[0], file_str.size());
      f.close();
      bookpath = file_str.substr(
	  0, file_str.find("</bp>") + std::string("</bp>").size());
      file_str.erase(0, bookpath.size());
      bookpath.erase(0, std::string("<bp>").size());
      bookpath = bookpath.substr(0, bookpath.find("</bp>"));
      while(!file_str.empty())
	{
	  std::string archpath = file_str.substr(
	      0, file_str.find("<?e>") + std::string("<?e>").size());
	  file_str.erase(0, archpath.size());
	  archpath.erase(0, std::string("<?a>").size());
	  archpath = archpath.substr(0, archpath.find("<?e>"));
	  std::string archgr = file_str.substr(0, file_str.find("<?a>"));
	  file_str.erase(0, archgr.size());
	  std::vector<std::string> tv;
	  while(!archgr.empty())
	    {
	      std::string line = archgr.substr(
		  0, archgr.find("<?L>") + std::string("<?L>").size());
	      archgr.erase(0, line.size());
	      line = line.substr(0, line.find("<?L>"));
	      tv.push_back(line);
	    }
	  std::tuple<std::string, std::vector<std::string>> ttup;
	  std::get<0>(ttup) = archpath;
	  std::get<1>(ttup) = tv;
	  zipbase.push_back(ttup);
	}
    }

  filename = t_c_p.parent_path().u8string();
  filename = filename + "/ziphash";
  t_c_p = std::filesystem::u8path(filename);
  f.open(t_c_p, std::ios_base::in);
  if(f.is_open())
    {
      int count = 0;
      while(!f.eof())
	{
	  std::string line;
	  getline(f, line);
	  if(!line.empty())
	    {
	      if(count == 0)
		{
		  bookpath = line;
		}
	      else
		{
		  hash.push_back(line);
		}
	    }
	  count++;
	}
      f.close();
    }

  f.open(filepath, std::ios_base::out | std::ios_base::binary);
  if(f.is_open())
    {
      std::string line = "<bp>" + bookpath + "</bp>";
      f.write(line.c_str(), line.size());
      for(size_t i = 0; i < zipbase.size(); i++)
	{
	  line = std::get<0>(zipbase[i]);
	  line = "<?a>" + line + "<?e>";
	  f.write(line.c_str(), line.size());
	  std::vector<std::string> tv = std::get<1>(zipbase[i]);
	  for(size_t j = 0; j < tv.size(); j++)
	    {
	      line = tv[j] + "<?L>";
	      f.write(line.c_str(), line.size());
	    }
	}
      f.close();
    }
  f.open(hashpath, std::ios_base::out | std::ios_base::binary);
  if(f.is_open())
    {
      std::string line = bookpath + "\n";
      f.write(line.c_str(), line.size());
      for(size_t i = 0; i < hash.size(); i++)
	{
	  line = hash[i] + "\n";
	  f.write(line.c_str(), line.size());
	}
      f.close();
    }
  zipbase.clear();
  hash.clear();

  std::vector<std::string> epubbase;
  filename = filepath.parent_path().u8string();
  filename = filename + "/epubbase";
  filepath = std::filesystem::u8path(filename);
  filename = filepath.parent_path().u8string();
  filename = filename + "/epubhash";
  hashpath = std::filesystem::u8path(filename);
  f.open(filepath, std::ios_base::in | std::ios_base::binary);
  if(f.is_open())
    {
      std::string file_str;
      file_str.resize(std::filesystem::file_size(filepath));
      f.read(&file_str[0], file_str.size());
      f.close();
      bookpath = file_str.substr(
	  0, file_str.find("</bp>") + std::string("</bp>").size());
      file_str.erase(0, bookpath.size());
      bookpath.erase(0, std::string("<bp>").size());
      bookpath = bookpath.substr(0, bookpath.find("</bp>"));
      while(!file_str.empty())
	{
	  std::string line = file_str.substr(
	      0, file_str.find("<?L>") + std::string("<?L>").size());
	  file_str.erase(0, line.size());
	  line = line.substr(0, line.find("<?L>"));
	  epubbase.push_back(line);
	}
    }

  std::filesystem::remove(filepath);

  f.open(hashpath, std::ios_base::in);
  if(f.is_open())
    {
      int count = 0;
      while(!f.eof())
	{
	  std::string line;
	  getline(f, line);
	  if(!line.empty())
	    {
	      if(count == 0)
		{
		  bookpath = line;
		}
	      else
		{
		  hash.push_back(line);
		}
	    }
	  count++;
	}
      f.close();
    }
  std::filesystem::remove(hashpath);

  for(size_t i = 0; i < epubremove.size(); i++)
    {
      std::filesystem::path p = epubremove[i];
      std::string p_str = p.u8string();
      p_str.erase(0, bookpath.size());
      epubbase.erase(std::remove_if(epubbase.begin(), epubbase.end(), [p_str]
      (auto &el)
	{
	  std::string::size_type n;
	  n = el.find(p_str);
	  if(n != std::string::npos)
	    {
	      return true;
	    }
	  else
	    {
	      return false;
	    }
	}),
		     epubbase.end());

      hash.erase(std::remove_if(hash.begin(), hash.end(), [p_str]
      (auto &el)
	{
	  std::string::size_type n;
	  n = el.find(p_str);
	  if(n != std::string::npos)
	    {
	      return true;
	    }
	  else
	    {
	      return false;
	    }
	}),
		 hash.end());
    }

  for(size_t i = 0; i < epubparse.size(); i++)
    {
      std::filesystem::path p = epubparse[i];
      std::string p_str = p.u8string();
      p_str.erase(0, bookpath.size());
      epubbase.erase(std::remove_if(epubbase.begin(), epubbase.end(), [p_str]
      (auto &el)
	{
	  std::string::size_type n;
	  n = el.find(p_str);
	  if(n != std::string::npos)
	    {
	      return true;
	    }
	  else
	    {
	      return false;
	    }
	}),
		     epubbase.end());

      hash.erase(std::remove_if(hash.begin(), hash.end(), [p_str]
      (auto &el)
	{
	  std::string::size_type n;
	  n = el.find(p_str);
	  if(n != std::string::npos)
	    {
	      return true;
	    }
	  else
	    {
	      return false;
	    }
	}),
		 hash.end());
    }
  filename = t_c_p.parent_path().u8string();
  filename = filename + "/epubbase";
  t_c_p = std::filesystem::u8path(filename);
  f.open(t_c_p, std::ios_base::in | std::ios_base::binary);
  if(f.is_open())
    {
      std::string file_str;
      file_str.resize(std::filesystem::file_size(t_c_p));
      f.read(&file_str[0], file_str.size());
      f.close();
      bookpath = file_str.substr(
	  0, file_str.find("</bp>") + std::string("</bp>").size());
      file_str.erase(0, bookpath.size());
      bookpath.erase(0, std::string("<bp>").size());
      bookpath = bookpath.substr(0, bookpath.find("</bp>"));
      while(!file_str.empty())
	{
	  std::string line = file_str.substr(
	      0, file_str.find("<?L>") + std::string("<?L>").size());
	  file_str.erase(0, line.size());
	  line = line.substr(0, line.find("<?L>"));
	  epubbase.push_back(line);
	}
    }

  filename = t_c_p.parent_path().u8string();
  filename = filename + "/epubhash";
  t_c_p = std::filesystem::u8path(filename);
  f.open(t_c_p, std::ios_base::in);
  if(f.is_open())
    {
      int count = 0;
      while(!f.eof())
	{
	  std::string line;
	  getline(f, line);
	  if(!line.empty())
	    {
	      if(count == 0)
		{
		  bookpath = line;
		}
	      else
		{
		  hash.push_back(line);
		}
	    }
	  count++;
	}
      f.close();
    }

  f.open(filepath, std::ios_base::out | std::ios_base::binary);
  if(f.is_open())
    {
      std::string line = "<bp>" + bookpath + "</bp>";
      f.write(line.c_str(), line.size());
      for(size_t i = 0; i < epubbase.size(); i++)
	{
	  line = epubbase[i] + "<?L>";
	  f.write(line.c_str(), line.size());
	}
      f.close();
    }

  f.open(hashpath, std::ios_base::out | std::ios_base::binary);
  if(f.is_open())
    {
      std::string line = bookpath + "\n";
      f.write(line.c_str(), line.size());
      for(size_t i = 0; i < hash.size(); i++)
	{
	  line = hash[i] + "\n";
	  f.write(line.c_str(), line.size());
	}
      f.close();
    }
  std::filesystem::remove_all(t_c_p.parent_path());

  if(refresh_finished)
    {
      refresh_finished();
    }
}