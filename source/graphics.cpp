#include "graphics.hpp"

C3D_RenderTarget* topLeft, *topRight, *bottom;

//Shader info
static DVLB_s* vshader_dvlb;
static shaderProgram_s program;
static int uLoc_projection, uLoc_modelView;
static C3D_Mtx projection;

static C3D_LightEnv lightEnv;
static C3D_Light light;
static C3D_LightLut lut_Spec;

static C3D_AttrInfo vbo_attrInfo;
static C3D_BufInfo vbo_bufInfo;
static void* vbo_data;
float angleX = 45, angleY = 45;


void drawVBO(float iod){
	// Bind the program to render the VBO scene
	sceneBind();

	// Compute the projection matrix
	Mtx_PerspStereoTilt(&projection, C3D_AngleFromDegrees(40.0f), C3D_AspectRatioTop, 0.01f, 1000.0f, iod, 2.0f, false);

	C3D_FVec objPos   = FVec4_New(0.0f, 0.0f, -3.0f, 1.0f);
	C3D_FVec lightPos = FVec4_New(0.0f, 0.0f, -0.5f, 1.0f);

	// Calculate the modelView matrix
	C3D_Mtx modelView;
	Mtx_Identity(&modelView);
	Mtx_Translate(&modelView, objPos.x, objPos.y, objPos.z, true);
	Mtx_RotateY(&modelView, C3D_Angle(angleY), true);
	Mtx_RotateX(&modelView, C3D_Angle(angleX), true);
	//Mtx_Scale(&modelView, 2.0f, 2.0f, 2.0f);

	C3D_LightPosition(&light, &lightPos);

	// Update the uniforms
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projection);
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView,  &modelView);

	// Draw the VBO
	C3D_DrawArrays(GPU_TRIANGLES, 0, vertex_list_count);
}
void updateVBO(){
	memcpy(vbo_data,vertex_list,vertex_list_count*sizeof(vertex));
}
void initShader(){
	// Load the vertex shader, create a shader program and bind it
	vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
	shaderProgramInit(&program);
	shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);

	// Get the location of the uniforms
	uLoc_projection   = shaderInstanceGetUniformLocation(program.vertexShader, "projection");
	uLoc_modelView    = shaderInstanceGetUniformLocation(program.vertexShader, "modelView");

	// Configure attributes for use with the vertex shader
	AttrInfo_Init(&vbo_attrInfo);
	AttrInfo_AddLoader(&vbo_attrInfo, 0, GPU_FLOAT, 3); // v0=position
	AttrInfo_AddLoader(&vbo_attrInfo, 1, GPU_FLOAT, 3); // v1=normal

	// Create the VBO (vertex buffer object)
	vbo_data = linearAlloc(sizeof(vertex_list));
	memcpy(vbo_data, vertex_list, sizeof(vertex_list));

	// Configure buffers
	BufInfo_Init(&vbo_bufInfo);
	BufInfo_Add(&vbo_bufInfo, vbo_data, sizeof(vertex), 2, 0x10);

	static const C3D_Material material =
	{
		{ 0.1f, 0.1f, 0.1f }, //ambient
		{ 0.4f, 0.4f, 0.4f }, //diffuse
		{ 0.5f, 0.5f, 0.5f }, //specular0
		{ 0.0f, 0.0f, 0.0f }, //specular1
		{ 0.0f, 0.0f, 0.0f }, //emission
	};

	C3D_LightEnvInit(&lightEnv);
	C3D_LightEnvMaterial(&lightEnv, &material);

	LightLut_Phong(&lut_Spec, 20.0f);
	C3D_LightEnvLut(&lightEnv, GPU_LUT_D0, GPU_LUTINPUT_NH, false, &lut_Spec);

	C3D_LightInit(&light, &lightEnv);
}
void deinitShader(){
	linearFree(vbo_data);
	shaderProgramFree(&program);
	DVLB_Free(vshader_dvlb);
}
void sceneBind()
{
	C3D_BindProgram(&program);
	C3D_SetAttrInfo(&vbo_attrInfo);
	C3D_SetBufInfo(&vbo_bufInfo);
	C3D_LightEnvBind(&lightEnv);
	C3D_DepthTest(true, GPU_GREATER, GPU_WRITE_ALL);
	C3D_CullFace(GPU_CULL_BACK_CCW);

	// Configure the first fragment shading substage to blend the fragment primary color
	// with the fragment secondary color.
	// See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvInit(env);
	C3D_TexEnvSrc(env, C3D_Both, GPU_FRAGMENT_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR);
	C3D_TexEnvFunc(env, C3D_Both, GPU_ADD);

	// Clear out the other texenvs
	C3D_TexEnvInit(C3D_GetTexEnv(1));
	C3D_TexEnvInit(C3D_GetTexEnv(2));
	C3D_TexEnvInit(C3D_GetTexEnv(3));
	C3D_TexEnvInit(C3D_GetTexEnv(4));
	C3D_TexEnvInit(C3D_GetTexEnv(5));
}
void renderPipeline(Scene *current_scene){
	float slider = osGet3DSliderState();
	float iod = slider/3;

	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	{
		//Top Left
		C3D_RenderTargetClear(topLeft, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
		C3D_FrameDrawOn(topLeft);
		C2D_SceneTarget(topLeft);
		current_scene->renderTop3D(-iod);

		C2D_Prepare();
		current_scene->renderTop2D(-iod);
		C2D_Flush();
		//Top Right if 3D slider
		if (iod > 0.0f)
		{
			C3D_RenderTargetClear(topRight, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
			C3D_FrameDrawOn(topRight);
			C2D_SceneTarget(topRight);
			current_scene->renderTop3D(iod);

			C2D_Prepare();
			current_scene->renderTop2D(iod);
			C2D_Flush();
		}
		//Bottom
		C3D_RenderTargetClear(bottom, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
		C3D_FrameDrawOn(bottom);
		C2D_SceneTarget(bottom);
		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projection);
		current_scene->renderBottom3D();

		C2D_Prepare();
		current_scene->renderBottom2D();
		C2D_Flush();
	}
	C3D_FrameEnd(0);
}
void createScreens(){
    topLeft  = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	topRight = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	bottom = C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);

	C3D_RenderTargetSetOutput(topLeft,  GFX_TOP, GFX_LEFT,  DISPLAY_TRANSFER_FLAGS);
	C3D_RenderTargetSetOutput(topRight, GFX_TOP, GFX_RIGHT, DISPLAY_TRANSFER_FLAGS);
	C3D_RenderTargetSetOutput(bottom, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

}