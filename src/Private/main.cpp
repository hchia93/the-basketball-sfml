#pragma once
#include "Application.h"

int main()
{
	std::unique_ptr<Application> Instance = std::make_unique<Application>(); 
	
	if (Instance)
	{
		if(Instance->Initialize())
			Instance->BeginPlay();
	}
	return 0;
}