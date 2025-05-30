#include "libraryInterface.hpp"
#include "scene.hpp"

int main(int argc, char* argv[]) {
	initLib();
	
	changeScene(scene_start_menu);

	while (aptMainLoop())
	{
		if(current_scene->userInput()) break;

		current_scene->tick();

		renderPipeline(current_scene);
	}

	deinitLib();
	return 0;
}
