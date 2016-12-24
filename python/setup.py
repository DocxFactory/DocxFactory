
from distutils.core import setup, Extension

import platform

ext_module = Extension(
	"_docxfactory",
	sources			= ["DocxFactory_wrap.cxx", "../src/WordProcessingCompiler.cpp", "../src/WordProcessingMerger.cpp"],
	include_dirs	= ["../include"],
	library_dirs	= ["../lib"],
	libraries		= ["DocxFactory"])

setup(
	name			= "DocxFactory",
	version			= "1",
	url				= "docxfactory.com",
	author			= "Alon Blich",
	author_email	= "alonb@docxfactory.com",
	description		= "DocxFactory Python Interface",
	ext_modules		= [ext_module],
	py_modules		= ["docxfactory"])
