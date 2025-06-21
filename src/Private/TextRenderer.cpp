#include "TextRenderer.h"
#include "Application.h"

bool FTextRenderer::BindApplication(Application* Object)
{
	ContextObject = Object;
	return ContextObject != nullptr;
}

void FTextRenderer::Tick()
{
	for (auto& i : Texts)
	{
		// Lerps
		if (i->FadeTime > 0.0f && i->bIsActive && !i->bIsPaused)
		{
			const float LerpDelta = i->LifeTime / i->FadeTime;
			const sf::Vector2f PositionDiff = i->EndLocation - i->StartLocation;
			const sf::Vector2f ResultPosition = i->StartLocation + (PositionDiff * LerpDelta);
			i->Text.setPosition(ResultPosition);
	
			sf::Color DeltaFillColor = i->Text.getFillColor();
			DeltaFillColor.a = (sf::Uint8)((1.f - LerpDelta) * 255);
			i->Text.setFillColor(DeltaFillColor);

			sf::Color DeltaOutlineColor = i->Text.getOutlineColor();
			DeltaOutlineColor.a = (sf::Uint8)((1.f - LerpDelta) * 255);
			i->Text.setOutlineColor(DeltaOutlineColor);

			i->LifeTime += DELTA_TIME_STEP;

			if (i->LifeTime >= i->FadeTime)
			{
				i->bIsActive = false;
			}
		}
	}
}
