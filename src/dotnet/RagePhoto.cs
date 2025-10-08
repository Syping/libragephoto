using System;
using System.Runtime.InteropServices;

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
            UIntPtr size = (UIntPtr)data.LongLength;
            bool result = ragephoto_load(_instance, data, size);
            if (!result)
                throw new RagePhotoException(this, string.Format("Failed to load Photo: {0}", Error.ToString()));
        }

        public void LoadFile(String path) {
            bool result = ragephoto_loadfile(_instance, path);
            if (!result)
                throw new RagePhotoException(this, string.Format("Failed to load Photo: {0}", Error.ToString()));
        }

        public RagePhotoData Data {
            get => Marshal.PtrToStructure<RagePhotoData>(ragephoto_getphotodata(_instance));
        }

        public String Description {
            get {
                IntPtr descPtr = ragephoto_getphotodesc(_instance);
                return descPtr == IntPtr.Zero ? string.Empty : Marshal.PtrToStringUTF8(descPtr);
            }
            set => ragephoto_setphotodesc(_instance, value, 0);
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
                IntPtr jpegPtr = ragephoto_getphotojpeg(_instance);
                byte[] jpeg = new Byte[size];
                Marshal.Copy(jpegPtr, jpeg, 0, (Int32)size);
                return jpeg;
            }
            set {
                UInt32 bufferSize;
                UInt32 size = (UInt32)value.Length;
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
                bool result = ragephoto_setphotojpeg(_instance, value, size, bufferSize);
                if (!result)
                    throw new RagePhotoException(this, string.Format("Failed to set Jpeg: {0}", Error.ToString()));
            }
        }

        public UInt32 JpegSize {
            get => ragephoto_getphotosize(_instance);
        }

        public String Json {
            get {
                IntPtr jsonPtr = ragephoto_getphotojson(_instance);
                return jsonPtr == IntPtr.Zero ? string.Empty : Marshal.PtrToStringUTF8(jsonPtr);
            }
            set => ragephoto_setphotojson(_instance, value, 0);
        }

        public String Header {
            get {
                IntPtr headerPtr = ragephoto_getphotoheader(_instance);
                return headerPtr == IntPtr.Zero ? string.Empty : Marshal.PtrToStringUTF8(headerPtr);
            }
        }

        public UIntPtr SaveSize {
            get => ragephoto_getsavesize(_instance);
        }

        public UInt64 Sign {
            get => ragephoto_getphotosign(_instance);
        }

        public String Title {
            get {
                IntPtr titlePtr = ragephoto_getphototitle(_instance);
                return titlePtr == IntPtr.Zero ? string.Empty : Marshal.PtrToStringUTF8(titlePtr);
            }
            set => ragephoto_setphototitle(_instance, value, 0);
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
                throw new RagePhotoException(this, string.Format("Failed to save Photo: {0}", Error.ToString()));
            return photo;
        }

        public Byte[] Save(PhotoFormat photoFormat) {
            Byte[] photo = new Byte[Environment.Is64BitProcess ? (UInt64)GetSaveSize(photoFormat) : (UInt32)GetSaveSize(photoFormat)];
            bool result = ragephoto_savef(_instance, photo, (UInt32)photoFormat);
            if (!result)
                throw new RagePhotoException(this, string.Format("Failed to save Photo: {0}", Error.ToString()));
            return photo;
        }

        public void SaveFile(String path) {
            bool result = ragephoto_savefile(_instance, path);
            if (!result)
                throw new RagePhotoException(this, string.Format("Failed to save Photo: {0}", Error.ToString()));
        }

        public void SaveFile(String path, PhotoFormat photoFormat) {
            bool result = ragephoto_savefilef(_instance, path, (UInt32)photoFormat);
            if (!result)
                throw new RagePhotoException(this, string.Format("Failed to save Photo: {0}", Error.ToString()));
        }

        public void SetBufferDefault() {
            ragephoto_setbufferdefault(_instance);
        }

        public void SetBufferOffsets() {
            ragephoto_setbufferoffsets(_instance);
        }

        public void SetHeader(String header, UInt32 headerSum, UInt32 headerSum2 = 0) {
            ragephoto_setphotoheader2(_instance, header, headerSum, headerSum2);
        }

        public bool SetJpeg(Byte[] jpeg, UInt32 bufferSize = 0) {
            UInt32 size = (UInt32)jpeg.Length;
            return ragephoto_setphotojpeg(_instance, jpeg, size, bufferSize);
        }

        public static String Version {
            get {
                IntPtr versionPtr = ragephoto_version();
                return versionPtr == IntPtr.Zero ? string.Empty : Marshal.PtrToStringAnsi(versionPtr);
            }
        }
    }
}
