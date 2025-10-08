using System;

namespace Syping.RagePhoto {

    public enum DefaultSize : UInt32 {
        DEFAULT_GTA5_PHOTOBUFFER = 524288U,
        DEFAULT_RDR2_PHOTOBUFFER = 1048576U,
        DEFAULT_DESCBUFFER = 256U,
        DEFAULT_JSONBUFFER = 3072U,
        DEFAULT_TITLBUFFER = 256U,
        GTA5_HEADERSIZE = 264U,
        RDR2_HEADERSIZE = 272U
    }

    public enum PhotoError : Int32 {
        DescBufferTight = 39,
        DescMallocError = 31,
        DescReadError = 32,
        HeaderBufferTight = 35,
        HeaderMallocError = 4,
        IncompatibleFormat = 2,
        IncompleteChecksum = 7,
        IncompleteDescBuffer = 30,
        IncompleteDescMarker = 28,
        IncompleteDescOffset = 11,
        IncompleteEOF = 8,
        IncompleteHeader = 3,
        IncompleteJendMarker = 33,
        IncompleteJpegMarker = 12,
        IncompleteJsonBuffer = 20,
        IncompleteJsonMarker = 18,
        IncompleteJsonOffset = 9,
        IncompletePhotoBuffer = 14,
        IncompletePhotoSize = 15,
        IncompleteTitleBuffer = 25,
        IncompleteTitleMarker = 23,
        IncompleteTitleOffset = 10,
        IncorrectDescMarker = 29,
        IncorrectJendMarker = 34,
        IncorrectJpegMarker = 13,
        IncorrectJsonMarker = 19,
        IncorrectTitleMarker = 24,
        JsonBufferTight = 37,
        JsonMallocError = 21,
        JsonReadError = 22,
        NoError = 255,
        NoFormatIdentifier = 1,
        PhotoBufferTight = 36,
        PhotoMallocError = 16,
        PhotoReadError = 17,
        TitleBufferTight = 38,
        TitleMallocError = 26,
        TitleReadError = 27,
        UnicodeInitError = 5,
        UnicodeHeaderError = 6,
        Uninitialised = 0
    }

    public enum PhotoFormat : UInt32 {
        GTA5 = 0x01000000U,
        RDR2 = 0x04000000U
    }
}
