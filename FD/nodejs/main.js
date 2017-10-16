"use strict";
var ffi = require('ffi');
var ref = require('ref');
var fs = require('fs');
var ArcSoftFD = require('./ArcSoftFD.js');
var ArcSoftBase = require('./ArcSoftBase.js');

var APPID = 'XXXXXXXXXX';
var FD_SDKKEY = 'YYYYYYYYYY';

//init Engine
var MAX_FACE_NUM = 50;
var FD_WORKBUF_SIZE = 20 * 1024 * 1024;
var pFDWorkMem = ArcSoftBase.malloc(FD_WORKBUF_SIZE);
var phFDEngine = ref.ref(new Buffer(ArcSoftBase.MIntPtr_t.size));
var ret = ArcSoftFD.AFD_FSDK_InitialFaceEngine(APPID, FD_SDKKEY, pFDWorkMem, FD_WORKBUF_SIZE, phFDEngine, ArcSoftFD.OrientPriority.AFD_FSDK_OPF_0_HIGHER_EXT, 32, MAX_FACE_NUM);
if (ret != 0) {
    ArcSoftBase.free(pFDWorkMem);
    console.log('AFD_FSDK_InitialFaceEngine ret == ' + ret);
    process.exit();
}
var hFDEngine = ref.deref(phFDEngine);

//print FD Engine version
var pVersionFD = ArcSoftFD.AFD_FSDK_GetVersion(hFDEngine);
var versionFD = pVersionFD.deref();
console.log('' + versionFD.lCodebase + ' ' + versionFD.lMajor + ' ' + versionFD.lMinor + ' ' + versionFD.lBuild);
console.log(versionFD.Version);
console.log(versionFD.BuildDate);
console.log(versionFD.CopyRight);

function doFaceDetection(filename, faces_callback, width, height, format) {

    if (arguments.length === 2) {
        ArcSoftBase.loadImage(filename, function(err, inputImage) {
            if (err) throw err;
            ArcSoftFD.process(hFDEngine, inputImage, faces_callback);
        });
    } else if (arguments.length === 5) {
        ArcSoftBase.loadYUVImage(filename, width, height, format, (err, inputImage) => {
            if (err) throw err;
            ArcSoftFD.process(hFDEngine, inputImage, faces_callback);
        });

    } else {
        throw new Error('wrong number of arguments');
    }
}

if (process.platform === "win32") {
    var rl = require("readline").createInterface({
        input: process.stdin,
        output: process.stdout
    });

    rl.on("SIGINT", function() {
        process.emit("SIGINT");
    });
}

process.on("SIGINT", function() {
    //uninit Engine
    console.log('release resource');
    ArcSoftFD.AFD_FSDK_UninitialFaceEngine(hFDEngine);
    ArcSoftBase.free(pFDWorkMem);
    process.exit();
});

doFaceDetection('xxx.bmp', function(err, asvl, faces) {
    if (err) throw err;
    for (var i = 0; i < faces.nFace; i++) {
        console.log('A:' + i + ' (' + faces.info[i].left + ' ' + faces.info[i].top + ' ' + faces.info[i].right + ' ' + faces.info[i].bottom + ')' + ' orient ' + faces.info[i].orient);
    }
});

doFaceDetection('003.jpg', function(err, asvl, faces) {
    if (err) throw err;
    for (var i = 0; i < faces.nFace; i++) {
        console.log('B:' + i + ' (' + faces.info[i].left + ' ' + faces.info[i].top + ' ' + faces.info[i].right + ' ' + faces.info[i].bottom + ')' + ' orient ' + faces.info[i].orient);
    }
});

doFaceDetection('222.png', function(err, asvl, faces) {
    if (err) throw err;
    for (var i = 0; i < faces.nFace; i++) {
        console.log('C:' + i + ' (' + faces.info[i].left + ' ' + faces.info[i].top + ' ' + faces.info[i].right + ' ' + faces.info[i].bottom + ')' + ' orient ' + faces.info[i].orient);
    }
});

doFaceDetection('20170911160206.jpg', function(err, asvl, faces) {
    if (err) throw err;
    for (var i = 0; i < faces.nFace; i++) {
        console.log('C:' + i + ' (' + faces.info[i].left + ' ' + faces.info[i].top + ' ' + faces.info[i].right + ' ' + faces.info[i].bottom + ')' + ' orient ' + faces.info[i].orient);
    }
});

doFaceDetection('001_640x480_I420.YUV', function(err, asvl, faces) {
    if (err) throw err;
    for (var i = 0; i < faces.nFace; i++) {
        console.log('D:' + i + ' (' + faces.info[i].left + ' ' + faces.info[i].top + ' ' + faces.info[i].right + ' ' + faces.info[i].bottom + ')' + ' orient ' + faces.info[i].orient);
    }
}, 640, 480, ArcSoftBase.Format.ASVL_PAF_I420);

doFaceDetection('001_640x480_NV12.YUV', function(err, asvl, faces) {
    if (err) throw err;
    for (var i = 0; i < faces.nFace; i++) {
        console.log('E:' + i + ' (' + faces.info[i].left + ' ' + faces.info[i].top + ' ' + faces.info[i].right + ' ' + faces.info[i].bottom + ')' + ' orient ' + faces.info[i].orient);
    }
}, 640, 480, ArcSoftBase.Format.ASVL_PAF_NV12);

doFaceDetection('001_640x480_NV21.YUV', function(err, asvl, faces) {
    if (err) throw err;
    for (var i = 0; i < faces.nFace; i++) {
        console.log('F:' + i + ' (' + faces.info[i].left + ' ' + faces.info[i].top + ' ' + faces.info[i].right + ' ' + faces.info[i].bottom + ')' + ' orient ' + faces.info[i].orient);
    }
}, 640, 480, ArcSoftBase.Format.ASVL_PAF_NV21);

doFaceDetection('001_640x480_YUYV.YUV', function(err, asvl, faces) {
    if (err) throw err;
    for (var i = 0; i < faces.nFace; i++) {
        console.log('G:' + i + ' (' + faces.info[i].left + ' ' + faces.info[i].top + ' ' + faces.info[i].right + ' ' + faces.info[i].bottom + ')' + ' orient ' + faces.info[i].orient);
    }
}, 640, 480, ArcSoftBase.Format.ASVL_PAF_YUYV);