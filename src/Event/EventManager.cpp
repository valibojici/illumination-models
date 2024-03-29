#include "EventManager.h"

const ImGuiIO* EventManager::s_imguiIO = nullptr;

EventManager& EventManager::getInstance()
{
	static EventManager eventManager;
	return eventManager;
}

void EventManager::registerCallbacks(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, EventManager::handleResize);
	glfwSetKeyCallback(window, EventManager::handleKey);
	glfwSetCursorPosCallback(window, EventManager::handleMousePosition);
	glfwSetMouseButtonCallback(window, EventManager::handleMouseButton);
}

void EventManager::addHandler(EventHandler* handler)
{
	m_handlers.push_back(handler);
}

void EventManager::removeHandler(EventHandler* handler)
{
	// search through array, and delete (maybe use set to be faster)
	auto pos = std::find(m_handlers.begin(), m_handlers.end(), handler);
	if (pos == m_handlers.end()) {
		// handler not found
		return;
	}
	m_handlers.erase(pos);
}

void EventManager::sendEvent(const Event& e)
{
	// go through all handlers and call handlEvent
	for (auto& handler : m_handlers) {
		handler->handleEvent(e);
	}
}

void EventManager::handleResize(GLFWwindow* window, int width, int height)
{
	if (s_imguiIO->WantCaptureKeyboard || s_imguiIO->WantCaptureMouse) {
		return; // if imgui wants input => skip
	}

	// create and send event to handlers
	Event e;
	e.m_type = Event::Type::WINDOW_RESIZE;
	e.window = { width, height };
	getInstance().sendEvent(e);
}

void EventManager::handleKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (s_imguiIO->WantCaptureKeyboard) {
		return; // if imgui wants input => skip
	}

	// create and send event to handlers
	Event e;
	if (action == GLFW_PRESS) {
		e.m_type = Event::Type::KEY_PRESS;
	}
	else if (action == GLFW_RELEASE) {
		e.m_type = Event::Type::KEY_RELEASE;
	}
	e.key = { key };
	getInstance().sendEvent(e);
}

void EventManager::handleMousePosition(GLFWwindow* window, double xpos, double ypos)
{
	if (s_imguiIO->WantCaptureMouse) {
		return; // if imgui wants input => skip
	}

	// create and send event to handlers
	Event e;
	e.m_type = Event::Type::MOUSE_MOVE;
	e.mouse.x = (float)xpos;
	e.mouse.y = (float)ypos;
	getInstance().sendEvent(e);
}

void EventManager::handleMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	if (s_imguiIO->WantCaptureMouse) {
		return; // if imgui wants input => skip
	}

	// create and send event to handlers
	Event e;
	if (action == GLFW_PRESS) {
		e.m_type = Event::Type::MOUSE_BUTTON_PRESS;
	}
	else if (action == GLFW_RELEASE) {
		e.m_type = Event::Type::MOUSE_BUTTON_RELEASE;
	}
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	e.mouse.keyCode = button;
	e.mouse.x = (float)x;
	e.mouse.y = (float)y;
	getInstance().sendEvent(e);
}
