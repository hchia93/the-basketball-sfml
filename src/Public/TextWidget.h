#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include "Defines.h"

class Application;

class FTextWidget
{
public:
	FTextWidget(std::shared_ptr<sf::Font> font, unsigned int characterSize, const sf::Vector2f& startLocation)
		: m_Font(std::move(font))
		, m_CharacterSize(characterSize)
		, m_StartLocation(startLocation)
	{
	}

	void Init();

	// Assets
	std::unique_ptr<sf::Text> m_Text;
	std::shared_ptr<sf::Font> m_Font;
	unsigned int m_CharacterSize = 30;

	// Layout
	sf::Vector2f m_StartLocation;
	sf::Vector2f m_EndLocation;

	// Lifetime
	float m_FadeTime = 0.0f;
	float m_LifeTime;

	// Data
	std::string m_RawString = "NO TEXT";
	
	// Callback system
	std::function<std::string()> m_UpdateCallback;
	std::string m_Prefix = "";
	std::string m_Suffix = "";

	// States
	bool m_bIsActive = true;
	bool m_bIsPaused = false;
	bool m_bIsPersistance = true;
	
	// Update method
	void UpdateText();
}; 