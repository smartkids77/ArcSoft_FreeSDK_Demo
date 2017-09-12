using System.Runtime.InteropServices;

namespace arcsoft {
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct AFD_FSDK_Version {
        public int lCodebase;
        public int lMajor;
        public int lMinor;
        public int lBuild;
        public string Version;
        public string BuildDate;
        public string CopyRight;
    };
}
