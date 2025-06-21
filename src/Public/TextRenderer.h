#pragma once
#include <SFML/Graphics.hpp>
#include "Defines.h"
#include "Interface.h"

class Application;

class FTextData
{
public: 
	float FadeTime					= 0.0f;
	float LifeTime;
	SFML::Vector2f StartLocation;
	SFML::Vector2f EndLocation;
	SFML::Text	Text;
	SFML::Font* Font;
	std::string TextData			= "NO TEXT";
	bool bIsActive					= true;
	bool bIsPaused					= false;
	bool bIsPersistance				= true;

	void Init()
	{
		Text.setString(TextData);
		Text.setPosition(StartLocation);
		Text.setFont(*Font);
		LifeTime = 0.0f;

		// Restore alpha
		SFML::Color DefaultFillColor = Text.getFillColor();
		DefaultFillColor.a = 255;
		Text.setFillColor(DefaultFillColor);

		SFML::Color DefaultOutlineColor = Text.getOutlineColor();
		DefaultOutlineColor.a = 255;
		Text.setOutlineColor(DefaultOutlineColor);
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

