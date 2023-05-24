#include "Camera.h"

void Camera::handleEvent(const Event& e)
{
	switch (e.type())
	{

	case Event::Type::MOUSE_BUTTON_PRESS:
		// ignore other mouse clicks except left mouse click
		if (e.mouse.keyCode != GLFW_MOUSE_BUTTON_LEFT) return;
		m_mousePressed = true;
		m_lastMousePos = { e.mouse.x, e.mouse.y };
		break;

	case Event::Type::MOUSE_BUTTON_RELEASE:
		// ignore other mouse clicks except left mouse click
		if (e.mouse.keyCode != GLFW_MOUSE_BUTTON_LEFT) return;
		m_mousePressed = false;
		break;

	// on key press a flag is set indicating direction
	case Event::Type::KEY_PRESS:
		setMovingDirection(e.key.keyCode, true);
		// if left shift is pressed => move faster
		if (e.key.keyCode == GLFW_KEY_LEFT_SHIFT) {
			CAMERA_SPEED = 2.5f;
		}
		break;
		// on release, that flag is removed
	case Event::Type::KEY_RELEASE:
		setMovingDirection(e.key.keyCode, false);
		if (e.key.keyCode == GLFW_KEY_LEFT_SHIFT) {
			CAMERA_SPEED = 1.25f;
		}
		break;
	case Event::Type::MOUSE_MOVE:
		m_currentMousePos = { e.mouse.x, e.mouse.y };
		break;
	default:
		return;
	}
}

void Camera::setMovingDirection(unsigned int key, bool pressed)
{
	unsigned int flag = CAMERA_MOVE_NONE;
	switch (key)
	{
	case GLFW_KEY_A:
		flag = CAMERA_MOVE_LEFT;
		break;
	case GLFW_KEY_D:
		flag = CAMERA_MOVE_RIGHT;
		break;
	case GLFW_KEY_W:
		flag = CAMERA_MOVE_FORWARD;
		break;
	case GLFW_KEY_S:
		flag = CAMERA_MOVE_BACKWARD;
		break;
	case GLFW_KEY_R:
		flag = CAMERA_MOVE_UP;
		break;
	case GLFW_KEY_F:
		flag = CAMERA_MOVE_DOWN;
		break;
	default:
		return;
		break;
	}

	if (pressed) {
		m_moveDirection |= flag;
	}
	else {
		m_moveDirection &= (~flag);
	}
}

void Camera::changeOrientation()
{
	// mouse movement doesnt depend on time => no deltatime

	// if mouse is not being clicked then camera orientation does not change
	if (!m_mousePressed) {
		return;
	}
	
	// view matrix needs update
	m_needsUpdate = true;

	float deltaX = m_currentMousePos.x - m_lastMousePos.x;
	// mouse y starts from top left so it is negated
	float deltaY = -(m_currentMousePos.y - m_lastMousePos.y);

	m_lastMousePos = m_currentMousePos;

	// multiply by sensitivity so mouse movement is not too much
	deltaX *=  MOUSE_SENSIVITY;
	deltaY *=  MOUSE_SENSIVITY;

	// pitch (up/down), rotate along the camera 'right' vector
	glm::mat3 pitchRotateMatrix = glm::rotate(glm::radians(deltaY), m_right);
	
	// check to not look straight down/up
	// get the dot product absolute value and compare with 1.0 (cos 0 degrees == 1.0 )
	if (1.0f - abs(glm::dot(pitchRotateMatrix * m_direction, m_UP)) > 0.005f) {
		m_direction = pitchRotateMatrix * m_direction;
	}

	// yaw (left/right), rotate along camera real up vector
	glm::mat3 yawRotateMatrix = glm::rotate(-glm::radians(deltaX), m_cameraUp);
	m_direction = yawRotateMatrix * m_direction;
	
	// recalculate camera axis based on new look direction
	m_right = glm::normalize(glm::cross(m_direction, m_UP));
	m_cameraUp = glm::cross(-m_direction, m_right);
}

Camera::Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
{
	m_position = position;
	m_direction = glm::normalize(target - position);
	m_right = glm::normalize(glm::cross(m_direction, m_UP));
	m_cameraUp = glm::cross(-m_direction, m_right);
}

void Camera::update(double deltaTime)
{
	// update axis based on mouse movement
	changeOrientation();

	// update position based on keys pressed
	if (m_moveDirection & CAMERA_MOVE_FORWARD) {
		m_position += float(deltaTime * CAMERA_SPEED) * m_direction;
		m_needsUpdate = true;
	}
	if (m_moveDirection & CAMERA_MOVE_BACKWARD) {
		m_position -= float(deltaTime * CAMERA_SPEED) * m_direction;
		m_needsUpdate = true;
	}
	if (m_moveDirection & CAMERA_MOVE_LEFT) {
		m_position -= float(deltaTime * CAMERA_SPEED) * m_right;
		m_needsUpdate = true;
	}
	if (m_moveDirection & CAMERA_MOVE_RIGHT) {
		m_position += float(deltaTime * CAMERA_SPEED) * m_right;
		m_needsUpdate = true;
	}
	if (m_moveDirection & CAMERA_MOVE_UP) {
		m_position += float(deltaTime * CAMERA_SPEED) * m_cameraUp;
		m_needsUpdate = true;
	}
	if (m_moveDirection & CAMERA_MOVE_DOWN) {
		m_position -= float(deltaTime * CAMERA_SPEED) * m_cameraUp;
		m_needsUpdate = true;
	}

	if (m_needsUpdate) {
		// manually calculate view matrix, the axis vectors are already calculated
		m_matrix = glm::mat4(
			// axis change part, negate direction to point 'backwards'
			glm::vec4(m_right.x, m_cameraUp.x, -m_direction.x, 0.0f),
			glm::vec4(m_right.y, m_cameraUp.y, -m_direction.y, 0.0f),
			glm::vec4(m_right.z, m_cameraUp.z, -m_direction.z, 0.0f),
			// translation part
			glm::vec4(-glm::dot(m_right, m_position),  -glm::dot(m_cameraUp,m_position), -glm::dot(-m_direction, m_position), 1)
		);
		m_needsUpdate = false;
	}
}

void Camera::setPosition(const glm::vec3& pos)
{
	m_position = pos;
	m_needsUpdate = true;
}

void Camera::setTarget(const glm::vec3& target) {
	// calculate where the camera is looking
	m_direction = glm::normalize(target - m_position);
	// recalculate camera right vector
	m_right = glm::normalize(glm::cross(m_direction, m_UP));
	// recalculate camera up vector
	m_cameraUp = glm::cross(-m_direction, m_right);
	m_needsUpdate = true; // the view matrix needs to be recalculated
}
