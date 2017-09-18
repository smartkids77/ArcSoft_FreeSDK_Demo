using System;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Drawing.Imaging;

namespace arcsoft {
    namespace utils {
        public class ImageLoader {
            public static BufferInfo getI420FromFile(String filePath) {
                byte[] yuv = null;
                int w = 0;
                int h = 0;
                {
                    Bitmap img = new Bitmap(filePath);
                    if (((img.Width & 0x1) != 0) || ((img.Height & 0x1) != 0)) {
                        img = img.Clone(new RectangleF(0, 0, img.Width & 0xFFFFFFFE, img.Height & 0xFFFFFFFE), PixelFormat.Format32bppRgb);
                    }
                    w = img.Width;
                    h = img.Height;


                    int[] bgra = new int[img.Width * img.Height];
                    Rectangle rect = new Rectangle(0, 0, img.Width, img.Height);
                    BitmapData bmpData = img.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadOnly, PixelFormat.Format32bppRgb);
                    Marshal.Copy(bmpData.Scan0, bgra, 0, img.Width * img.Height );
                    img.UnlockBits(bmpData);

                    yuv = BGRA2I420(bgra, w, h);
                }
                return new BufferInfo(w, h, yuv);
            }

            public static byte[] BGRA2I420(int[] bgra, int width, int height) {

                byte[] yuv = new byte[width * height * 3 / 2];
                int u_offset = width * height;
                int y_offset = width * height * 5 / 4;

                for (int i = 0; i < height; i++) {
                    for (int j = 0; j < width; j++) {
                        int rgb = bgra[i * width + j] & 0x00FFFFFF;
                        int b = rgb & 0xFF;
                        int g = (rgb >> 8) & 0xFF;
                        int r = (rgb >> 16) & 0xFF;

                        int y = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
                        int u = ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
                        int v = ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;

                        y = y < 0 ? 0 : (y > 255 ? 255 : y);
                        u = u < 0 ? 0 : (u > 255 ? 255 : u);
                        v = v < 0 ? 0 : (v > 255 ? 255 : v);

                        yuv[i * width + j] = (byte)y;
                        yuv[u_offset + (i >> 1) * (width >> 1) + (j >> 1)] = (byte)u;
                        yuv[y_offset + (i >> 1) * (width >> 1) + (j >> 1)] = (byte)v;
                    }
                }
                return yuv;
            }
        }
    }
}
