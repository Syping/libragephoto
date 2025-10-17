using System;

namespace Syping.RagePhoto {

    public class RagePhotoException : Exception {

        private readonly PhotoError _error;
        private readonly Photo _photo;

        public RagePhotoException(String message) : base(message) {
            _error = PhotoError.Uninitialised;
        }

        public RagePhotoException(String message, Exception innerException) : base(message, innerException) {
            _error = PhotoError.Uninitialised;
        }

        public RagePhotoException(Photo photo, String message, PhotoError error) : base(message) {
            _error = error;
            _photo = photo;
        }

        public PhotoError Error {
            get => _error;
        }

        public Photo Photo {
            get => _photo;
        }
    }
}
