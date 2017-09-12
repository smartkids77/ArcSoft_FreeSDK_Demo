package com.arcsoft.utils;

public class BufferInfo {
    public int width;
    public int height;
    public byte[] buffer;

    public BufferInfo(int w, int h, byte[] buf) {
        width = w;
        height = h;
        buffer = buf;
    }
}