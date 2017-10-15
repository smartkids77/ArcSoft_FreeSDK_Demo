using System;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
namespace arcsoft
{
    namespace utils
    {
        public class ImageLoader
        {
            public const bool USING_FLOAT = true;
            public static BufferInfo getI420FromFile(String filePath)
            {
                byte[] yuv = null;
                int w = 0;
                int h = 0;
                {
                    Bitmap img = new Bitmap(filePath);
                    if (((img.Width & 0x1) != 0) || ((img.Height & 0x1) != 0))
                    {
                        img = img.Clone(new RectangleF(0, 0, img.Width & 0xFFFFFFFE, img.Height & 0xFFFFFFFE), PixelFormat.Format32bppRgb);
                    }
                    w = img.Width;
                    h = img.Height;

                    int[] bgra = new int[img.Width * img.Height];
                    Rectangle rect = new Rectangle(0, 0, img.Width, img.Height);
                    BitmapData bmpData = img.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadOnly, PixelFormat.Format32bppRgb);
                    Marshal.Copy(bmpData.Scan0, bgra, 0, bgra.Length);
                    img.UnlockBits(bmpData);

                    if (USING_FLOAT)
                    {
                        yuv = BGRA2I420_float(bgra, w, h);
                    }
                    else
                    {
                        yuv = BGRA2I420(bgra, w, h);
                    }
                }
                return new BufferInfo(w, h, yuv);
            }

            public static BufferInfo getBGRFromFile(String filePath)
            {
                byte[] bgr = null;
                int width = 0;
                int height = 0;
                int stride = 0;
                {
                    Bitmap img = new Bitmap(filePath);
                    width = img.Width;
                    height = img.Height;

                    Rectangle rect = new Rectangle(0, 0, img.Width, img.Height);
                    BitmapData bmpData = img.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);
                    stride = bmpData.Stride;
                    bgr = new byte[stride * height];
                    Marshal.Copy(bmpData.Scan0, bgr, 0, bgr.Length);

                }
                return new BufferInfo(width, height, stride, bgr);
            }


            //Full swing for BT.601
            public static byte[] BGRA2I420(int[] bgra, int width, int height)
            {

                byte[] yuv = new byte[width * height * 3 / 2];
                int u_offset = width * height;
                int y_offset = width * height * 5 / 4;

                for (int i = 0; i < height; i++)
                {
                    for (int j = 0; j < width; j++)
                    {
                        int rgb = bgra[i * width + j] & 0x00FFFFFF;
                        int b = rgb & 0xFF;
                        int g = (rgb >> 8) & 0xFF;
                        int r = (rgb >> 16) & 0xFF;


                        int y = ((77 * r + 150 * g + 29 * b + 128) >> 8);
                        int u = (((-43) * r - 84 * g + 127 * b + 128) >> 8) + 128;
                        int v = ((127 * r - 106 * g - 21 * b + 128) >> 8) + 128;

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

            // ITU-R standard for YCbCr
            // Y =  0.299R + 0.587G + 0.114B
            // U = -0.169R - 0.331G + 0.499B + 128
            // V =  0.499R - 0.418G - 0.0813B + 128

            public static byte[] BGRA2I420_float(int[] bgra, int width, int height)
            {

                byte[] yuv = new byte[width * height * 3 / 2];
                int u_offset = width * height;
                int y_offset = width * height * 5 / 4;

                for (int i = 0; i < height; i++)
                {
                    for (int j = 0; j < width; j++)
                    {
                        int rgb = bgra[i * width + j] & 0x00FFFFFF;
                        float b = (rgb & 0xFF);
                        float g = ((rgb >> 8) & 0xFF);
                        float r = ((rgb >> 16) & 0xFF);

                        float y = (0.299f * r + 0.587f * g + 0.114f * b);
                        float u = (-0.169f) * r - 0.331f * g + 0.499f * b + 128.0f;
                        float v = 0.499f * r - 0.418f * g - 0.0813f * b + 128.0f;

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
