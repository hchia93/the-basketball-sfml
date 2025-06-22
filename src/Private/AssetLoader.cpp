#include "AssetLoader.h"

FAssetLoader::~FAssetLoader() 
{
	for (auto&& element : m_MusicMap)
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

SFML::Texture* FAssetLoader::FindTexture(FAssetLoader* ContextObject, const std::string Name)
{
	if (!ContextObject) return nullptr;

	auto Iterator = ContextObject->m_TextureMap.find(Name);
	return (Iterator != ContextObject->m_TextureMap.end()) ? Iterator->second.get() : nullptr ;
}

SFML::Font* FAssetLoader::FindFont(FAssetLoader* ContextObject, const std::string Name)
{
	if (!ContextObject) return nullptr;

	auto Iterator = ContextObject->m_FontMap.find(Name);
	return (Iterator != ContextObject->m_FontMap.end()) ? Iterator->second.get() : nullptr;
}

SFML::Music* FAssetLoader::FindMusic(FAssetLoader* ContextObject, const std::string Name)
{
	if (!ContextObject) return nullptr;

	auto Iterator = ContextObject->m_MusicMap.find(Name);
	return (Iterator != ContextObject->m_MusicMap.end()) ? Iterator->second.get() : nullptr;
}

bool FAssetLoader::LoadTexture(const std::string FileName)
{
	std::unique_ptr<SFML::Texture> pNewTexture  = std::make_unique<SFML::Texture>();
	bool bResult = pNewTexture->loadFromFile(FileName);
	LOAD_CHECK(bResult, FileName);

	if (bResult)
	{
		m_TextureMap.insert(std::pair<const std::string, std::unique_ptr<SFML::Texture>>(FileName, std::move(pNewTexture)));
	}
	return bResult;
}

bool FAssetLoader::LoadFont(const std::string FileName)
{
	std::unique_ptr<SFML::Font> pNewFont = std::make_unique<SFML::Font>();
	bool bResult = pNewFont->loadFromFile(FileName);
	LOAD_CHECK(bResult, FileName);
	
	if (bResult)
	{
		m_FontMap.insert(std::pair<const std::string, std::unique_ptr<SFML::Font>>(FileName, std::move(pNewFont)));
	}
	return bResult;
}

bool FAssetLoader::LoadMusic(const std::string FileName)
{
	std::unique_ptr<SFML::Music> pNewMusic = std::make_unique<SFML::Music>();
	bool bResult = pNewMusic->openFromFile(FileName);
	LOAD_CHECK(bResult, FileName);

	if (bResult)
	{
		m_MusicMap.insert(std::pair<const std::string, std::unique_ptr<SFML::Music>>(FileName, std::move(pNewMusic)));
	}
	return bResult;
}
