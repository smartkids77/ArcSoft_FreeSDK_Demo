"use strict";
var ffi = require('ffi');
var ref = require('ref');
var fs = require('fs');
var ArcSoftFD = require('./ArcSoftFD.js');
var ArcSoftFR = require('./ArcSoftFR.js');
var ArcSoftBase = require('./ArcSoftBase.js');

var APPID = 'XXXXXXXXXX';
var FD_SDKKEY = 'YYYYYYYYYY';
var FR_SDKKEY = 'ZZZZZZZZZZ'

//init Engine
var MAX_FACE_NUM = 50;
var FD_WORKBUF_SIZE = 20 * 1024 * 1024;
var FR_WORKBUF_SIZE = 40 * 1024 * 1024;
var pFDWorkMem = ArcSoftBase.malloc(FD_WORKBUF_SIZE);
var pFRWorkMem = ArcSoftBase.malloc(FR_WORKBUF_SIZE);

var phFDEngine = ref.ref(new Buffer(ArcSoftBase.MIntPtr_t.size));
var ret = ArcSoftFD.AFD_FSDK_InitialFaceEngine(APPID, FD_SDKKEY, pFDWorkMem, FD_WORKBUF_SIZE, phFDEngine, ArcSoftFD.OrientPriority.AFD_FSDK_OPF_0_HIGHER_EXT, 32, MAX_FACE_NUM);
if (ret != 0) {
    ArcSoftBase.free(pFDWorkMem);
    ArcSoftBase.free(pFRWorkMem);
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

var phFREngine = ref.ref(new Buffer(ArcSoftBase.MIntPtr_t.size));
ret = ArcSoftFR.AFR_FSDK_InitialEngine(APPID, FR_SDKKEY, pFRWorkMem, FR_WORKBUF_SIZE, phFREngine);
if (ret != 0) {
    ArcSoftFD.AFD_FSDK_UninitialFaceEngine(hFDEngine);
    ArcSoftBase.free(pFDWorkMem);
    ArcSoftBase.free(pFRWorkMem);
    console.log('AFR_FSDK_InitialEngine ret == ' + ret);
    System.exit(0);
}
var hFREngine = ref.deref(phFREngine);

//print FR Engine version
var pVersionFR = ArcSoftFR.AFR_FSDK_GetVersion(hFREngine);
var versionFR = pVersionFR.deref();
console.log('' + versionFR.lCodebase + ' ' + versionFR.lMajor + ' ' + versionFR.lMinor + ' ' + versionFR.lBuild);
console.log(versionFR.Version);
console.log(versionFR.BuildDate);
console.log(versionFR.CopyRight);

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
    ArcSoftFR.AFR_FSDK_UninitialEngine(hFREngine);
    ArcSoftBase.free(pFDWorkMem);
    ArcSoftBase.free(pFRWorkMem);
    process.exit();
});

function compareTwoImage(filenameA, filenameB, result_callback) {
    doFaceDetection(filenameA, function(err, asvlA, facesA) {
        if (err) throw err;
        if (facesA.nFace < 1) {
            console.log("no face in Image A " + filenameA);
            result_callback(0, filenameA, filenameB, 0.0);
            return;
        }

        doFaceDetection(filenameB, function(err, asvlB, facesB) {
            if (err) throw err;
            if (facesB.nFace < 1) {
                console.log("no face in Image A " + filenameB);
                result_callback(0, filenameA, filenameB, 0.0);
                return;
            }

            //for (var i = 0; i < facesA.nFace; i++) {
            //  console.log('A:'+i+' ('+facesA.info[i].left+' '+facesA.info[i].top+' '+facesA.info[i].right+' '+facesA.info[i].bottom+')'+' orient '+facesA.info[i].orient);
            //}

            var featureA = ArcSoftFR.extractFeature(hFREngine, asvlA, facesA.info[0]);
            if (featureA.lFeatureSize == 0) {
                console.log("extract face feature in Image A failed " + filenameA);
                result_callback(0, filenameA, filenameB, 0.0);
                return;
            }

            //for (var i = 0; i < facesB.nFace; i++) {
            //  console.log('B:'+i+' ('+facesB.info[i].left+' '+facesB.info[i].top+' '+facesB.info[i].right+' '+facesB.info[i].bottom+')'+' orient '+facesB.info[i].orient);
            //}
            var featureB = ArcSoftFR.extractFeature(hFREngine, asvlB, facesB.info[0]);
            if (featureB.lFeatureSize == 0) {
                ArcSoftBase.free(featureA.pbFeature);
                console.log("extract face feature in Image B failed " + filenameB);
                result_callback(0, filenameA, filenameB, 0.0);
                return;
            }
            var score = ArcSoftFR.compareFaceSimilarity(hFREngine, featureA, featureB)
            ArcSoftBase.free(featureA.pbFeature);
            ArcSoftBase.free(featureB.pbFeature);
            result_callback(0, filenameA, filenameB, score);
        });
    });
}

compareTwoImage('001.jpg', '1_9.jpg', function(err, filenameA, filenameB, score) {
    console.log("similarity between faceA and faceB is " + score);
});

compareTwoImage('003.jpg', '1_9.jpg', function(err, filenameA, filenameB, score) {
    console.log("similarity between faceA and faceB is " + score);
});

compareTwoImage('003.jpg', 'fgg_003.jpg', function(err, filenameA, filenameB, score) {
    console.log("similarity between faceA and faceB is " + score);
});