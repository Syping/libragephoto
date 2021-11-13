## libragephoto
Open Source RAGE Photo Parser for GTA V and RDR 2

- Read/Write RAGE Photos error free and correct
- Support for metadata stored in RAGE Photos
- Export RAGE Photos to jpeg with ragephoto-extract
- High Efficient and Simple C/C++ API

#### Build libragephoto

```bash
git clone https://github.com/Syping/libragephoto && cd libragephoto
mkdir -p build && cd build
cmake ../
make -j $(nproc --all)
sudo make install
```

##### Optional CMake flags
`-DWITH_DOCUMENTATION=ON`
`-DWITH_EXTRACT=OFF`
`-DWITH_GTK_EXAMPLE=ON`
`-DWITH_QT_EXAMPLE=ON`
`-DBUILD_SHARED=OFF`

#### How to Use libragephoto

```cpp
RagePhoto ragePhoto;
bool loaded = ragePhoto.load(data, size);
std::string photo = ragePhoto.photo();
const char* photoData = ragePhoto.photoData();
uint32_t photoSize = ragePhoto.photoSize();
std::string json = ragePhoto.json();
std::string title = ragePhoto.title();
RagePhoto::Error error = ragePhoto.error();
uint32_t format = ragePhoto.format(); // RagePhoto::GTA5 or RagePhoto::RDR2
```

[RagePhoto C API](https://libragephoto.syping.de/doc/_rage_photo_c_8h.html)  
[RagePhoto C++ API](https://libragephoto.syping.de/doc/class_rage_photo.html)

#### How to Use ragephoto-extract

```bash
ragephoto-extract PGTA5123456789 photo.jpg
ragephoto-extract PRDR3123456789 photo.jpg
```
