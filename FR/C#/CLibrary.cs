using System;
using System.Runtime.InteropServices;

namespace arcsoft {
    public class CLibrary {
        [DllImport("msvcrt.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr memcpy(IntPtr dest, IntPtr src, int count);
    }
}
