#include "graphics.hpp"

static Vertex* list;
static int list_length;

C3D_RenderTarget* topLeft, *topRight, *bottom;

//Shader info
static DVLB_s* vshader_dvlb;
static shaderProgram_s program;
static int uLoc_projection, uLoc_modelView;
static C3D_Mtx projection;
static int uLoc_lightVec, uLoc_lightHalfVec, uLoc_lightClr, uLoc_material;
static C3D_Mtx material =
{
	{
	{ { 0.0f, 0.2f, 0.2f, 0.2f } }, // Ambient
	{ { 0.0f, 0.4f, 0.4f, 0.4f } }, // Diffuse
	{ { 0.0f, 0.8f, 0.8f, 0.8f } }, // Specular
	{ { 1.0f, 0.0f, 0.0f, 0.0f } }, // Emission
	}
};

static C3D_AttrInfo vbo_attrInfo;
static C3D_BufInfo vbo_bufInfo;
static void* vbo_data;

ViewState state = VIEW_TOP;

void drawVBO(float iod,C3D_Tex *tex){
	// Bind the program to render the VBO scene
	sceneBind(tex);

	// Compute the projection matrix
	Mtx_PerspStereoTilt(&projection, C3D_AngleFromDegrees(40.0f), C3D_AspectRatioTop, 0.01f, 1000.0f, iod, 2.0f, false);

	// Calculate the modelView matrix
	// // Step 1: Compute the rotated camera position
	C3D_FVec baseCamPos = FVec4_New(0.0f, 0.0f, 3.0f, 1.0f); // camera 3 units *behind* origin
	C3D_FVec camPos = Mtx_MultiplyFVec4(&cameraRotation, baseCamPos);

	// Step 2: Build look-at view matrix from camPos -> target
	C3D_FVec target = FVec3_New(0.0f, 0.0f, 0.0f);
	// Rotate the up vector (0, 1, 0) by the cameraRotation matrix
	C3D_FVec defaultUp = FVec3_New(0.0f, 1.0f, 0.0f);
	C3D_FVec up = Mtx_MultiplyFVec3(&cameraRotation, defaultUp);

	C3D_Mtx modelView;
	Mtx_LookAt(&modelView, camPos, target, up, false);

	// Update the uniforms
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projection);
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView,  &modelView);
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_material,   &material);
	C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightVec,     0.0f, 0.0f, -1.0f, 0.0f);
	C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightHalfVec, 0.0f, 0.0f, -1.0f, 0.0f);
	C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightClr,     1.0f, 1.0f,  1.0f, 1.0f);


	// Draw the VBO
	C3D_DrawArrays(GPU_TRIANGLES, 0, list_length);
}
void updateVBO(Vertex* _list, int count){
	list = _list;
	list_length = count;
	memcpy(vbo_data,list,count*sizeof(Vertex));
}
void initShader(){
	// Load the vertex shader, create a shader program and bind it
	vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
	shaderProgramInit(&program);
	shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);

	// Get the location of the uniforms
	uLoc_projection   = shaderInstanceGetUniformLocation(program.vertexShader, "projection");
	uLoc_modelView    = shaderInstanceGetUniformLocation(program.vertexShader, "modelView");
	uLoc_lightVec     = shaderInstanceGetUniformLocation(program.vertexShader, "lightVec");
	uLoc_lightHalfVec = shaderInstanceGetUniformLocation(program.vertexShader, "lightHalfVec");
	uLoc_lightClr     = shaderInstanceGetUniformLocation(program.vertexShader, "lightClr");
	uLoc_material     = shaderInstanceGetUniformLocation(program.vertexShader, "material");

	// Configure attributes for use with the vertex shader
	AttrInfo_Init(&vbo_attrInfo);
	AttrInfo_AddLoader(&vbo_attrInfo, 0, GPU_FLOAT, 3); // v0=position
	AttrInfo_AddLoader(&vbo_attrInfo, 1, GPU_FLOAT, 2); // v1=uv
	AttrInfo_AddLoader(&vbo_attrInfo, 2, GPU_FLOAT, 3); // v2=normal

	

	//Init with a default
	// Create the VBO (vertex buffer object)
	vbo_data = linearAlloc(sizeof(model->triCount));
    updateVBO(model->triArray,model->triCount);

	//rotation
	Mtx_Identity(&cameraRotation);

	// Configure buffers
	BufInfo_Init(&vbo_bufInfo);
	BufInfo_Add(&vbo_bufInfo, vbo_data, sizeof(Vertex), 3, 0x210);

}
void deinitShader(){
	linearFree(vbo_data);
	shaderProgramFree(&program);
	DVLB_Free(vshader_dvlb);
}
void sceneBind(C3D_Tex *tex)
{
	C3D_BindProgram(&program);
	C3D_SetAttrInfo(&vbo_attrInfo);
	C3D_SetBufInfo(&vbo_bufInfo);
	//C3D_DepthTest(true, GPU_GREATER, GPU_WRITE_ALL);
	//C3D_CullFace(GPU_CULL_BACK_CCW);
	C3D_TexSetFilter(tex, GPU_LINEAR, GPU_NEAREST);
	C3D_TexSetWrap(tex, GPU_CLAMP_TO_EDGE, GPU_CLAMP_TO_EDGE);

	C3D_TexBind(0, tex);
	// Configure the first fragment shading substage to blend the fragment primary color
	// with the fragment secondary color.
	// See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvInit(env);
	if (!tex || !tex->data) {
		C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
		C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
	}
	else {
		C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR);
		C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
	}
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

	#ifdef DEBUG
	consoleInit(GFX_TOP, NULL);
	#else
	C3D_RenderTargetSetOutput(topLeft,  GFX_TOP, GFX_LEFT,  DISPLAY_TRANSFER_FLAGS);
	C3D_RenderTargetSetOutput(topRight, GFX_TOP, GFX_RIGHT, DISPLAY_TRANSFER_FLAGS);
	#endif
	C3D_RenderTargetSetOutput(bottom, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

}