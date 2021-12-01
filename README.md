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
`-DRAGEPHOTO_C_API=OFF`  
`-DRAGEPHOTO_DOC=ON`  
`-DRAGEPHOTO_EXAMPLE_GTKVIEWER=ON`  
`-DRAGEPHOTO_EXAMPLE_QTVIEWER=ON`  
`-DRAGEPHOTO_EXTRACT=OFF`  
`-DRAGEPHOTO_STATIC=ON`

#### RagePhoto API

[RagePhoto C API](https://libragephoto.syping.de/doc/RagePhotoC_8h.html)  
[RagePhoto C++ API](https://libragephoto.syping.de/doc/classRagePhoto.html)

#### How to Use ragephoto-extract

```bash
ragephoto-extract PGTA5123456789 photo.jpg
ragephoto-extract PRDR3123456789 photo.jpg
```
