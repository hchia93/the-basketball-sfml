#include "AssetLoader.h"

FAssetLoader::~FAssetLoader()
{
	for (auto&& element : m_MusicMap)
	{
		if (element.second.get())
		{
			element.second.get()->stop();
		}
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

	auto iterator = contextObject->m_TextureMap.find(name);
	if (iterator != contextObject->m_TextureMap.end())
	{
		return iterator->second.get();
	}
	return nullptr;
}

sf::Font* FAssetLoader::FindFont(FAssetLoader* contextObject, const std::string name)
{
	if (!contextObject)
	{
		return nullptr;
	}

	auto iterator = contextObject->m_FontMap.find(name);
	if (iterator != contextObject->m_FontMap.end())
	{
		return iterator->second.get();
	}
	return nullptr;
}

sf::Music* FAssetLoader::FindMusic(FAssetLoader* contextObject, const std::string name)
{
	if (!contextObject)
	{
		return nullptr;
	}

	auto iterator = contextObject->m_MusicMap.find(name);
	if (iterator != contextObject->m_MusicMap.end())
	{
		return iterator->second.get();
	}
	return nullptr;
}

bool FAssetLoader::LoadTexture(const std::string filename)
{
	auto pNewTexture = std::make_unique<sf::Texture>();
	bool bResult = pNewTexture->loadFromFile(filename);
	LOAD_CHECK(bResult, filename);

	if (bResult)
	{
		m_TextureMap.insert(std::make_pair(filename, std::move(pNewTexture)));
	}
	return bResult;
}

bool FAssetLoader::LoadFont(const std::string filename)
{
	auto pNewFont = std::make_unique<sf::Font>();
	bool bResult = pNewFont->loadFromFile(filename);
	LOAD_CHECK(bResult, filename);

	if (bResult)
	{
		m_FontMap.insert(std::make_pair(filename, std::move(pNewFont)));
	}
	return bResult;
}

bool FAssetLoader::LoadMusic(const std::string filename)
{
	auto pNewMusic = std::make_unique<sf::Music>();
	bool bResult = pNewMusic->openFromFile(filename);
	LOAD_CHECK(bResult, filename);

	if (bResult)
	{
		m_MusicMap.insert(std::make_pair(filename, std::move(pNewMusic)));
	}
	return bResult;
}
