package com.arcsoft.utils;

import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;

import javax.imageio.ImageIO;
import java.io.File;
import java.io.IOException;

public class ImageLoader {

	public static BufferInfo getBGRFromFile(String filePath) {
		byte[] data = null;
        int w = 0;
        int h = 0;
        try {
            BufferedImage img = ImageIO.read(new File(filePath));
            if (((img.getWidth() & 0x1) != 0) || ((img.getHeight() & 0x1) != 0)) {
                img = img.getSubimage(0, 0, img.getWidth() & 0xFFFFFFFE, img.getHeight() & 0xFFFFFFFE);
            }
            w = img.getWidth();
            h = img.getHeight();
            BufferedImage bgrimg = new BufferedImage(img.getWidth(), img.getHeight(), BufferedImage.TYPE_3BYTE_BGR);
            bgrimg.setRGB(0, 0, w, h, img.getRGB(0, 0, w, h, null, 0, w), 0, w);
            data = ((DataBufferByte)bgrimg.getRaster().getDataBuffer()).getData();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return new BufferInfo(w, h, data);
	}
	
    public static BufferInfo getI420FromFile(String filePath) {
        byte[] yuv = null;
        int w = 0;
        int h = 0;
        try {
            BufferedImage img = ImageIO.read(new File(filePath));
            if (((img.getWidth() & 0x1) != 0) || ((img.getHeight() & 0x1) != 0)) {
                img = img.getSubimage(0, 0, img.getWidth() & 0xFFFFFFFE, img.getHeight() & 0xFFFFFFFE);
            }
            w = img.getWidth();
            h = img.getHeight();
            int[] bgra = img.getRGB(0, 0, w, h, null, 0, w);
            yuv = BGRA2I420(bgra, w, h);
        } catch (IOException e) {
            e.printStackTrace();
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

                yuv[i * width + j] = (byte) y;
                yuv[u_offset + (i >> 1) * (width >> 1) + (j >> 1)] = (byte) u;
                yuv[y_offset + (i >> 1) * (width >> 1) + (j >> 1)] = (byte) v;
            }
        }
        return yuv;
    }
}
