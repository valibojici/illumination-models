#include "EventManager.h"

EventManager& EventManager::getInstance()
{
	static EventManager eventManager;
	return eventManager;
}

void EventManager::registerCallbacks(GLFWwindow* window)
{
	printf("Registering callbacks...\n");
	// TODO: add other callbacks here
	glfwSetWindowSizeCallback(window, EventManager::handleResize);
}

void EventManager::addHandler(EventHandler* handler)
{
	m_handlers.push_back(handler);
}

void EventManager::sendEvent(const Event& e)
{
	for (auto& handler : m_handlers) {
		handler->handleEvent(e);
	}
}

void EventManager::handleResize(GLFWwindow* window, int width, int height)
{
	Event e;
	e.m_type = Event::Type::WINDOW_RESIZE;
	e.window = { width, height };
	getInstance().sendEvent(e);
}
