
#include "common.h"
#include "gl_local.h"

GLuint glsl_CurProgramId = 0;
GLuint glsl_2DprogramId = 0;
GLuint glsl_WorldProgramId = 0;
GLint u_color = -1;
GLint u_mvMtx = -1;
GLint u_projMtx = -1;

GLuint R_CreateShader(char *src, GLint type)
{
	GLuint id = pglCreateShader(type);
	pglShaderSource(id,1,&src,NULL);
	pglCompileShader(id);

	GLint compile_ok=GL_FALSE;
	pglGetShaderiv(id,GL_COMPILE_STATUS, &compile_ok);
	if(compile_ok==GL_FALSE)
	{
		printf("Error glCompileShader: %x\n",id);
		//print_log(id);
		pglDeleteShader(id);
		return -1;
	}

	return id;
}

GLuint R_CreateProgram(char *vert, char *frag)
{
	GLuint vs = R_CreateShader(vert,GL_VERTEX_SHADER);
	GLuint fs = R_CreateShader(frag,GL_FRAGMENT_SHADER);
	GLuint id = pglCreateProgram();

	pglAttachShader(id,vs);
	pglAttachShader(id,fs);
	pglLinkProgram(id);
	GLint link_ok=GL_FALSE;
	pglGetProgramiv(id,GL_LINK_STATUS,&link_ok);
	if(!link_ok)
	{
		printf("Error glLinkProgram: %x\n",id);
		//print_log(id);
		return -1;
	}

	return id;
}

void R_InitShaders()
{
	char vertS[]="attribute vec4 a_position;\n"\
	"attribute vec2 a_uv;\n"\
	"varying vec2 v_uv;\n"\
	"const vec2 screen= vec2(0.0025,0.003333333);"\
	"void main(){\n"\
	"	gl_Position = vec4((a_position.xy*screen)-1.0,0.0,1.0);\n"\
	"	gl_Position.y*=-1.0;"\
	"	v_uv=a_uv;\n"\
	"}";
	char fragS[]="varying vec2 v_uv;\n"\
	"uniform sampler2D u_tex;\n"\
	"uniform vec4 u_color;"\
	"void main(){\n"\
	"	gl_FragColor = texture2D(u_tex,v_uv)*u_color;\n"\
	"}";

	char WorldVertS[]="attribute vec4 a_position;\n"\
	"attribute vec2 a_uv;\n"\
	"varying vec2 v_uv;\n"\
	"uniform mat4 u_mvMtx;"\
	"uniform mat4 u_projMtx;"\
	"void main(){\n"\
	"	gl_Position = u_projMtx*u_mvMtx*a_position;\n"\
	"	v_uv = a_uv;\n"\
	"}";
	char WorldFragS[]="varying vec2 v_uv;\n"\
	"uniform sampler2D u_tex;\n"\
	"uniform vec4 u_color;"\
	"void main(){\n"\
	"	gl_FragColor = texture2D(u_tex,v_uv)*u_color;\n"\
	"}";

	glsl_2DprogramId=R_CreateProgram(vertS,fragS);
	u_color = pglGetUniformLocation(glsl_2DprogramId,"u_color");
	//printf("u_color %x\n",u_color);
	//R_Use2DProgram();
	//R_ColorUniform(1, 1, 1, 1);

	glsl_WorldProgramId=R_CreateProgram(WorldVertS,WorldFragS);
	//printf("glsl_WorldProgramId %x\n",glsl_WorldProgramId);

	u_mvMtx = pglGetUniformLocation(glsl_WorldProgramId,"u_mvMtx");
	u_projMtx = pglGetUniformLocation(glsl_WorldProgramId,"u_projMtx");
}

void R_Use2DProgram()
{
	glsl_CurProgramId = glsl_2DprogramId;
	pglUseProgram(glsl_2DprogramId);
}

void R_UseWorldProgram()
{
	glsl_CurProgramId = glsl_WorldProgramId;
	pglUseProgram(glsl_WorldProgramId);
}

void R_ColorUniform(GLfloat r,GLfloat g, GLfloat b, GLfloat a)
{
	if(!glsl_CurProgramId)
		return;

	pglUniform4f(u_color,r,g,b,a);
}

void R_ProjMtxUniform(const matrix4x4 source)
{
	GLfloat	dest[16];

	Matrix4x4_ToArrayFloatGL( source, dest );
	R_UseWorldProgram();
	pglUniformMatrix4fv(u_projMtx,1,GL_FALSE,dest);
}

void R_ModelViewMtxUniform(const matrix4x4 source)
{
	GLfloat	dest[16];

	Matrix4x4_ToArrayFloatGL( source, dest );
	R_UseWorldProgram();
	pglUniformMatrix4fv(u_mvMtx,1,GL_FALSE,dest);
}