## libragephoto
Open Source RAGE Photo Parser for GTA V and RDR 2

- Read RAGE Photos error free and correct
- Support for metadata stored in RAGE Photos
- Export RAGE Photos to jpeg with ragephoto-extract
- High Efficient and Simple C++ API

#### Build libragephoto

```bash
git clone https://gitlab.com/Syping/libragephoto && cd libragephoto
mkdir -p build && cd build
cmake ../
make -j $(nproc --all)
sudo make install
```

##### Optional CMake flags
`-DWITH_EXTRACT=OFF` `-DWITH_GTK_EXAMPLE=ON` `-DWITH_QT_EXAMPLE=ON` `-DBUILD_SHARED=OFF`

#### How to Use libragephoto

```cpp
RagePhoto ragePhoto;
bool loaded = ragePhoto.load(data, size);
const char* photoData = ragePhoto.photoData();
uint32_t photoSize = ragePhoto.photoSize();
std::string json = ragePhoto.json();
std::string title = ragePhoto.title();
RagePhoto::Error error = ragePhoto.error();
```

#### How to Use ragephoto-extract

```bash
ragephoto-extract PGTA5123456789 photo.jpg
```
