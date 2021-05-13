#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <scrnsave.h>
#include <strsafe.h>
#include <GL/gl.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <wchar.h>
#include <stdint.h>

static UINT_PTR uTimer;

#define FPS_AVG_MAX 10

enum
{
	GL_FRAGMENT_SHADER = 0x8B30,
	GL_VERTEX_SHADER,
	GL_ARRAY_BUFFER = 0x8892,
	GL_POINT_SPITE = 0x8861,
	GL_PROGRAM_POINT_SPRITE = 0x8642,
	GL_STATIC_DRAW = 0x88E4,
};


static HDC hdc;
static HGLRC context;
typedef int (WINAPI *wglSwapIntervalEXT_t)(int interval);
wglSwapIntervalEXT_t wglSwapIntervalEXT;

struct
{
	unsigned (WINAPI *CreateProgram)();
	unsigned (WINAPI *CreateShader)(GLenum shadertype);
	void (WINAPI *ShaderSource)(unsigned shader, int count, const char * const * string, const int * length);
	void (WINAPI *CompileShader)(unsigned shader);
	void (WINAPI *AttachShader)(unsigned prog, unsigned shader);
	void (WINAPI *LinkProgram)(unsigned prog);
	void (WINAPI *UseProgram)(unsigned prog);
	void (WINAPI *Uniform1f)(int location, float v0);
	void (WINAPI *Uniform2f)(int location, float v0, float v1);
	int (WINAPI *GetUniformLocation)(unsigned program, const char * name);
	void (WINAPI *VertexAttribPointer)(unsigned index, int size, GLenum type, GLboolean normalized, int stride, const void* pointer);
	void (WINAPI *EnableVertexAttribArray)(unsigned index);
	void (WINAPI *DisableVertexAttribArray)(unsigned index);
	void (WINAPI *BindBuffer)(GLenum target, unsigned buffer);
	void (WINAPI *BufferData)(GLenum target, intptr_t size, const void* data, GLenum usage);
	void (WINAPI *GenBuffers)(int n, unsigned* buffers);
	int (WINAPI *GetAttribLocation)(unsigned program, const char * name);
} glf;

void scrtoy_GLLoadFunctions();

typedef struct LLfile_s LLfile;
LLfile * scrtoy_LoadFile(const char * filename);
char * scrtoy_GetContents(LLfile * file);
void scrtoy_CloseFile(LLfile * file);

static struct
{
	unsigned int prog;
	int sizex, sizey;
	
	int resolutionLoc;
	int timeLoc;

	GLuint bufId;
} glData;

unsigned int scrtoy_GLCompileShader(const char * shadercode, GLenum shadertype);
void scrtoy_GLLoadShader();
void scrtoy_GLRender(uint64_t time);

static inline float averagef(float* floats, size_t num)
{
	float avg = 0.f;
	for (size_t i = 0; i < num; i++)
	{
		avg += floats[i];
	}
	return avg / (float)num;
}
static HWND mainWindow;
LRESULT WINAPI ScreenSaverProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static clock_t prevtime;
	static HFONT fpsFont;
	static float fps10[FPS_AVG_MAX] = { 0.f };
	static int fps10index = 0;
	static uint64_t time = 0;

	switch (msg)
	{
	case WM_TIMER:
		InvalidateRect(hwnd, NULL, FALSE);
		break;
	case WM_PAINT:
	{
		clock_t currenttime = clock();
		time += (uint64_t)(currenttime - prevtime);
		/* OpenGL drawing */

		fps10[fps10index] = 1.f / ((float)(currenttime - prevtime) / 1000.f);
		fps10index = (fps10index + 1) % FPS_AVG_MAX;
		prevtime = currenttime;

		int intfps = (int)(averagef(fps10, FPS_AVG_MAX) + .5f);
		char fps_[9] = { "FPS:     " };
		_itoa(intfps, fps_ + 5, 10);
		for (int i = 6; i < 9; i++) if (fps_[i] == '\0') fps_[i] = ' ';
		
		scrtoy_GLRender(time);

		SwapBuffers(hdc);
		break;
	}
	case WM_SIZE:
		// Update resolution of pixel shader
		glData.sizex = LOWORD(lp);
		glData.sizey = HIWORD(lp);
		glf.Uniform2f(glData.resolutionLoc, (float)glData.sizex, (float)glData.sizey);
		break;
	case WM_CLOSE:	
		DeleteObject(fpsFont);
		ReleaseDC(hwnd, hdc);
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
	{
		mainWindow = hwnd;
		hdc = GetDC(hwnd);

		static const PIXELFORMATDESCRIPTOR pfd = {
			0, 0, PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER
		};
		SetPixelFormat(hdc, ChoosePixelFormat(hdc, &pfd), &pfd);
		context = wglCreateContext(hdc);
		wglMakeCurrent(hdc, context);
		if ((wglSwapIntervalEXT = (wglSwapIntervalEXT_t)wglGetProcAddress("wglSwapIntervalEXT")))
			wglSwapIntervalEXT(1);
		
		scrtoy_GLLoadFunctions();

		fpsFont = CreateFontW(
			48,
			0,
			0,
			0,
			FW_SEMIBOLD,
			FALSE,
			FALSE,
			FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			NONANTIALIASED_QUALITY,
			FF_DONTCARE,
			L"Terminal"
		);

		scrtoy_GLLoadShader();

		// Create timer for refreshing the screen
		uTimer = SetTimer(hwnd, 1, 1, NULL);
		break;
	}
	default:
		return DefScreenSaverProc(hwnd, msg, wp, lp);
	}

	return 0;
}

BOOL WINAPI RegisterDialogClasses(HANDLE __attribute__((unused)) hInst)
{
	return TRUE;
}

BOOL WINAPI ScreenSaverConfigureDialog(
	HWND   __attribute__((unused)) hDlg,
	UINT   __attribute__((unused)) message,
	WPARAM __attribute__((unused)) wParam,
	LPARAM __attribute__((unused)) lParam
)
{
	return FALSE;
}


void scrtoy_GLLoadFunctions()
{
	#define scrtoy_LoadFunc(func) (glf.func = (__typeof(glf.func))wglGetProcAddress("gl" #func))
	
	scrtoy_LoadFunc(CreateProgram);
	scrtoy_LoadFunc(CreateShader);
	scrtoy_LoadFunc(ShaderSource);
	scrtoy_LoadFunc(CompileShader);
	scrtoy_LoadFunc(AttachShader);
	scrtoy_LoadFunc(LinkProgram);
	scrtoy_LoadFunc(UseProgram);
	scrtoy_LoadFunc(Uniform1f);
	scrtoy_LoadFunc(Uniform2f);
	scrtoy_LoadFunc(GetUniformLocation);
	scrtoy_LoadFunc(VertexAttribPointer);
	scrtoy_LoadFunc(EnableVertexAttribArray);
	scrtoy_LoadFunc(DisableVertexAttribArray);
	scrtoy_LoadFunc(BindBuffer);
	scrtoy_LoadFunc(BufferData);
	scrtoy_LoadFunc(GenBuffers);
	scrtoy_LoadFunc(GetAttribLocation);

	#undef scrtoy_LoadFunc
}


struct LLfile_s
{
	HANDLE file;
	char * contents;
};
LLfile * scrtoy_LoadFile(const char * filename)
{
	HANDLE hFile = CreateFileA(
		filename,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hFile == INVALID_HANDLE_VALUE)
		return NULL;
	
	LLfile * file = malloc(sizeof(LLfile));
	if (file == NULL)
		return NULL;
	
	DWORD filesz = GetFileSize(hFile, NULL);
	file->contents = malloc(filesz);
	if (file->contents == NULL)
	{
		CloseHandle(hFile);
		free(file);
		return NULL;
	}

	DWORD dwRead;
	ReadFile(hFile, file->contents, filesz, &dwRead, NULL);
	
	if (dwRead != filesz)
	{
		CloseHandle(hFile);
		free(file->contents);
		free(file);
		return NULL;
	}

	file->file = hFile;

	return file;
}
char * scrtoy_GetContents(LLfile * file)
{
	return file->contents;
}
void scrtoy_CloseFile(LLfile * file)
{
	if (file == NULL)
		return;
	CloseHandle(file->file);
	free(file->contents);
	free(file);
}

unsigned int scrtoy_GLCompileShader(const char * shadercode, GLenum shadertype)
{
	unsigned int sh = glf.CreateShader(shadertype);
	glf.ShaderSource(sh, 1, &shadercode, NULL);
	glf.CompileShader(sh);
	return sh;
}

void scrtoy_GLLoadShader()
{
	static const char * vertexshadercode =
		"#version 330\n"
		"layout(location = 0) in vec3 vertexPosition_modelspace;"
		"out vec2 fragCoord;"
		"void main()"
		"{"
			"gl_Position.xyz = vertexPosition_modelspace;"
			"gl_Position.w = 1.0;"
			"fragCoord = gl_Position.xy;"
		"}\n";


	// Compile shader
	
	glData.prog = glf.CreateProgram();
	// Fragment shader
	LLfile * shadercode = scrtoy_LoadFile("shader.glsl");
	const unsigned int vertexshader = scrtoy_GLCompileShader(vertexshadercode, GL_VERTEX_SHADER);
	const unsigned int fragshader = scrtoy_GLCompileShader(scrtoy_GetContents(shadercode), GL_FRAGMENT_SHADER);
	scrtoy_CloseFile(shadercode);

	glf.AttachShader(glData.prog, vertexshader);
	glf.AttachShader(glData.prog, fragshader);
	glf.LinkProgram(glData.prog);

	// Get variable locations
	glData.resolutionLoc = glf.GetUniformLocation(glData.prog, "iResolution");
	glData.timeLoc = glf.GetUniformLocation(glData.prog, "iTime");

	glData.sizex = GetSystemMetrics(SM_CXSCREEN);
	glData.sizey = GetSystemMetrics(SM_CXSCREEN);


	glf.GenBuffers(1, &glData.bufId);
	glf.BindBuffer(GL_ARRAY_BUFFER, glData.bufId);
	/*float points[] = {
		-1.0f, -1.0f,  0.0f,
		-1.0f,  1.0f,  0.0f,
		 1.0f,  1.0f,  0.0f,
		-1.0f, -1.0f,  0.0f,
		 1.0f,  1.0f,  0.0f,
		 1.0f, -1.0f,  0.0f
	};*/
	float points[] = {
		-1.0f, -1.0f,  0.0f,
		-1.0f,  1.0f,  0.0f,
		 1.0f,  1.0f,  0.0f,
		 1.0f, -1.0f,  0.0f
	};
	glf.BufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), points, GL_STATIC_DRAW);
	glf.EnableVertexAttribArray(0);
}
void scrtoy_GLRender(uint64_t time)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glf.UseProgram(glData.prog);
	glf.Uniform1f(glData.timeLoc, (float)time / 1000.f);
	glf.Uniform2f(glData.resolutionLoc, (float)glData.sizex, (float)glData.sizey);

	glf.BindBuffer(GL_ARRAY_BUFFER, glData.bufId);
	glf.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glDrawArrays(GL_QUADS, 0, 4);
}
