package com.arcsoft;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import com.sun.jna.ptr.ByteByReference;

public class AFR_FSDK_FACEMODEL extends Structure {

    public ByteByReference pbFeature;
    public int lFeatureSize;

    @Override
    protected List getFieldOrder() { 
        return Arrays.asList(new String[] { 
            "pbFeature", "lFeatureSize"
        });
    }
    
    public AFR_FSDK_FACEMODEL deepCopy(){
    	AFR_FSDK_FACEMODEL copyFeature = new AFR_FSDK_FACEMODEL();
    	copyFeature.lFeatureSize = copyFeature.lFeatureSize&0x80000000;
    	if((pbFeature != null)&&(Pointer.nativeValue(pbFeature.getPointer())!= 0)){
        	copyFeature.lFeatureSize |= lFeatureSize&0x7FFFFFFF;
        	copyFeature.pbFeature = new ByteByReference();
        	copyFeature.pbFeature.setPointer(CLibrary.INSTANCE.malloc(lFeatureSize));
        	CLibrary.INSTANCE.memcpy(copyFeature.pbFeature.getPointer(),pbFeature.getPointer(),lFeatureSize);
    	}
    	
    	return copyFeature;
    }
    
    public synchronized void freeUnmanaged(){
    	if(((lFeatureSize&0x80000000) != 0)&&(pbFeature != null)&&(Pointer.nativeValue(pbFeature.getPointer())!= 0)){
    		CLibrary.INSTANCE.free(pbFeature.getPointer());
    		pbFeature = null;
    	}
    }
    
    @Override
    protected void finalize() throws   Throwable  {  
    	freeUnmanaged();
    }
    
    public int getFeatureSize() {
		return lFeatureSize&0x7FFFFFFF;
	}
}
