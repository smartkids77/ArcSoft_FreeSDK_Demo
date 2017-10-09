#-*- encoding=utf-8 -*-
from PIL import Image
from . import BufferInfo

def BGRA2I420(img, width, height):
    yuv = bytearray(width * height * 3 // 2)
    u_offset = width * height
    y_offset = width * height * 5 // 4

    for i in range(0, height):
        for j in range(0, width):
            rgb = img.getpixel((j,i))
            r = rgb[0]
            g = rgb[1]
            b = rgb[2]

            y = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16
            u = ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128
            v = ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128

            y = 0 if y < 0 else (255 if y > 255 else (y & 0xFF))
            u = 0 if u < 0 else (255 if u > 255 else (u & 0xFF))
            v = 0 if v < 0 else (255 if v > 255 else (v & 0xFF))

            yuv[i * width + j] = y
            yuv[u_offset + (i >> 1) * (width >> 1) + (j >> 1)] = u
            yuv[y_offset + (i >> 1) * (width >> 1) + (j >> 1)] = v

    return bytes(yuv)


def getI420FromFile(filePath):
    img = Image.open(filePath).convert('RGB')
    w = img.width& 0xFFFFFFFE
    h = img.height& 0xFFFFFFFE
    yuv = BGRA2I420(img, w, h)

    return BufferInfo(w, h, yuv)
