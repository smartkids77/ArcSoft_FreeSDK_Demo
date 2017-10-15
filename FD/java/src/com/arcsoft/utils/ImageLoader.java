package com.arcsoft.utils;

import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;
import javax.imageio.ImageIO;
import java.io.File;
import java.io.IOException;

public class ImageLoader {
    public static final boolean USING_FLOAT = false;
    
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
            if(USING_FLOAT){
                yuv = BGRA2I420_float(bgra, w, h);
            }else{
                yuv = BGRA2I420(bgra, w, h);
            }
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(0);
        }
        return new BufferInfo(w, h, yuv);
    }
    
    public static BufferInfo getBGRFromFile(String filePath) {
        byte[] bgr = null;
        int width = 0;
        int height = 0;
        try {
            BufferedImage img = ImageIO.read(new File(filePath));
            width = img.getWidth();
            height = img.getHeight();
            BufferedImage bgrimg = new BufferedImage(img.getWidth(), img.getHeight(), BufferedImage.TYPE_3BYTE_BGR);
            bgrimg.setRGB(0, 0, width, height, img.getRGB(0, 0, width, height, null, 0, width), 0, width);
            bgr = ((DataBufferByte)bgrimg.getRaster().getDataBuffer()).getData();

        } catch (IOException e) {
            e.printStackTrace();
            System.exit(0);
        }
        return new BufferInfo(width, height, bgr);
    }
    
    //Full swing for BT.601
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

                int y = ((77 * r + 150 * g + 29 * b + 128) >> 8);
                int u = (((-43) * r - 84 * g + 127 * b + 128) >> 8) + 128;
                int v = ((127 * r - 106 * g - 21 * b + 128) >> 8) + 128;

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
    
    // ITU-R standard for YCbCr
    // Y =  0.299R + 0.587G + 0.114B
    // U = -0.169R - 0.331G + 0.499B + 128
    // V =  0.499R - 0.418G - 0.0813B + 128
    
    public static byte[] BGRA2I420_float(int[] bgra, int width, int height) {

        byte[] yuv = new byte[width * height * 3 / 2];
        int u_offset = width * height;
        int y_offset = width * height * 5 / 4;

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                int rgb = bgra[i * width + j] & 0x00FFFFFF;
                float b = (rgb & 0xFF);
                float g = ((rgb >> 8) & 0xFF);
                float r = ((rgb >> 16) & 0xFF);

                float y = (0.299f * r + 0.587f * g + 0.114f * b) ;
                float u = (-0.169f) * r - 0.331f* g + 0.499f * b + 128.0f;
                float v = 0.499f * r - 0.418f * g - 0.0813f * b   + 128.0f;

                yuv[i * width + j] = (byte) y;
                yuv[u_offset + (i >> 1) * (width >> 1) + (j >> 1)] = (byte) u;
                yuv[y_offset + (i >> 1) * (width >> 1) + (j >> 1)] = (byte) v;
            }
        }
        return yuv;
    } 
}
