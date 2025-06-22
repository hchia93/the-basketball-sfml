#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include "Defines.h"
#include "Interface.h"
#include "TextWidget.h"

class Application;

class FTextWidgetProcessor : public ITickable
{
public:
	virtual void Tick() override;
	void ForceUpdateAllWidgets();

	FTextWidget* AddAndInitialize(std::unique_ptr<FTextWidget>& data);
	std::vector<std::unique_ptr<FTextWidget>>& GetTextWidgets();

private:

	std::vector<std::unique_ptr<FTextWidget>> m_TextWidgets;
}; 