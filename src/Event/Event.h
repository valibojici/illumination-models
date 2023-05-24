#pragma once
/// <summary>
/// Class representing a GLFW event
/// </summary>
class Event {
public:
	// Event type
	enum class Type {
		MOUSE_BUTTON_PRESS,
		MOUSE_BUTTON_RELEASE,
		MOUSE_MOVE,
		KEY_PRESS,
		KEY_RELEASE,
		WINDOW_RESIZE,
		NONE
	};
	
	struct{
		int keyCode = 0;
		float x = 0;
		float y = 0;
	}mouse;

	struct{
		int width = 0;
		int height = 0;
	}window;

	struct{
		int keyCode = 0;
	}key;

	Type type() const { return m_type; }
private:
	friend class EventManager; // set as friend so it can have access to this class
	Type m_type = Type::NONE;
};