#include "stdafx.h"
#include "SpriteImage.h"
#include <Windows.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <cmath>
#include <iostream>
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")

namespace
{
    struct ComScope
    {
        HRESULT result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        ~ComScope() { if (SUCCEEDED(result)) CoUninitialize(); }
    };

    std::wstring FindAsset(const std::wstring& filename)
    {
        wchar_t module[32768] = {};
        DWORD length = GetModuleFileNameW(nullptr, module, 32768);
        std::vector<std::wstring> candidates;
        if (length > 0 && length < 32768) {
            std::wstring path(module, length);
            size_t slash = path.find_last_of(L"\\/");
            if (slash != std::wstring::npos)
                candidates.push_back(path.substr(0, slash + 1) + L"Assets\\Characters\\" + filename);
        }
        candidates.push_back(L"Assets\\Characters\\" + filename);
        candidates.push_back(L"SimpleGame\\Assets\\Characters\\" + filename);
        for (const auto& path : candidates) {
            DWORD attributes = GetFileAttributesW(path.c_str());
            if (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY)) return path;
        }
        return L"";
    }

    // Only the slate preview backdrop: avoid the mage's blue robe and cyan gloves.
    bool IsBackdrop(const unsigned char* p)
    {
        int r = p[0], g = p[1], b = p[2];
        return p[3] == 0 || (r >= 34 && r <= 51 && g >= 39 && g <= 57 &&
            b >= 47 && b <= 65 && g-r >= 2 && g-r <= 10 && b-g >= 4 && b-g <= 11);
    }
}

bool LoadMercenarySprite(const std::wstring& filename, SpriteImage& image)
{
    image.rgba.clear();
    std::wstring path = FindAsset(filename);
    if (path.empty()) {
        std::wcerr << L"Missing sprite: " << filename << L" (expected Assets/Characters beside executable).\n";
        return false;
    }
    ComScope com;
    if (FAILED(com.result) && com.result != RPC_E_CHANGED_MODE) return false;
    using Microsoft::WRL::ComPtr;
    ComPtr<IWICImagingFactory> factory;
    ComPtr<IWICBitmapDecoder> decoder;
    ComPtr<IWICBitmapFrameDecode> frame;
    ComPtr<IWICFormatConverter> converter;
    HRESULT result = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(factory.GetAddressOf()));
    if (SUCCEEDED(result)) result = factory->CreateDecoderFromFilename(path.c_str(), nullptr,
        GENERIC_READ, WICDecodeMetadataCacheOnLoad, decoder.GetAddressOf());
    if (SUCCEEDED(result)) result = decoder->GetFrame(0, frame.GetAddressOf());
    if (SUCCEEDED(result)) result = factory->CreateFormatConverter(converter.GetAddressOf());
    if (SUCCEEDED(result)) result = converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppRGBA,
        WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom);
    UINT width = 0, height = 0;
    if (SUCCEEDED(result)) result = converter->GetSize(&width, &height);
    // These crop coordinates belong to v2, not to the rejected youthful/v1 images.
    if (FAILED(result) || width != 1024 || height != 1536) {
        std::wcerr << L"Cannot decode expected 1024x1536 v2 sprite: " << path << L"\n";
        return false;
    }
    std::vector<unsigned char> pixels(static_cast<size_t>(width)*height*4);
    result = converter->CopyPixels(nullptr, width*4, static_cast<UINT>(pixels.size()), pixels.data());
    if (FAILED(result)) return false;

    // Common padded crop and logical grid keep the two professions the same size.
    const int left = 288, top = 400, cropWidth = 432, cropHeight = 704;
    image.width = 32; image.height = 52;
    image.rgba.resize(image.width*image.height*4);
    std::vector<bool> backdrop(image.width*image.height, false);
    std::vector<int> flood;
    for (int y = 0; y < image.height; ++y) for (int x = 0; x < image.width; ++x) {
        int sx = left + static_cast<int>((x+.5f)*cropWidth/image.width);
        int sy = top + static_cast<int>((y+.5f)*cropHeight/image.height);
        const unsigned char* src = &pixels[(static_cast<size_t>(sy)*width+sx)*4];
        int index = y*image.width+x;
        for (int channel = 0; channel < 4; ++channel) image.rgba[index*4+channel] = src[channel];
        backdrop[index] = IsBackdrop(src);
        if (backdrop[index] && (x == 0 || y == 0 || x == image.width-1 || y == image.height-1)) {
            image.rgba[index*4+3] = 0;
            flood.push_back(index);
        }
    }
    // Exterior-connected removal retains similarly colored enclosed garment pixels.
    std::vector<bool> visited(image.width*image.height, false);
    for (int index : flood) visited[index] = true;
    for (size_t head = 0; head < flood.size(); ++head) {
        int index = flood[head], x = index%image.width, y = index/image.width;
        const int offsets[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
        for (const auto& offset : offsets) {
            int nx = x+offset[0], ny = y+offset[1];
            if (nx < 0 || ny < 0 || nx >= image.width || ny >= image.height) continue;
            int neighbor = ny*image.width+nx;
            if (!visited[neighbor] && backdrop[neighbor]) {
                visited[neighbor] = true;
                image.rgba[neighbor*4+3] = 0;
                flood.push_back(neighbor);
            }
        }
    }
    return true;
}
