/* date = April 30th 2021 8:38 pm */

#ifndef OTTER_D3D_H
#define OTTER_D3D_H

#include <d3d11_4.h>
#include <d3d10.h>
#include <dxgi1_5.h>
#if DEBUG_RENDERER
#include <dxgidebug.h>
#endif
//~ STRUCTURES
typedef struct og_D3D {
    IDXGIFactory5* factory;
    IDXGISwapChain1* swapChain;
    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    ID3D11RenderTargetView* renderTargetView;
    ID3D11Texture2D* backBuffer;
    ID3D11Texture2D* depthStencilBuffer;
    ID3D11DepthStencilView* depthStencilView;
    ID3D11BlendState* blendState;
    ID3D11DepthStencilState* depthStencilState;
    ID3D11RasterizerState* rasterizerState;
    ID3D11InputLayout* inputLayout;
    
    memoryIndex vertexCount;
} og_D3D;

typedef struct d3d_Vertex {
    union {
        struct {
            f32 x, y, z, w;
        };
        f32 position[4];
    };
    union {
        struct {
            f32 r, g, b, a;
        };
        f32 colour[4];
    };
} d3d_Vertex;

//~ FUNCTIONS
internal void
win32_error_format(HRESULT hr, u32 line, char* file);

inline internal void
win32_d3d_shader_load(char* compiledShaderFileName, win32_State* platformState, ID3DBlob** shaderBlob);

#if OG_INTERNAL | DEBUG_RENDERER

#ifndef HR
#define HR(x)\
{\
HRESULT hr = x;\
if (FAILED(hr)) {\
win32_error_format(hr, __LINE__, __FILE__);\
}\
}
#endif // ifndef HR

#else

#ifndef HR
#define HR(x) x
#endif // ifndef HR

#endif // OG_INTERNAL

internal void
ogD3D_render_hello_triangle(og_D3D* d3d,
                            win32_State* platformState) {
    d3d_Vertex triangleVertices[] = {
        {
            .x = 0.0f, .y = 0.5f, .z = 0.0f, .w = 1.0f,
            .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f
        },
        {
            .x = 0.5f, .y = -0.5f, .z = 0.0f, .w = 1.0f,
            .r = 0.0f, .g = 1.0f, .b = 0.0f,  .a = 1.0f
        },
        {
            .x = -0.5f, .y = -0.5f, .z = 0.0f, .w = 1.0f,
            .r =  0.0f, .g =  0.0f, .b = 1.0f, .a = 1.0f
        }
    };
    d3d->vertexCount = ARRAY_COUNT(triangleVertices);
    
    //- Create Vertex, Index, & Constant Buffers
    D3D11_BUFFER_DESC vertexBufferDesc = {
        .Usage = D3D11_USAGE_DEFAULT,
        .ByteWidth = sizeof(triangleVertices),
        .BindFlags = D3D11_BIND_VERTEX_BUFFER
    };
    D3D11_SUBRESOURCE_DATA vertInitData = { .pSysMem = triangleVertices };
    ID3D11Buffer* vertexBuffer;
    HR(ID3D11Device_CreateBuffer(d3d->device,
                                 &vertexBufferDesc, &vertInitData, &vertexBuffer));
    UINT vertStride = (UINT)sizeof(d3d_Vertex);
    UINT vertOffset = 0;
    ID3D11DeviceContext_IASetVertexBuffers(d3d->deviceContext,
                                           0, 1, &vertexBuffer, &vertStride, &vertOffset);
    
    //- Pixel Shader
    ID3D11PixelShader* pixelShader;
    ID3DBlob* shaderBlob = 0;
    win32_d3d_shader_load("pixel.cso", platformState, &shaderBlob);
    HR(ID3D11Device_CreatePixelShader(d3d->device,
                                      ID3D10Blob_GetBufferPointer(shaderBlob),
                                      ID3D10Blob_GetBufferSize(shaderBlob),
                                      0,
                                      &pixelShader));
    
    //- Vertex Shader
    ID3D11VertexShader* vertexShader;
    win32_d3d_shader_load("vertex.cso", platformState, &shaderBlob);
    HR(ID3D11Device_CreateVertexShader(d3d->device,
                                       ID3D10Blob_GetBufferPointer(shaderBlob),
                                       ID3D10Blob_GetBufferSize(shaderBlob),
                                       0,
                                       &vertexShader));
    
    //- Create Input Layout
    D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
        {
            .SemanticName = "POSITION",
            .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
        },
        {
            .SemanticName = "COLOUR",
            .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .AlignedByteOffset = 16,
        }
    };
    
    HR(ID3D11Device_CreateInputLayout(d3d->device,
                                      vertexDesc,
                                      2,
                                      ID3D10Blob_GetBufferPointer(shaderBlob),
                                      ID3D10Blob_GetBufferSize(shaderBlob),
                                      &d3d->inputLayout));
    
    //- Bind The Shaders
    ID3D11DeviceContext_VSSetShader(d3d->deviceContext,
                                    vertexShader, 0, 0);
    ID3D11DeviceContext_PSSetShader(d3d->deviceContext,
                                    pixelShader, 0, 0);
    
}
#endif //OTTER_D3D_H
