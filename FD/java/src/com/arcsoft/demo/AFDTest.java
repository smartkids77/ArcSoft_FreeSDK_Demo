package com.arcsoft.demo;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

import com.arcsoft.AFD_FSDKLibrary;
import com.arcsoft.AFD_FSDK_FACERES;
import com.arcsoft.AFD_FSDK_Version;
import com.arcsoft.ASVLOFFSCREEN;
import com.arcsoft.ASVL_COLOR_FORMAT;
import com.arcsoft.CLibrary;
import com.arcsoft.MRECT;
import com.arcsoft._AFD_FSDK_OrientPriority;
import com.sun.jna.Memory;
import com.sun.jna.NativeLong;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.PointerByReference;


public class AFDTest {
	public static final String APPID     = "XXXXXXXXXXXXXXX";
	public static final String FD_SDKKEY = "YYYYYYYYYYYYYYY";
	
	public static final  int WORKBUF_SIZE = 20*1024*1024;
	public static final  int MAX_FACE_NUM = 50;

    public static void main(String[] args) {

    	String yuv_filePath = "001_640x480_I420.YUV";
    	int yuv_width = 640;
    	int yuv_height = 480;
    	int yuv_format = ASVL_COLOR_FORMAT.ASVL_PAF_I420;
    	
    	Pointer pWorkMem = CLibrary.INSTANCE.malloc(WORKBUF_SIZE);
        
        PointerByReference phEngine = new PointerByReference();
        NativeLong ret = AFD_FSDKLibrary.INSTANCE.AFD_FSDK_InitialFaceEngine(
				        		APPID, FD_SDKKEY, pWorkMem, WORKBUF_SIZE, 
				        		phEngine, _AFD_FSDK_OrientPriority.AFD_FSDK_OPF_0_HIGHER_EXT,
			                    16, MAX_FACE_NUM);
        if (ret.intValue() != 0) {
        	 System.out.println("AFD_FSDK_InitialFaceEngine ret == "+ret);
        	 System.exit(0);
        }
        
        Pointer hEngine = phEngine.getValue();
        AFD_FSDK_Version version = AFD_FSDKLibrary.INSTANCE.AFD_FSDK_GetVersion(hEngine);
        System.out.println(String.format("%d %d %d %d", version.lCodebase, version.lMajor, version.lMinor,version.lBuild));
        System.out.println(version.Version);
        System.out.println(version.BuildDate);
        System.out.println(version.CopyRight);
        
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
        PointerByReference ppFaceRes = new PointerByReference();
        
        ret = AFD_FSDKLibrary.INSTANCE.AFD_FSDK_StillImageFaceDetection(hEngine,inputImg,ppFaceRes);
        if (ret.intValue() != 0) {
       	    System.out.println("AFD_FSDK_StillImageFaceDetection ret == "+ret);
       	    System.exit(0);
        }
        AFD_FSDK_FACERES faceRes = new AFD_FSDK_FACERES(ppFaceRes.getValue());
        for (int i = 0; i < faceRes.nFace; i++) {
        	MRECT rect = new MRECT(new Pointer(Pointer.nativeValue(faceRes.rcFace.getPointer())+faceRes.rcFace.size()*i));
        	int orient = faceRes.lfaceOrient.getPointer().getInt(i*4);
            System.out.println(String.format("%d (%d %d %d %d) orient %d",i,rect.left, rect.top,rect.right,rect.bottom,orient));
		}
        
        ret = AFD_FSDKLibrary.INSTANCE.AFD_FSDK_UninitialFaceEngine(hEngine);

    	CLibrary.INSTANCE.free(pWorkMem);
    }
}
