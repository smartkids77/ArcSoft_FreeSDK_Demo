"use strict";
var StructType = require('ref-struct');
var ArrayType = require('ref-array');
var ref = require('ref');
var ffi = require('ffi');
var base = require('./ArcSoftBase.js');

var OrientPriority = {
    AFD_FSDK_OPF_0_ONLY: 0x1, // 0; 0; ...
    AFD_FSDK_OPF_90_ONLY: 0x2, // 90; 90; ...
    AFD_FSDK_OPF_270_ONLY: 0x3, // 270; 270; ...        
    AFD_FSDK_OPF_180_ONLY: 0x4, // 180; 180; ...
    AFD_FSDK_OPF_0_HIGHER_EXT: 0x5 // 0; 90; 270; 180; 0; 90; 270; 180; ...
};

var OrientCode = {
    AFD_FSDK_FOC_0: 0x1, // 0 degree
    AFD_FSDK_FOC_90: 0x2, // 90 degree
    AFD_FSDK_FOC_270: 0x3, // 270 degree
    AFD_FSDK_FOC_180: 0x4, // 180 degree
    AFD_FSDK_FOC_30: 0x5, // 30 degree
    AFD_FSDK_FOC_60: 0x6, // 60 degree
    AFD_FSDK_FOC_120: 0x7, // 120 degree
    AFD_FSDK_FOC_150: 0x8, // 150 degree
    AFD_FSDK_FOC_210: 0x9, // 210 degree
    AFD_FSDK_FOC_240: 0xa, // 240 degree
    AFD_FSDK_FOC_300: 0xb, // 300 degree
    AFD_FSDK_FOC_330: 0xc // 330 degree
};

var AFD_FSDK_Version = StructType({
    lCodebase: base.MInt32,
    lMajor: base.MInt32,
    lMinor: base.MInt32,
    lBuild: base.MInt32,
    Version: base.MPChar,
    BuildDate: base.MPChar,
    CopyRight: base.MPChar
});

var AFD_FSDK_FACERES = StructType({
    nFace: base.MInt32,
    rcFace: base.MIntPtr_t,
    lfaceOrient: base.MIntPtr_t
});

var LPAFD_FSDK_FACERES = StructType({
    pointer: ref.refType(AFD_FSDK_FACERES)
});

var libname = process.platform == 'win32' ? './libarcsoft_fsdk_face_detection.dll' : './libarcsoft_fsdk_face_detection.so'
var Library = ffi.Library(libname, {
    'AFD_FSDK_GetVersion': [ref.refType(AFD_FSDK_Version), [base.MHandleType]],
    'AFD_FSDK_InitialFaceEngine': [base.MLong, [base.MPChar, base.MPChar, base.MHandleType, base.MInt32, base.MHandleRefType, base.MInt32, base.MInt32, base.MInt32]],
    'AFD_FSDK_UninitialFaceEngine': [base.MLong, [base.MHandleType]],
    'AFD_FSDK_StillImageFaceDetection': [base.MLong, [base.MHandleType, ref.refType(base.ASVLOFFSCREEN), ref.refType(LPAFD_FSDK_FACERES)]]
});

function _process(hEngine, asvl, result_callback) {
    var pFaceRes = new LPAFD_FSDK_FACERES();
    pFaceRes.pointer = ref.NULL;
    var ppFaceRes = pFaceRes.ref();
    var ret = Library.AFD_FSDK_StillImageFaceDetection(hEngine, asvl.ref(), ppFaceRes);
    if (ret != 0) {
        console.log('AFD_FSDK_StillImageFaceDetection ret == ' + ret);
        result_callback(0, asvl, { nFace: 0, info: [] });
    } else {
        var pFaceRes = ppFaceRes.deref();
        var faceRes = pFaceRes.pointer.deref();
        var faces = {};
        faces.nFace = faceRes.nFace;
        faces.info = [];
        if (faceRes.nFace != 0) {
            var rect = new base.MRECT();
            var orientBuffer = new Buffer(base.MInt32.size);
            for (var i = 0; i < faceRes.nFace; i++) {
                base.memcpy(ref.address(rect.ref()),
                    faceRes.rcFace + base.MRECT.size * i,
                    base.MRECT.size);
                base.memcpy(ref.address(orientBuffer),
                    faceRes.lfaceOrient + base.MInt32.size * i,
                    base.MInt32.size);
                var _orient = orientBuffer.readInt32LE(0);
                faces.info.push({ left: rect.left, top: rect.top, right: rect.right, bottom: rect.bottom, orient: _orient });
            }
        }
        result_callback(0, asvl, faces);
    }
}

exports.OrientPriority = OrientPriority;
exports.OrientCode = OrientCode
exports.AFD_FSDK_Version = AFD_FSDK_Version;
exports.AFD_FSDK_FACERES = AFD_FSDK_FACERES;
exports.LPAFD_FSDK_FACERES = LPAFD_FSDK_FACERES;
exports.AFD_FSDK_GetVersion = Library.AFD_FSDK_GetVersion;
exports.AFD_FSDK_InitialFaceEngine = Library.AFD_FSDK_InitialFaceEngine;
exports.AFD_FSDK_UninitialFaceEngine = Library.AFD_FSDK_UninitialFaceEngine;
exports.AFD_FSDK_StillImageFaceDetection = Library.AFD_FSDK_StillImageFaceDetection;
exports.process = _process;