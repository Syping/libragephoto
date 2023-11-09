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

from .libragephoto import *
from enum import IntEnum
from json import loads as parseJson
from json import dumps as serializeJson

class RagePhoto:
  class DefaultSize(IntEnum):
    DEFAULT_GTA5_PHOTOBUFFER = 524288
    DEFAULT_RDR2_PHOTOBUFFER = 1048576
    DEFAULT_DESCBUFFER = 256
    DEFAULT_JSONBUFFER = 3072
    DEFAULT_TITLBUFFER = 256

  class Error(IntEnum):
    DescBufferTight = 39
    DescMallocError = 31
    DescReadError = 32
    HeaderBufferTight = 35
    HeaderMallocError = 4
    IncompatibleFormat = 2
    IncompleteChecksum = 7
    IncompleteDescBuffer = 30
    IncompleteDescMarker = 28
    IncompleteDescOffset = 11
    IncompleteEOF = 8
    IncompleteHeader = 3
    IncompleteJendMarker = 33
    IncompleteJpegMarker = 12
    IncompleteJsonBuffer = 20
    IncompleteJsonMarker = 18
    IncompleteJsonOffset = 9
    IncompletePhotoBuffer = 14
    IncompletePhotoSize = 15
    IncompleteTitleBuffer = 25
    IncompleteTitleMarker = 23
    IncompleteTitleOffset = 10
    IncorrectDescMarker = 29
    IncorrectJendMarker = 34
    IncorrectJpegMarker = 13
    IncorrectJsonMarker = 19
    IncorrectTitleMarker = 24
    JsonBufferTight = 37
    JsonMallocError = 21
    JsonReadError = 22
    NoError = 255
    NoFormatIdentifier = 1
    PhotoBufferTight = 36
    PhotoMallocError = 16
    PhotoReadError = 17
    TitleBufferTight = 38
    TitleMallocError = 26
    TitleReadError = 27
    UnicodeInitError = 5
    UnicodeHeaderError = 6
    Uninitialised = 0

  class PhotoFormat(IntEnum):
    GTA5 = 0x01000000
    RDR2 = 0x04000000

  def __init__(self):
    self.__instance = libragephoto.ragephoto_open()

  def __del__(self):
    libragephoto.ragephoto_close(self.__instance)

  def clear(self):
    libragephoto.ragephoto_clear(self.__instance)

  def load(self, data):
    return libragephoto.ragephoto_load(self.__instance, data, len(data))

  def loadFile(self, file):
    if isinstance(file, str):
      return libragephoto.ragephoto_loadfile(self.__instance, file.encode())
    else:
      return libragephoto.ragephoto_loadfile(self.__instance, file)

  def error(self):
    return libragephoto.ragephoto_error(self.__instance)

  def description(self):
    _desc = libragephoto.ragephoto_getphotodesc(self.__instance)
    if _desc:
      return _desc
    else:
      return b""

  def format(self):
    return libragephoto.ragephoto_getphotoformat(self.__instance)

  def jpeg(self):
    _jpeg = libragephoto.ragephoto_getphotojpeg(self.__instance)
    if _jpeg:
      return _jpeg[:self.jpegSize()]
    else:
      return b""

  def jpegSign(self, format = None):
    if format is None:
      return libragephoto.ragephoto_getphotosign(self.__instance)
    else:
      return libragephoto.ragephoto_getphotosignf(self.__instance, format)

  def jpegSize(self):
    return libragephoto.ragephoto_getphotosize(self.__instance)

  def json(self):
    _json = libragephoto.ragephoto_getphotojson(self.__instance)
    if _json:
      return _json
    else:
      return b""

  def header(self):
    _header = libragephoto.ragephoto_getphotoheader(self.__instance)
    if _header:
      return _header
    else:
      return b""

  def save(self, format = None):
    _data = bytearray(self.saveSize(format))
    _ptr = (c_char * len(_data)).from_buffer(_data)
    if format is None:
      _ret = libragephoto.ragephoto_save(self.__instance, _ptr)
    else:
      _ret = libragephoto.ragephoto_savef(self.__instance, _ptr, format)
    if _ret:
      return _data
    else:
      return None

  def saveFile(self, file, format = None):
    if isinstance(file, str):
      _file = file.encode()
    else:
      _file = file
    if format is None:
      return libragephoto.ragephoto_savefile(self.__instance, _file)
    else:
      return libragephoto.ragephoto_savefilef(self.__instance, _file, format)

  def saveSize(self, format = None):
    if format is None:
      return libragephoto.ragephoto_getsavesize(self.__instance)
    else:
      return libragephoto.ragephoto_getsavesizef(self.__instance, format)

  def setBufferDefault(self):
    return libragephoto.ragephoto_setbufferdefault(self.__instance)

  def setBufferOffsets(self):
    return libragephoto.ragephoto_setbufferoffsets(self.__instance)

  def setDescription(self, desc, buffer = None):
    if isinstance(desc, str):
      _desc = desc.encode()
    else:
      _desc = desc
    if buffer is None:
      libragephoto.ragephoto_setphotodesc(self.__instance, _desc, self.DefaultSize.DEFAULT_DESCBUFFER)
    else:
      libragephoto.ragephoto_setphotodesc(self.__instance, _desc, buffer)

  def setFormat(self, format):
    libragephoto.ragephoto_setphotoformat(self.__instance, format)

  def setJpeg(self, jpeg, buffer = None):
    _buffer = 0
    if buffer is None:
      _format = self.format()
      if _format == self.PhotoFormat.GTA5:
        _buffer = self.DefaultSize.DEFAULT_GTA5_PHOTOBUFFER
      elif _format == self.PhotoFormat.RDR2:
        _buffer = self.DefaultSize.DEFAULT_RDR2_PHOTOBUFFER
    if _buffer < len(jpeg):
      _buffer = len(jpeg)
    return libragephoto.ragephoto_setphotojpeg(self.__instance, jpeg, len(jpeg), _buffer)

  def setJson(self, json, buffer = None):
    if isinstance(json, str):
      _json = json.encode()
    else:
      _json = json
    if buffer is None:
      libragephoto.ragephoto_setphotojson(self.__instance, _json, self.DefaultSize.DEFAULT_JSONBUFFER)
    else:
      libragephoto.ragephoto_setphotojson(self.__instance, _json, buffer)

  def setHeader(self, header, headerSum1, headerSum2 = 0):
    if isinstance(header, str):
      _header = header.encode()
    else:
      _header = header
    libragephoto.ragephoto_setphotoheader2(self.__instance, _header, headerSum1, headerSum2)

  def setTitle(self, title, buffer = None):
    if isinstance(title, str):
      _title = title.encode()
    else:
      _title = title
    if buffer is None:
      libragephoto.ragephoto_setphototitle(self.__instance, _title, self.DefaultSize.DEFAULT_TITLBUFFER)
    else:
      libragephoto.ragephoto_setphototitle(self.__instance, _title, buffer)

  def title(self):
    _title = libragephoto.ragephoto_getphototitle(self.__instance)
    if _title:
      return _title
    else:
      return b""

  def updateSign(self):
    try:
      _json = parseJson(self.json())
    except JSONDecodeError:
      return False
    _json["sign"] = self.jpegSign()
    self.setJson(serializeJson(_json, separators=(',', ':')))
    return True

  def version(self):
    return libragephoto.ragephoto_version()
