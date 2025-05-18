#include "libraryInterface.hpp"
#include "scenes.hpp"

static Scene* current_scene;

int main(int argc, char* argv[]) {
	initLib();
	
	current_scene = new SceneEditor;
	current_scene->init();

	while (aptMainLoop())
	{
		if(current_scene->userInput()) break;

		current_scene->tick();

		renderPipeline(current_scene);
	}

	deinitLib();
	return 0;
}
