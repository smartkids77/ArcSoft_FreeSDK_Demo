using System;
using System.Runtime.InteropServices;

namespace arcsoft {

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct AFR_FSDK_FACEMODEL {
        public IntPtr pbFeature;
        public int lFeatureSize;

        public AFR_FSDK_FACEMODEL(IntPtr buffer,int size) {
            pbFeature = IntPtr.Zero;
            lFeatureSize = 0;
        }

        public AFR_FSDK_FACEMODEL deepCopy() {

            if (IntPtr.Zero == pbFeature) {
                throw new Exception("invalid feature");
            }
        
            AFR_FSDK_FACEMODEL feature = new AFR_FSDK_FACEMODEL(); 
            feature.lFeatureSize = lFeatureSize;
            feature.pbFeature = Marshal.AllocCoTaskMem(feature.lFeatureSize);
            byte[] tmp_pFeature = new byte[feature.lFeatureSize];
            Marshal.Copy(pbFeature, tmp_pFeature, 0, feature.lFeatureSize);
            Marshal.Copy(tmp_pFeature, 0, feature.pbFeature, feature.lFeatureSize);
            return feature;
        }

		public void freeUnmanaged(){
            if(IntPtr.Zero != pbFeature){
                Marshal.FreeCoTaskMem(pbFeature);
                pbFeature = IntPtr.Zero;
            }
	    }

    }
	
	
}
