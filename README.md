## libragephoto
Open Source RAGE Photo Parser for GTA V and RDR 2

  - Read/Write RAGE Photos error free and correct
  - Support for metadata stored in RAGE Photos
  - Export RAGE Photos to jpeg with ragephoto-extract
  - High Efficient and Simple C/C++ API

#### Build libragephoto

```bash
git clone https://github.com/Syping/libragephoto
cmake -B libragephoto-build libragephoto
cmake --build libragephoto-build
sudo cmake --install libragephoto-build
```

##### Optional CMake flags
`-DRAGEPHOTO_CXX_STANDARD=17`  
`-DRAGEPHOTO_BENCHMARK=ON`  
`-DRAGEPHOTO_C_API=OFF`  
`-DRAGEPHOTO_C_LIBRARY=ON`  
`-DRAGEPHOTO_DOC=ON`  
`-DRAGEPHOTO_EXAMPLE_GTKVIEWER=ON`  
`-DRAGEPHOTO_EXAMPLE_QTVIEWER=ON`  
`-DRAGEPHOTO_EXTRACT=OFF`  
`-DRAGEPHOTO_STATIC=ON`

#### RagePhoto API

[RagePhoto C API](https://libragephoto.syping.de/doc/RagePhoto_8h.html)  
[RagePhoto C++ API](https://libragephoto.syping.de/doc/classRagePhoto.html)

#### libragephoto Documentation

[Main Page](https://libragephoto.syping.de/doc/)  
[Build libragephoto](https://libragephoto.syping.de/doc/Build.html)  
[Using libragephoto](https://libragephoto.syping.de/doc/Usage.html)

#### How to Use ragephoto-extract

```bash
ragephoto-extract PGTA5123456789 photo.jpg
ragephoto-extract PRDR3123456789 photo.jpg
```
