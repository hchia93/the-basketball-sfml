#pragma once
#include "Application.h"

int main()
{
	std::unique_ptr<Application> instance = std::make_unique<Application>(); 
	
	if (instance)
	{
		if (instance->Initialize())
		{
			instance->BeginPlay();
		}
	}
	return 0;
}