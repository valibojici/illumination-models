#pragma once

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

	union Data {
		struct {
			float x;
			float y;
		};
		struct {
			float width;
			float height;
		};
		struct{
			int keyCode;
		};
	};

	Data data() const { return m_data; }
	Type type() const { return m_type; }
private:
	Data m_data;
	Type m_type;
};