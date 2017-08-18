package com.arcsoft.demo;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

import com.arcsoft.AFD_FSDKLibrary;
import com.arcsoft.AFD_FSDK_FACERES;
import com.arcsoft.AFD_FSDK_Version;
import com.arcsoft.ASVLOFFSCREEN;
import com.arcsoft.CLibrary;
import com.arcsoft.MRECT;
import com.arcsoft._AFD_FSDK_OrientPriority;
import com.sun.jna.Memory;
import com.sun.jna.NativeLong;
import com.sun.jna.Platform;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.PointerByReference;


public class AFDTest {
	public static final String APPID = "XXXXXXXXXXXXXXX";
	public static final String SDKKEY = "YYYYYYYYYYYYYY";
	
    public static void main(String[] args) {
    	if(Platform.is64Bit()){
    		System.out.println("current dll is 32bit,64bit java runtime do not support");
    	}

    	int WORKBUF_SIZE = 40*1024*1024;
    	int MAX_FACE_NUM = 50;
    	
    	Pointer pWorkMem = CLibrary.INSTANCE.malloc(WORKBUF_SIZE);
        
        PointerByReference phEngine = new PointerByReference();
        NativeLong ret = AFD_FSDKLibrary.INSTANCE.AFD_FSDK_InitialFaceEngine(
				        		APPID, SDKKEY, pWorkMem, WORKBUF_SIZE, 
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
        
        ASVLOFFSCREEN  inputImg = new ASVLOFFSCREEN();
        inputImg.u32PixelArrayFormat = 0x601;
        inputImg.i32Width = 640;
        inputImg.i32Height = 480;
        inputImg.pi32Pitch[0] = inputImg.i32Width;
        inputImg.pi32Pitch[1] = inputImg.i32Width/2;
        inputImg.pi32Pitch[2] = inputImg.i32Width/2;
        
        byte[] imagedata = new byte[inputImg.i32Width*inputImg.i32Height*3/2];
        File f = new File("003_640x480_I420.YUV");
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
        inputImg.ppu8Plane[0] = new Memory(inputImg.pi32Pitch[0]*inputImg.i32Height);
        inputImg.ppu8Plane[0].write(0, imagedata, 0, inputImg.pi32Pitch[0]*inputImg.i32Height);
        inputImg.ppu8Plane[1] = new Memory(inputImg.pi32Pitch[1]*inputImg.i32Height/2);
        inputImg.ppu8Plane[1].write(0, imagedata, inputImg.pi32Pitch[0]*inputImg.i32Height, inputImg.pi32Pitch[1]*inputImg.i32Height/2);
        inputImg.ppu8Plane[2] = new Memory(inputImg.pi32Pitch[2]*inputImg.i32Height/2);
        inputImg.ppu8Plane[2].write(0, imagedata,inputImg.pi32Pitch[0]*inputImg.i32Height+ inputImg.pi32Pitch[1]*inputImg.i32Height/2, inputImg.pi32Pitch[2]*inputImg.i32Height/2);
        inputImg.ppu8Plane[3] = Pointer.NULL;
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
