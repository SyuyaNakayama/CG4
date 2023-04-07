#include "MyGame.h"
#pragma warning(push)
#pragma warning(disable:28251)

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	std::unique_ptr<Framework> game = std::make_unique<MyGame>();
	game->Run();
	return 0;
}
#pragma warning(pop)