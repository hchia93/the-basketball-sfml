#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "Defines.h"
#include "Interface.h"

class Application;

class FTextData
{
public: 
	float m_FadeTime = 0.0f;
	float m_LifeTime;
	sf::Vector2f m_StartLocation;
	sf::Vector2f m_EndLocation;
	sf::Text m_Text;
	sf::Font* m_Font;
	std::string m_TextData = "NO TEXT";
	bool m_bIsActive = true;
	bool m_bIsPaused = false;
	bool m_bIsPersistance = true;

	void Init()
	{
		m_Text.setString(m_TextData);
		m_Text.setPosition(m_StartLocation);
		m_Text.setFont(*m_Font);
		m_LifeTime = 0.0f;

		// Restore alpha
		sf::Color DefaultFillColor = m_Text.getFillColor();
		DefaultFillColor.a = 255;
		m_Text.setFillColor(DefaultFillColor);

		sf::Color DefaultOutlineColor = m_Text.getOutlineColor();
		DefaultOutlineColor.a = 255;
		m_Text.setOutlineColor(DefaultOutlineColor);
	}
};

class FTextRenderer : public ITickable
{
public:
	void Add(std::unique_ptr<FTextData>& Data) { m_Texts.push_back(std::move(Data)); }
	bool BindApplication(Application* Object);
	virtual void Tick() override;

	std::vector<std::unique_ptr<FTextData>>& GetTextData() { return m_Texts; }

private:
	std::vector<std::unique_ptr<FTextData>> m_Texts;
	Application* m_ContextObject;
};

