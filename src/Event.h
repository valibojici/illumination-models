#pragma once
/// <summary>
/// Class representing a GLFW event
/// </summary>
class Event {
public:
	enum class Type {
		MOUSE_BUTTON_PRESS,
		MOUSE_BUTTON_RELEASE,
		MOUSE_MOVE,
		KEY_PRESS,
		KEY_RELEASE,
		WINDOW_RESIZE
	};
	// TODO: read more about tagged unions
	union {
		struct{
			double x;
			double y;
		}mouse;

		struct {
			int width;
			int height;
		}window;

		struct {
			int keyCode;
		}key;
	};

	Type type() const { return m_type; }
private:
	friend class EventManager; // set as friend so it can have access to this class
	Type m_type;
};