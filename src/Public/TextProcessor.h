#pragma once
#include <SFML/Graphics.hpp>
#include "Defines.h"
#include "Interface.h"

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

	// States
	bool m_bIsActive = true;
	bool m_bIsPaused = false;
	bool m_bIsPersistance = true;
};

class FTextWidgetProcessor : public ITickable
{
public:
	virtual void Tick() override;

	FTextWidget* AddAndInitialize(std::unique_ptr<FTextWidget>& data);
	std::vector<std::unique_ptr<FTextWidget>>& GetTextWidgets();

private:

	std::vector<std::unique_ptr<FTextWidget>> m_TextWidgets;
};

