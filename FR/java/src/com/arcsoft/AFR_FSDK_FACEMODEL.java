package com.arcsoft;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import com.sun.jna.ptr.ByteByReference;

public class AFR_FSDK_FACEMODEL extends Structure {

    public ByteByReference pbFeature;
    public int lFeatureSize;

    protected boolean bAllocByMalloc;
    @Override
    protected List getFieldOrder() { 
        return Arrays.asList(new String[] { 
            "pbFeature", "lFeatureSize"
        });
    }
    
    public AFR_FSDK_FACEMODEL deepCopy() throws Exception{
        
        if(!isValid()){
            throw new Exception("invalid feature");
        }
        
        AFR_FSDK_FACEMODEL feature = new AFR_FSDK_FACEMODEL();
        feature.bAllocByMalloc = true; 
        feature.lFeatureSize = lFeatureSize;
        feature.pbFeature = new ByteByReference();
        feature.pbFeature.setPointer(CLibrary.INSTANCE.malloc(feature.lFeatureSize));
        CLibrary.INSTANCE.memcpy(feature.pbFeature.getPointer(),pbFeature.getPointer(),feature.lFeatureSize);
        return feature;
    }
    
    public synchronized void freeUnmanaged(){
        if(bAllocByMalloc&&isValid()){
            CLibrary.INSTANCE.free(pbFeature.getPointer());
            pbFeature = null;
            //System.out.println("gc feature freeUnmanaged");
        }
    }
    
    @Override
    protected void finalize() throws   Throwable  {  
        freeUnmanaged();
    }
    
    
    public static AFR_FSDK_FACEMODEL fromByteArray(byte[] byteArray) throws Exception{
        if(byteArray == null){
            throw new Exception("invalid byteArray");
        }
        
        AFR_FSDK_FACEMODEL feature = new AFR_FSDK_FACEMODEL();
        feature.lFeatureSize = byteArray.length;
        feature.bAllocByMalloc = true; 
        feature.pbFeature = new ByteByReference();
        feature.pbFeature.setPointer(CLibrary.INSTANCE.malloc(feature.lFeatureSize));
        feature.pbFeature.getPointer().write(0, byteArray, 0, feature.lFeatureSize);
        return feature;
    }
    
    public byte[] toByteArray() throws Exception{
        if(!isValid()){
            throw new Exception("invalid feature");
        }
        return pbFeature.getPointer().getByteArray(0, lFeatureSize);
    }
    
    private boolean isValid() {
        return ((pbFeature != null)&&(Pointer.nativeValue(pbFeature.getPointer())!= 0));
    }
}
