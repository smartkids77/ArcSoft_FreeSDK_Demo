package com.arcsoft.demo;
import java.io.File;
import java.io.FileInputStream;
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
import com.arcsoft.ASVL_COLOR_FORMAT;
import com.arcsoft.CLibrary;
import com.arcsoft.MRECT;
import com.arcsoft._AFD_FSDK_OrientPriority;
import com.sun.jna.Memory;
import com.sun.jna.NativeLong;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.FloatByReference;
import com.sun.jna.ptr.PointerByReference;


public class AFRTest {
	public static final String    APPID  = "XXXXXXXXXX";
	public static final String FD_SDKKEY = "YYYYYYYYYY";
	public static final String FR_SDKKEY = "WWWWWWWWWW";
	
	public static final int FD_WORKBUF_SIZE = 20*1024*1024;
	public static final int FR_WORKBUF_SIZE = 40*1024*1024;
	public static final int MAX_FACE_NUM = 50;
	
    public static void main(String[] args) {
    	System.out.println("#####################################################");
    	
        String yuv_filePathA = "001_640x480_I420.YUV";
        int yuv_widthA = 640;
        int yuv_heightA = 480;
        int yuv_formatA = ASVL_COLOR_FORMAT.ASVL_PAF_I420;
      	String yuv_filePathB = "003_640x480_I420.YUV";
        int yuv_widthB = 640;
        int yuv_heightB = 480;
        int yuv_formatB = ASVL_COLOR_FORMAT.ASVL_PAF_I420;
      
        //Init Engine
    	Pointer pFDWorkMem = CLibrary.INSTANCE.malloc(FD_WORKBUF_SIZE);
    	Pointer pFRWorkMem = CLibrary.INSTANCE.malloc(FR_WORKBUF_SIZE);
        
        PointerByReference phFDEngine = new PointerByReference();
        NativeLong ret = AFD_FSDKLibrary.INSTANCE.AFD_FSDK_InitialFaceEngine(
				        		APPID, FD_SDKKEY, pFDWorkMem, FD_WORKBUF_SIZE, 
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
				        		APPID, FR_SDKKEY, pFRWorkMem, FR_WORKBUF_SIZE, phFREngine);
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
    
        //Do Face Detect in ImageA
      	MRECT faceA_rect = new MRECT();
      	int faceA_Orient = 0;
      	
    	ASVLOFFSCREEN inputImgA = loadYUVImage(yuv_filePathA,yuv_widthA,yuv_heightA,yuv_formatA);
        {
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
	            	faceA_Orient = orient;
	            }
			}
        }
        
        //Do Face Detect in ImageB
      	MRECT faceB_rect = new MRECT();
      	int faceB_Orient = 0;
      	
        ASVLOFFSCREEN inputImgB = loadYUVImage(yuv_filePathB,yuv_widthB,yuv_heightB,yuv_formatB);
        {
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
	            	faceB_Orient = orient;
	            }
			}
        }
        
        //Extract FaceA Feature
        AFR_FSDK_FACEINPUT faceinputA = new AFR_FSDK_FACEINPUT();
        faceinputA.lOrient = faceA_Orient;
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
        
        //Extract FaceB Feature
        AFR_FSDK_FACEINPUT faceinputB = new AFR_FSDK_FACEINPUT();
        faceinputB.lOrient = faceB_Orient;
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
        
        //calc similarity between faceA and faceB
        FloatByReference fSimilScore = new FloatByReference(0.0f);
        ret = AFR_FSDKLibrary.INSTANCE.AFR_FSDK_FacePairMatching(hFREngine, faceFeatureA, faceFeatureB, fSimilScore);
        if (ret.intValue() != 0) {
       	    System.out.println("AFR_FSDK_FacePairMatching ret == "+ret);
       	    System.exit(0);
        }
        System.out.println("similarity between faceA and faceB is "+fSimilScore.getValue());
        
        //release Engine
        ret = AFD_FSDKLibrary.INSTANCE.AFD_FSDK_UninitialFaceEngine(hFDEngine);
        ret = AFR_FSDKLibrary.INSTANCE.AFR_FSDK_UninitialEngine(hFREngine);
        
        faceFeatureA.freeUnmanaged();
        faceFeatureB.freeUnmanaged();
        
    	CLibrary.INSTANCE.free(pFDWorkMem);
    	CLibrary.INSTANCE.free(pFRWorkMem);
    	
    	System.out.println("#####################################################");
    }
    
    
	public static ASVLOFFSCREEN loadYUVImage(String yuv_filePath,int yuv_width,int yuv_height,int yuv_format) {
        int yuv_rawdata_size = 0;
        
        ASVLOFFSCREEN  inputImg = new ASVLOFFSCREEN();
        inputImg.u32PixelArrayFormat = yuv_format;
        inputImg.i32Width = yuv_width;
        inputImg.i32Height = yuv_height;
        if (ASVL_COLOR_FORMAT.ASVL_PAF_I420 == inputImg.u32PixelArrayFormat) {
            inputImg.pi32Pitch[0] = inputImg.i32Width;
            inputImg.pi32Pitch[1] = inputImg.i32Width/2;
            inputImg.pi32Pitch[2] = inputImg.i32Width/2;
            yuv_rawdata_size = inputImg.i32Width*inputImg.i32Height*3/2;
        } else if (ASVL_COLOR_FORMAT.ASVL_PAF_NV12 == inputImg.u32PixelArrayFormat) {
            inputImg.pi32Pitch[0] = inputImg.i32Width;
            inputImg.pi32Pitch[1] = inputImg.i32Width;
            yuv_rawdata_size = inputImg.i32Width*inputImg.i32Height*3/2;
        } else if (ASVL_COLOR_FORMAT.ASVL_PAF_NV21 == inputImg.u32PixelArrayFormat) {
            inputImg.pi32Pitch[0] = inputImg.i32Width;
            inputImg.pi32Pitch[1] = inputImg.i32Width;
            yuv_rawdata_size = inputImg.i32Width*inputImg.i32Height*3/2;
        } else if (ASVL_COLOR_FORMAT.ASVL_PAF_YUYV == inputImg.u32PixelArrayFormat) {
            inputImg.pi32Pitch[0] = inputImg.i32Width*2;
            yuv_rawdata_size = inputImg.i32Width*inputImg.i32Height*2;
        }else{
	       	 System.out.println("unsupported  yuv format");
	       	 System.exit(0);
        }
        
        //load YUV Image Data from File
        byte[] imagedata = new byte[yuv_rawdata_size];
        File f = new File(yuv_filePath);
        InputStream ios = null;
        try {
            ios = new FileInputStream(f);
            ios.read(imagedata);
     
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("error in loading yuv file");
       	    System.exit(0);
		} finally {
            try {
                if (ios != null){
                    ios.close();
                }
            } catch (IOException e) {
            }
        }
        
        if (ASVL_COLOR_FORMAT.ASVL_PAF_I420 == inputImg.u32PixelArrayFormat) {
            inputImg.ppu8Plane[0] = new Memory(inputImg.pi32Pitch[0]*inputImg.i32Height);
            inputImg.ppu8Plane[0].write(0, imagedata, 0, inputImg.pi32Pitch[0]*inputImg.i32Height);
            inputImg.ppu8Plane[1] = new Memory(inputImg.pi32Pitch[1]*inputImg.i32Height/2);
            inputImg.ppu8Plane[1].write(0, imagedata, inputImg.pi32Pitch[0]*inputImg.i32Height, inputImg.pi32Pitch[1]*inputImg.i32Height/2);
            inputImg.ppu8Plane[2] = new Memory(inputImg.pi32Pitch[2]*inputImg.i32Height/2);
            inputImg.ppu8Plane[2].write(0, imagedata,inputImg.pi32Pitch[0]*inputImg.i32Height+ inputImg.pi32Pitch[1]*inputImg.i32Height/2, inputImg.pi32Pitch[2]*inputImg.i32Height/2);
            inputImg.ppu8Plane[3] = Pointer.NULL;
        } else if (ASVL_COLOR_FORMAT.ASVL_PAF_NV12 == inputImg.u32PixelArrayFormat) {
            inputImg.ppu8Plane[0] = new Memory(inputImg.pi32Pitch[0]*inputImg.i32Height);
            inputImg.ppu8Plane[0].write(0, imagedata, 0, inputImg.pi32Pitch[0]*inputImg.i32Height);
            inputImg.ppu8Plane[1] = new Memory(inputImg.pi32Pitch[1]*inputImg.i32Height/2);
            inputImg.ppu8Plane[1].write(0, imagedata, inputImg.pi32Pitch[0]*inputImg.i32Height, inputImg.pi32Pitch[1]*inputImg.i32Height/2);
            inputImg.ppu8Plane[2] = Pointer.NULL;
            inputImg.ppu8Plane[3] = Pointer.NULL;
        } else if (ASVL_COLOR_FORMAT.ASVL_PAF_NV21 == inputImg.u32PixelArrayFormat) {
            inputImg.ppu8Plane[0] = new Memory(inputImg.pi32Pitch[0]*inputImg.i32Height);
            inputImg.ppu8Plane[0].write(0, imagedata, 0, inputImg.pi32Pitch[0]*inputImg.i32Height);
            inputImg.ppu8Plane[1] = new Memory(inputImg.pi32Pitch[1]*inputImg.i32Height/2);
            inputImg.ppu8Plane[1].write(0, imagedata, inputImg.pi32Pitch[0]*inputImg.i32Height, inputImg.pi32Pitch[1]*inputImg.i32Height/2);
            inputImg.ppu8Plane[2] = Pointer.NULL;
            inputImg.ppu8Plane[3] = Pointer.NULL;
        } else if (ASVL_COLOR_FORMAT.ASVL_PAF_YUYV == inputImg.u32PixelArrayFormat) {
            inputImg.ppu8Plane[0] = new Memory(inputImg.pi32Pitch[0]*inputImg.i32Height);
            inputImg.ppu8Plane[0].write(0, imagedata, 0, inputImg.pi32Pitch[0]*inputImg.i32Height);
            inputImg.ppu8Plane[1] = Pointer.NULL;
            inputImg.ppu8Plane[2] = Pointer.NULL;
            inputImg.ppu8Plane[3] = Pointer.NULL;
        }else{
	       	 System.out.println("unsupported yuv format");
	       	 System.exit(0);
        }

        inputImg.setAutoRead(false);
        return inputImg;
	}
}
