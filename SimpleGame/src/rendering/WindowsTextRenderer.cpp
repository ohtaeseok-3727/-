#include "stdafx.h"
#include "WindowsTextRenderer.h"
#include <Windows.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <utility>
#include <vector>

namespace
{
    const std::size_t MaximumCachedStrings = 256;

    bool ToWideString(const std::string& utf8Text, std::wstring& wideText)
    {
        wideText.clear();
        if (utf8Text.empty())
        {
            return true;
        }

        const int byteCount = static_cast<int>(utf8Text.size());
        const int characterCount = MultiByteToWideChar(
            CP_UTF8, MB_ERR_INVALID_CHARS, utf8Text.data(), byteCount, nullptr, 0);
        if (characterCount <= 0)
        {
            return false;
        }

        wideText.resize(static_cast<std::size_t>(characterCount));
        return MultiByteToWideChar(CP_UTF8,
                                   MB_ERR_INVALID_CHARS,
                                   utf8Text.data(),
                                   byteCount,
                                   &wideText[0],
                                   characterCount) == characterCount;
    }
}

class WindowsTextRenderer::Implementation
{
  public:
    ~Implementation();

    bool Initialize();
    bool IsInitialized() const;
    float TextWidth(const std::string& utf8Text) const;
    bool Prepare(const std::string& utf8Text, TextSprite& sprite, bool* cacheHit);

  private:
    struct CacheEntry
    {
        TextSprite sprite;
        std::uint64_t lastUse = 0;
    };

    bool Measure(const std::wstring& text, SIZE& size) const;
    bool Rasterize(const std::wstring& text,
                   int width,
                   int height,
                   std::vector<unsigned char>& rgba);
    bool Upload(int width, int height, const unsigned char* rgba, GLuint& texture) const;
    void EvictOldest();
    void Release();

    HDC m_DeviceContext = nullptr;
    HFONT m_Font = nullptr;
    HGDIOBJ m_PreviousFont = nullptr;
    TEXTMETRICW m_Metrics = {};
    std::uint64_t m_UseCounter = 0;
    std::unordered_map<std::string, CacheEntry> m_Cache;
};

WindowsTextRenderer::Implementation::~Implementation()
{
    Release();
}

bool WindowsTextRenderer::Implementation::Initialize()
{
    if (IsInitialized())
    {
        return true;
    }

    m_DeviceContext = CreateCompatibleDC(nullptr);
    if (!m_DeviceContext)
    {
        return false;
    }

    m_Font = CreateFontW(-16,
                         0,
                         0,
                         0,
                         FW_NORMAL,
                         FALSE,
                         FALSE,
                         FALSE,
                         DEFAULT_CHARSET,
                         OUT_TT_PRECIS,
                         CLIP_DEFAULT_PRECIS,
                         ANTIALIASED_QUALITY,
                         DEFAULT_PITCH | FF_DONTCARE,
                         L"Malgun Gothic");
    if (!m_Font)
    {
        Release();
        return false;
    }

    m_PreviousFont = SelectObject(m_DeviceContext, m_Font);
    if (!m_PreviousFont || m_PreviousFont == HGDI_ERROR ||
        !GetTextMetricsW(m_DeviceContext, &m_Metrics))
    {
        Release();
        return false;
    }

    if (SetTextAlign(m_DeviceContext, TA_LEFT | TA_TOP | TA_NOUPDATECP) == GDI_ERROR ||
        SetTextColor(m_DeviceContext, RGB(255, 255, 255)) == CLR_INVALID ||
        SetBkMode(m_DeviceContext, TRANSPARENT) == 0)
    {
        Release();
        return false;
    }

    return true;
}

bool WindowsTextRenderer::Implementation::IsInitialized() const
{
    return m_DeviceContext && m_Font && m_PreviousFont && m_PreviousFont != HGDI_ERROR &&
           m_Metrics.tmHeight > 0;
}

float WindowsTextRenderer::Implementation::TextWidth(const std::string& utf8Text) const
{
    if (!IsInitialized() || utf8Text.empty())
    {
        return 0.f;
    }

    const auto cached = m_Cache.find(utf8Text);
    if (cached != m_Cache.end())
    {
        return static_cast<float>(cached->second.sprite.width);
    }

    std::wstring wideText;
    SIZE size = {};
    if (!ToWideString(utf8Text, wideText) || !Measure(wideText, size))
    {
        return 0.f;
    }

    return static_cast<float>(size.cx);
}

bool WindowsTextRenderer::Implementation::Prepare(const std::string& utf8Text,
                                                  TextSprite& sprite,
                                                  bool* cacheHit)
{
    if (cacheHit)
    {
        *cacheHit = false;
    }
    sprite = {};
    if (!IsInitialized() || utf8Text.empty())
    {
        return false;
    }

    auto cached = m_Cache.find(utf8Text);
    if (cached != m_Cache.end())
    {
        if (cacheHit)
        {
            *cacheHit = true;
        }
        cached->second.lastUse = ++m_UseCounter;
        sprite = cached->second.sprite;
        return true;
    }

    std::wstring wideText;
    SIZE size = {};
    if (!ToWideString(utf8Text, wideText) || !Measure(wideText, size) || size.cx <= 0 ||
        size.cy <= 0)
    {
        return false;
    }

    std::vector<unsigned char> rgba;
    if (!Rasterize(wideText, size.cx, size.cy, rgba))
    {
        return false;
    }

    TextSprite prepared;
    prepared.width = size.cx;
    prepared.height = size.cy;
    prepared.baseline = m_Metrics.tmAscent;
    if (!Upload(prepared.width, prepared.height, rgba.data(), prepared.texture))
    {
        return false;
    }

    if (m_Cache.size() >= MaximumCachedStrings)
    {
        EvictOldest();
    }

    CacheEntry entry;
    entry.sprite = prepared;
    entry.lastUse = ++m_UseCounter;
    m_Cache.emplace(utf8Text, entry);
    sprite = prepared;
    return true;
}

bool WindowsTextRenderer::Implementation::Measure(const std::wstring& text, SIZE& size) const
{
    if (text.empty())
    {
        size = {};
        return true;
    }

    return GetTextExtentPoint32W(
               m_DeviceContext, text.data(), static_cast<int>(text.size()), &size) != FALSE;
}

bool WindowsTextRenderer::Implementation::Rasterize(const std::wstring& text,
                                                    int width,
                                                    int height,
                                                    std::vector<unsigned char>& rgba)
{
    BITMAPINFO bitmapInfo = {};
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = -height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    void* pixels = nullptr;
    HBITMAP bitmap =
        CreateDIBSection(m_DeviceContext, &bitmapInfo, DIB_RGB_COLORS, &pixels, nullptr, 0);
    if (!bitmap || !pixels)
    {
        if (bitmap)
        {
            DeleteObject(bitmap);
        }
        return false;
    }

    HGDIOBJ previousBitmap = SelectObject(m_DeviceContext, bitmap);
    if (!previousBitmap || previousBitmap == HGDI_ERROR)
    {
        DeleteObject(bitmap);
        return false;
    }

    const std::size_t byteCount = static_cast<std::size_t>(width) * height * 4;
    std::memset(pixels, 0, byteCount);
    const bool rendered =
        TextOutW(m_DeviceContext, 0, 0, text.data(), static_cast<int>(text.size())) != FALSE &&
        GdiFlush() != FALSE;
    if (rendered)
    {
        const unsigned char* bgra = static_cast<const unsigned char*>(pixels);
        rgba.resize(byteCount);
        for (std::size_t index = 0; index < byteCount; index += 4)
        {
            const unsigned char coverage =
                (std::max)(bgra[index], (std::max)(bgra[index + 1], bgra[index + 2]));
            rgba[index] = 255;
            rgba[index + 1] = 255;
            rgba[index + 2] = 255;
            rgba[index + 3] = coverage;
        }
    }

    SelectObject(m_DeviceContext, previousBitmap);
    DeleteObject(bitmap);
    return rendered;
}

bool WindowsTextRenderer::Implementation::Upload(int width,
                                                 int height,
                                                 const unsigned char* rgba,
                                                 GLuint& texture) const
{
    glGenTextures(1, &texture);
    if (!texture)
    {
        return false;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);

    GLint uploadedWidth = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &uploadedWidth);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (uploadedWidth == width)
    {
        return true;
    }

    glDeleteTextures(1, &texture);
    texture = 0;
    return false;
}

void WindowsTextRenderer::Implementation::EvictOldest()
{
    if (m_Cache.empty())
    {
        return;
    }

    auto oldest = (std::min_element)(m_Cache.begin(),
                                     m_Cache.end(),
                                     [](const std::pair<const std::string, CacheEntry>& left,
                                        const std::pair<const std::string, CacheEntry>& right)
                                     {
                                         return left.second.lastUse < right.second.lastUse;
                                     });
    glDeleteTextures(1, &oldest->second.sprite.texture);
    m_Cache.erase(oldest);
}

void WindowsTextRenderer::Implementation::Release()
{
    for (auto& item : m_Cache)
    {
        glDeleteTextures(1, &item.second.sprite.texture);
    }
    m_Cache.clear();

    if (m_DeviceContext && m_PreviousFont && m_PreviousFont != HGDI_ERROR)
    {
        SelectObject(m_DeviceContext, m_PreviousFont);
    }
    m_PreviousFont = nullptr;

    if (m_Font)
    {
        DeleteObject(m_Font);
        m_Font = nullptr;
    }
    if (m_DeviceContext)
    {
        DeleteDC(m_DeviceContext);
        m_DeviceContext = nullptr;
    }
    m_Metrics = {};
}

WindowsTextRenderer::WindowsTextRenderer() : m_Impl(new Implementation())
{
}

WindowsTextRenderer::~WindowsTextRenderer() = default;

bool WindowsTextRenderer::Initialize()
{
    return m_Impl->Initialize();
}

bool WindowsTextRenderer::IsInitialized() const
{
    return m_Impl->IsInitialized();
}

float WindowsTextRenderer::TextWidth(const std::string& utf8Text) const
{
    return m_Impl->TextWidth(utf8Text);
}

bool WindowsTextRenderer::Prepare(const std::string& utf8Text, TextSprite& sprite, bool* cached)
{
    return m_Impl->Prepare(utf8Text, sprite, cached);
}
