#include "glhelper.h"

#include <QOpenGLFunctions>
#include <QDebug>

GLHelper::GLHelper(){
	initializeOpenGLFunctions();
	mFormat = 0;
	mWidth = 0;
	mHeight = 0;
    bFrontCamera = false;
    mCameraDegree = 0;

	mTextureYid = 0;
	mTextureUid = 0;
	mTextureVid = 0;
}

bool GLHelper::setMirror(bool newBool){
	bool oldBool = bFrontCamera;
	bFrontCamera = newBool;
	return oldBool;
}

int GLHelper::setDegree(int newDegree){
	int oldDegree = mCameraDegree;
	mCameraDegree = newDegree;
	return oldDegree;
}

void GLHelper::drawTexture(unsigned char* indata, int format, int width, int height) {

	float projMatrix[16];
	if (bFrontCamera) {
		frustumM(projMatrix, 0.5f, -0.5f, -0.5f, 0.5f, 2.0f, 6.0f);
	} else {
		frustumM(projMatrix, -0.5f, 0.5f, -0.5f, 0.5f, 2.0f, 6.0f);
	}
	float mvMatrix[16];
	lookatM(mvMatrix, 0.0f, 0.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	float vertices[] = {
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
	};

	float texCoords_base[8] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
	float texCoords[8];

	float rotateMatrix[16];
	setRotateM(rotateMatrix, (float)mCameraDegree, 0.0f, 0.0f, 1.0f);
	for (int i = 0; i < 4; i++) {
		float inpos[4];
		float outpos[4];
		inpos[0] = texCoords_base[i * 2] * 2.0f - 1.0f;
		inpos[1] = texCoords_base[i * 2 + 1] * 2.0f - 1.0f;
		inpos[2] = 0.0f;
		inpos[3] = 1.0f;
		matrixMultPos(rotateMatrix, inpos, outpos);
		texCoords[i * 2] = (outpos[0] + 1.0f) / 2.0f;;
		texCoords[i * 2 + 1] = (outpos[1] + 1.0f) / 2.0f;;
	}
	drawTexture(indata, format, width, height, projMatrix, mvMatrix, &vertices[0], 4, &texCoords[0], 4, nullptr, 0, GL_TRIANGLE_STRIP);
}

void GLHelper::drawTexture(unsigned char* indata, int format, int width, int height, float *projMatrix, float *mvMatrix, float *pVertex, int vertexNum, float *pTexture, int textureNum, uint16_t* pIndex, int indexNum, GLuint mode) {

    if ((format != ASVL_PAF_GRAY)  && (format != ASVL_PAF_NV12) && (format != ASVL_PAF_NV21) && (format != ASVL_PAF_YUYV) && (format != ASVL_PAF_UYVY)
        && (format != ASVL_PAF_I444) && (format != ASVL_PAF_I422H) && (format != ASVL_PAF_I420) && (format != ASVL_PAF_BGR) && (format != ASVL_PAF_RGB)
            && (format != ASVL_PAF_RGBA) && (format != ASVL_PAF_RGBX)&&(format != ASVL_PAF_BGRA)) {
		qDebug() << "drawTexture:format not support";
		return;
	}

	GLuint programid;
	if (format == ASVL_PAF_GRAY) {
		programid = createProgram(ShaderTextureGRAY, VertexShaderTexture, FragmentShaderTextureGRAY);
		GL_CHECK(glUniform1i(glGetUniformLocation(programid, "u_textureY"), 0 + 4));
	} else if (format == ASVL_PAF_NV12) {
		programid = createProgram(ShaderTextureNV12, VertexShaderTexture, FragmentShaderTextureNV12);
		GL_CHECK(glUniform1i(glGetUniformLocation(programid, "u_textureY"), 0 + 4));
		GL_CHECK(glUniform1i(glGetUniformLocation(programid, "u_textureU"), 1 + 4));
	} else if (format == ASVL_PAF_NV21) {
		programid = createProgram(ShaderTextureNV21, VertexShaderTexture, FragmentShaderTextureNV21);
		GL_CHECK(glUniform1i(glGetUniformLocation(programid, "u_textureY"), 0 + 4));
		GL_CHECK(glUniform1i(glGetUniformLocation(programid, "u_textureU"), 1 + 4));
	} else if (format == ASVL_PAF_YUYV) {
		programid = createProgram(ShaderTextureYUYV, VertexShaderTexture, FragmentShaderTextureYUYV);
		GL_CHECK(glUniform1i(glGetUniformLocation(programid, "u_textureY"), 0 + 4));
		GL_CHECK(glUniform1i(glGetUniformLocation(programid, "u_textureU"), 1 + 4));
	} else if (format == ASVL_PAF_UYVY) {
		programid = createProgram(ShaderTextureUYVY, VertexShaderTexture, FragmentShaderTextureUYVY);
		GL_CHECK(glUniform1i(glGetUniformLocation(programid, "u_textureY"), 0 + 4));
		GL_CHECK(glUniform1i(glGetUniformLocation(programid, "u_textureU"), 1 + 4));
	} else if ((format == ASVL_PAF_I444) || (format == ASVL_PAF_I422H) || (format == ASVL_PAF_I420)) {
		programid = createProgram(ShaderTextureI440_I422_I444, VertexShaderTexture, FragmentShaderTextureI420_I422_I444);
		GL_CHECK(glUniform1i(glGetUniformLocation(programid, "u_textureY"), 0 + 4));
		GL_CHECK(glUniform1i(glGetUniformLocation(programid, "u_textureU"), 1 + 4));
		GL_CHECK(glUniform1i(glGetUniformLocation(programid, "u_textureV"), 2 + 4));

    } else if ((format == ASVL_PAF_RGB) || (format == ASVL_PAF_BGR) || (format == ASVL_PAF_RGBX)) {
		programid = createProgram(ShaderTextureRGBX, VertexShaderTexture, FragmentShaderTextureRGBX);
		GL_CHECK(glUniform1i(glGetUniformLocation(programid, "u_textureY"), 0 + 4));
    } else if (format == ASVL_PAF_BGRA) {
        programid = createProgram(ShaderTextureBGRA, VertexShaderTexture, FragmentShaderTextureBGRA);
        GL_CHECK(glUniform1i(glGetUniformLocation(programid, "u_textureY"), 0 + 4));
	} else if (format == ASVL_PAF_RGBA) {
		programid = createProgram(ShaderTextureRGBA, VertexShaderTexture, FragmentShaderTextureRGBA);
		GL_CHECK(glUniform1i(glGetUniformLocation(programid, "u_textureY"), 0 + 4));
	} else {
		qDebug() << ("drawTexture:format not support");
		programid = 0;
	}

	GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(programid, "u_mvMatrix"), 1, GL_FALSE, mvMatrix));
	GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(programid, "u_projMatrix"), 1, GL_FALSE, projMatrix));


	if ((format != mFormat) || (width != mWidth) || (height != mHeight)) {

		if (mTextureYid != 0) {
			glDeleteTextures(1, &mTextureYid);
			mTextureYid = 0;
		}

		if (mTextureUid != 0) {
			glDeleteTextures(1, &mTextureUid);
			mTextureUid = 0;
		}

		if (mTextureVid != 0) {
			glDeleteTextures(1, &mTextureVid);
			mTextureVid = 0;
		}

		if (format == ASVL_PAF_GRAY) {
			GL_CHECK(glActiveTexture(GL_TEXTURE0 + 4));
			GL_CHECK(glGenTextures(1, &mTextureYid));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureYid));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

		} else if ((format == ASVL_PAF_NV12) || (format == ASVL_PAF_NV21)) {
			GL_CHECK(glActiveTexture(GL_TEXTURE1 + 4));
			GL_CHECK(glGenTextures(1, &mTextureUid));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureUid));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width / 2, height / 2, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, nullptr));

			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

			GL_CHECK(glActiveTexture(GL_TEXTURE0 + 4));
			GL_CHECK(glGenTextures(1, &mTextureYid));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureYid));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr));

			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

		} else if ((format == ASVL_PAF_YUYV) || (format == ASVL_PAF_UYVY)) {
			GL_CHECK(glActiveTexture(GL_TEXTURE1 + 4));
			GL_CHECK(glGenTextures(1, &mTextureUid));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureUid));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width / 2, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

			GL_CHECK(glActiveTexture(GL_TEXTURE0 + 4));
			GL_CHECK(glGenTextures(1, &mTextureYid));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureYid));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, nullptr));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

		} else if (format == ASVL_PAF_I444) {
			GL_CHECK(glActiveTexture(GL_TEXTURE2 + 4));
			GL_CHECK(glGenTextures(1, &mTextureVid));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureVid));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr));

			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

			GL_CHECK(glActiveTexture(GL_TEXTURE1 + 4));
			GL_CHECK(glGenTextures(1, &mTextureUid));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureUid));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr));

			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

			GL_CHECK(glActiveTexture(GL_TEXTURE0 + 4));
			GL_CHECK(glGenTextures(1, &mTextureYid));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureYid));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr));

			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

		} else if (format == ASVL_PAF_I422H) {
			GL_CHECK(glActiveTexture(GL_TEXTURE2 + 4));
			GL_CHECK(glGenTextures(1, &mTextureVid));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureVid));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width / 2, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr));

			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

			GL_CHECK(glActiveTexture(GL_TEXTURE1 + 4));
			GL_CHECK(glGenTextures(1, &mTextureUid));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureUid));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width / 2, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr));

			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

			GL_CHECK(glActiveTexture(GL_TEXTURE0 + 4));
			GL_CHECK(glGenTextures(1, &mTextureYid));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureYid));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr));

			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

		} else if (format == ASVL_PAF_I420) {
			GL_CHECK(glActiveTexture(GL_TEXTURE2 + 4));
			GL_CHECK(glGenTextures(1, &mTextureVid));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureVid));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width / 2, height / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr));

			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

			GL_CHECK(glActiveTexture(GL_TEXTURE1 + 4));
			GL_CHECK(glGenTextures(1, &mTextureUid));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureUid));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width / 2, height / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr));

			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

			GL_CHECK(glActiveTexture(GL_TEXTURE0 + 4));
			GL_CHECK(glGenTextures(1, &mTextureYid));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureYid));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr));

			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

		} else if (format == ASVL_PAF_RGB) {
			GL_CHECK(glActiveTexture(GL_TEXTURE0 + 4));
			GL_CHECK(glGenTextures(1, &mTextureYid));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureYid));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

		} else if (format == ASVL_PAF_BGR) {
			GL_CHECK(glActiveTexture(GL_TEXTURE0 + 4));
			GL_CHECK(glGenTextures(1, &mTextureYid));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureYid));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, nullptr));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

        } else if ((format == ASVL_PAF_RGBA) || (format == ASVL_PAF_RGBX)||(format == ASVL_PAF_BGRA)) {
			GL_CHECK(glActiveTexture(GL_TEXTURE0 + 4));
			GL_CHECK(glGenTextures(1, &mTextureYid));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureYid));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

		} else if (format == 0) {

		} else {
			qDebug() << ("prepare:format not support");
		}

		mFormat = format;
		mWidth = width;
		mHeight = height;
	}

	if (format == ASVL_PAF_GRAY) {
		GL_CHECK(glActiveTexture(GL_TEXTURE0 + 4));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureYid));

		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, indata));

	} else if ((format == ASVL_PAF_NV12) || (format == ASVL_PAF_NV21)) {
		GL_CHECK(glActiveTexture(GL_TEXTURE1 + 4));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureUid));

		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, indata + (width*height)));

		GL_CHECK(glActiveTexture(GL_TEXTURE0 + 4));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureYid));

		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, indata));

	} else if ((format == ASVL_PAF_YUYV) || (format == ASVL_PAF_UYVY)) {

		GL_CHECK(glActiveTexture(GL_TEXTURE1 + 4));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureUid));
		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height, GL_RGBA, GL_UNSIGNED_BYTE, indata));

		GL_CHECK(glActiveTexture(GL_TEXTURE0 + 4));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureYid));

		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, indata));	
	} else if (format == ASVL_PAF_I444) {
		GL_CHECK(glActiveTexture(GL_TEXTURE2 + 4));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureVid));

		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, indata + (width*height * 2)));

		GL_CHECK(glActiveTexture(GL_TEXTURE1 + 4));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureUid));

		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, indata + (width*height)));

		GL_CHECK(glActiveTexture(GL_TEXTURE0 + 4));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureYid));

		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, indata));

	} else if (format == ASVL_PAF_I422H) {

		GL_CHECK(glActiveTexture(GL_TEXTURE2 + 4));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureVid));

		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, indata + (width*height * 3 / 2)));

		GL_CHECK(glActiveTexture(GL_TEXTURE1 + 4));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureUid));

		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, indata + (width*height)));

		GL_CHECK(glActiveTexture(GL_TEXTURE0 + 4));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureYid));

		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, indata));

	} else if (format == ASVL_PAF_I420) {
		GL_CHECK(glActiveTexture(GL_TEXTURE2 + 4));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureVid));

		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, indata + (width*height * 5 / 4)));

		GL_CHECK(glActiveTexture(GL_TEXTURE1 + 4));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureUid));

		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, indata + (width*height)));

		GL_CHECK(glActiveTexture(GL_TEXTURE0 + 4));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureYid));

		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, indata));

	} else if (format == ASVL_PAF_RGB) {
		GL_CHECK(glActiveTexture(GL_TEXTURE0 + 4));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureYid));
		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, indata));
	} else if (format == ASVL_PAF_BGR) {
		GL_CHECK(glActiveTexture(GL_TEXTURE0 + 4));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureYid));
		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, indata));
    } else if ((format == ASVL_PAF_RGBA) || (format == ASVL_PAF_RGBX)||(format == ASVL_PAF_BGRA)) {
		GL_CHECK(glActiveTexture(GL_TEXTURE0 + 4));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureYid));
		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, indata));
	} else {
		qDebug() << ("drawTexture:format not support");
	}

	GLuint vertex_loc = glGetAttribLocation(programid, "aPos");
	GL_CHECK(glVertexAttribPointer(vertex_loc, 3, GL_FLOAT, GL_FALSE, 0, pVertex));
	GL_CHECK(glEnableVertexAttribArray(vertex_loc));

	GLuint texture_loc = glGetAttribLocation(programid, "aText");
	GL_CHECK(glVertexAttribPointer(texture_loc, 2, GL_FLOAT, GL_FALSE, 0, pTexture));
	GL_CHECK(glEnableVertexAttribArray(texture_loc));

	if ((pIndex != nullptr) && (indexNum != 0)) {
		GL_CHECK(glDrawElements(mode, indexNum, GL_UNSIGNED_SHORT, pIndex));
	} else {
		GL_CHECK(glDrawArrays(mode, 0, vertexNum));
	}
	GL_CHECK(glDisableVertexAttribArray(vertex_loc));
	GL_CHECK(glDisableVertexAttribArray(texture_loc));


	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
}


void GLHelper::drawPoints(int pointsNum, float *pointsXY, int width, int height, float red, float green, float blue, float alpha, GLuint mode) {

	if (pointsNum > 0) {
		float projMatrix[16];
		if (bFrontCamera) {
			frustumM(projMatrix, 0.5f, -0.5f, -0.5f, 0.5f, 2.0f, 6.0f);
		} else {
			frustumM(projMatrix, -0.5f, 0.5f, -0.5f, 0.5f, 2.0f, 6.0f);
		}
		float mvMatrix[16];
		lookatM(mvMatrix, 0.0f, 0.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		rotateM(mvMatrix, (float)mCameraDegree, 0.0f, 0.0f, 1.0f);
		float *pointsXYZ = (float *)alloca(sizeof(float) * 3 * pointsNum);;
		for (int i = 0; i < pointsNum; i++) {
			pointsXYZ[i * 3 + 0] = ((2.0f*pointsXY[i * 2]) / (width - 1)) - 1.0f;
			pointsXYZ[i * 3 + 1] = 1.0f - ((2.0f*pointsXY[i * 2 + 1]) / (height - 1));
			pointsXYZ[i * 3 + 2] = 0.0f;
		}
		drawPoints(projMatrix, mvMatrix, pointsXYZ, pointsNum, nullptr, 0, red, green, blue, alpha, mode);
	}
}


void GLHelper::drawPoints(float *projMatrix, float *mvMatrix, float* pVertex, int vertexNum, uint16_t* pIndex, int indexNum, float red, float green, float blue, float alpha, GLuint mode) {


	GLuint programid = createProgram(ShaderTriangle, VertexShaderTriangle, FragmentShaderTriangle);
	float colors[4] = { red, green, blue, alpha };

	GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(programid, "u_mvMatrix"), 1, false, mvMatrix));
	GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(programid, "u_projMatrix"), 1, false, projMatrix));
	GL_CHECK(glUniform4fv(glGetUniformLocation(programid, "v_color"), 1, colors));

	GLuint vertex_loc = glGetAttribLocation(programid, "aPos");
	GL_CHECK(glVertexAttribPointer(vertex_loc, 3, GL_FLOAT, GL_FALSE, 0, pVertex));
	GL_CHECK(glEnableVertexAttribArray(vertex_loc));

	if ((pIndex != nullptr) && (indexNum != 0)) {
        GL_CHECK(glDrawElements(mode, indexNum, GL_UNSIGNED_SHORT, pIndex));
	} else {
        GL_CHECK(glDrawArrays(mode, 0, vertexNum));
	}
	GL_CHECK(glDisableVertexAttribArray(vertex_loc));

}

void GLHelper::releaseShader(Shader &shader){
	if (shader.vertexShader != 0) {
		GL_CHECK(glDetachShader(shader.programid, shader.vertexShader));
		GL_CHECK(glDeleteShader(shader.vertexShader));
		shader.vertexShader = 0;
	}

	if (shader.fragmentShader != 0) {
		GL_CHECK(glDetachShader(shader.programid, shader.fragmentShader));
		GL_CHECK(glDeleteShader(shader.fragmentShader));
		shader.fragmentShader = 0;
	}

	if (shader.programid != 0) {
		GL_CHECK(glDeleteProgram(shader.programid));
		shader.programid = 0;
	}
}

void GLHelper::uninit(){

	if (mTextureYid != 0) {
		GL_CHECK(glDeleteTextures(1, &mTextureYid));
		mTextureYid = 0;
	}

	if (mTextureUid != 0) {
		GL_CHECK(glDeleteTextures(1, &mTextureUid));
		mTextureUid = 0;
	}

	if (mTextureVid != 0) {
		GL_CHECK(glDeleteTextures(1, &mTextureVid));
		mTextureVid = 0;
	}

	mFormat = 0;
	mWidth = 0;
	mHeight = 0;

	releaseShader(ShaderTriangle);
	releaseShader(ShaderPointsCloud);
	releaseShader(ShaderTextureGRAY);
	releaseShader(ShaderTextureNV12);
	releaseShader(ShaderTextureNV21);
	releaseShader(ShaderTextureYUYV);
	releaseShader(ShaderTextureUYVY);
	releaseShader(ShaderTextureI440_I422_I444);
	releaseShader(ShaderTextureRGBX);
	releaseShader(ShaderTextureRGBA);
    releaseShader(ShaderTextureBGRA);
}

GLHelper::~GLHelper(){

}


const char GLHelper::VertexShaderTexture[] =
"#version 100  \n"
"uniform mat4 u_mvMatrix;\n"
"uniform mat4 u_projMatrix;\n"
"attribute vec4 aPos;\n"
"attribute vec2 aText;\n"
"varying vec2 v_texCoord;\n"
"void main() {\n"
"  gl_Position = u_projMatrix*u_mvMatrix*aPos;\n"
"  v_texCoord = aText;\n"
"}\n";


const char GLHelper::FragmentShaderTextureGRAY[] =
"#version 100  \n"
"precision mediump float;\n"
"uniform sampler2D u_textureY;\n"
"varying vec2 v_texCoord;\n"
"void main() {\n"
"   float r, g, b, y;\n"
"   y = texture2D(u_textureY, v_texCoord).r;\n"
"   r = y;\n"
"   g = y;\n"
"   b = y;\n"
"   gl_FragColor = vec4(r, g, b, 1.0);\n"
"}\n";


const char GLHelper::FragmentShaderTextureNV12[] =
"#version 100  \n"
"precision mediump float;\n"
"uniform sampler2D u_textureY;\n"
"uniform sampler2D u_textureU;\n"
"varying vec2 v_texCoord;\n"
"void main() {\n"
"   float r, g, b, y, u, v;\n"
"   y = texture2D(u_textureY, v_texCoord).r;\n"
"   v = texture2D(u_textureU, v_texCoord).a;\n"
"   u = texture2D(u_textureU, v_texCoord).r;\n"
"   r = y + 1.13983*(v-0.5);\n"
"   g = y - 0.39465*(u-0.5) - 0.58060*(v-0.5);\n"
"   b = y + 2.03211*(u-0.5);\n"
"   gl_FragColor = vec4(r, g, b, 1.0);\n"
"}\n";

const char GLHelper::FragmentShaderTextureNV21[] =
"#version 100  \n"
"precision mediump float;\n"
"uniform sampler2D u_textureY;\n"
"uniform sampler2D u_textureU;\n"
"varying vec2 v_texCoord;\n"
"void main() {\n"
"   float r, g, b, y, u, v;\n"
"   y = texture2D(u_textureY, v_texCoord).r;\n"
"   u = texture2D(u_textureU, v_texCoord).a;\n"
"   v = texture2D(u_textureU, v_texCoord).r;\n"
"   r = y + 1.13983*(v-0.5);\n"
"   g = y - 0.39465*(u-0.5) - 0.58060*(v-0.5);\n"
"   b = y + 2.03211*(u-0.5);\n"
"   gl_FragColor = vec4(r, g, b, 1.0);\n"
"}\n";



const char GLHelper::FragmentShaderTextureYUYV[] =
"#version 100  \n"
"precision mediump float;\n"
"uniform sampler2D u_textureY;\n"
"uniform sampler2D u_textureU;\n"
"varying vec2 v_texCoord;\n"
"void main() {\n"
"   float r, g, b, y, u, v;\n"
"   y = texture2D(u_textureY, v_texCoord).r;\n"
"   u = texture2D(u_textureU, v_texCoord).g;\n"
"   v = texture2D(u_textureU, v_texCoord).a;\n"
"   r = y + 1.13983*(v-0.5);\n"
"   g = y - 0.39465*(u-0.5) - 0.58060*(v-0.5);\n"
"   b = y + 2.03211*(u-0.5);\n"
"   gl_FragColor = vec4(r, g, b, 1.0);\n"
"}\n";

const char GLHelper::FragmentShaderTextureUYVY[] =
"#version 100  \n"
"precision mediump float;\n"
"uniform sampler2D u_textureY;\n"
"uniform sampler2D u_textureU;\n"
"varying vec2 v_texCoord;\n"
"void main() {\n"
"   float r, g, b, y, u, v;\n"
"   y = texture2D(u_textureY, v_texCoord).a;\n"
"   u = texture2D(u_textureU, v_texCoord).r;\n"
"   v = texture2D(u_textureU, v_texCoord).b;\n"
"   r = y + 1.13983*(v-0.5);\n"
"   g = y - 0.39465*(u-0.5) - 0.58060*(v-0.5);\n"
"   b = y + 2.03211*(u-0.5);\n"
"   gl_FragColor = vec4(r, g, b, 1.0);\n"
"}\n";

const char GLHelper::FragmentShaderTextureI420_I422_I444[] =
"#version 100  \n"
"precision mediump float;\n"
"uniform sampler2D u_textureY;\n"
"uniform sampler2D u_textureU;\n"
"uniform sampler2D u_textureV;\n"
"varying vec2 v_texCoord;\n"
"void main() {\n"
"   float r, g, b, y, u, v;\n"
"   y = texture2D(u_textureY, v_texCoord).r;\n"
"   u = texture2D(u_textureU, v_texCoord).r;\n"
"   v = texture2D(u_textureV, v_texCoord).r;\n"
"   r = y + 1.13983*(v-0.5);\n"
"   g = y - 0.39465*(u-0.5) - 0.58060*(v-0.5);\n"
"   b = y + 2.03211*(u-0.5);\n"
"   gl_FragColor = vec4(r, g, b, 1.0);\n"
"}\n";

const char GLHelper::FragmentShaderTextureRGBX[] =
"#version 100  \n"
"precision mediump float;\n"
"uniform sampler2D u_textureY; \n"
"varying vec2 v_texCoord;\n"
"void main() {\n"
" gl_FragColor = vec4(texture2D(u_textureY, v_texCoord).xyz,1.0);\n"
"}\n";

const char GLHelper::FragmentShaderTextureRGBA[] =
"#version 100  \n"
"precision mediump float;\n"
"uniform sampler2D u_textureY; \n"
"varying vec2 v_texCoord;\n"
"void main() {\n"
" gl_FragColor = texture2D(u_textureY, v_texCoord);\n"
"}\n";

const char GLHelper::FragmentShaderTextureBGRA[] =
"#version 100  \n"
"precision mediump float;\n"
"uniform sampler2D u_textureY; \n"
"varying vec2 v_texCoord;\n"
"void main() {\n"
" gl_FragColor = texture2D(u_textureY, v_texCoord).zyxw;\n"
"}\n";

const char GLHelper::VertexShaderTriangle[] =
"#version 100  \n"
"uniform mat4 u_mvMatrix;\n"
"uniform mat4 u_projMatrix;\n"
"attribute vec4 aPos;\n"
"void main() {\n"
"  gl_Position = u_projMatrix*u_mvMatrix*aPos;\n"
"}\n";


const char GLHelper::FragmentShaderTriangle[] =
"#version 100  \n"
"precision mediump float;\n"
"uniform vec4 v_color;\n"
"void main() {\n"
" gl_FragColor = v_color;\n"
"}\n";


GLuint GLHelper::createProgram(Shader &shader, const char* pVertexSource, const char* pFragmentSource){
	if (shader.programid != 0) {
		GL_CHECK(glUseProgram(shader.programid));
		return shader.programid;
	}
	shader.vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
	shader.fragmentShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
	GLuint programid = glCreateProgram();
	if (0 == programid) {
		qDebug() << ("glCreateProgram error!");
	}
	GL_CHECK(glAttachShader(programid, shader.vertexShader));
	GL_CHECK(glAttachShader(programid, shader.fragmentShader));

	GL_CHECK(glLinkProgram(programid));
	GLint linkStatus;
	glGetProgramiv(programid, GL_LINK_STATUS, &linkStatus);
	if (GL_FALSE == linkStatus) {
		GLchar errInfo[4096];
		GLsizei errInfoLen;
		glGetProgramInfoLog(programid, sizeof(errInfo), &errInfoLen, errInfo);
		if (errInfoLen > 0) {
			qDebug() << errInfo;
		}
		qDebug() << ("Error linking program:");
	}

	glValidateProgram(programid);
	GLint validateStatus;
	glGetProgramiv(programid, GL_VALIDATE_STATUS, &validateStatus);
	if (GL_FALSE == validateStatus) {
		qDebug() << ("Error validate program:");
	}
	shader.programid = programid;
	GL_CHECK(glUseProgram(shader.programid));

	return shader.programid;
}


GLuint GLHelper::loadShader(GLenum shaderType, const char* pSource) {
	GLuint shader = glCreateShader(shaderType);
	GL_CHECK(glShaderSource(shader, 1, &pSource, nullptr));
	GL_CHECK(glCompileShader(shader));

	{
		int infologLength = 0;
		char infoLog[1024];

		if (glIsShader(shader)) {
			glGetShaderInfoLog(shader, 1024, &infologLength, infoLog);
		} else {
			glGetProgramInfoLog(shader, 1024, &infologLength, infoLog);
		}

		if (infologLength > 0) {
			qDebug() << infoLog;
		}
	}
	GLint compiled = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (GL_FALSE == compiled) {
		qDebug() << "compile error! " << pSource;
		return shader;
	}
	return shader;
}


float GLHelper::lengthXYZ(float x, float y, float z) {
	return sqrtf(x * x + y * y + z * z);
}

void GLHelper::loadIdentityM(float* m){
	m[0] = 1.0f;
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;

	m[4] = 0.0f;
	m[5] = 1.0f;
	m[6] = 0.0f;
	m[7] = 0.0f;

	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = 1.0f;
	m[11] = 0.0f;

	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
}

void GLHelper::multMatrixM(float *rm, float *m){
	float tmp[16];
	// Fisrt Column
	tmp[0] = rm[0] * m[0] + rm[4] * m[1] + rm[8] * m[2] + rm[12] * m[3];
	tmp[1] = rm[1] * m[0] + rm[5] * m[1] + rm[9] * m[2] + rm[13] * m[3];
	tmp[2] = rm[2] * m[0] + rm[6] * m[1] + rm[10] * m[2] + rm[14] * m[3];
	tmp[3] = rm[3] * m[0] + rm[7] * m[1] + rm[11] * m[2] + rm[15] * m[3];

	// Second Column
	tmp[4] = rm[0] * m[4] + rm[4] * m[5] + rm[8] * m[6] + rm[12] * m[7];
	tmp[5] = rm[1] * m[4] + rm[5] * m[5] + rm[9] * m[6] + rm[13] * m[7];
	tmp[6] = rm[2] * m[4] + rm[6] * m[5] + rm[10] * m[6] + rm[14] * m[7];
	tmp[7] = rm[3] * m[4] + rm[7] * m[5] + rm[11] * m[6] + rm[15] * m[7];

	// Third Column
	tmp[8] = rm[0] * m[8] + rm[4] * m[9] + rm[8] * m[10] + rm[12] * m[11];
	tmp[9] = rm[1] * m[8] + rm[5] * m[9] + rm[9] * m[10] + rm[13] * m[11];
	tmp[10] = rm[2] * m[8] + rm[6] * m[9] + rm[10] * m[10] + rm[14] * m[11];
	tmp[11] = rm[3] * m[8] + rm[7] * m[9] + rm[11] * m[10] + rm[15] * m[11];

	// Fourth Column
	tmp[12] = rm[0] * m[12] + rm[4] * m[13] + rm[8] * m[14] + rm[12] * m[15];
	tmp[13] = rm[1] * m[12] + rm[5] * m[13] + rm[9] * m[14] + rm[13] * m[15];
	tmp[14] = rm[2] * m[12] + rm[6] * m[13] + rm[10] * m[14] + rm[14] * m[15];
	tmp[15] = rm[3] * m[12] + rm[7] * m[13] + rm[11] * m[14] + rm[15] * m[15];

	memcpy(rm, tmp, sizeof(float) * 16);
}

void GLHelper::translateM(float* m, float x, float y, float z) {
	int i;
	for (i = 0; i < 4; i++) {
		m[12 + i] += m[i] * x + m[4 + i] * y + m[8 + i] * z;
	}
}

void GLHelper::scaleM(float *m, float x, float y, float z){
	for (int i = 0; i < 4; i++) {
		m[i] *= x;
		m[4 + i] *= y;
		m[8 + i] *= z;
	}
}

void GLHelper::rotateM(float* m, float angle, float x, float y, float z){
	float tmp[16];
	setRotateM(tmp, angle, x, y, z);
	multMatrixM(m, tmp);
}

void GLHelper::setRotateM(float* m, float a, float x, float y, float z) {
	m[3] = 0;
	m[7] = 0;
	m[11] = 0;
	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;
	a *= (3.14159265358979323846f / 180.0f);
	float s = sinf(a);
	float c = cosf(a);
	if (1.0f == x && 0.0f == y && 0.0f == z) {
		m[5] = c;   m[10] = c;
		m[6] = s;   m[9] = -s;
		m[1] = 0;   m[2] = 0;
		m[4] = 0;   m[8] = 0;
		m[0] = 1;
	} else if (0.0f == x && 1.0f == y && 0.0f == z) {
		m[0] = c;   m[10] = c;
		m[8] = s;   m[2] = -s;
		m[1] = 0;   m[4] = 0;
		m[6] = 0;   m[9] = 0;
		m[5] = 1;
	} else if (0.0f == x && 0.0f == y && 1.0f == z) {
		m[0] = c;   m[5] = c;
		m[1] = s;   m[4] = -s;
		m[2] = 0;   m[6] = 0;
		m[8] = 0;   m[9] = 0;
		m[10] = 1;
	} else {
		float len = lengthXYZ(x, y, z);
		if (1.0f != len) {
			float recipLen = 1.0f / len;
			x *= recipLen;
			y *= recipLen;
			z *= recipLen;
		}
		float nc = 1.0f - c;
		float xy = x * y;
		float yz = y * z;
		float zx = z * x;
		float xs = x * s;
		float ys = y * s;
		float zs = z * s;
		m[0] = x*x*nc + c;
		m[4] = xy*nc - zs;
		m[8] = zx*nc + ys;
		m[1] = xy*nc + zs;
		m[5] = y*y*nc + c;
		m[9] = yz*nc - xs;
		m[2] = zx*nc - ys;
		m[6] = yz*nc + xs;
		m[10] = z*z*nc + c;
	}
}

void GLHelper::frustumM(float* m, float left, float right, float bottom, float top, float near1, float far1){
	float r_width = 1.0f / (right - left);
	float r_height = 1.0f / (top - bottom);
	float r_depth = 1.0f / (near1 - far1);
	float x = 2.0f * (near1 * r_width);
	float y = 2.0f * (near1 * r_height);
	float A = 2.0f * ((right + left) * r_width);
	float B = (top + bottom) * r_height;
	float C = (far1 + near1) * r_depth;
	float D = 2.0f * (far1 * near1 * r_depth);

	m[0] = x;
	m[5] = y;
	m[8] = A;
	m[9] = B;
	m[10] = C;
	m[14] = D;
	m[11] = -1.0f;
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;
	m[4] = 0.0f;
	m[6] = 0.0f;
	m[7] = 0.0f;
	m[12] = 0.0f;
	m[13] = 0.0f;
	m[15] = 0.0f;
}

void GLHelper::perspectiveM(float* m,
	float fovy, float aspect, float zNear, float zFar) {
	float f = 1.0f / (float)tanf(fovy * (3.14159265358979323846f / 360.0f));
	float rangeReciprocal = 1.0f / (zNear - zFar);

	m[0] = f / aspect;
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;

	m[4] = 0.0f;
	m[5] = f;
	m[6] = 0.0f;
	m[7] = 0.0f;

	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = (zFar + zNear) * rangeReciprocal;
	m[11] = -1.0f;

	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 2.0f * zFar * zNear * rangeReciprocal;
	m[15] = 0.0f;
}

void GLHelper::lookatM(float* m, float eyeX, float eyeY, float eyeZ,
	float centerX, float centerY, float centerZ,
	float upX, float upY, float upZ){

	float tmp[16];

	float fx = centerX - eyeX;
	float fy = centerY - eyeY;
	float fz = centerZ - eyeZ;

	// Normalize f
	float rlf = 1.0f / lengthXYZ(fx, fy, fz);
	fx *= rlf;
	fy *= rlf;
	fz *= rlf;

	// compute s = f x up (x means "cross product")
	float sx = fy * upZ - fz * upY;
	float sy = fz * upX - fx * upZ;
	float sz = fx * upY - fy * upX;

	// and normalize s
	float rls = 1.0f / lengthXYZ(sx, sy, sz);
	sx *= rls;
	sy *= rls;
	sz *= rls;

	// compute u = s x f
	float ux = sy * fz - sz * fy;
	float uy = sz * fx - sx * fz;
	float uz = sx * fy - sy * fx;

	tmp[0] = sx;
	tmp[1] = ux;
	tmp[2] = -fx;
	tmp[3] = 0.0f;

	tmp[4] = sy;
	tmp[5] = uy;
	tmp[6] = -fy;
	tmp[7] = 0.0f;

	tmp[8] = sz;
	tmp[9] = uz;
	tmp[10] = -fz;
	tmp[11] = 0.0f;

	tmp[12] = 0.0f;
	tmp[13] = 0.0f;
	tmp[14] = 0.0f;
	tmp[15] = 1.0f;

	translateM(tmp, -eyeX, -eyeY, -eyeZ);
	loadIdentityM(m);
	multMatrixM(m, tmp);
}

void GLHelper::transposeM(float* rm, float* m) {
	for (int i = 0; i < 4; i++) {
		int mBase = i * 4;
		rm[i] = m[mBase];
		rm[i + 4] = m[mBase + 1];
		rm[i + 8] = m[mBase + 2];
		rm[i + 12] = m[mBase + 3];
	}
}

void GLHelper::invertM(float* rm, float* m) {
	// Invert a 4 x 4 matrix using Cramer's Rule

	// transpose matrix
	float src0 = m[0];
	float src4 = m[1];
	float src8 = m[2];
	float src12 = m[3];

	float src1 = m[4];
	float src5 = m[5];
	float src9 = m[6];
	float src13 = m[7];

	float src2 = m[8];
	float src6 = m[9];
	float src10 = m[10];
	float src14 = m[11];

	float src3 = m[12];
	float src7 = m[13];
	float src11 = m[14];
	float src15 = m[15];

	// calculate pairs for first 8 elements (cofactors)
	float atmp0 = src10 * src15;
	float atmp1 = src11 * src14;
	float atmp2 = src9  * src15;
	float atmp3 = src11 * src13;
	float atmp4 = src9  * src14;
	float atmp5 = src10 * src13;
	float atmp6 = src8  * src15;
	float atmp7 = src11 * src12;
	float atmp8 = src8  * src14;
	float atmp9 = src10 * src12;
	float atmp10 = src8  * src13;
	float atmp11 = src9  * src12;

	// calculate first 8 elements (cofactors)
	float dst0 = (atmp0 * src5 + atmp3 * src6 + atmp4  * src7)
		- (atmp1 * src5 + atmp2 * src6 + atmp5  * src7);
	float dst1 = (atmp1 * src4 + atmp6 * src6 + atmp9  * src7)
		- (atmp0 * src4 + atmp7 * src6 + atmp8  * src7);
	float dst2 = (atmp2 * src4 + atmp7 * src5 + atmp10 * src7)
		- (atmp3 * src4 + atmp6 * src5 + atmp11 * src7);
	float dst3 = (atmp5 * src4 + atmp8 * src5 + atmp11 * src6)
		- (atmp4 * src4 + atmp9 * src5 + atmp10 * src6);
	float dst4 = (atmp1 * src1 + atmp2 * src2 + atmp5  * src3)
		- (atmp0 * src1 + atmp3 * src2 + atmp4  * src3);
	float dst5 = (atmp0 * src0 + atmp7 * src2 + atmp8  * src3)
		- (atmp1 * src0 + atmp6 * src2 + atmp9  * src3);
	float dst6 = (atmp3 * src0 + atmp6 * src1 + atmp11 * src3)
		- (atmp2 * src0 + atmp7 * src1 + atmp10 * src3);
	float dst7 = (atmp4 * src0 + atmp9 * src1 + atmp10 * src2)
		- (atmp5 * src0 + atmp8 * src1 + atmp11 * src2);

	// calculate pairs for second 8 elements (cofactors)
	float btmp0 = src2 * src7;
	float btmp1 = src3 * src6;
	float btmp2 = src1 * src7;
	float btmp3 = src3 * src5;
	float btmp4 = src1 * src6;
	float btmp5 = src2 * src5;
	float btmp6 = src0 * src7;
	float btmp7 = src3 * src4;
	float btmp8 = src0 * src6;
	float btmp9 = src2 * src4;
	float btmp10 = src0 * src5;
	float btmp11 = src1 * src4;

	// calculate second 8 elements (cofactors)
	float dst8 = (btmp0  * src13 + btmp3  * src14 + btmp4  * src15)
		- (btmp1  * src13 + btmp2  * src14 + btmp5  * src15);
	float dst9 = (btmp1  * src12 + btmp6  * src14 + btmp9  * src15)
		- (btmp0  * src12 + btmp7  * src14 + btmp8  * src15);
	float dst10 = (btmp2  * src12 + btmp7  * src13 + btmp10 * src15)
		- (btmp3  * src12 + btmp6  * src13 + btmp11 * src15);
	float dst11 = (btmp5  * src12 + btmp8  * src13 + btmp11 * src14)
		- (btmp4  * src12 + btmp9  * src13 + btmp10 * src14);
	float dst12 = (btmp2  * src10 + btmp5  * src11 + btmp1  * src9)
		- (btmp4  * src11 + btmp0  * src9 + btmp3  * src10);
	float dst13 = (btmp8  * src11 + btmp0  * src8 + btmp7  * src10)
		- (btmp6  * src10 + btmp9  * src11 + btmp1  * src8);
	float dst14 = (btmp6  * src9 + btmp11 * src11 + btmp3  * src8)
		- (btmp10 * src11 + btmp2  * src8 + btmp7  * src9);
	float dst15 = (btmp10 * src10 + btmp4  * src8 + btmp9  * src9)
		- (btmp8  * src9 + btmp11 * src10 + btmp5  * src8);

	// calculate determinant
	float det =
		src0 * dst0 + src1 * dst1 + src2 * dst2 + src3 * dst3;


	//MYASSERT(det != 0.0f);

	// calculate matrix inverse
	float invdet = 1.0f / det;
	rm[0] = dst0  * invdet;
	rm[1] = dst1  * invdet;
	rm[2] = dst2  * invdet;
	rm[3] = dst3  * invdet;

	rm[4] = dst4  * invdet;
	rm[5] = dst5  * invdet;
	rm[6] = dst6  * invdet;
	rm[7] = dst7  * invdet;

	rm[8] = dst8  * invdet;
	rm[9] = dst9  * invdet;
	rm[10] = dst10 * invdet;
	rm[11] = dst11 * invdet;

	rm[12] = dst12 * invdet;
	rm[13] = dst13 * invdet;
	rm[14] = dst14 * invdet;
	rm[15] = dst15 * invdet;

}

void GLHelper::checkGLError(const char* stmt, const char* fname, int line){
	GLenum err = glGetError();
	if (GL_NO_ERROR != err) {
		qDebug() << "OpenGL error " << err << " " << fname << " " << line << " " << stmt;
	}
}

void GLHelper::matrixMultPos(float *m, float *inpos, float *outpos){
	for (int i = 0; i < 4; i++) {
		outpos[i] = (m[i + 0] * inpos[0]) + (m[i + 4] * inpos[1]) + (m[i + 8] * inpos[2]) + (m[i + 12] * inpos[3]);
	}
}


