#include "images.hpp"

#include <windows.h>
#include <d3d11.h>
#include <wincodec.h>
#include <vector>

#pragma comment(lib, "windowscodecs.lib")

extern "C" unsigned char icon[];

namespace images
{
    static ImTextureID s_icon = 0;
    static const int kIconBytes = 36323;

    static ID3D11ShaderResourceView* LoadPng(ID3D11Device* device, ID3D11DeviceContext* ctx, const unsigned char* data, int size)
    {
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

        IWICImagingFactory* factory = nullptr;
        if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory))))
        {
            return nullptr;
        }

        ID3D11ShaderResourceView* srv = nullptr;
        IWICStream* stream = nullptr;
        IWICBitmapDecoder* decoder = nullptr;
        IWICBitmapFrameDecode* frame = nullptr;
        IWICFormatConverter* conv = nullptr;

        if (SUCCEEDED(factory->CreateStream(&stream)) &&
            SUCCEEDED(stream->InitializeFromMemory((BYTE*)data, (DWORD)size)) &&
            SUCCEEDED(factory->CreateDecoderFromStream(stream, nullptr, WICDecodeMetadataCacheOnLoad, &decoder)) &&
            SUCCEEDED(decoder->GetFrame(0, &frame)) &&
            SUCCEEDED(factory->CreateFormatConverter(&conv)) &&
            SUCCEEDED(conv->Initialize(frame, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeMedianCut)))
        {
            UINT w = 0;
            UINT h = 0;
            conv->GetSize(&w, &h);
            std::vector<unsigned char> pixels((size_t)w * (size_t)h * 4);
            if (SUCCEEDED(conv->CopyPixels(nullptr, w * 4, (UINT)pixels.size(), pixels.data())))
            {
                D3D11_TEXTURE2D_DESC desc = {};
                desc.Width = w;
                desc.Height = h;
                desc.MipLevels = 0;
                desc.ArraySize = 1;
                desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                desc.SampleDesc.Count = 1;
                desc.Usage = D3D11_USAGE_DEFAULT;
                desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
                desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

                ID3D11Texture2D* tex = nullptr;
                if (SUCCEEDED(device->CreateTexture2D(&desc, nullptr, &tex)))
                {
                    ctx->UpdateSubresource(tex, 0, nullptr, pixels.data(), w * 4, 0);
                    D3D11_SHADER_RESOURCE_VIEW_DESC sd = {};
                    sd.Format = desc.Format;
                    sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                    sd.Texture2D.MipLevels = (UINT)-1;
                    device->CreateShaderResourceView(tex, &sd, &srv);
                    if (srv != nullptr)
                    {
                        ctx->GenerateMips(srv);
                    }
                    tex->Release();
                }
            }
        }

        if (conv) { conv->Release(); }
        if (frame) { frame->Release(); }
        if (decoder) { decoder->Release(); }
        if (stream) { stream->Release(); }
        if (factory) { factory->Release(); }
        return srv;
    }

    void Init(void* d3dDevice, void* d3dContext)
    {
        ID3D11ShaderResourceView* srv = LoadPng((ID3D11Device*)d3dDevice, (ID3D11DeviceContext*)d3dContext, icon, kIconBytes);
        if (srv != nullptr)
        {
            s_icon = (ImTextureID)(uintptr_t)srv;
        }
    }

    ImTextureID Icon()
    {
        return s_icon;
    }
}
