#pragma once
#include <SFML/Graphics.hpp>
#include "Defines.h"
#include "Interface.h"

class Application;

class FTextData
{
public: 
	// FTextData() : Font(nullptr), Text() {} // This is the cause of the error.

	float FadeTime					= 0.0f;
	float LifeTime;
	sf::Vector2f StartLocation;
	sf::Vector2f EndLocation;
	std::unique_ptr<sf::Text> Text;
	sf::Font* Font					= nullptr;
	std::string TextData			= "NO TEXT";
	unsigned int CharacterSize		= 30;
	bool bIsActive					= true;
	bool bIsPaused					= false;
	bool bIsPersistance				= true;

	void Init()
	{
		Text = std::make_unique<sf::Text>(*Font, TextData, CharacterSize);
		Text->setPosition(StartLocation);
		LifeTime = 0.0f;

		// Restore alpha
		sf::Color DefaultFillColor = Text->getFillColor();
		DefaultFillColor.a = 255;
		Text->setFillColor(DefaultFillColor);

		sf::Color DefaultOutlineColor = Text->getOutlineColor();
		DefaultOutlineColor.a = 255;
		Text->setOutlineColor(DefaultOutlineColor);
	}
};

class FTextRenderer : public ITickable
{
public:
	
	void Add(std::unique_ptr<FTextData>& Data) { Texts.push_back(std::move(Data)); }
	bool BindApplication(Application* Object);
	virtual void Tick() override;

	std::vector<std::unique_ptr<FTextData>>& GetTextData()	 {return Texts;}

private:

	std::vector<std::unique_ptr<FTextData>> Texts;
	Application* ContextObject;
};

