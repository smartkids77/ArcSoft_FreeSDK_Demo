#ifndef __GLHELPER_H__
#define __GLHELPER_H__

#include <QOpenGLFunctions>
#include "asvloffscreen.h"

#define MYGL_DEBUG
#ifdef  MYGL_DEBUG
#define GL_CHECK(stmt) do { \
            stmt; \
            checkGLError(#stmt, __FILE__, __LINE__); \
		        } while (0)
#else
#define GL_CHECK(stmt) stmt
#endif


class GLHelper :public QOpenGLFunctions{
private:
	class Shader{

	public:
		Shader(){
			programid = 0;
			vertexShader = 0;
			fragmentShader = 0;
		}
		GLuint programid;
		GLuint vertexShader;
		GLuint fragmentShader;
	};


	GLuint createProgram(Shader &shader, const char* pVertexSource, const char* pFragmentSource);
	GLuint loadShader(GLenum shaderType, const char* pSource);
    void releaseShader(Shader &shader);
    void drawTexture(unsigned char* indata, int format, int width, int height, float* projMatrix, float* mvMatrix, float* pVertex, int vertexNum, float* pTexture, int textureNum, uint16_t* pIndex, int indexNum, GLuint mode);
    void drawPoints(float *projMatrix, float *mvMatrix, float* pVertex, int vertexNum, uint16_t* pIndex, int indexNum, float red, float green, float blue, float alpha, GLuint mode);

    float lengthXYZ(float x, float y, float z);
	void loadIdentityM(float* m);
	void multMatrixM(float *rm, float *m);
	void translateM(float* m, float x, float y, float z);
	void scaleM(float *m, float x, float y, float z);
	void rotateM(float* m, float angle, float x, float y, float z);
	void setRotateM(float* m, float a, float x, float y, float z);
	void frustumM(float* m, float left, float right, float bottom, float top, float near1, float far1);
	void perspectiveM(float* m, float fovy, float aspect, float zNear, float zFar);
	void lookatM(float* m, float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ);
	void transposeM(float* rm, float* m);
	void invertM(float* rm, float* m);

	void myglMatrixMode(GLenum mode);
	void myglLoadIdentity(void);
	void mygllookat(float eyeX, float eyeY, float eyeZ,
        float centerX, float centerY, float centerZ,
		float upX, float upY, float upZ);
	void myglFrustum(float left, float right,
		float bottom, float top,
		float near1, float far1);

	void myglRotatef(float angle, float x, float y, float z);
	void myglScalef(float x, float y, float z);
	void myglTranslatef(float x, float y, float z);
	void myglMultMatrixf(float *m);


	void getCurMatrix(float *pMatrix);
	void setCurMatrix(float *pMatrix);
	float *getModelViewMatrix(void);
	float *getProjectionMatrix(void);
	float *getColor();

	void myglColor4f(float red, float green, float blue, float alpha);
	void checkGLError(const char* stmt, const char* fname, int line);
	void matrixMultPos(float *m, float *inpos, float *outpos);


	bool bFrontCamera;
	int mCameraDegree;

	GLuint mTextureYid;
	GLuint mTextureUid;
	GLuint mTextureVid;


public:
	int mFormat;
	int mWidth;
	int mHeight;
private:

	Shader ShaderTriangle;
	Shader ShaderPointsCloud;
	Shader ShaderTextureGRAY;
	Shader ShaderTextureNV12;
	Shader ShaderTextureNV21;
	Shader ShaderTextureYUYV;
	Shader ShaderTextureUYVY;
	Shader ShaderTextureI440_I422_I444;
	Shader ShaderTextureRGBX;
	Shader ShaderTextureRGBA;
    Shader ShaderTextureBGRA;


	const static char VertexShaderTexture[];
	const static char FragmentShaderTextureGRAY[];
	const static char FragmentShaderTextureNV12[];
	const static char FragmentShaderTextureNV21[];
	const static char FragmentShaderTextureYUYV[];
	const static char FragmentShaderTextureUYVY[];
	const static char FragmentShaderTextureI420_I422_I444[];
	const static char FragmentShaderTextureRGBX[];
	const static char FragmentShaderTextureRGBA[];
    const static char FragmentShaderTextureBGRA[];

	const static char VertexShaderTriangle[];
	const static char FragmentShaderTriangle[];


public:
    GLHelper();
	~GLHelper();

	bool setMirror(bool newBool);
	int setDegree(int newDegree);
	void drawTexture(unsigned char* indata, int format, int width, int height);
	void drawPoints(int pointsNum, float* pointsXY, int width, int height, float red, float green, float blue, float alpha, GLuint mode);
	void uninit();

	const static int ASVL_PAF_BGR = ASVL_PAF_RGB24_B8G8R8;
	const static int ASVL_PAF_RGB = ASVL_PAF_RGB24_R8G8B8;
	const static int ASVL_PAF_RGBA = ASVL_PAF_RGB32_R8G8B8A8;
    const static int ASVL_PAF_BGRA = ASVL_PAF_RGB32_B8G8R8A8;
	const static int ASVL_PAF_RGBX = ASVL_PAF_RGB32_R8G8B8;
};

#endif
