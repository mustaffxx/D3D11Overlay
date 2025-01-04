#pragma once
#include <Windows.h>
#include <string>

class Window {
public:
    Window(HINSTANCE hInstance, const std::wstring& title, int width, int height);
    ~Window();
    bool ProcessMessages();
	HWND GetWindow() const { return hWindow; }
private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HWND hWindow;
    static constexpr const wchar_t* WINDOW_CLASS_NAME = L"OverlayWindow";
};