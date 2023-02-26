#include "../lib/AGL/agl.hpp"

int main()
{
	agl::RenderWindow window;
	window.setup({1920, 1080}, "PhysicsTest");
	window.setFPS(60);
	window.setClearColor(agl::Color::Black);

	agl::Event event;
	event.setWindow(window);

	while (!event.windowClose())
	{
		event.poll([](XEvent xev) {});

		window.clear();
		window.display();
	}

	window.close();
}
