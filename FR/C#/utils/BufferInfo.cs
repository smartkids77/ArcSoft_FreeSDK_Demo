using arcsoft.utils;

namespace arcsoft {
    namespace utils {
        public class BufferInfo {
            public int width;
            public int height;
            public int stride;
            public byte[] buffer;

            public BufferInfo(int w, int h, byte[] buf) {
                width = w;
                height = h;
                stride = 0;
                buffer = buf;
            }

            public BufferInfo(int w, int h, int s, byte[] buf)
            {
                width = w;
                height = h;
                stride = s;
                buffer = buf;
            }
        }
    }
}
