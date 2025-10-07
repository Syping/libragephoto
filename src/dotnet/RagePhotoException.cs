using System;

namespace Syping.RagePhoto {

    public class RagePhotoException : Exception {

        private readonly Photo _photo;

        public RagePhotoException(Photo photo, String message) : base(message) {
            _photo = photo;
        }

        public RagePhotoException(Photo photo, String message, Exception innerException) : base(message, innerException) {
            _photo = photo;
        }
    }
}
