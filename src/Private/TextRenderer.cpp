#include "TextRenderer.h"
#include "Application.h"

bool FTextRenderer::BindApplication(Application* object)
{
	m_ContextObject = object;
	return m_ContextObject != nullptr;
}

void FTextRenderer::Tick()
{
	for (auto& i : m_Texts)
	{
		// Lerps
		if (i->m_FadeTime > 0.0f && i->m_bIsActive && !i->m_bIsPaused)
		{
			const float lerpDelta = i->m_LifeTime / i->m_FadeTime;
			const sf::Vector2f positionDiff = i->m_EndLocation - i->m_StartLocation;
			const sf::Vector2f resultPosition = i->m_StartLocation + (positionDiff * lerpDelta);
			i->m_Text.setPosition(resultPosition);
	
			sf::Color deltaFillColor = i->m_Text.getFillColor();
			deltaFillColor.a = (sf::Uint8)((1.f - lerpDelta) * 255);
			i->m_Text.setFillColor(deltaFillColor);

			sf::Color deltaOutlineColor = i->m_Text.getOutlineColor();
			deltaOutlineColor.a = (sf::Uint8)((1.f - lerpDelta) * 255);
			i->m_Text.setOutlineColor(deltaOutlineColor);

			i->m_LifeTime += DELTA_TIME_STEP;

			if (i->m_LifeTime >= i->m_FadeTime)
			{
				i->m_bIsActive = false;
			}
		}
	}
}
