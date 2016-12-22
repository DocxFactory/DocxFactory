DocxFactory Changelog
=====================


Version 1.2.18, on October 25, 2015
-----------------------------------
### Bug Fixes

* Fixed Linux 4 compatibility bug.
* Fixed Progress/OpenEdge  ZIP library on Linux bug.


Version 1.2.17, on August 16, 2015
----------------------------------
### Enhancements

* Upgraded the ImageMagick library to the latest version 6.9.1 on Linux.

### Bug Fixes

* Fixed Windows 10 compatibility bug.
* Fixed table borders bug.


Version 1.2.16, on July 27, 2015
--------------------------------

### Bug Fixes

* Fixed a bug that caused number fields to always be displayed in the American number fomat (e.g. 1,000.00) in the
  created document regardless of the thousand and fraction separator settings.


Version 1.2.15, on July 24, 2015
--------------------------------

### Bug Fixes

* Fixed a bug that caused DocxFactory to crash because of non-unique picture ID references.


Version 1.2.14, on July 21, 2015
--------------------------------

### Enhancements

* Added support and tutorial for C#.
* Added a chapter in the tutorials on table of contents automatic update.
* Added a note in the tutorials on automatic embedding of local images in HTML.
* Added an example in the tutorials on changing the codepage.
* Added support for customized styles and image backgrounds in charts.


Version 1.2.13, on June 31, 2015
--------------------------------

### Bug Fixes

* Fixed a bug that caused DocxFactory to crash when inserting HTML fields with <img src=”<file name>”/> for non-existing files.


Version 1.2.12, on June 27, 2015
--------------------------------

### Enhancements

* Local images (that are not file://,  http(s)://, ftp(s):// URI’s) in HTML fields are automatically embedded into the
  document as an MHTML file so the document can be sent out and the images can still be shown. Documentation coming soon.
* Added support for tab character (“\t”) in text fields.


Version 1.2.11, on June 23, 2015
--------------------------------

### Bug Fixes

* Temporarily reverted to ImageMagick 6.8.6 from 6.9.1 on Linux because the latest version had dependencies on deprecated
  libraries. The latest ImageMagick library will be recompiled soon without the dependencies on deprecated libraries.
* Fixed a bug in the Python wrapper that would cause the module import to fail.
* Fixed a bug that removed Watermarks from templates.


Version 1.2.10, on June 21, 2015
--------------------------------

### Enhancements

* Added table of contents automatic update. Documentation coming soon.

### Bug Fixes

* Fixed a bug in the Progress XLSX library on Windows that in some cases required administrator privileges to create .XLSX files.


Version 1.2.9, on June 13, 2015
-------------------------------

### Enhancements

* Added Java wrapper and tutorial.

### Bug Fixes

* Fixed a bug that caused Word to close and lose any unsaved work when printing or converting to .PDF or any other file
  format while Word was open.
* Fixed a bug that caused an unknown error message when opening a generated document because of duplicate <docPr> tag,
  id attribute values.


Version 1.2.8, on May 30, 2015
------------------------------

### Enhancements

* Upgraded the ImageMagick library to v6.9.1 to add support for .WMF image files.
* DocxFactory.h C include file was split into a dfwc.h include file for the Word Processing Compiler dfwc_* functions
  and a dfw.h include file for the Word Processing Merger dfw_* functions respectively.

### Bug Fixes

* Embedded the ImageMagick libraries (static libraries) into the DocxFactory.dll on Windows because the ImageMagick .dll’s (dynamic libraries) caused a mixup between the Progress and DocxFactory versions of ImageMagick.


Version 1.2.7, on May 19, 2015
------------------------------

### Bug Fixes

* Fixed a bug that caused the Python installation on Linux to fail.


Version 1.2.6, on May 17, 2015
------------------------------

### Enhancements

* Added tutorial for C.
* Updated the installation instruction in the C++ tutorial to match the changes in the DocxFactory library.

### Bug Fixes

* Fixed a bug that caused issues when mixing different runtime libraries in C/C++ and compatibility issues with Python
  version 2.* and Visual Studio 2008 by changing the DocxFactory library to not pass std objects.
