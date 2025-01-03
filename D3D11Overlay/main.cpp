#include "Window.hpp"
#include <thread>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	int WIDHT = 800;
	int HEIGHT = 600;

	Window window(hInstance, L"Overlay", WIDHT, HEIGHT);
	while (window.ProcessMessages()) {

	}
}