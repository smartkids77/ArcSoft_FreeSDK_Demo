package com.arcsoft.demo;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

import com.arcsoft.AFD_FSDKLibrary;
import com.arcsoft.AFD_FSDK_FACERES;
import com.arcsoft.AFD_FSDK_Version;
import com.arcsoft.AFR_FSDKLibrary;
import com.arcsoft.AFR_FSDK_FACEINPUT;
import com.arcsoft.AFR_FSDK_FACEMODEL;
import com.arcsoft.AFR_FSDK_Version;
import com.arcsoft.ASVLOFFSCREEN;
import com.arcsoft.CLibrary;
import com.arcsoft.MRECT;
import com.arcsoft._AFD_FSDK_OrientPriority;
import com.sun.jna.Memory;
import com.sun.jna.NativeLong;
import com.sun.jna.Platform;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.FloatByReference;
import com.sun.jna.ptr.PointerByReference;


public class AFRTest {
	public static final String FD_APPID = "XXXXXXXXXX";
	public static final String FD_SDKKEY = "XXXXXXXXXXXXXXX";
	
	public static final String FR_APPID = "XXXXXXXXXXXXXXXX";
	public static final String FR_SDKKEY = "XXXXXXXXXXXXXXXXXX";
	
    public static void main(String[] args) {
    	System.out.println("#####################################################");
    	if(Platform.is64Bit()){
    		System.out.println("current dll is 32bit,64bit java runtime do not support");
    	}
    	
    	String yuvFilePathA = "002_I420_fromJPG.yuv";
      	String yuvFilePathB = "001_640x480_I420.YUV";
      	
      	MRECT faceA_rect = new MRECT();
      	MRECT faceB_rect = new MRECT();

    	int FD_WORKBUF_SIZE = 20*1024*1024;
    	int FR_WORKBUF_SIZE = 40*1024*1024;
    	int MAX_FACE_NUM = 50;
    	
    	Pointer pFDWorkMem = CLibrary.INSTANCE.malloc(FD_WORKBUF_SIZE);
    	Pointer pFRWorkMem = CLibrary.INSTANCE.malloc(FR_WORKBUF_SIZE);
        
        PointerByReference phFDEngine = new PointerByReference();
        NativeLong ret = AFD_FSDKLibrary.INSTANCE.AFD_FSDK_InitialFaceEngine(
				        		FD_APPID, FD_SDKKEY, pFDWorkMem, FD_WORKBUF_SIZE, 
				        		phFDEngine, _AFD_FSDK_OrientPriority.AFD_FSDK_OPF_0_HIGHER_EXT,
			                    16, MAX_FACE_NUM);
        if (ret.intValue() != 0) {
        	 System.out.println("AFD_FSDK_InitialFaceEngine ret == "+ret);
        	 System.exit(0);
        }
        
        Pointer hFDEngine = phFDEngine.getValue();
        AFD_FSDK_Version versionFD = AFD_FSDKLibrary.INSTANCE.AFD_FSDK_GetVersion(hFDEngine);
        System.out.println(String.format("%d %d %d %d", versionFD.lCodebase, versionFD.lMajor, versionFD.lMinor,versionFD.lBuild));
        System.out.println(versionFD.Version);
        System.out.println(versionFD.BuildDate);
        System.out.println(versionFD.CopyRight);
        
        PointerByReference phFREngine = new PointerByReference();
        ret = AFR_FSDKLibrary.INSTANCE.AFR_FSDK_InitialEngine(
				        		FR_APPID, FR_SDKKEY, pFRWorkMem, FR_WORKBUF_SIZE, phFREngine);
        if (ret.intValue() != 0) {
        	 System.out.println("AFD_FSDK_InitialFaceEngine ret == "+ret);
        	 System.exit(0);
        }
        Pointer hFREngine = phFREngine.getValue();
        AFR_FSDK_Version versionFR = AFR_FSDKLibrary.INSTANCE.AFR_FSDK_GetVersion(hFREngine);
        System.out.println(String.format("%d %d %d %d", versionFR.lCodebase, versionFR.lMajor, versionFR.lMinor,versionFR.lBuild));
        System.out.println(versionFR.Version);
        System.out.println(versionFR.BuildDate);
        System.out.println(versionFR.CopyRight);
        
        
        ASVLOFFSCREEN  inputImgA = new ASVLOFFSCREEN();
        inputImgA.u32PixelArrayFormat = 0x601;
        inputImgA.i32Width = 750;
        inputImgA.i32Height = 1334;
        inputImgA.pi32Pitch[0] = inputImgA.i32Width;
        inputImgA.pi32Pitch[1] = inputImgA.i32Width/2;
        inputImgA.pi32Pitch[2] = inputImgA.i32Width/2;
        
        {
	        byte[] imagedata = new byte[inputImgA.i32Width*inputImgA.i32Height*3/2];
	        File f = new File(yuvFilePathA); 
	        InputStream ios = null;
	        try {
	            ios = new FileInputStream(f);
	            ios.read(imagedata);
	     
	        } catch (FileNotFoundException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			} finally {
	            try {
	                if (ios != null){
	                    ios.close();
	                }
	            } catch (IOException e) {
	            }
	        }
	        inputImgA.ppu8Plane[0] = new Memory(inputImgA.pi32Pitch[0]*inputImgA.i32Height);
	        inputImgA.ppu8Plane[0].write(0, imagedata, 0, inputImgA.pi32Pitch[0]*inputImgA.i32Height);
	        inputImgA.ppu8Plane[1] = new Memory(inputImgA.pi32Pitch[1]*inputImgA.i32Height/2);
	        inputImgA.ppu8Plane[1].write(0, imagedata, inputImgA.pi32Pitch[0]*inputImgA.i32Height, inputImgA.pi32Pitch[1]*inputImgA.i32Height/2);
	        inputImgA.ppu8Plane[2] = new Memory(inputImgA.pi32Pitch[2]*inputImgA.i32Height/2);
	        inputImgA.ppu8Plane[2].write(0, imagedata,inputImgA.pi32Pitch[0]*inputImgA.i32Height+ inputImgA.pi32Pitch[1]*inputImgA.i32Height/2, inputImgA.pi32Pitch[2]*inputImgA.i32Height/2);
	        inputImgA.ppu8Plane[3] = Pointer.NULL;
	        inputImgA.setAutoRead(false);
	        PointerByReference ppFaceRes = new PointerByReference();
	        
	        ret = AFD_FSDKLibrary.INSTANCE.AFD_FSDK_StillImageFaceDetection(hFDEngine,inputImgA,ppFaceRes);
	        if (ret.intValue() != 0) {
	       	    System.out.println("AFD_FSDK_StillImageFaceDetection ret == "+ret);
	       	    System.exit(0);
	        }
	        AFD_FSDK_FACERES faceRes = new AFD_FSDK_FACERES(ppFaceRes.getValue());
	        for (int i = 0; i < faceRes.nFace; i++) {
	        	MRECT rect = new MRECT(new Pointer(Pointer.nativeValue(faceRes.rcFace.getPointer())+faceRes.rcFace.size()*i));
	        	int orient = faceRes.lfaceOrient.getPointer().getInt(i*4);
	            System.out.println(String.format("%d (%d %d %d %d) orient %d",i,rect.left, rect.top,rect.right,rect.bottom,orient));
	            if(i == 0){
	            	faceA_rect.left = rect.left;
	            	faceA_rect.top = rect.top;
	            	faceA_rect.right = rect.right;
	            	faceA_rect.bottom = rect.bottom;
	            }
			}
        }
        
        
        ASVLOFFSCREEN  inputImgB = new ASVLOFFSCREEN();
        inputImgB.u32PixelArrayFormat = 0x601;
        inputImgB.i32Width = 640;
        inputImgB.i32Height = 480;
        inputImgB.pi32Pitch[0] = inputImgB.i32Width;
        inputImgB.pi32Pitch[1] = inputImgB.i32Width/2;
        inputImgB.pi32Pitch[2] = inputImgB.i32Width/2;
        {
	        byte[] imagedata = new byte[inputImgB.i32Width*inputImgB.i32Height*3/2];
	        File f = new File(yuvFilePathB); 
	        InputStream ios = null;
	        try {
	            ios = new FileInputStream(f);
	            ios.read(imagedata);
	     
	        } catch (FileNotFoundException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			} finally {
	            try {
	                if (ios != null){
	                    ios.close();
	                }
	            } catch (IOException e) {
	            }
	        }
	        inputImgB.ppu8Plane[0] = new Memory(inputImgB.pi32Pitch[0]*inputImgB.i32Height);
	        inputImgB.ppu8Plane[0].write(0, imagedata, 0, inputImgB.pi32Pitch[0]*inputImgB.i32Height);
	        inputImgB.ppu8Plane[1] = new Memory(inputImgB.pi32Pitch[1]*inputImgB.i32Height/2);
	        inputImgB.ppu8Plane[1].write(0, imagedata, inputImgB.pi32Pitch[0]*inputImgB.i32Height, inputImgB.pi32Pitch[1]*inputImgB.i32Height/2);
	        inputImgB.ppu8Plane[2] = new Memory(inputImgB.pi32Pitch[2]*inputImgB.i32Height/2);
	        inputImgB.ppu8Plane[2].write(0, imagedata,inputImgB.pi32Pitch[0]*inputImgB.i32Height+ inputImgB.pi32Pitch[1]*inputImgB.i32Height/2, inputImgB.pi32Pitch[2]*inputImgB.i32Height/2);
	        inputImgB.ppu8Plane[3] = Pointer.NULL;
	        inputImgB.setAutoRead(false);
	        PointerByReference ppFaceRes = new PointerByReference();
	        
	        ret = AFD_FSDKLibrary.INSTANCE.AFD_FSDK_StillImageFaceDetection(hFDEngine,inputImgB,ppFaceRes);
	        if (ret.intValue() != 0) {
	       	    System.out.println("AFD_FSDK_StillImageFaceDetection ret == "+ret);
	       	    System.exit(0);
	        }
	        AFD_FSDK_FACERES faceRes = new AFD_FSDK_FACERES(ppFaceRes.getValue());
	        for (int i = 0; i < faceRes.nFace; i++) {
	        	MRECT rect = new MRECT(new Pointer(Pointer.nativeValue(faceRes.rcFace.getPointer())+faceRes.rcFace.size()*i));
	        	int orient = faceRes.lfaceOrient.getPointer().getInt(i*4);
	            System.out.println(String.format("%d (%d %d %d %d) orient %d",i,rect.left, rect.top,rect.right,rect.bottom,orient));
	            if(i == 0){
	            	faceB_rect.left = rect.left;
	            	faceB_rect.top = rect.top;
	            	faceB_rect.right = rect.right;
	            	faceB_rect.bottom = rect.bottom;
	            }
			}
        }
        
        AFR_FSDK_FACEINPUT faceinputA = new AFR_FSDK_FACEINPUT();
        faceinputA.lOrient = 1;
        faceinputA.rcFace.left = faceA_rect.left;
        faceinputA.rcFace.top = faceA_rect.top;
        faceinputA.rcFace.right = faceA_rect.right;
        faceinputA.rcFace.bottom = faceA_rect.bottom;
        
        AFR_FSDK_FACEMODEL faceFeature = new AFR_FSDK_FACEMODEL();
        
        ret = AFR_FSDKLibrary.INSTANCE.AFR_FSDK_ExtractFRFeature(hFREngine, inputImgA, faceinputA, faceFeature);
        if (ret.intValue() != 0) {
       	    System.out.println("A AFR_FSDK_ExtractFRFeature ret == "+ret);
       	    System.exit(0);
        }
        AFR_FSDK_FACEMODEL faceFeatureA = faceFeature.deepCopy();
        
        AFR_FSDK_FACEINPUT faceinputB = new AFR_FSDK_FACEINPUT();
        faceinputB.lOrient = 1;
        faceinputB.rcFace.left = faceB_rect.left;
        faceinputB.rcFace.top = faceB_rect.top;
        faceinputB.rcFace.right = faceB_rect.right;
        faceinputB.rcFace.bottom = faceB_rect.bottom;
        
        ret = AFR_FSDKLibrary.INSTANCE.AFR_FSDK_ExtractFRFeature(hFREngine, inputImgB, faceinputB, faceFeature);
        if (ret.intValue() != 0) {
       	    System.out.println("B AFR_FSDK_ExtractFRFeature ret == "+ret);
       	    System.exit(0);
        }    
        AFR_FSDK_FACEMODEL faceFeatureB = faceFeature.deepCopy();
        
        FloatByReference fSimilScore = new FloatByReference(0.0f);
        ret = AFR_FSDKLibrary.INSTANCE.AFR_FSDK_FacePairMatching(hFREngine, faceFeatureA, faceFeatureB, fSimilScore);
        if (ret.intValue() != 0) {
       	    System.out.println("AFR_FSDK_FacePairMatching ret == "+ret);
       	    System.exit(0);
        }
        
        System.out.println("similarity between faceA and faceB is "+fSimilScore.getValue());
        
        ret = AFD_FSDKLibrary.INSTANCE.AFD_FSDK_UninitialFaceEngine(hFDEngine);
        ret = AFR_FSDKLibrary.INSTANCE.AFR_FSDK_UninitialEngine(hFREngine);
        
        faceFeatureA.free();
        faceFeatureB.free();
    	CLibrary.INSTANCE.free(pFDWorkMem);
    	CLibrary.INSTANCE.free(pFRWorkMem);
    	
    	System.out.println("#####################################################");
    }
}
