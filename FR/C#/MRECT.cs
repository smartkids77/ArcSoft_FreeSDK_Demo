using System.Runtime.InteropServices;

namespace arcsoft {

    [StructLayout(LayoutKind.Sequential)]
    public struct MRECT {
        public int left;
        public int top;
        public int right;
        public int bottom;
    }
}