#include "AssetLoader.h"
#include "Defines.h"
#include <iostream>
#include <fstream>
#include <vector>

FAssetLoader::~FAssetLoader() 
{
	for (auto&& element : m_TextureMap)
	{
		element.second.reset();
	}

	for (auto&& element : m_FrontMap)
	{
		element.second.reset();
	}

	for (auto&& element : m_MusicMap)
	{
		element.second.reset();
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

sf::Texture* FAssetLoader::FindTexture(const std::string fileName)
{
	auto it = m_TextureMap.find(fileName);
	return (it != m_TextureMap.end()) ? it->second.get() : nullptr ;
}

sf::Font* FAssetLoader::FindFont(const std::string fileName)
{
	auto it = m_FrontMap.find(fileName);
	return (it != m_FrontMap.end()) ? it->second.get() : nullptr;
}

sf::Music* FAssetLoader::FindMusic(const std::string fileName)
{
	auto it = m_MusicMap.find(fileName);
	return (it != m_MusicMap.end()) ? it->second.get() : nullptr;
}

bool FAssetLoader::LoadTexture(const std::string& filename)
{
	std::unique_ptr<sf::Texture> pNewTexture  = std::make_unique<sf::Texture>();
	bool bResult = pNewTexture->loadFromFile(filename);
	LOAD_CHECK(bResult, filename);

	if (bResult)
	{
		m_TextureMap.insert(std::pair<const std::string, std::unique_ptr<sf::Texture>>(filename, std::move(pNewTexture)));
	}
	return bResult;
}

bool FAssetLoader::LoadFont(const std::string& filename)
{
	auto pNewFont = std::make_unique<sf::Font>();
	bool bResult = pNewFont->openFromFile(filename);
	LOAD_CHECK(bResult, filename);

	if (bResult)
	{
		m_FrontMap.insert(std::pair<const std::string, std::unique_ptr<sf::Font>>(filename, std::move(pNewFont)));
	}
	return bResult;
}

bool FAssetLoader::LoadMusic(const std::string& filename)
{
	std::unique_ptr<sf::Music> pNewMusic = std::make_unique<sf::Music>();
	bool bResult = pNewMusic->openFromFile(filename);
	LOAD_CHECK(bResult, filename);

	if (bResult)
	{
		m_MusicMap.insert(std::pair<const std::string, std::unique_ptr<sf::Music>>(filename, std::move(pNewMusic)));
	}
	return bResult;
}
