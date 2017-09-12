using System;
using System.Runtime.InteropServices;

namespace arcsoft.demo {
    [StructLayout(LayoutKind.Sequential)]
    public struct AFD_FSDK_FACERES {
        public int nFace;
        public IntPtr rcFace;
        public IntPtr lfaceOrient;
    };
}
