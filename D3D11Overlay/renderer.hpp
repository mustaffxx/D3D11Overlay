#include <Windows.h>
#include <d3d11.h>
//#include <DirectXMath.h>
//#include <DirectXPackedVector.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")


class Renderer {
public:
	struct Vertex {
		float x, y, z;
		float r, g, b, a;
	};

	Renderer(HWND hwnd, int width, int height);
	~Renderer() = default;
private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;

	void initializeDevice(HWND hwnd, int width, int height);
	void initializeShaders();
};