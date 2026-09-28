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

        ~ComScope()
        {
            if (SUCCEEDED(result))
                CoUninitialize();
        }
    };

    std::wstring FindAsset(const std::wstring& filename)
    {
        wchar_t module[32768] = {};
        DWORD length = GetModuleFileNameW(nullptr, module, 32768);
        std::vector<std::wstring> candidates;
        if (length > 0 && length < 32768)
        {
            std::wstring path(module, length);
            size_t slash = path.find_last_of(L"\\/");
            if (slash != std::wstring::npos)
                candidates.push_back(path.substr(0, slash + 1) + L"Assets\\Characters\\" +
                                     filename);
        }
        candidates.push_back(L"Assets\\Characters\\" + filename);
        candidates.push_back(L"SimpleGame\\Assets\\Characters\\" + filename);
        for (const auto& path : candidates)
        {
            DWORD attributes = GetFileAttributesW(path.c_str());
            if (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY))
                return path;
        }
        return L"";
    }

    // Only the slate preview backdrop: avoid the mage's blue robe and cyan gloves.
    bool IsBackdrop(const unsigned char* p)
    {
        int r = p[0], g = p[1], b = p[2];
        return p[3] == 0 || (r >= 34 && r <= 51 && g >= 39 && g <= 57 && b >= 47 && b <= 65 &&
                             g - r >= 2 && g - r <= 10 && b - g >= 4 && b - g <= 11);
    }
}

namespace
{
    bool DecodeSprite(const std::wstring& filename,
                      UINT& width,
                      UINT& height,
                      std::vector<unsigned char>& pixels)
    {
        pixels.clear();
        std::wstring path = FindAsset(filename);
        if (path.empty())
        {
            std::wcerr << L"Missing sprite: " << filename
                       << L" (expected Assets/Characters beside executable).\n";
            return false;
        }
        ComScope com;
        if (FAILED(com.result) && com.result != RPC_E_CHANGED_MODE)
            return false;
        using Microsoft::WRL::ComPtr;
        ComPtr<IWICImagingFactory> factory;
        ComPtr<IWICBitmapDecoder> decoder;
        ComPtr<IWICBitmapFrameDecode> frame;
        ComPtr<IWICFormatConverter> converter;
        HRESULT result = CoCreateInstance(CLSID_WICImagingFactory,
                                          nullptr,
                                          CLSCTX_INPROC_SERVER,
                                          IID_PPV_ARGS(factory.GetAddressOf()));
        if (SUCCEEDED(result))
            result = factory->CreateDecoderFromFilename(path.c_str(),
                                                        nullptr,
                                                        GENERIC_READ,
                                                        WICDecodeMetadataCacheOnLoad,
                                                        decoder.GetAddressOf());
        if (SUCCEEDED(result))
            result = decoder->GetFrame(0, frame.GetAddressOf());
        if (SUCCEEDED(result))
            result = factory->CreateFormatConverter(converter.GetAddressOf());
        if (SUCCEEDED(result))
            result = converter->Initialize(frame.Get(),
                                           GUID_WICPixelFormat32bppRGBA,
                                           WICBitmapDitherTypeNone,
                                           nullptr,
                                           0,
                                           WICBitmapPaletteTypeCustom);
        width = 0;
        height = 0;
        if (SUCCEEDED(result))
            result = converter->GetSize(&width, &height);
        if (FAILED(result) || width == 0 || height == 0 || width > 4096 || height > 4096)
        {
            std::wcerr << L"Cannot decode sprite image: " << path << L"\n";
            return false;
        }
        pixels.resize(static_cast<size_t>(width) * height * 4);
        result = converter->CopyPixels(
            nullptr, width * 4, static_cast<UINT>(pixels.size()), pixels.data());
        if (FAILED(result))
            return false;

        return true;
    }
}

bool LoadRawSpriteSheet(const std::wstring& filename, SpriteImage& image)
{
    UINT width = 0, height = 0;
    image.rgba.clear();
    image.width = 0;
    image.height = 0;
    if (!DecodeSprite(filename, width, height, image.rgba))
        return false;
    image.width = static_cast<int>(width);
    image.height = static_cast<int>(height);
    return true;
}

bool LoadMercenarySprite(const std::wstring& filename, SpriteImage& image)
{
    image.rgba.clear();
    UINT width = 0, height = 0;
    std::vector<unsigned char> pixels;
    if (!DecodeSprite(filename, width, height, pixels))
        return false;
    if (width != 1024 || height != 1536)
    {
        std::wcerr << L"Unexpected idle sprite dimensions: " << filename << L"\n";
        return false;
    }

    // Common padded crop and logical grid keep the two professions the same size.
    const int left = 288, top = 400, cropWidth = 432, cropHeight = 704;
    image.width = 32;
    image.height = 52;
    image.rgba.resize(image.width * image.height * 4);
    std::vector<bool> backdrop(image.width * image.height, false);
    std::vector<int> flood;
    for (int y = 0; y < image.height; ++y)
        for (int x = 0; x < image.width; ++x)
        {
            int sx = left + static_cast<int>((x + .5f) * cropWidth / image.width);
            int sy = top + static_cast<int>((y + .5f) * cropHeight / image.height);
            const unsigned char* src = &pixels[(static_cast<size_t>(sy) * width + sx) * 4];
            int index = y * image.width + x;
            for (int channel = 0; channel < 4; ++channel)
                image.rgba[index * 4 + channel] = src[channel];
            backdrop[index] = IsBackdrop(src);
            if (backdrop[index] &&
                (x == 0 || y == 0 || x == image.width - 1 || y == image.height - 1))
            {
                image.rgba[index * 4 + 3] = 0;
                flood.push_back(index);
            }
        }
    // Exterior-connected removal retains similarly colored enclosed garment pixels.
    std::vector<bool> visited(image.width * image.height, false);
    for (int index : flood)
        visited[index] = true;
    for (size_t head = 0; head < flood.size(); ++head)
    {
        int index = flood[head], x = index % image.width, y = index / image.width;
        const int offsets[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        for (const auto& offset : offsets)
        {
            int nx = x + offset[0], ny = y + offset[1];
            if (nx < 0 || ny < 0 || nx >= image.width || ny >= image.height)
                continue;
            int neighbor = ny * image.width + nx;
            if (!visited[neighbor] && backdrop[neighbor])
            {
                visited[neighbor] = true;
                image.rgba[neighbor * 4 + 3] = 0;
                flood.push_back(neighbor);
            }
        }
    }
    return true;
}

bool LoadMercenaryWalkSheet(const std::wstring& filename, SpriteImage& atlas)
{
    atlas.rgba.clear();
    UINT width = 0, height = 0;
    std::vector<unsigned char> pixels;
    if (!DecodeSprite(filename, width, height, pixels))
        return false;
    if (width != 1254 || height != 1254)
    {
        std::wcerr << L"Unexpected walk sheet dimensions: " << filename << L"\n";
        return false;
    }

    struct Cell
    {
        int x, y, width, height, left, top, right, bottom;
        std::vector<unsigned char> background;
    };

    Cell cells[16];
    int maxWidth = 1, maxHeight = 1;
    for (int row = 0; row < 4; ++row)
        for (int column = 0; column < 4; ++column)
        {
            Cell& cell = cells[row * 4 + column];
            // Integer endpoints handle the source's 313.5px nominal grid without drift.
            cell.x = column * static_cast<int>(width) / 4;
            cell.y = row * static_cast<int>(height) / 4;
            cell.width = (column + 1) * static_cast<int>(width) / 4 - cell.x;
            cell.height = (row + 1) * static_cast<int>(height) / 4 - cell.y;
            cell.background.assign(cell.width * cell.height, 0);
            std::vector<int> queue;
            auto enqueue = [&](int x, int y)
            {
                int index = y * cell.width + x;
                if (cell.background[index])
                    return;
                const unsigned char* p = &pixels[((cell.y + y) * width + cell.x + x) * 4];
                int low = p[0], high = p[0];
                for (int channel = 1; channel < 3; ++channel)
                {
                    if (p[channel] < low)
                        low = p[channel];
                    if (p[channel] > high)
                        high = p[channel];
                }
                // Pale neutral checkerboard only; dark outlines retain enclosed silver highlights.
                if (p[3] == 0 || (low >= 178 && high - low <= 22))
                {
                    cell.background[index] = 1;
                    queue.push_back(index);
                }
            };
            for (int x = 0; x < cell.width; ++x)
            {
                enqueue(x, 0);
                enqueue(x, cell.height - 1);
            }
            for (int y = 0; y < cell.height; ++y)
            {
                enqueue(0, y);
                enqueue(cell.width - 1, y);
            }
            for (size_t head = 0; head < queue.size(); ++head)
            {
                int x = queue[head] % cell.width, y = queue[head] / cell.width;
                if (x > 0)
                    enqueue(x - 1, y);
                if (x + 1 < cell.width)
                    enqueue(x + 1, y);
                if (y > 0)
                    enqueue(x, y - 1);
                if (y + 1 < cell.height)
                    enqueue(x, y + 1);
            }
            cell.left = cell.width;
            cell.top = cell.height;
            cell.right = cell.bottom = -1;
            for (int y = 0; y < cell.height; ++y)
                for (int x = 0; x < cell.width; ++x)
                {
                    if (cell.background[y * cell.width + x])
                        continue;
                    if (x < cell.left)
                        cell.left = x;
                    if (x > cell.right)
                        cell.right = x;
                    if (y < cell.top)
                        cell.top = y;
                    if (y > cell.bottom)
                        cell.bottom = y;
                }
            if (cell.right < cell.left || cell.bottom < cell.top)
            {
                std::wcerr << L"Empty walk frame in " << filename << L"\n";
                return false;
            }
            int w = cell.right - cell.left + 1, h = cell.bottom - cell.top + 1;
            if (w > maxWidth)
                maxWidth = w;
            if (h > maxHeight)
                maxHeight = h;
        }
    // One scale across the whole sheet, never stretch each animation frame independently.
    float scaleX = 30.f / maxWidth, scaleY = 48.f / maxHeight;
    float scale = scaleX < scaleY ? scaleX : scaleY;
    atlas.width = 128;
    atlas.height = 208;
    atlas.rgba.assign(atlas.width * atlas.height * 4, 0);
    for (int i = 0; i < 16; ++i)
    {
        const Cell& cell = cells[i];
        // Shared row center avoids centering on a swinging sword on every frame.
        int rowLeft = cell.width, rowRight = 0;
        for (int j = (i / 4) * 4; j < (i / 4) * 4 + 4; ++j)
        {
            if (cells[j].left < rowLeft)
                rowLeft = cells[j].left;
            if (cells[j].right > rowRight)
                rowRight = cells[j].right;
        }
        float center = (rowLeft + rowRight + 1) * .5f;
        for (int y = 0; y < 52; ++y)
            for (int x = 0; x < 32; ++x)
            {
                int sx = static_cast<int>(std::floor(center + (x + .5f - 16.f) / scale));
                int sy = static_cast<int>(std::floor(cell.bottom + 1 + (y + .5f - 50.f) / scale));
                if (sx < 0 || sy < 0 || sx >= cell.width || sy >= cell.height)
                    continue;
                if (cell.background[sy * cell.width + sx])
                    continue;
                const unsigned char* src = &pixels[((cell.y + sy) * width + cell.x + sx) * 4];
                unsigned char* dst =
                    &atlas.rgba[(((i / 4) * 52 + y) * atlas.width + (i % 4) * 32 + x) * 4];
                for (int channel = 0; channel < 4; ++channel)
                    dst[channel] = src[channel];
            }
    }
    return true;
}
