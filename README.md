# MyLibrary

## Description

MyLibrary is a simple program for managing `.fb2`, `.epub`, `.pdf` and `djvu` e-book file collections. It can also works with same formats packed in zip, 7z, jar, cpio, iso, a, ar, tar, tgz, tar.gz, tar.bz2, tar.xz, rar (see notes) archives itself or packed in same types of  archives with `.fbd` files (epub, djvu and pdf books). MyLibrary creates own database and does not change files content, names or location.

## Installation

### Linux

`git clone https://github.com/ProfessorNavigator/mylibrary.git`\
`cd mylibrary`\
`meson setup -Dbuildtype=release build`\
`ninja -C build install`

You may need superuser privileges to execute last command.

### Windows

You can build MyLibrary from source by MSYS2 project [https://www.msys2.org/](https://www.msys2.org/). Follow installation instructions from projects site. Install dependencies from `Dependencies` section and git (mingw packages). Than create directory where you want to download source code (path must not include spaces or non ASCII symbols). Open mingw console and execute following commands (in example we download code to C:\MyLibrary)\

`cd /c/MyLibrary`\
`git clone https://github.com/ProfessorNavigator/mylibrary.git`\
`cd mylibrary`\
`meson setup -Dbuildtype=release build`\
`ninja -C build install`

If everything was correct, you can find mylibrary.exe file in `msys_dir/mingw_dir/bin`. Icon to create desktop shortcut can be found in `msys_dir/mingw_dir/share/icons/hicolor/256x256/apps/mylibrary.ico` path.

Experimental installer is available now (see releases). 

## Dependencies

MyLibrary uses meson building system, so to build it from source you need meson and ninja.\
Also you need [gtkmm-4.0](http://www.gtkmm.org/), [poppler](https://poppler.freedesktop.org/), [DjVuLibre](https://djvu.sourceforge.net/), [libzip](https://libzip.org/), [libarchive](https://libarchive.org/), [icu](https://icu.unicode.org/) (version >= 69), [libgcrypt](https://www.gnupg.org/software/libgcrypt/) and [libgpg-error](https://www.gnupg.org/software/libgpg-error/). All libraries must have headers (for building), so if you use for example Debian Linux, you need ...-dev versions of packages.

## Usage

It is simple. Just create collection (see proper menu item), search book and open it (right mouse click on book). Book will be opened in default system file reader proper to each type of file. Also you can create book-marks (right mouse click on book) and read it later. Book can be removed from collection, added to collection or copied to any path you want. You can refresh collection, remove it, export or import collection database. Also you can manually edit database entries.

## Notes about archives usage
1. rar archives are supported partially. This means, that you can carry out all operations, except book removing. In case of rar archive you can remove whole archive, but not separate book from it.
2. Archives, packed in another archives, are not supported. This means, that MyLibrary does not parse for example zip archive packed in another zip archive. Only exception is epub format (epub books are zip archives itself).

## License

GPLv3 (see `COPYING`).

## Donation

If you want to help to develop this project, you can assist it by [donation](https://yoomoney.ru/to/4100117795409573)

## Contacts

You can contact author by email \
bobilev_yury@mail.ru