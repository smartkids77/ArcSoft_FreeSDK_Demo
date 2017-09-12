package com.arcsoft;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Structure;

public class AFR_FSDK_FACEINPUT extends Structure {
    
    public MRECT.ByValue rcFace;
    public int lOrient;
    
    @Override
    protected List getFieldOrder() { 
        return Arrays.asList(new String[] { 
             "rcFace", "lOrient"
        });
    }
}
