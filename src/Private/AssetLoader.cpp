#include "AssetLoader.h"

FAssetLoader::~FAssetLoader()
{
    for (auto&& element : m_MusicLibrary)
    {
        if (!element.second.get())
        {
            continue;
        }
        element.second.get()->stop();
    }
}

bool FAssetLoader::LoadResources()
{
    bool bResult = true;
    bResult &= LoadFont(RESOURCES_FONT_CHALK);
    bResult &= LoadFont(RESOURCES_FONT_PIXEL);
    bResult &= LoadTexture(RESOURCES_TEXTURE_CHALKBOARD);
    bResult &= LoadTexture(RESOURCES_TEXTURE_BASKETBALL);
    bResult &= LoadTexture(RESOURCES_TEXTURE_BASKETBALLBOARD);
    bResult &= LoadTexture(RESOURCES_TEXTURE_BACKGROUND);
    bResult &= LoadTexture(RESOURCES_TEXTURE_WOOD);
    bResult &= LoadTexture(RESOURCES_TEXTURE_BOXALT);
    bResult &= LoadMusic(RESOURCES_AUDIO_TROLOLO);
    return bResult;
}

sf::Texture* FAssetLoader::FindTexture(FAssetLoader* contextObject, const std::string name)
{
    if (!contextObject)
    {
        return nullptr;
    }

    auto it = contextObject->m_TextureLibrary.find(name);
    return (it != contextObject->m_TextureLibrary.end()) ? it->second.get() : nullptr;
}

sf::Font* FAssetLoader::FindFont(FAssetLoader* ContextObject, const std::string Name)
{
    if (!ContextObject)
    {
        return nullptr;
    }

    auto it = ContextObject->m_FontLibrary.find(Name);
    return (it != ContextObject->m_FontLibrary.end()) ? it->second.get() : nullptr;
}

sf::Music* FAssetLoader::FindMusic(FAssetLoader* ContextObject, const std::string Name)
{
    if (!ContextObject)
    {
        return nullptr;
    }

    auto it = ContextObject->m_MusicLibrary.find(Name);
    return (it != ContextObject->m_MusicLibrary.end()) ? it->second.get() : nullptr;
}

bool FAssetLoader::LoadTexture(const std::string FileName)
{
    std::unique_ptr<sf::Texture> pNewTexture = std::make_unique<sf::Texture>();
    bool bResult = pNewTexture->loadFromFile(FileName);
    LOAD_CHECK(bResult, FileName);

    if (bResult)
    {
        m_TextureLibrary.insert(std::pair<const std::string, std::unique_ptr<sf::Texture>>(FileName, std::move(pNewTexture)));
    }
    return bResult;
}

bool FAssetLoader::LoadFont(const std::string FileName)
{
    std::unique_ptr<sf::Font> pNewFont = std::make_unique<sf::Font>();
    bool bResult = pNewFont->loadFromFile(FileName);
    LOAD_CHECK(bResult, FileName);

    if (bResult)
    {
        m_FontLibrary.insert(std::pair<const std::string, std::unique_ptr<sf::Font>>(FileName, std::move(pNewFont)));
    }
    return bResult;
}

bool FAssetLoader::LoadMusic(const std::string FileName)
{
    std::unique_ptr<sf::Music> pNewMusic = std::make_unique<sf::Music>();
    bool bResult = pNewMusic->openFromFile(FileName);
    LOAD_CHECK(bResult, FileName);

    if (bResult)
    {
        m_MusicLibrary.insert(std::pair<const std::string, std::unique_ptr<sf::Music>>(FileName, std::move(pNewMusic)));
    }
    return bResult;
}
