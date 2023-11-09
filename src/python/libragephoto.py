##############################################################################
# libragephoto for Python
# Copyright (C) 2023 Syping
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# This software is provided as-is, no warranties are given to you, we are not
# responsible for anything with use of the software, you are self responsible.
##############################################################################

from ctypes import *
from ctypes.util import find_library
from pathlib import Path
from platform import system

if system() == "Windows":
  bundle_library_path = Path(__file__).parent.resolve() / "libragephoto.dll"
  if bundle_library_path.is_file():
    library_path = str(bundle_library_path)
else:
  bundle_library_path = Path(__file__).parent.resolve() / "libragephoto.so"
  if bundle_library_path.is_file():
    library_path = str(bundle_library_path)
  else:
    library_path = find_library("ragephoto")

if not library_path:
  raise ImportError("libragephoto is required.")

libragephoto = cdll.LoadLibrary(library_path)
libragephoto.ragephoto_open.restype = c_void_p
libragephoto.ragephoto_clear.argtypes = [c_void_p]
libragephoto.ragephoto_close.argtypes = [c_void_p]
libragephoto.ragephoto_load.argtypes = [c_void_p, POINTER(c_char), c_size_t]
libragephoto.ragephoto_load.restype = c_bool
libragephoto.ragephoto_loadfile.argtypes = [c_void_p, c_char_p]
libragephoto.ragephoto_loadfile.restype = c_bool
libragephoto.ragephoto_error.argtypes = [c_void_p]
libragephoto.ragephoto_error.restype = c_int32
libragephoto.ragephoto_getphotodesc.argtypes = [c_void_p]
libragephoto.ragephoto_getphotodesc.restype = c_char_p
libragephoto.ragephoto_getphotoformat.argtypes = [c_void_p]
libragephoto.ragephoto_getphotoformat.restype = c_uint32
libragephoto.ragephoto_getphotojpeg.argtypes = [c_void_p]
libragephoto.ragephoto_getphotojpeg.restype = POINTER(c_char)
libragephoto.ragephoto_getphotojson.argtypes = [c_void_p]
libragephoto.ragephoto_getphotojson.restype = c_char_p
libragephoto.ragephoto_getphotoheader.argtypes = [c_void_p]
libragephoto.ragephoto_getphotoheader.restype = c_char_p
libragephoto.ragephoto_getphotosign.argtypes = [c_void_p]
libragephoto.ragephoto_getphotosign.restype = c_uint64
libragephoto.ragephoto_getphotosignf.argtypes = [c_void_p, c_uint32]
libragephoto.ragephoto_getphotosignf.restype = c_uint64
libragephoto.ragephoto_getphotosize.argtypes = [c_void_p]
libragephoto.ragephoto_getphotosize.restype = c_uint32
libragephoto.ragephoto_getphototitle.argtypes = [c_void_p]
libragephoto.ragephoto_getphototitle.restype = c_char_p
libragephoto.ragephoto_getsavesize.argtypes = [c_void_p]
libragephoto.ragephoto_getsavesize.restype = c_size_t
libragephoto.ragephoto_getsavesizef.argtypes = [c_void_p, c_uint32]
libragephoto.ragephoto_getsavesizef.restype = c_size_t
libragephoto.ragephoto_save.argtypes = [c_void_p, POINTER(c_char)]
libragephoto.ragephoto_save.restype = c_bool
libragephoto.ragephoto_savef.argtypes = [c_void_p, POINTER(c_char), c_uint32]
libragephoto.ragephoto_savef.restype = c_bool
libragephoto.ragephoto_savefile.argtypes = [c_void_p, c_char_p]
libragephoto.ragephoto_savefile.restype = c_bool
libragephoto.ragephoto_savefilef.argtypes = [c_void_p, c_char_p, c_uint32]
libragephoto.ragephoto_savefilef.restype = c_bool
libragephoto.ragephoto_setbufferdefault.argtypes = [c_void_p]
libragephoto.ragephoto_setbufferoffsets.argtypes = [c_void_p]
libragephoto.ragephoto_setphotodesc.argtypes = [c_void_p, c_char_p, c_uint32]
libragephoto.ragephoto_setphotoformat.argtypes = [c_void_p, c_uint32]
libragephoto.ragephoto_setphotojpeg.argtypes = [c_void_p, POINTER(c_char), c_uint32, c_uint32]
libragephoto.ragephoto_setphotojpeg.restype = c_bool
libragephoto.ragephoto_setphotojson.argtypes = [c_void_p, c_char_p, c_uint32]
libragephoto.ragephoto_setphotoheader.argtypes = [c_void_p, c_char_p, c_uint32]
libragephoto.ragephoto_setphotoheader2.argtypes = [c_void_p, c_char_p, c_uint32, c_uint32]
libragephoto.ragephoto_setphototitle.argtypes = [c_void_p, c_char_p, c_uint32]
libragephoto.ragephoto_version.restype = c_char_p
