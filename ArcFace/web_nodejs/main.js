"use strict";
var http = require('http');
var url = require('url');
var fs = require('fs');
var path = require('path');
var cur_html = fs.readFileSync(path.resolve(__dirname, 'index.html'));
var cur_jquery = fs.readFileSync(path.resolve(__dirname, 'jquery-3.2.1.min.js'));

var ffi = require('ffi');
var ref = require('ref');
var fs = require('fs');
var ArcSoftFD = require('./ArcSoftFD.js');
var ArcSoftFR = require('./ArcSoftFR.js');
var ArcSoftBase = require('./ArcSoftBase.js');

var APPID  = 'XXXXXXXXXX';
var FD_SDKKEY = 'YYYYYYYYYY';
var FR_SDKKEY = 'ZZZZZZZZZZ';

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

function compareTwoImage(ImageA, ImageB, result_callback) {
    var compareTwoImage_arguments = arguments;
    doFaceDetection(ImageA, function(err, asvlA, facesA) {
        if (err) throw err;
        if (facesA.nFace < 1) {
            console.log("no face in Image A ");
            result_callback(0, ImageA, ImageB, 0.0);
            return;
        }

        var faceAIndex = 0;
        if (compareTwoImage_arguments.length >= 5) {
            if (facesA.nFace <= compareTwoImage_arguments[3]) {
                console.log("face index out of range in Image A ");
                result_callback(0, ImageA, ImageB, 0.0);
                return;
            }
            faceAIndex = compareTwoImage_arguments[3];
        }

        doFaceDetection(ImageB, function(err, asvlB, facesB) {
            if (err) throw err;
            if (facesB.nFace < 1) {
                console.log("no face in Image B ");
                result_callback(0, ImageA, ImageB, 0.0);
                return;
            }

            var faceBIndex = 0;
            if (compareTwoImage_arguments.length >= 5) {
                if (facesB.nFace <= compareTwoImage_arguments[4]) {
                    console.log("face index out of range in Image B ");
                    result_callback(0, ImageA, ImageB, 0.0);
                }
                faceBIndex = compareTwoImage_arguments[4];
            }

            var featureA = ArcSoftFR.extractFeature(hFREngine, asvlA, facesA.info[faceAIndex]);
            if (featureA.lFeatureSize == 0) {
                console.log("extract face feature in Image A failed ");
                result_callback(0, ImageA, ImageB, 0.0);
                return;
            }

            var featureB = ArcSoftFR.extractFeature(hFREngine, asvlB, facesB.info[faceBIndex]);
            if (featureB.lFeatureSize == 0) {
                ArcSoftBase.free(featureA.pbFeature);
                console.log("extract face feature in Image B failed ");
                result_callback(0, ImageA, ImageB, 0.0);
                return;
            }
            var score = ArcSoftFR.compareFaceSimilarity(hFREngine, featureA, featureB)
            ArcSoftBase.free(featureA.pbFeature);
            ArcSoftBase.free(featureB.pbFeature);
            result_callback(0, ImageA, ImageB, score);
        });
    });
}

http.createServer(function(request, response) {
    var postData = '';
    var pathname = url.parse(request.url).pathname;
    console.log('Request for ' + pathname + ' received.');

    if ('/' == pathname) {
        response.writeHead(200, {
            'Content-Type': 'text/html; charset=utf8'
        });
        response.write(cur_html);
        response.end();
    } else if ('/jquery-3.2.1.min.js' == pathname) {
        response.writeHead(200, {
            'Content-Type': 'application/x-javascript'
        });
        response.write(cur_jquery);
        response.end();
    } else if ('/dofd' == pathname) {
        request.setEncoding('utf8');
        request.addListener('data', function(postDataChunk) {
            postData += postDataChunk;
        });

        request.addListener('end', function() {
            if (postData != '') {
                var imageRawBase64 = postData.substr(postData.indexOf('base64') + 7)
                var imageRawBuffer = new Buffer(imageRawBase64, 'base64');

                doFaceDetection(imageRawBuffer, function(err, asvlB, faces) {
                    if (err) throw err;
                    response.writeHead(200, {
                        'Content-Type': 'application/json'
                    });
                    response.write(JSON.stringify(faces));
                    response.end();
                });
            }
        });
    } else if ('/dofr' == pathname) {
        request.setEncoding('utf8');
        request.addListener('data', function(postDataChunk) {
            postData += postDataChunk;
        });

        request.addListener('end', function() {
            var fr_result = {};
            if (postData != '') {
                try {
                    var frImageData = JSON.parse(postData);
                    var leftImageRawBase64 = frImageData.leftImage.substr(frImageData.leftImage.indexOf('base64') + 7)
                    var leftImage = new Buffer(leftImageRawBase64, 'base64');
                    var rightImageRawBase64 = frImageData.rightImage.substr(frImageData.rightImage.indexOf('base64') + 7)
                    var rightImage = new Buffer(rightImageRawBase64, 'base64');

                    compareTwoImage(leftImage, rightImage, function(err, BufferA, BufferB, score) {
                        if (err) {
                            fr_result.error = -3;
                            fr_result.message = 'engine internal error';
                        } else {
                            fr_result.error = 0;
                            fr_result.message = 'success';
                            fr_result.score = score;
                        }

                        response.writeHead(200, {
                            'Content-Type': 'application/json'
                        });
                        response.write(JSON.stringify(fr_result));
                        response.end();
                    }, frImageData.leftIndex, frImageData.rightIndex);

                    return;
                } catch (e) {
                    fr_result.error = -2;
                    fr_result.message = 'invalid json string';
                }

            } else {
                fr_result.error = -1;
                fr_result.message = 'no data in http post';
            }

            response.writeHead(200, {
                'Content-Type': 'application/json'
            });
            response.write(JSON.stringify(fr_result));
            response.end();
        });
    } else {
        response.writeHead(400);
        response.end();
    }

}).listen(7777, '0.0.0.0');