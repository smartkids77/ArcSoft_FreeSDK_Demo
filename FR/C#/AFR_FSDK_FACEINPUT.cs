using System;
using System.Runtime.InteropServices;

namespace arcsoft {
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct AFR_FSDK_FACEINPUT {
        public MRECT rcFace;
        public int lOrient;
    }
}
