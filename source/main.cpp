#include "libraryInterface.hpp"
#include "sceneStartMenu.hpp"

int main(int argc, char* argv[]) {
	initLib();
	
	changeScene(new SceneStartMenu());

	while (aptMainLoop())
	{
		if(current_scene->userInput()) break;

		current_scene->tick();

		renderPipeline(current_scene);
	}

	deinitLib();
	return 0;
}
