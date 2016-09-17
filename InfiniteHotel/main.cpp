#include <CodeAdapter\EasyCA.h>
#include <CodeAdapterSFML\SFMLFactory.h>

#include "MainScene.h"




int main()
{
	initializeCA<SFMLFactory>();


	auto window = caFactory->createWindow();
	window->create(caDraw::Size(1280, 960), caUtil::String("Infinite Hotel"), false);
	window->disableFramerate();


	auto mainScene = canew<MainScene>();

	auto sceneManager = canew<caUtil::SceneManager>();
	sceneManager->replaceScene(*window, mainScene);

	window->setSceneManager(sceneManager);


	bool bDraw = true;


	while (window->isRunning())
	{
		caTouch->update();
		caKeyboard->update(*window);


		if (sceneManager->hasScene() == false)
			window->exit();


		if (caKeyboard->isKeyPressed(caSys::Keys::LShift)
			&& caKeyboard->isKeyDown(caSys::Keys::Tab))
		{
			bDraw = !bDraw;
			mainScene->enable(bDraw);
		}


		window->update();
		if (bDraw)
		{
			window->draw(caDraw::Color(230, 230, 230));
		}
	}


	return 0;
}

