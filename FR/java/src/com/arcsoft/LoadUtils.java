package com.arcsoft;

import com.sun.jna.Native;
import com.sun.jna.Platform;

public class LoadUtils {
    public static <T> T loadOSLibrary(String dirPath,String libname, Class<T> interfaceClass) {
        String filePath = dirPath+"/";
        if(Platform.isWindows()){
            if(Platform.is64Bit()){
                filePath += "win/x64/"+"lib"+libname+".dll";
            }else{
                filePath += "win/x86/"+"lib"+libname+".dll";
            }
        }else if(Platform.is64Bit() && Platform.isLinux()){
            filePath += "linux/x64/"+"lib"+libname+".so";
        }else{
             System.out.println("unsupported platform");
             System.exit(0);
        }
        
        return loadLibrary(filePath,interfaceClass);
    }
    
    public static <T> T loadLibrary(String filePath, Class<T> interfaceClass) {
        return Native.loadLibrary(filePath,interfaceClass);
    }
}
