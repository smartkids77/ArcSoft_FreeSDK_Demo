package com.arcsoft;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Platform;
import com.sun.jna.Pointer;

public interface CLibrary extends Library {
    CLibrary INSTANCE = (CLibrary)Native.loadLibrary((Platform.isWindows() ? "msvcrt" : "c"),CLibrary.class);

    Pointer malloc(int len);
    void free(Pointer p);
    void printf(String format, Object... args);
    Pointer memcpy(Pointer dst,Pointer src,long size);
}