using System;
using System.Runtime.InteropServices;

namespace Syping.RagePhoto {
    [StructLayout(LayoutKind.Sequential)]
    public readonly struct RagePhotoData {
        readonly IntPtr jpeg;
        [MarshalAs(UnmanagedType.LPUTF8Str)]
        readonly String description;
        [MarshalAs(UnmanagedType.LPUTF8Str)]
        readonly String json;
        [MarshalAs(UnmanagedType.LPUTF8Str)]
        readonly String header;
        [MarshalAs(UnmanagedType.LPUTF8Str)]
        readonly String title;
        readonly Int32 error;
        readonly UInt32 descBuffer;
        readonly UInt32 descOffset;
        readonly UInt32 endOfFile;
        readonly UInt32 headerSum;
        readonly UInt32 headerSum2;
        readonly UInt32 jpegBuffer;
        readonly UInt32 jpegSize;
        readonly UInt32 jsonBuffer;
        readonly UInt32 jsonOffset;
        readonly UInt32 photoFormat;
        readonly UInt32 titlBuffer;
        readonly UInt32 titlOffset;
    }
}
