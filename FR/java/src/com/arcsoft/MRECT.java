package com.arcsoft;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;

public class MRECT extends Structure {
    public static class ByValue extends MRECT implements Structure.ByValue {
        public ByValue() {
            
        }
        
        public ByValue(Pointer p) { 
            super(p); 
        }
    }
    
    public static class ByReference extends MRECT implements Structure.ByReference{
        public ByReference() {
            
        }
        
        public ByReference(Pointer p) {
            super(p);
        }
    };
    
    public int left;
    public int top;
    public int right;
    public int bottom;
    
    public MRECT() {

    }
    
    public MRECT(Pointer p) {
        super(p);
        read();
    }
    
    @Override
    protected List getFieldOrder() { 
        return Arrays.asList(new String[] { 
            "left", "top", "right", "bottom"
        });
    }
}