## RagePhoto.Core
Open Source RAGE Photo Parser for GTA V and RDR 2

- Read/Write RAGE Photos error free and correct
- Support for metadata stored in RAGE Photos
- Simple .NET API
- Based on libragephoto

#### How to Use RagePhoto.Core

```c#
using RagePhoto;

/* Get Image from Photo */
static Image GetImageFromPhoto(String inputFile) {
    using Photo photo = new();
    photo.LoadFile(inputFile);
    using MemoryStream jpegStream = new(photo.Jpeg);
    return Image.FromStream(jpegStream);
}
```
