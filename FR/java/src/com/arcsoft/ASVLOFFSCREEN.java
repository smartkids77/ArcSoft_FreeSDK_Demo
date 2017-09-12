package com.arcsoft;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;

public class ASVLOFFSCREEN extends Structure {
    public int u32PixelArrayFormat;
    public int i32Width;
    public int i32Height;
    public Pointer[] ppu8Plane = new Pointer[4];
    public int[] pi32Pitch = new int[4];
    
    public ASVLOFFSCREEN(){
    
    }

    @Override
    protected List getFieldOrder() { 
        return Arrays.asList(new String[] { 
            "u32PixelArrayFormat", "i32Width", "i32Height", "ppu8Plane","pi32Pitch"
        });
    }
}
