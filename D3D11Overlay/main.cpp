#include "Window.hpp"
#include "renderer.hpp"
#include <thread>
#include <chrono>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	int WIDTH = 800;
	int HEIGHT = 600;

	Window window(hInstance, L"Overlay", WIDTH, HEIGHT);
	Renderer renderer(window.GetWindow(), WIDTH, HEIGHT);

	while (window.ProcessMessages()) {
		renderer.beginFrame();

		Renderer::Rectangle rect = {
			WIDTH / 2.0f, HEIGHT / 2.0f,
			200.0f, 100.0f,
			{1.0f, 0.0f, 0.0f, 0.25f}
		};

		renderer.addRectangle(rect);
		renderer.render();

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	return 0;
}