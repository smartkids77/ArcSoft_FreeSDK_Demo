package com.arcsoft;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import com.sun.jna.ptr.IntByReference;

public class AFD_FSDK_FACERES extends Structure {
    public static class ByReference extends AFD_FSDK_FACERES implements Structure.ByReference {
        public ByReference() {

        }

        public ByReference(Pointer p) {
            super(p);
        }
    };

    public int nFace;
    public MRECT.ByReference rcFace;
    public IntByReference lfaceOrient;

    public AFD_FSDK_FACERES() {

    }

    public AFD_FSDK_FACERES(Pointer p) {
        super(p);
        read();
    }

    @Override
    protected List getFieldOrder() {
        return Arrays.asList(new String[] { "nFace", "rcFace", "lfaceOrient" });
    }
}
