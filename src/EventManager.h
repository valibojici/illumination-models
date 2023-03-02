#pragma once
#include <vector>
#include "EventHandler.h"
#include "GLFW/glfw3.h"
/// <summary>
/// Singleton class for registering and dispatching events.
/// </summary>
class EventManager
{
public:
	static EventManager& getInstance();

	/// <summary>
	/// Register GLFW callback to use private methods which create an event.
	/// </summary>
	void registerCallbacks(GLFWwindow* window);

	/// <summary>
	/// Register a listener of a type that derives from EventHandler and implements the handleEvent method.
	/// </summary>
	void addHandler(EventHandler* handler);

	/// <summary>
	/// Remove a listener of a type that derives from EventHandler and implements the handleEvent method.
	/// </summary>
	void removeHandler(EventHandler* handler);
private:
	std::vector<EventHandler*> m_handlers;
	
	/// <summary>
	/// Dispatch event to all listeners
	/// </summary>
	void sendEvent(const Event& e);
	
	static void handleResize(GLFWwindow* window, int width, int height);
	
	// make constructor private for singleton
	EventManager() = default;
	// delete copy constructor and assignment operator
	EventManager(const EventManager& o) = delete;
	EventManager& operator=(const EventManager& o) = delete;
};

