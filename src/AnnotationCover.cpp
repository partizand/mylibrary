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

#include "AnnotationCover.h"

AnnotationCover::AnnotationCover(std::string filename)
{
  this->rcvd_filename = filename;
  fileRead();
}

AnnotationCover::~AnnotationCover()
{

}

void
AnnotationCover::fileRead()
{
  std::string::size_type n;
  n = rcvd_filename.find("<zip>");
  if(n == std::string::npos)
    {
      std::filesystem::path filepath = std::filesystem::u8path(rcvd_filename);
      std::string ext = filepath.extension().u8string();
      AuxFunc af;
      af.stringToLower(ext);
      if(ext == ".fb2")
	{
	  fb2_ch_f = true;
	  fb2Parse(filepath);
	}
      else if(ext == ".epub")
	{
	  epub_ch_f = true;
	  epubParse(filepath);
	}
      else if(ext == ".pdf")
	{
	  pdf_ch_f = true;
	  pdfParse(filepath);
	}
      else if(ext == ".djvu")
	{
	  djvu_ch_f = true;
	  djvuParse(filepath);
	}
    }
  else
    {
      zip_ch_f = true;
      std::string index = rcvd_filename;
      index.erase(0, index.find("<index>") + std::string("<index>").size());
      index = index.substr(0, index.find("</index>"));
      std::stringstream strm;
      std::locale loc("C");
      strm.imbue(loc);
      strm << index;
      int ind;
      strm >> ind;
      std::string archaddr = rcvd_filename;
      archaddr.erase(
	  0, archaddr.find("<archpath>") + std::string("<archpath>").size());
      archaddr = archaddr.substr(0, archaddr.find("</archpath>"));
      AuxFunc af;
      std::string outfolder = af.temp_path();
      outfolder = outfolder + "/" + af.randomFileName();
      std::filesystem::path filepath = std::filesystem::u8path(outfolder);
      if(std::filesystem::exists(filepath))
	{
	  std::filesystem::remove_all(filepath);
	}
      std::filesystem::path check_non_zip = std::filesystem::u8path(archaddr);
      if(check_non_zip.extension().u8string() == ".zip")
	{
	  af.unpackByIndex(archaddr, outfolder, ind);
	}
      else
	{
	  af.unpackByIndexNonZip(archaddr, outfolder, ind);
	}
      if(std::filesystem::exists(filepath))
	{
	  for(auto &dirit : std::filesystem::directory_iterator(filepath))
	    {
	      std::filesystem::path p = dirit.path();
	      std::string ext = p.extension().u8string();
	      if(!std::filesystem::is_directory(p) && ext == ".fb2")
		{
		  zip_ch_f = false;
		  fb2_ch_f = true;
		  fb2Parse(p);
		  std::filesystem::remove_all(filepath);
		  break;
		}
	      else if(!std::filesystem::is_directory(p) && ext == ".epub")
		{
		  epub_ch_f = true;
		  zip_ch_f = false;
		  epub_path = filepath;
		  std::vector<std::tuple<int, int, std::string>> listv;
		  if(check_non_zip.extension().u8string() == ".zip")
		    {
		      af.fileNames(archaddr, listv);
		    }
		  else
		    {
		      af.fileNamesNonZip(archaddr, listv);
		    }
		  auto itlv =
		      std::find_if(
			  listv.begin(),
			  listv.end(),
			  [p]
			  (auto &el)
			    {
			      std::filesystem::path lp = std::filesystem::u8path(std::get<2>(el));
			      if(lp.stem().u8string() == p.stem().u8string() &&
				  lp.extension().u8string() == ".fbd")
				{
				  return true;
				}
			      else
				{
				  return false;
				}
			    });
		  if(itlv != listv.end())
		    {
		      if(check_non_zip.extension().u8string() == ".zip")
			{
			  file = af.unpackByIndex(archaddr, std::get<0>(*itlv),
						  std::get<1>(*itlv));
			}
		      else
			{
			  file = af.unpackByIndexNonZipStr(archaddr,
							   std::get<0>(*itlv));
			}
		      epub_ch_f = false;
		      fb2_ch_f = true;
		      std::filesystem::remove_all(filepath);
		    }
		  else
		    {
		      epubParse(p);
		    }
		  break;
		}
	      else if(!std::filesystem::is_directory(p) && ext == ".pdf")
		{
		  pdf_ch_f = true;
		  zip_ch_f = false;
		  pdfZipParse(p, filepath, archaddr);
		  break;
		}
	      else if(!std::filesystem::is_directory(p) && ext == ".djvu")
		{
		  djvu_ch_f = true;
		  zip_ch_f = false;
		  std::vector<std::tuple<int, int, std::string>> listv;
		  if(check_non_zip.extension().u8string() == ".zip")
		    {
		      af.fileNames(archaddr, listv);
		    }
		  else
		    {
		      af.fileNamesNonZip(archaddr, listv);
		    }
		  auto itlv =
		      std::find_if(
			  listv.begin(),
			  listv.end(),
			  [p]
			  (auto &el)
			    {
			      std::filesystem::path lp = std::filesystem::u8path(std::get<2>(el));
			      if(lp.stem().u8string() == p.stem().u8string() &&
				  lp.extension().u8string() == ".fbd")
				{
				  return true;
				}
			      else
				{
				  return false;
				}
			    });
		  if(itlv != listv.end())
		    {
		      if(check_non_zip.extension().u8string() == ".zip")
			{
			  file = af.unpackByIndex(archaddr, std::get<0>(*itlv),
						  std::get<1>(*itlv));
			}
		      else
			{
			  file = af.unpackByIndexNonZipStr(archaddr,
							   std::get<0>(*itlv));
			}
		      djvu_ch_f = false;
		      fb2_ch_f = true;
		    }
		  else
		    {
		      djvuParse(p);
		    }
		  std::filesystem::remove_all(filepath);
		  break;
		}
	    }
	}
    }
}

std::string
AnnotationCover::annotationRet()
{
  std::string result;
  if(epub_ch_f)
    {
      result = annotationEpub();
    }
  else if(fb2_ch_f || zip_ch_f)
    {
      std::string::size_type n = 0;
      std::string conv_name = file;
      n = conv_name.find("<?xml");
      if(n != std::string::npos)
	{
	  conv_name.erase(0, n);
	  conv_name = conv_name.substr(
	      0, conv_name.find("?>") + std::string("?>").size());
	  n = conv_name.find("encoding=");
	  if(n != std::string::npos)
	    {
	      conv_name.erase(0, n + std::string("encoding=").size());
	      n = conv_name.find("\"");
	      if(n != std::string::npos)
		{
		  conv_name.erase(
		      0, conv_name.find("\"") + std::string("\"").size());
		  conv_name = conv_name.substr(0, conv_name.find("\""));
		}
	      else
		{
		  n = conv_name.find("\'");
		  if(n != std::string::npos)
		    {
		      conv_name.erase(
			  0, conv_name.find("\'") + std::string("\'").size());
		      conv_name = conv_name.substr(0, conv_name.find("\'"));
		    }
		}
	    }
	  else
	    {
	      conv_name.clear();
	    }
	}
      else
	{
	  conv_name.clear();
	}
      result = file;
      if(!conv_name.empty())
	{
	  AuxFunc af;
	  af.toutf8(result, conv_name);
	}

      n = result.find("<annotation>");
      if(n != std::string::npos)
	{
	  result.erase(0, n + std::string("<annotation>").size());
	  result = result.substr(0, result.find("</annotation>"));
	}
      else
	{
	  result.clear();
	}
      n = result.find("</p>");
      if(n != std::string::npos)
	{
	  std::string line = result;
	  result.clear();
	  while(n != std::string::npos)
	    {
	      n = line.find("</p>");
	      if(n != std::string::npos)
		{
		  std::string tmp = line.substr(0,
						n + std::string("</p>").size());
		  tmp.erase(0, tmp.rfind("<p"));
		  line.erase(line.find(tmp), tmp.size());
		  std::string::size_type n_tmp;
		  n_tmp = tmp.find("<p");
		  if(n_tmp != std::string::npos)
		    {
		      tmp.erase(0, tmp.find(">") + std::string(">").size());
		      tmp = tmp.substr(0, tmp.find("</p>"));
		      if(!tmp.empty())
			{
			  result = result + tmp + "\n\n";
			}
		    }
		  else
		    {
		      result = result + tmp.substr(0, tmp.find("</p>"))
			  + "\n\n";
		    }
		}
	    }
	}
      for(;;)
	{
	  n = result.find("<strong>");
	  if(n == std::string::npos)
	    {
	      break;
	    }
	  else
	    {
	      result.erase(n, std::string("<strong>").size());
	      result.insert(n, "<b>");
	      std::string::size_type n_tmp = result.find("</strong>");
	      if(n_tmp != std::string::npos)
		{
		  result.erase(n_tmp, std::string("</strong>").size());
		  result.insert(n_tmp, "</b>");
		}
	      else
		{
		  n_tmp = result.find("<strong/>");
		  if(n_tmp != std::string::npos)
		    {
		      result.erase(n_tmp, std::string("<strong/>").size());
		      result.insert(n_tmp, "</b>");
		    }
		}
	    }
	}

      for(;;)
	{
	  n = result.find("<emphasis>");
	  if(n == std::string::npos)
	    {
	      break;
	    }
	  else
	    {
	      result.erase(n, std::string("<emphasis>").size());
	      result.insert(n, "<i>");
	      std::string::size_type n_tmp = result.find("</emphasis>");
	      if(n_tmp != std::string::npos)
		{
		  result.erase(n_tmp, std::string("</emphasis>").size());
		  result.insert(n_tmp, "</i>");
		}
	      else
		{
		  n_tmp = result.find("<emphasis/>");
		  if(n_tmp != std::string::npos)
		    {
		      result.erase(n_tmp, std::string("<emphasis/>").size());
		      result.insert(n_tmp, "</i>");
		    }
		}
	    }
	}

      for(;;)
	{
	  n = result.find("<strikethrough>");
	  if(n == std::string::npos)
	    {
	      break;
	    }
	  else
	    {
	      result.erase(n, std::string("<strikethrough>").size());
	      result.insert(n, "<s>");
	      std::string::size_type n_tmp = result.find("</strikethrough>");
	      if(n_tmp != std::string::npos)
		{
		  result.erase(n_tmp, std::string("</strikethrough>").size());
		  result.insert(n_tmp, "</s>");
		}
	      else
		{
		  n_tmp = result.find("<strikethrough/>");
		  if(n_tmp != std::string::npos)
		    {
		      result.erase(n_tmp,
				   std::string("<strikethrough/>").size());
		      result.insert(n_tmp, "</s>");
		    }
		}
	    }
	}
      for(;;)
	{
	  n = result.find("<image");
	  if(n == std::string::npos)
	    {
	      break;
	    }
	  else
	    {
	      std::string::size_type n_tmp;
	      n_tmp = result.find(">");
	      if(n_tmp != std::string::npos)
		{
		  std::string tmp(
		      result.begin() + n,
		      result.begin() + n_tmp + std::string(">").size());
		  result.erase(n, tmp.size());
		}
	    }
	}

      for(;;)
	{
	  n = result.find("\n");
	  if(n == 0)
	    {
	      result.erase(0, std::string("\n").size());
	    }
	  else
	    {
	      break;
	    }
	}

      for(;;)
	{
	  n = result.find("\n\n\n");
	  if(n != std::string::npos)
	    {
	      result.erase(n, std::string("\n").size());
	    }
	  else
	    {
	      break;
	    }
	}
    }
  else if(pdf_ch_f)
    {
      result = file;
    }
  else if(djvu_ch_f)
    {
      result = file;
    }

  std::string::size_type n = 0;
  while(n != std::string::npos)
    {
      n = result.find("<br>");
      if(n != std::string::npos)
	{
	  result.erase(n, std::string("<br>").size());
	}
    }
  n = 0;
  while(n != std::string::npos)
    {
      n = result.find("<br/>");
      if(n != std::string::npos)
	{
	  result.erase(n, std::string("<br/>").size());
	  result.insert(n, "\n\n");
	}
    }
  n = 0;
  while(n != std::string::npos)
    {
      n = result.find("</br>");
      if(n != std::string::npos)
	{
	  result.erase(n, std::string("</br>").size());
	  result.insert(n, "\n\n");
	}
    }
  n = result.find("<a");
  if(n != std::string::npos)
    {
      std::string::size_type n_end;
      n_end = result.find(">", n);
      std::string http_addr;
      if(n_end != std::string::npos)
	{
	  std::string::size_type n_http;
	  n_http = result.find("http", n_end);
	  if(n_http != std::string::npos)
	    {
	      n_end = result.find("<", n_http);
	      if(n_end != std::string::npos)
		{
		  http_addr = result.substr(n_http, n_end - n_http);
		}
	      else
		{
		  http_addr = result;
		  http_addr.erase(0, n_http);
		}
	    }
	}
      n_end = result.find("</a>", n);
      if(n_end != std::string::npos)
	{
	  result.erase(n, n_end - n + std::string("</a>").size());
	}
      else
	{
	  n_end = result.find("<a/>", n);
	  if(n_end != std::string::npos)
	    {
	      result.erase(n, n_end - n + std::string("<a/>").size());
	    }
	  else
	    {
	      result = result.substr(0, n);
	    }
	}

      while(result.begin() != result.end())
	{
	  if(*result.rbegin() == '\n')
	    {
	      result.pop_back();
	    }
	  else
	    {
	      break;
	    }
	}
      if(!http_addr.empty())
	{
	  result = result + " " + http_addr;
	}
    }
  result.shrink_to_fit();
  return result;
}

cover_image
AnnotationCover::coverRet()
{
  cover_image result;
  if(epub_ch_f)
    {
      std::filesystem::path p = std::filesystem::u8path(coverEpub());
      std::fstream f;
      f.open(p, std::ios_base::in | std::ios_base::binary);
      if(f.is_open())
	{
	  result.image.resize(std::filesystem::file_size(p));
	  f.read(result.image.data(), result.image.size());
	  f.close();
	  result.format = p.extension().u8string();
	  for(auto it = result.format.begin(); it != result.format.end();)
	    {
	      if((*it) == '.')
		{
		  result.format.erase(it);
		}
	      else
		{
		  it++;
		}
	    }
	}
      std::filesystem::remove_all(p);
    }
  else if(fb2_ch_f || zip_ch_f)
    {
      std::string href = file;
      href.erase(0, href.find("<coverpage>"));
      if(!href.empty())
	{
	  href = href.substr(
	      0,
	      href.find("</coverpage>") + std::string("</coverpage>").size());
	  href.erase(0, href.find("href=") + std::string("href=").size());

	  std::string::size_type n;
	  n = href.find("\"");
	  if(n != std::string::npos)
	    {
	      href.erase(0, n + std::string("\"").size());
	      n = href.find("#");
	      if(n != std::string::npos)
		{
		  href.erase(0, n + std::string("#").size());
		}
	      href = href.substr(0, href.find("\""));
	    }
	  std::string image = file;
	  for(;;)
	    {
	      image.erase(0, image.find("<binary"));
	      if(!image.empty())
		{
		  std::string ch_str = image;
		  ch_str = ch_str.substr(0, ch_str.find(">"));
		  n = ch_str.find(href);
		  if(n != std::string::npos)
		    {
		      image.erase(0, image.find(">") + std::string(">").size());
		      image = image.substr(0, image.find("</binary>"));
		      image.erase(std::remove_if(image.begin(), image.end(), []
		      (auto &el)
			{
			  return el == '\n';
			}),
				  image.end());
		      result.format = "base64";
		      std::copy(image.begin(), image.end(),
				std::back_inserter(result.image));
		      break;
		    }
		  else
		    {
		      image.erase(
			  0,
			  image.find("</binary>")
			      + std::string("</binary>").size());
		      if(image.empty())
			{
			  break;
			}
		    }
		}
	      else
		{
		  break;
		}
	    }
	}
    }
  else if(pdf_ch_f)
    {
      result = pdf_result;
    }
  else if(djvu_ch_f)
    {
      result = djvu_result;
    }
  result.image.shrink_to_fit();
  return result;
}
std::string
AnnotationCover::annotationEpub()
{
  std::string result;
  result = file;
  std::string::size_type n = 0;
  std::string conv_name = file;
  n = conv_name.find("<?xml");
  if(n != std::string::npos)
    {
      conv_name.erase(0, n);
      conv_name = conv_name.substr(
	  0, conv_name.find("?>") + std::string("?>").size());
      n = conv_name.find("encoding=");
      if(n != std::string::npos)
	{
	  conv_name.erase(0, n + std::string("encoding=").size());
	  n = conv_name.find("\"");
	  if(n != std::string::npos)
	    {
	      conv_name.erase(0,
			      conv_name.find("\"") + std::string("\"").size());
	      conv_name = conv_name.substr(0, conv_name.find("\""));
	    }
	  else
	    {
	      n = conv_name.find("\'");
	      if(n != std::string::npos)
		{
		  conv_name.erase(
		      0, conv_name.find("\'") + std::string("\'").size());
		  conv_name = conv_name.substr(0, conv_name.find("\'"));
		}
	    }
	}
      else
	{
	  conv_name.clear();
	}
    }
  else
    {
      conv_name.clear();
    }
  result = file;
  if(!conv_name.empty())
    {
      AuxFunc af;
      af.toutf8(result, conv_name);
    }
  n = result.find("<dc:description>");
  if(n != std::string::npos)
    {
      result.erase(0, n + std::string("<dc:description>").size());
      result = result.substr(0, result.find("</dc:description>"));
    }
  else
    {
      result.clear();
    }

  return result;
}

std::string
AnnotationCover::coverEpub()
{
  std::string result;
  result = file;
  std::string::size_type n = 0;
  std::string conv_name = file;
  n = conv_name.find("<?xml");
  if(n != std::string::npos)
    {
      conv_name.erase(0, n);
      conv_name = conv_name.substr(
	  0, conv_name.find("?>") + std::string("?>").size());
      n = conv_name.find("encoding=");
      if(n != std::string::npos)
	{
	  conv_name.erase(0, n + std::string("encoding=").size());
	  n = conv_name.find("\"");
	  if(n != std::string::npos)
	    {
	      conv_name.erase(0,
			      conv_name.find("\"") + std::string("\"").size());
	      conv_name = conv_name.substr(0, conv_name.find("\""));
	    }
	  else
	    {
	      n = conv_name.find("\'");
	      if(n != std::string::npos)
		{
		  conv_name.erase(
		      0, conv_name.find("\'") + std::string("\'").size());
		  conv_name = conv_name.substr(0, conv_name.find("\'"));
		}
	    }
	}
      else
	{
	  conv_name.clear();
	}
    }
  else
    {
      conv_name.clear();
    }
  result = file;
  if(!conv_name.empty())
    {
      AuxFunc af;
      af.toutf8(result, conv_name);
    }
  std::string line = result;
  result.clear();
  n = 0;
  std::string cover;
  std::string content;
  while(n != std::string::npos)
    {
      n = line.find("<meta ");
      if(n != std::string::npos)
	{
	  std::string meta = line;
	  meta.erase(0, n);
	  meta = meta.substr(0, meta.find("/>") + std::string("/>").size());
	  line.erase(line.find(meta), meta.size());
	  std::string::size_type meta_n = meta.find("cover");
	  if(meta_n != std::string::npos)
	    {
	      content = meta;
	      content.erase(
		  0,
		  content.find("content=\"")
		      + std::string("content=\"").size());
	      content = content.substr(0, content.find("\""));
	      cover = file;
	      break;
	    }
	}
    }
  if(!cover.empty())
    {
      n = 0;
      while(n != std::string::npos)
	{
	  n = cover.find("<item");
	  if(n != std::string::npos)
	    {
	      std::string c_p = cover;
	      c_p.erase(0, n);
	      c_p = c_p.substr(0, c_p.find("/>") + std::string("/>").size());
	      cover.erase(cover.find(c_p), c_p.size());
	      std::string::size_type c_n;
	      c_n = c_p.find("id=\"" + content);
	      if(c_n != std::string::npos)
		{
		  c_p.erase(
		      0, c_p.find("href=\"") + std::string("href=\"").size());
		  c_p = c_p.substr(0, c_p.find("\""));
		  result = c_p;
		  break;
		}
	    }
	}
    }

  std::vector<std::tuple<int, int, std::string>> list;
  AuxFunc af;
  std::filesystem::path ch_p = std::filesystem::u8path(rcvd_filename);
  if(ch_p.extension().u8string() == ".epub")
    {
      af.fileNames(rcvd_filename, list);
      ch_p = std::filesystem::u8path(rcvd_filename);
    }
  else
    {
      if(std::filesystem::exists(epub_path))
	{
	  for(auto &dirit : std::filesystem::directory_iterator(epub_path))
	    {
	      std::filesystem::path p = dirit.path();
	      if(!std::filesystem::is_directory(p)
		  && p.extension().u8string() == ".epub")
		{
		  af.fileNames(p.u8string(), list);
		  ch_p = p;
		  break;
		}
	    }
	}
    }
  cover = result;
  result.clear();
  if(!cover.empty() && list.size() > 0)
    {
      auto itl = std::find_if(list.begin(), list.end(), [cover]
      (auto &el)
	{
	  std::string line = std::get<2>(el);
	  std::string::size_type n;
	  n = line.find(cover);
	  if(n != std::string::npos)
	    {
	      return true;
	    }
	  else
	    {
	      return false;
	    }
	});
      if(itl != list.end())
	{
	  std::string p_str = af.temp_path();
	  p_str = p_str + "/" + af.randomFileName();
	  std::filesystem::path filepath = std::filesystem::u8path(p_str);
	  if(std::filesystem::exists(filepath))
	    {
	      std::filesystem::remove_all(filepath);
	    }
	  af.unpackByIndex(ch_p.u8string(), filepath.u8string(),
			   std::get<0>(*itl));
	  if(std::filesystem::exists(filepath))
	    {
	      for(auto &dirit : std::filesystem::directory_iterator(filepath))
		{
		  std::filesystem::path p = dirit.path();
		  if(!std::filesystem::is_directory(p))
		    {
		      result = p.u8string();
		      break;
		    }
		}
	    }
	}
    }
  if(std::filesystem::exists(epub_path))
    {
      std::filesystem::remove_all(epub_path);
    }

  return result;
}

void
AnnotationCover::fb2Parse(std::filesystem::path filepath)
{
  std::fstream f;
  f.open(filepath, std::ios_base::in | std::ios_base::binary);
  if(!f.is_open())
    {
      std::cerr << "AnnotationCover: fb2 file not opened" << std::endl;
    }
  else
    {
      file.resize(std::filesystem::file_size(filepath));
      f.read(&file[0], file.size());
      f.close();
    }
}

void
AnnotationCover::epubParse(std::filesystem::path filepath)
{
  std::vector<std::tuple<int, int, std::string>> list;
  AuxFunc af;
  af.fileNames(filepath.u8string(), list);
  std::string filename = af.temp_path();
  filename = filename + "/" + af.randomFileName();
  std::filesystem::path outfolder = std::filesystem::u8path(filename);
  if(std::filesystem::exists(outfolder))
    {
      std::filesystem::remove_all(outfolder);
    }
  auto itl = std::find_if(list.begin(), list.end(), []
  (auto &el)
    {
      std::filesystem::path p = std::filesystem::u8path(std::get<2>(el));
      std::string ext = p.extension().u8string();
      AuxFunc af;
      af.stringToLower(ext);
      return ext == ".opf";
    });
  if(itl != list.end())
    {
      af.unpackByIndex(filepath.u8string(), outfolder.u8string(),
		       std::get<0>(*itl));
      if(std::filesystem::exists(outfolder))
	{
	  for(auto &dirit : std::filesystem::directory_iterator(outfolder))
	    {
	      std::filesystem::path p = dirit.path();
	      std::string ext = p.extension().u8string();
	      af.stringToLower(ext);
	      if(ext == ".opf")
		{
		  std::fstream f;
		  f.open(p, std::ios_base::in | std::ios_base::binary);
		  if(f.is_open())
		    {
		      file.resize(std::filesystem::file_size(p));
		      f.read(&file[0], file.size());
		      f.close();
		    }
		}
	    }
	  std::filesystem::remove_all(outfolder);
	}
    }
}

void
AnnotationCover::pdfParse(std::filesystem::path filepath)
{
  AuxFunc af;
  poppler::document *doc = poppler::document::load_from_file(filepath.string());
  if(doc)
    {
      poppler::ustring subj = doc->get_subject();
      std::vector<char> buf = subj.to_utf8();
      std::copy(buf.begin(), buf.end(), std::back_inserter(file));
      poppler::page *pg = doc->create_page(0);
      poppler::page_renderer pr;
      pr.set_image_format(poppler::image::format_rgb24);
      poppler::image img = pr.render_page(pg, 100.0, 100.0, -1, -1, -1, -1,
					  poppler::rotate_0);
      pdf_result.image.resize(img.bytes_per_row() * img.height());
      std::memcpy(pdf_result.image.data(), img.const_data(),
		  pdf_result.image.size());
      pdf_result.format = "rgb";
      pdf_result.rowsize = img.bytes_per_row();
      delete pg;
      delete doc;
    }
}

void
AnnotationCover::pdfZipParse(std::filesystem::path temp_path,
			     std::filesystem::path unpacked_path,
			     std::string archaddr)
{
  AuxFunc af;
  poppler::document *doc = poppler::document::load_from_file(
      temp_path.string());
  if(doc)
    {
      poppler::ustring subj = doc->get_subject();
      std::vector<char> buf = subj.to_utf8();
      std::copy(buf.begin(), buf.end(), std::back_inserter(file));
      std::vector<std::tuple<int, int, std::string>> listv;
      std::filesystem::path ch_zip = std::filesystem::u8path(archaddr);
      if(ch_zip.extension().u8string() == ".zip")
	{
	  af.fileNames(archaddr, listv);
	}
      else
	{
	  af.fileNamesNonZip(archaddr, listv);
	}
      auto itlv = std::find_if(listv.begin(), listv.end(), [temp_path]
      (auto &el)
	{
	  std::filesystem::path lp = std::filesystem::u8path(std::get<2>(el));
	  if(lp.stem().u8string() == temp_path.stem().u8string() &&
	      lp.extension().u8string() == ".fbd")
	    {
	      return true;
	    }
	  else
	    {
	      return false;
	    }
	});
      if(itlv != listv.end())
	{
	  file = af.unpackByIndex(archaddr, std::get<0>(*itlv),
				  std::get<1>(*itlv));
	  pdf_ch_f = false;
	  fb2_ch_f = true;
	}
      else
	{
	  poppler::page *pg = doc->create_page(0);
	  poppler::page_renderer pr;
	  pr.set_image_format(poppler::image::format_rgb24);
	  poppler::image img = pr.render_page(pg, 100.0, 100.0, -1, -1, -1, -1,
					      poppler::rotate_0);
	  pdf_result.image.resize(img.bytes_per_row() * img.height());
	  std::memcpy(pdf_result.image.data(), img.const_data(),
		      pdf_result.image.size());
	  pdf_result.format = "rgb";
	  pdf_result.rowsize = img.bytes_per_row();
	  delete pg;
	}
    }
  delete doc;
  std::filesystem::remove_all(unpacked_path);
}

void
AnnotationCover::djvuParse(std::filesystem::path filepath)
{
  std::string progrnm = "MyLibrary";
  ddjvu_context_t *djvu = ddjvu_context_create(progrnm.c_str());
  if(djvu)
    {
      handle_ddjvu_messages(djvu, false);
      ddjvu_document_t *doc = ddjvu_document_create_by_filename_utf8(
	  djvu, filepath.u8string().c_str(), true);
      if(doc)
	{
	  handle_ddjvu_messages(djvu, true);
	  ddjvu_page_t *page = ddjvu_page_create_by_pageno(doc, 0);
	  if(page)
	    {
	      while(!ddjvu_page_decoding_done(page))
		{
		  handle_ddjvu_messages(djvu, true);
		}

	      int iw = ddjvu_page_get_width(page);
	      int ih = ddjvu_page_get_height(page);

	      ddjvu_rect_t prect;
	      ddjvu_rect_t rrect;
	      prect.x = 0;
	      prect.y = 0;
	      prect.w = iw;
	      prect.h = ih;
	      rrect = prect;

	      ddjvu_format_t *fmt = nullptr;
	      unsigned int bitmask[4];
	      bitmask[0] = 255;
	      bitmask[1] = 65280;
	      bitmask[2] = 16711680;
	      bitmask[3] = 4278190080;
	      fmt = ddjvu_format_create(DDJVU_FORMAT_RGBMASK32, 4, bitmask);
	      if(fmt)
		{
		  ddjvu_format_set_row_order(fmt, 1);
		  int rowsize = rrect.w * 4;
		  std::vector<char> djvu_cover_bufer;
		  djvu_cover_bufer.resize(rowsize * rrect.h);
		  if(!ddjvu_page_render(page, DDJVU_RENDER_COLOR, &prect,
					&rrect, fmt, rowsize,
					&djvu_cover_bufer[0]))
		    {
		      std::cerr
			  << "AnnotationCover::djvuParse: DJVU render error"
			  << std::endl;
		      djvu_cover_bufer.clear();
		    }
		  else
		    {
		      djvu_result.image = djvu_cover_bufer;
		      djvu_result.format = "rgba";
		      djvu_result.rowsize = rowsize;
		    }
		  ddjvu_format_release(fmt);
		}
	      else
		{
		  std::cerr << "AnnotationCover::djvuParse: incorrect format"
		      << std::endl;
		}

	      ddjvu_page_release(page);
	    }
	  else
	    {
	      std::cerr << "AnnotationCover::djvuParse: cover page not created"
		  << std::endl;
	    }
	  ddjvu_document_release(doc);
	}
      ddjvu_context_release(djvu);
    }
}

void
AnnotationCover::handle_ddjvu_messages(ddjvu_context_t *ctx, bool wait)
{
  ddjvu_message_t *msg = nullptr;
  if(wait)
    {
      msg = ddjvu_message_wait(ctx);
    }
  else
    {
      msg = ddjvu_message_peek(ctx);
    }
  while(msg)
    {
      const char *str = msg->m_error.message;
      if(str && msg->m_any.tag == DDJVU_ERROR)
	{
	  std::cerr << std::string(str) << std::endl;
	}
      ddjvu_message_pop(ctx);
      msg = ddjvu_message_peek(ctx);
    }
}
