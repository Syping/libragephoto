using System;
using System.Runtime.InteropServices;
using System.Text;

namespace Syping.RagePhoto {

    public class Photo : IDisposable {

        private bool _disposed;
        private readonly IntPtr _instance;
        private const String _library = "libragephoto";

        [DllImport(_library)]
        private static extern IntPtr ragephoto_open();
        [DllImport(_library)]
        private static extern void ragephoto_clear(IntPtr instance);
        [DllImport(_library)]
        private static extern void ragephoto_close(IntPtr instance);
        [DllImport(_library)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool ragephoto_load(IntPtr instance, Byte[] data, UIntPtr size);
        [DllImport(_library)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool ragephoto_loadfile(IntPtr instance, [MarshalAs(UnmanagedType.LPUTF8Str)] String filename);
        [DllImport(_library)]
        private static extern Int32 ragephoto_error(IntPtr instance);
        [DllImport(_library)]
        private static extern IntPtr ragephoto_getphotodata(IntPtr instance);
        [DllImport(_library)]
        private static extern IntPtr ragephoto_getphotodesc(IntPtr instance);
        [DllImport(_library)]
        private static extern UInt32 ragephoto_getphotoformat(IntPtr instance);
        [DllImport(_library)]
        private static extern IntPtr ragephoto_getphotojpeg(IntPtr instance);
        [DllImport(_library)]
        private static extern IntPtr ragephoto_getphotojson(IntPtr instance);
        [DllImport(_library)]
        private static extern IntPtr ragephoto_getphotoheader(IntPtr instance);
        [DllImport(_library)]
        private static extern UInt64 ragephoto_getphotosign(IntPtr instance);
        [DllImport(_library)]
        private static extern UInt32 ragephoto_getphotosize(IntPtr instance);
        [DllImport(_library)]
        private static extern IntPtr ragephoto_getphototitle(IntPtr instance);
        [DllImport(_library)]
        private static extern UIntPtr ragephoto_getsavesize(IntPtr instance);
        [DllImport(_library)]
        private static extern UIntPtr ragephoto_getsavesizef(IntPtr instance, UInt32 photoFormat);
        [DllImport(_library)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool ragephoto_save(IntPtr instance, [Out] Byte[] data);
        [DllImport(_library)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool ragephoto_savef(IntPtr instance, [Out] Byte[] data, UInt32 photoFormat);
        [DllImport(_library)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool ragephoto_savefile(IntPtr instance, [MarshalAs(UnmanagedType.LPUTF8Str)] String filename);
        [DllImport(_library)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool ragephoto_savefilef(IntPtr instance, [MarshalAs(UnmanagedType.LPUTF8Str)] String filename, UInt32 photoFormat);
        [DllImport(_library)]
        private static extern void ragephoto_setbufferdefault(IntPtr instance);
        [DllImport(_library)]
        private static extern void ragephoto_setbufferoffsets(IntPtr instance);
        [DllImport(_library)]
        private static extern void ragephoto_setphotodesc(IntPtr instance, [MarshalAs(UnmanagedType.LPUTF8Str)] String description, UInt32 bufferSize);
        [DllImport(_library)]
        private static extern void ragephoto_setphotoformat(IntPtr instance, UInt32 photoFormat);
        [DllImport(_library)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool ragephoto_setphotojpeg(IntPtr instance, Byte[] jpeg, UInt32 size, UInt32 bufferSize);
        [DllImport(_library)]
        private static extern void ragephoto_setphotojson(IntPtr instance, [MarshalAs(UnmanagedType.LPUTF8Str)] String json, UInt32 bufferSize);
        [DllImport(_library)]
        private static extern void ragephoto_setphotoheader2(IntPtr instance, [MarshalAs(UnmanagedType.LPUTF8Str)] String header, UInt32 headerSum, UInt32 headerSum2);
        [DllImport(_library)]
        private static extern void ragephoto_setphototitle(IntPtr instance, [MarshalAs(UnmanagedType.LPUTF8Str)] String title, UInt32 bufferSize);
        [DllImport(_library)]
        private static extern IntPtr ragephoto_version();

        public Photo() {
            try {
                _instance = ragephoto_open();
                if (_instance == IntPtr.Zero)
                    throw new RagePhotoException(this, "Failed to initialize libragephoto");
            }
            catch (Exception exception) {
                throw new RagePhotoException(this, "Failed to initialize libragephoto", exception);
            }
        }

        ~Photo() {
            Dispose(false);
        }

        public void Dispose() {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing) {
            if (_disposed)
                return;
            ragephoto_close(_instance);
            _disposed = true;
        }

        public void Clear() {
            ragephoto_clear(_instance);
        }

        public void Load(Byte[] data) {
            bool result = ragephoto_load(_instance, data, (UIntPtr)data.LongLength);
            if (!result)
                throw new RagePhotoException(this, string.Format("Failed to load Photo: {0}", Error));
        }

        public void LoadFile(String path) {
            bool result = ragephoto_loadfile(_instance, path);
            if (!result)
                throw new RagePhotoException(this, string.Format("Failed to load Photo: {0}", Error));
        }

        public RagePhotoData Data {
            get => Marshal.PtrToStructure<RagePhotoData>(ragephoto_getphotodata(_instance));
        }

        public String Description {
            get => PtrToStringUTF8(ragephoto_getphotodesc(_instance));
            set => ragephoto_setphotodesc(_instance, value, (UInt32)DefaultSize.DEFAULT_DESCBUFFER);
        }

        public PhotoError Error {
            get => (PhotoError)ragephoto_error(_instance);
        }

        public PhotoFormat Format {
            get => (PhotoFormat)ragephoto_getphotoformat(_instance);
            set => ragephoto_setphotoformat(_instance, (UInt32)value);
        }

        public Byte[] Jpeg {
            get {
                UInt32 size = ragephoto_getphotosize(_instance);
                if (size == 0)
                    return Array.Empty<Byte>();
                byte[] buffer = new Byte[size];
                IntPtr ptr = ragephoto_getphotojpeg(_instance);
                Marshal.Copy(ptr, buffer, 0, (Int32)size);
                return buffer;
            }
            set => SetJpeg(value);
        }

        public UInt32 JpegSize {
            get => ragephoto_getphotosize(_instance);
        }

        public String Json {
            get => PtrToStringUTF8(ragephoto_getphotojson(_instance));
            set => ragephoto_setphotojson(_instance, value, (UInt32)DefaultSize.DEFAULT_JSONBUFFER);
        }

        public String Header {
            get => PtrToStringUTF8(ragephoto_getphotoheader(_instance));
        }

        public UIntPtr SaveSize {
            get => ragephoto_getsavesize(_instance);
        }

        public UInt64 Sign {
            get => ragephoto_getphotosign(_instance);
        }

        public String Title {
            get => PtrToStringUTF8(ragephoto_getphototitle(_instance));
            set => ragephoto_setphototitle(_instance, value, (UInt32)DefaultSize.DEFAULT_TITLBUFFER);
        }

        public UIntPtr GetSaveSize() {
            return ragephoto_getsavesize(_instance);
        }

        public UIntPtr GetSaveSize(PhotoFormat photoFormat) {
            return ragephoto_getsavesizef(_instance, (UInt32)photoFormat);
        }

        public Byte[] Save() {
            Byte[] photo = new Byte[Environment.Is64BitProcess ? (UInt64)GetSaveSize() : (UInt32)GetSaveSize()];
            bool result = ragephoto_save(_instance, photo);
            if (!result)
                throw new RagePhotoException(this, string.Format("Failed to save Photo: {0}", Error));
            return photo;
        }

        public Byte[] Save(PhotoFormat photoFormat) {
            Byte[] photo = new Byte[Environment.Is64BitProcess ? (UInt64)GetSaveSize(photoFormat) : (UInt32)GetSaveSize(photoFormat)];
            bool result = ragephoto_savef(_instance, photo, (UInt32)photoFormat);
            if (!result)
                throw new RagePhotoException(this, string.Format("Failed to save Photo: {0}", Error));
            return photo;
        }

        public void SaveFile(String path) {
            bool result = ragephoto_savefile(_instance, path);
            if (!result)
                throw new RagePhotoException(this, string.Format("Failed to save Photo: {0}", Error));
        }

        public void SaveFile(String path, PhotoFormat photoFormat) {
            bool result = ragephoto_savefilef(_instance, path, (UInt32)photoFormat);
            if (!result)
                throw new RagePhotoException(this, string.Format("Failed to save Photo: {0}", Error));
        }

        public void SetBufferDefault() {
            ragephoto_setbufferdefault(_instance);
        }

        public void SetBufferOffsets() {
            ragephoto_setbufferoffsets(_instance);
        }

        public void SetDescription(String description) {
            ragephoto_setphotodesc(_instance, description, (UInt32)DefaultSize.DEFAULT_DESCBUFFER);
        }

        public void SetDescription(String description, UInt32 bufferSize) {
            ragephoto_setphotodesc(_instance, description, bufferSize);
        }

        public void SetHeader(String header, UInt32 headerSum, UInt32 headerSum2 = 0) {
            ragephoto_setphotoheader2(_instance, header, headerSum, headerSum2);
        }

        public void SetJpeg(Byte[] jpeg) {
            UInt32 bufferSize;
            UInt32 size = (UInt32)jpeg.Length;
            switch (Format) {
                case PhotoFormat.GTA5:
                    if (size > (UInt32)DefaultSize.DEFAULT_GTA5_PHOTOBUFFER)
                        bufferSize = size;
                    else
                        bufferSize = (UInt32)DefaultSize.DEFAULT_GTA5_PHOTOBUFFER;
                    break;
                case PhotoFormat.RDR2:
                    if (size > (UInt32)DefaultSize.DEFAULT_RDR2_PHOTOBUFFER)
                        bufferSize = size;
                    else
                        bufferSize = (UInt32)DefaultSize.DEFAULT_RDR2_PHOTOBUFFER;
                    break;
                default:
                    bufferSize = size;
                    break;
            }
            bool result = ragephoto_setphotojpeg(_instance, jpeg, size, bufferSize);
            if (!result)
                throw new RagePhotoException(this, string.Format("Failed to set Jpeg: {0}", Error));
        }

        public void SetJpeg(Byte[] jpeg, UInt32 bufferSize) {
            bool result = ragephoto_setphotojpeg(_instance, jpeg, (UInt32)jpeg.Length, bufferSize);
            if (!result)
                throw new RagePhotoException(this, string.Format("Failed to set Jpeg: {0}", Error));
        }

        public void SetJson(String json) {
            ragephoto_setphotojson(_instance, json, (UInt32)DefaultSize.DEFAULT_JSONBUFFER);
        }

        public void SetJson(String json, UInt32 bufferSize) {
            ragephoto_setphotojson(_instance, json, bufferSize);
        }

        public void SetTitle(String title) {
            ragephoto_setphototitle(_instance, title, (UInt32)DefaultSize.DEFAULT_TITLBUFFER);
        }

        public void SetTitle(String title, UInt32 bufferSize) {
            ragephoto_setphototitle(_instance, title, bufferSize);
        }

        public static String Version {
            get => PtrToStringAnsi(ragephoto_version());
        }

        private static String PtrToStringAnsi(IntPtr ptr) {
            if (ptr == IntPtr.Zero)
                return string.Empty;
            return Marshal.PtrToStringAnsi(ptr);
        }

        private static String PtrToStringUTF8(IntPtr ptr) {
            if (ptr == IntPtr.Zero)
                return string.Empty;
#if NETSTANDARD2_1_OR_GREATER
            return Marshal.PtrToStringUTF8(ptr);
#else
            Int32 length = 0;
            while (Marshal.ReadByte(ptr, length) != 0)
                length++;
            byte[] buffer = new byte[length];
            Marshal.Copy(ptr, buffer, 0, length);
            return Encoding.UTF8.GetString(buffer);
#endif
        }
    }
}
