"use strict";
var StructType = require('ref-struct');
var ArrayType = require('ref-array');
var ref = require('ref');
var ffi = require('ffi');
var fs = require('fs');
var Jimp = require("jimp");

var Format = {
    ASVL_PAF_I420:0x601,
    ASVL_PAF_NV12:0x801,
    ASVL_PAF_NV21:0x802,
    ASVL_PAF_YUYV:0x501
};

var MInt32 = ref.types.int32;
var MUInt32 = ref.types.uint32;
var MLong = ref.types.long;
var MIntPtr_t;
if(process.arch == 'x64'){
    MIntPtr_t = ref.types.uint64;
}else{
    MIntPtr_t = ref.types.uint32;
}
var MPChar = ref.types.CString;
var MHandleType = ref.refType(ref.types.void); 
var MHandleRefType = ref.refType(MHandleType); 
var MUInt8RefType = ref.refType(ref.types.uint8); 

var ASVLOFFSCREEN = StructType({
    u32PixelArrayFormat:MUInt32,
    i32Width:MInt32,
    i32Height:MInt32,
    ppu8Plane:ArrayType(MHandleType, 4),
    pi32Pitch:ArrayType(MInt32, 4) 
});

var MRECT = StructType({
    left:MInt32,
    top:MInt32,
    right:MInt32,
    bottom:MInt32
});

var libc_name = process.platform == 'win32' ? 'msvcrt' : 'libc';
var CLibrary = ffi.Library(libc_name, {
    'memcpy': [ ref.refType(ref.types.void), [ MIntPtr_t,MIntPtr_t,ref.types.size_t] ],
    'malloc': [ ref.refType(ref.types.void), [ ref.types.size_t] ],
    'free': [ ref.types.void, [ref.refType(ref.types.void)] ]
});

function loadYUVImage(filePath,width,height,format,result_callback){
    fs.readFile(filePath, (err, yuvdata) => {
        if (err) throw err;

        var asvl = new ASVLOFFSCREEN();
        asvl.u32PixelArrayFormat = format;
        asvl.i32Width = width;
        asvl.i32Height = height;

        if (Format.ASVL_PAF_I420 == asvl.u32PixelArrayFormat) {
            asvl.pi32Pitch[0] = asvl.i32Width;
            asvl.pi32Pitch[1] = asvl.i32Width/2;
            asvl.pi32Pitch[2] = asvl.i32Width/2;
            asvl.pi32Pitch[3] = 0;

            asvl.gc_ppu8Plane0 = yuvdata.slice(0,asvl.pi32Pitch[0]*asvl.i32Height);
            asvl.gc_ppu8Plane1 = yuvdata.slice(asvl.pi32Pitch[0]*asvl.i32Height,asvl.pi32Pitch[1]*asvl.i32Height/2);
            asvl.gc_ppu8Plane2 = yuvdata.slice(asvl.pi32Pitch[0]*asvl.i32Height+asvl.pi32Pitch[1]*asvl.i32Height/2,asvl.pi32Pitch[2]*asvl.i32Height/2);

            asvl.ppu8Plane[0] = asvl.gc_ppu8Plane0;
            asvl.ppu8Plane[1] = asvl.gc_ppu8Plane1;
            asvl.ppu8Plane[2] = asvl.gc_ppu8Plane2;
            asvl.ppu8Plane[3] = ref.NULL;
        } else if (Format.ASVL_PAF_NV12 == asvl.u32PixelArrayFormat) {
            asvl.pi32Pitch[0] = asvl.i32Width;
            asvl.pi32Pitch[1] = asvl.i32Width;
            asvl.pi32Pitch[2] = 0;
            asvl.pi32Pitch[3] = 0;

            asvl.gc_ppu8Plane0 = yuvdata.slice(0,asvl.pi32Pitch[0]*asvl.i32Height);
            asvl.gc_ppu8Plane1 = yuvdata.slice(asvl.pi32Pitch[0]*asvl.i32Height,asvl.pi32Pitch[1]*asvl.i32Height/2);

            asvl.ppu8Plane[0] = asvl.gc_ppu8Plane0;
            asvl.ppu8Plane[1] = asvl.gc_ppu8Plane1;
            asvl.ppu8Plane[2] = ref.NULL;
            asvl.ppu8Plane[3] = ref.NULL;
        } else if (Format.ASVL_PAF_NV21 == asvl.u32PixelArrayFormat) {
            asvl.pi32Pitch[0] = asvl.i32Width;
            asvl.pi32Pitch[1] = asvl.i32Width;
            asvl.pi32Pitch[2] = 0;
            asvl.pi32Pitch[3] = 0;

            asvl.gc_ppu8Plane0 = yuvdata.slice(0,asvl.pi32Pitch[0]*asvl.i32Height);
            asvl.gc_ppu8Plane1 = yuvdata.slice(asvl.pi32Pitch[0]*asvl.i32Height,asvl.pi32Pitch[1]*asvl.i32Height/2);
            
            asvl.ppu8Plane[0] = asvl.gc_ppu8Plane0;
            asvl.ppu8Plane[1] = asvl.gc_ppu8Plane1;
            asvl.ppu8Plane[2] = ref.NULL;
            asvl.ppu8Plane[3] = ref.NULL;
        } else if (Format.ASVL_PAF_YUYV == asvl.u32PixelArrayFormat) {
            asvl.pi32Pitch[0] = asvl.i32Width*2;
            asvl.pi32Pitch[1] = 0;
            asvl.pi32Pitch[2] = 0;
            asvl.pi32Pitch[3] = 0;

            asvl.gc_ppu8Plane0 = yuvdata;
            
            asvl.ppu8Plane[0] = asvl.gc_ppu8Plane0;
            asvl.ppu8Plane[1] = ref.NULL;
            asvl.ppu8Plane[2] = ref.NULL;
            asvl.ppu8Plane[3] = ref.NULL;
        }else{
            throw new Error('unsupported yuv format');
        }

        result_callback(0,asvl);
    });
}

function loadImage(filePath,result_callback) {
    Jimp.read(filePath, function (err, img) {
        if (err) throw err;

        if(((img.bitmap.width&0x1)!= 0)||((img.bitmap.height&0x1) != 0)){
            img = img.crop(0, 0, img.bitmap.width&0xFFFFFFFE, img.bitmap.height&0xFFFFFFFE);
        }

        var y_channel = new Buffer(img.bitmap.width*img.bitmap.height)
        var u_channel = new Buffer(img.bitmap.width*img.bitmap.height/4)
        var v_channel = new Buffer(img.bitmap.width*img.bitmap.height/4)
        img.scan(0, 0, img.bitmap.width, img.bitmap.height, function (i, j, idx) {
            var r   = this.bitmap.data[ idx + 0 ];
            var g = this.bitmap.data[ idx + 1 ];
            var b  = this.bitmap.data[ idx + 2 ];
            
            var y = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
            var u = ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
            var v = ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;

            y = y < 0 ? 0 : (y > 255 ? 255 : y);
            u = u < 0 ? 0 : (u > 255 ? 255 : u);
            v = v < 0 ? 0 : (v > 255 ? 255 : v);

            y_channel[j * this.bitmap.width + i] = y;
            u_channel[(j >> 1) * (this.bitmap.width>>1)+(i>>1)] = u;
            v_channel[(j >> 1) * (this.bitmap.width>>1)+(i>>1)] = v;
        });

        var asvl = new ASVLOFFSCREEN();
        asvl.u32PixelArrayFormat = Format.ASVL_PAF_I420;
        asvl.i32Width = img.bitmap.width;
        asvl.i32Height = img.bitmap.height;
        asvl.pi32Pitch[0] = asvl.i32Width;
        asvl.pi32Pitch[1] = asvl.i32Width/2;
        asvl.pi32Pitch[2] = asvl.i32Width/2;
        asvl.pi32Pitch[3] = 0;

        asvl.ppu8Plane[0] = y_channel;
        asvl.ppu8Plane[1] = u_channel;
        asvl.ppu8Plane[2] = v_channel;
        asvl.ppu8Plane[3] = ref.NULL;

        //make a strong reference, prevent garbage collection to free the memory
        asvl.gc_ppu8Plane0 = y_channel;
        asvl.gc_ppu8Plane1 = u_channel;
        asvl.gc_ppu8Plane2 = v_channel;

        if(false){
            console.log('do dump');
            console.log("y_channel size "+y_channel.length);
            console.log("u_channel size "+u_channel.length);
            console.log("v_channel size "+v_channel.length);
            fs.writeFileSync(filename+'_'+img.bitmap.width+'x'+img.bitmap.height+'_I420.YUV',y_channel);
            fs.writeFileSync(filename+'_'+img.bitmap.width+'x'+img.bitmap.height+'_I420.YUV',u_channel,{flag:'a'});
            fs.writeFileSync(filename+'_'+img.bitmap.width+'x'+img.bitmap.height+'_I420.YUV',v_channel,{flag:'a'});
        }
        result_callback(0,asvl);
    });
}

exports.MRECT = MRECT;
exports.ASVLOFFSCREEN = ASVLOFFSCREEN;
exports.Format = Format;
exports.MInt32 = MInt32;
exports.MLong = MLong;
exports.MIntPtr_t = MIntPtr_t;
exports.MPChar = MPChar;
exports.MHandleType = MHandleType; 
exports.MHandleRefType = MHandleRefType; 
exports.MUInt8RefType = MUInt8RefType; 
exports.memcpy = CLibrary.memcpy; 
exports.malloc = CLibrary.malloc; 
exports.free = CLibrary.free; 
exports.loadYUVImage = loadYUVImage; 
exports.loadImage = loadImage; 