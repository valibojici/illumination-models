#pragma once
#include "Event.h"
/// <summary>
/// Base class (interface) for all event handlers.
/// The EventManager class will notify its listeners when it receives an event.
/// </summary>
class EventHandler
{
public:
	virtual ~EventHandler() {};
	virtual void handleEvent(const Event& e) = 0;
};

