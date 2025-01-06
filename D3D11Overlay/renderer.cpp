#include "renderer.hpp"
#include <stdexcept>

Renderer::Renderer(HWND hwnd, int width, int height) : m_windowWidth(width), m_windowHeight(height) {
	initializeDevice(hwnd, width, height);
	initializeShaders();
	initializeVertexBuffer();
}

void Renderer::initializeDevice(HWND hwnd, int width, int height) {
    // Describe and create the swap chain
    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.Width = width;
    scd.BufferDesc.Height = height;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // Create the device and swap chain
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &scd,
        m_swapChain.GetAddressOf(),
        m_device.GetAddressOf(),
        nullptr,
        m_context.GetAddressOf());

    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create device and swap chain");
    }

    // Get the back buffer and create a render target view
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(backBuffer.GetAddressOf()));
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to get back buffer");
    }

    hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create render target view");
    }

    backBuffer.Reset();
	// Set the render target as the back buffer
    m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);


    // Set up the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    //viewport.MinDepth = 0.0f;
    //viewport.MaxDepth = 1.0f;

    m_context->RSSetViewports(1, &viewport);

    // Create blend state for transparency - future reference
 //   D3D11_BLEND_DESC blendDesc;
	//ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	//blendDesc.AlphaToCoverageEnable = FALSE;
 //   blendDesc.RenderTarget[0].BlendEnable = TRUE;
 //   blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
 //   blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
 //   blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
 //   blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
 //   blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
 //   blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
 //   blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

 //   hr = m_device->CreateBlendState(&blendDesc, m_blendState.GetAddressOf());
 //   if (FAILED(hr)) {
 //       throw std::runtime_error("Failed to create blend state");
 //   }

 //   float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
 //   m_context->OMSetBlendState(m_blendState.Get(), blendFactor, 0xffffffff);
}

void Renderer::initializeShaders() {
	// Compile the vertex shader
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    compileFlags |= D3DCOMPILE_DEBUG;
#endif

    HRESULT hr = D3DCompileFromFile(
        L"shaders.shader",
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "VShader",
        "vs_5_0",
        compileFlags,
        0,
        vertexShaderBlob.GetAddressOf(),
        errorBlob.GetAddressOf());

    if (FAILED(hr)) {
        if (errorBlob) {
			throw std::runtime_error(static_cast<char*>(errorBlob->GetBufferPointer()));
        }

		throw std::runtime_error("Failed to compile vertex shader");
    }

	// Compile the pixel shader
	hr = D3DCompileFromFile(
		L"shaders.shader",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PShader",
		"ps_5_0",
		compileFlags,
		0,
		pixelShaderBlob.GetAddressOf(),
		errorBlob.GetAddressOf());

	if (FAILED(hr)) {
		if (errorBlob) {
			throw std::runtime_error(static_cast<char*>(errorBlob->GetBufferPointer()));
		}

		throw std::runtime_error("Failed to compile pixel shader");
	}

	// Create the shaders
	hr = m_device->CreateVertexShader(
        vertexShaderBlob->GetBufferPointer(), 
        vertexShaderBlob->GetBufferSize(), 
        nullptr, 
        m_vertexShader.GetAddressOf());

    if (FAILED(hr)) {
		throw std::runtime_error("Failed to create vertex shader");
    }

	hr = m_device->CreatePixelShader(
		pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize(),
		nullptr,
		m_pixelShader.GetAddressOf());

    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create pixel shader");
    }

	// Set the shaders
	m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	// Create the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = m_device->CreateInputLayout(
		layout,
		ARRAYSIZE(layout),
		vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize(),
		m_inputLayout.GetAddressOf());

    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create input layout");
    }

	// Set the input layout
	m_context->IASetInputLayout(m_inputLayout.Get());
}

void Renderer::initializeVertexBuffer() {
	// Create the vertex buffer
    D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(Vertex) * MAX_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	HRESULT hr = m_device->CreateBuffer(&bd, nullptr, m_vertexBuffer.GetAddressOf());
    if (FAILED(hr)) {
		throw std::runtime_error("Failed to create vertex buffer");
    }
}

void Renderer::beginFrame() {
    const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
}

void Renderer::render() {
    if (m_rectVertices.empty()) return;

    // Map the vertex buffer to gain write access
    D3D11_MAPPED_SUBRESOURCE ms;
    HRESULT hr = m_context->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to map vertex buffer");
    }

    // Copy the vertex data into the mapped buffer
    memcpy(ms.pData, m_rectVertices.data(), sizeof(Vertex) * m_rectVertices.size());
    m_context->Unmap(m_vertexBuffer.Get(), 0);

    // Set the vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

    // Set the primitive topology to triangle list
    m_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_context->Draw(m_rectVertices.size(), 0);

    m_rectVertices.clear();

    // Present the rendered frame to the screen
    m_swapChain->Present(0, 0);
}

std::vector<Renderer::Vertex> Renderer::createRectangleVertices(const Rectangle& rect) {
    float normalizedX = (2.0f * rect.x / m_windowWidth) - 1.0f;
    float normalizedY = 1.0f - (2.0f * rect.y / m_windowHeight);
    float normalizedWidth = 2.0f * rect.width / m_windowWidth;
    float normalizedHeight = 2.0f * rect.height / m_windowHeight;

    std::vector<Vertex> vertices(6);
    vertices[0] = {normalizedX - normalizedWidth / 2, normalizedY + normalizedHeight / 2, 0.0f,
                   rect.color.r, rect.color.g, rect.color.b, rect.color.a};
    vertices[1] = {normalizedX + normalizedWidth / 2, normalizedY + normalizedHeight / 2, 0.0f,
                   rect.color.r, rect.color.g, rect.color.b, rect.color.a};
    vertices[2] = {normalizedX - normalizedWidth / 2, normalizedY - normalizedHeight / 2, 0.0f,
                   rect.color.r, rect.color.g, rect.color.b, rect.color.a};
    vertices[3] = {normalizedX + normalizedWidth / 2, normalizedY + normalizedHeight / 2, 0.0f,
                   rect.color.r, rect.color.g, rect.color.b, rect.color.a};
    vertices[4] = {normalizedX + normalizedWidth / 2, normalizedY - normalizedHeight / 2, 0.0f,
                   rect.color.r, rect.color.g, rect.color.b, rect.color.a};
    vertices[5] = {normalizedX - normalizedWidth / 2, normalizedY - normalizedHeight / 2, 0.0f,
                   rect.color.r, rect.color.g, rect.color.b, rect.color.a};

    return vertices;
}

void Renderer::addRectangle(const Rectangle& rect) {
	std::vector<Vertex> vertices = createRectangleVertices(rect);
	if (m_rectVertices.size() + vertices.size() > MAX_VERTEX) {
		throw std::runtime_error("Exceeded maximum number of vertices");
	}

	m_rectVertices.insert(m_rectVertices.end(), vertices.begin(), vertices.end());
}