var ref = require('ref');
var ffi = require('ffi');
var StructType = require('ref-struct')
var ArrayType = require('ref-array')
var fs = require('fs');

//var FD_APPID  = 'XXXXXXXXXXXXXXXXXXXXXX';
//var FD_SDKKEY = 'YYYYYYYYYYYYYYYYYYYYYY';

var MInt32 = ref.types.int32;
var MPChar = ref.types.CString;
var VoidPointerType = ref.refType(ref.types.void); 
var MHandleType = VoidPointerType; 
var MHandleRefType = ref.refType(MHandleType); 
var MUInt8RefType = ref.refType(ref.types.uint8); 

var AFD_FSDK_Version = StructType({
    lCodebase:MInt32,
    lMajor:MInt32,
    lMinor:MInt32,
    lBuild:MInt32,
    Version:MPChar, 
    BuildDate:MPChar,
    CopyRight:MPChar 
});

var ASVLOFFSCREEN = StructType({
    u32PixelArrayFormat:ref.types.uint32,
    i32Width:MInt32,
    i32Height:MInt32,
    ppu8Plane:ArrayType(MUInt8RefType, 4) ,
    pi32Pitch:ArrayType(MInt32, 4) 
});

var MRECT = StructType({
    left:MInt32,
    top:MInt32,
    right:MInt32,
    bottom:MInt32,
});

var AFD_FSDK_FACERES = StructType({
    nFace:MInt32,
    rcFace:ref.types.long,
    lfaceOrient:ref.types.long
});

var LPAFD_FSDK_FACERES = StructType({
    pointer:ref.refType(AFD_FSDK_FACERES)
});

var FDLibrary = ffi.Library('libarcsoft_fsdk_face_detection.dll', {
      'AFD_FSDK_GetVersion': [ ref.refType(AFD_FSDK_Version), [MHandleType] ],
      'AFD_FSDK_InitialFaceEngine': [ref.types.long, [MPChar,MPChar,VoidPointerType,MInt32,MHandleRefType,MInt32,MInt32,MInt32]],
      'AFD_FSDK_UninitialFaceEngine': [ ref.types.long, [MHandleType] ],
      'AFD_FSDK_StillImageFaceDetection':[ ref.types.long, [MHandleType,ref.refType(ASVLOFFSCREEN),ref.refType(LPAFD_FSDK_FACERES)]]
});

var libc_name = process.platform == 'win32' ? 'msvcrt' : 'libc'
var CLibrary = ffi.Library(libc_name, {
    'memcpy': [ VoidPointerType, [ ref.types.long,ref.types.long,ref.types.size_t] ],
    'malloc': [ VoidPointerType, [ ref.types.size_t] ],
    'free': [ ref.types.void, [ VoidPointerType] ]
})

var lMemSize = 20*1024*1024;
var pMem = CLibrary.malloc(lMemSize);
var phEngine = ref.ref(new Buffer(ref.types.long.size));

var ret = FDLibrary.AFD_FSDK_InitialFaceEngine(FD_APPID,FD_SDKKEY,pMem,lMemSize,phEngine,0x5,16,50);
if(ret != 0){
     console.log('AFD_FSDK_InitialFaceEngine ret == '+ret);
     process.exit();
}
hEngine = ref.deref(phEngine);

var pVersion = FDLibrary.AFD_FSDK_GetVersion(hEngine);
var version = pVersion.deref();
console.log(''+version.lCodebase+' '+version.lMajor+' '+version.lMinor+' '+version.lBuild);
console.log(version.Version);
console.log(version.BuildDate);
console.log(version.CopyRight);

var inputImg = new ASVLOFFSCREEN();
inputImg.u32PixelArrayFormat = 0x601;
inputImg.i32Width = 640;
inputImg.i32Height = 480;
inputImg.pi32Pitch[0] = inputImg.i32Width;
inputImg.pi32Pitch[1] = inputImg.i32Width/2;
inputImg.pi32Pitch[2] = inputImg.i32Width/2;
inputImg.pi32Pitch[3] = 0;

var curYUVData = fs.readFileSync('003_640x480_I420.YUV');
inputImg.ppu8Plane[0] = curYUVData.slice(0,inputImg.pi32Pitch[0]*inputImg.i32Height);
inputImg.ppu8Plane[1] = curYUVData.slice(inputImg.pi32Pitch[0]*inputImg.i32Height,inputImg.pi32Pitch[1]*inputImg.i32Height/2);
inputImg.ppu8Plane[2] = curYUVData.slice(inputImg.pi32Pitch[0]*inputImg.i32Height+inputImg.pi32Pitch[1]*inputImg.i32Height/2,inputImg.pi32Pitch[2]*inputImg.i32Height/2);
inputImg.ppu8Plane[3] = ref.NULL;

var pFaceRes = new LPAFD_FSDK_FACERES();
pFaceRes.pointer = ref.NULL;
var ppFaceRes = pFaceRes.ref()
ret = FDLibrary.AFD_FSDK_StillImageFaceDetection(hEngine,inputImg.ref(),ppFaceRes);
if (ret != 0) {
    console.log('AFD_FSDK_StillImageFaceDetection ret == '+ret);
    process.exit();
}

var pFaceRes = ppFaceRes.deref();
var faceRes = pFaceRes.pointer.deref();

if(faceRes.nFace != 0){
    //trick 
    var rect = new MRECT();  
    var orientBuffer = new Buffer(MInt32.size);
    for (var i = 0; i < faceRes.nFace; i++) {
        CLibrary.memcpy(ref.address(rect.ref()),
                        faceRes.rcFace+MRECT.size*i,
                        MRECT.size);
        CLibrary.memcpy(ref.address(orientBuffer),
                        faceRes.lfaceOrient+MInt32.size*i,
                        MInt32.size);
        orient = orientBuffer.readInt32LE(0);
        console.log(''+i+' ('+rect.left+' '+rect.top+' '+rect.right+' '+rect.bottom+')'+' orient '+orient);
    }
}

FDLibrary.AFD_FSDK_UninitialFaceEngine(hEngine);
CLibrary.free(pMem);
