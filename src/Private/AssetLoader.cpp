#include "AssetLoader.h"
#include "Defines.h"
#include <iostream>

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
	bool result = true;
	result &= LoadFont(RESOURCES_FONT_CHALK);
	result &= LoadFont(RESOURCES_FONT_PIXEL);
	result &= LoadTexture(RESOURCES_TEXTURE_CHALKBOARD);
	result &= LoadTexture(RESOURCES_TEXTURE_BASKETBALL);
	result &= LoadTexture(RESOURCES_TEXTURE_BASKETBALLBOARD);
	result &= LoadTexture(RESOURCES_TEXTURE_BACKGROUND);
	result &= LoadTexture(RESOURCES_TEXTURE_WOOD);
	result &= LoadTexture(RESOURCES_TEXTURE_BOXALT);
	result &= LoadMusic(RESOURCES_AUDIO_TROLOLO);
	return result;
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
	bool result = pNewTexture->loadFromFile(filename);
	LOAD_CHECK(result, filename);

	if (result)
	{
		m_TextureMap.insert(std::pair<const std::string, std::unique_ptr<sf::Texture>>(filename, std::move(pNewTexture)));
	}
	return result;
}

bool FAssetLoader::LoadFont(const std::string& filename)
{
	auto pNewFont = std::make_unique<sf::Font>();
	bool result = pNewFont->openFromFile(filename);
	LOAD_CHECK(result, filename);

	if (result)
	{
		m_FrontMap.insert(std::pair<const std::string, std::unique_ptr<sf::Font>>(filename, std::move(pNewFont)));
	}
	return result;
}

bool FAssetLoader::LoadMusic(const std::string& filename)
{
	std::unique_ptr<sf::Music> pNewMusic = std::make_unique<sf::Music>();
	bool result = pNewMusic->openFromFile(filename);
	LOAD_CHECK(result, filename);

	if (result)
	{
		m_MusicMap.insert(std::pair<const std::string, std::unique_ptr<sf::Music>>(filename, std::move(pNewMusic)));
	}
	return result;
}
