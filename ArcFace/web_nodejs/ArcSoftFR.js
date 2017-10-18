"use strict";
var StructType = require('ref-struct');
var ArrayType = require('ref-array');
var ref = require('ref');
var ffi = require('ffi');
var base = require('./ArcSoftBase.js');

var AFR_FSDK_Version = StructType({
    lCodebase: base.MInt32,
    lMajor: base.MInt32,
    lMinor: base.MInt32,
    lBuild: base.MInt32,
    lFeatureLevel: base.MInt32,
    Version: base.MPChar,
    BuildDate: base.MPChar,
    CopyRight: base.MPChar
});

var AFR_FSDK_FACERES = StructType({
    rcFace: base.MRECT,
    lOrient: base.MInt32
});

var AFR_FSDK_FACEMODEL = StructType({
    pbFeature: base.MHandleType,
    lFeatureSize: base.MInt32
});

var libname = process.platform == 'win32' ? './libarcsoft_fsdk_face_recognition.dll' : './libarcsoft_fsdk_face_recognition.so'
var Library = ffi.Library(libname, {
    'AFR_FSDK_GetVersion': [ref.refType(AFR_FSDK_Version), [base.MHandleType]],
    'AFR_FSDK_InitialEngine': [base.MLong, [base.MPChar, base.MPChar, base.MHandleType, base.MInt32, base.MHandleRefType]],
    'AFR_FSDK_UninitialEngine': [base.MLong, [base.MHandleType]],
    'AFR_FSDK_ExtractFRFeature': [base.MLong, [base.MHandleType, ref.refType(base.ASVLOFFSCREEN), ref.refType(AFR_FSDK_FACERES), ref.refType(AFR_FSDK_FACEMODEL)]],
    'AFR_FSDK_FacePairMatching': [base.MLong, [base.MHandleType, ref.refType(AFR_FSDK_FACEMODEL), ref.refType(AFR_FSDK_FACEMODEL), ref.refType(ref.types.float)]]
});

function extractFeature(hEngine, asvl, face) {
    var faceRes = new AFR_FSDK_FACERES();

    faceRes.rcFace.left = face.left;
    faceRes.rcFace.top = face.top;
    faceRes.rcFace.right = face.right;
    faceRes.rcFace.bottom = face.bottom;
    faceRes.lOrient = face.orient;
    var faceFeature = new AFR_FSDK_FACEMODEL();
    faceFeature.lFeatureSize = 0;
    faceFeature.pbFeature = ref.NULL;
    //console.log(asvl);
    var ret = Library.AFR_FSDK_ExtractFRFeature(hEngine, asvl.ref(), faceRes.ref(), faceFeature.ref());
    if (ret != 0) {
        console.log('AFR_FSDK_ExtractFRFeature ret == ' + ret);
        faceFeature.lFeatureSize = 0;
        faceFeature.pbFeature = ref.NULL;
        return faceFeature;
    } else {
        var faceFeatureCopy = new AFR_FSDK_FACEMODEL();
        faceFeatureCopy.lFeatureSize = faceFeature.lFeatureSize;
        faceFeatureCopy.pbFeature = base.malloc(faceFeatureCopy.lFeatureSize);
        base.memcpy(faceFeatureCopy.pbFeature.address(),
            faceFeature.pbFeature.address(),
            faceFeatureCopy.lFeatureSize);
        return faceFeatureCopy;
    }
}

function compareFaceSimilarity(hEngine, faceFeatureA, faceFeatureB) {
    var pfSimilScore = new Buffer(ref.sizeof.float);
    pfSimilScore.type = ref.refType(ref.types.float);
    pfSimilScore.writeFloatLE(0, 0.0);
    var ret = Library.AFR_FSDK_FacePairMatching(hEngine, faceFeatureA.ref(), faceFeatureB.ref(), pfSimilScore);
    if (ret != 0) {
        console.log('AFR_FSDK_FacePairMatching failed:ret == ' + ret);
        return 0.0;
    }
    return pfSimilScore.readFloatLE(0);
}

exports.AFR_FSDK_Version = AFR_FSDK_Version;
exports.AFR_FSDK_FACERES = AFR_FSDK_FACERES;
exports.AFR_FSDK_FACEMODEL = AFR_FSDK_FACEMODEL;
exports.AFR_FSDK_GetVersion = Library.AFR_FSDK_GetVersion;
exports.AFR_FSDK_InitialEngine = Library.AFR_FSDK_InitialEngine;
exports.AFR_FSDK_UninitialEngine = Library.AFR_FSDK_UninitialEngine;
exports.AFR_FSDK_ExtractFRFeature = Library.AFR_FSDK_ExtractFRFeature;
exports.AFR_FSDK_FacePairMatching = Library.AFR_FSDK_FacePairMatching;
exports.extractFeature = extractFeature;
exports.compareFaceSimilarity = compareFaceSimilarity;