#include "Camera.h"

void Camera::handleEvent(const Event& e)
{
	switch (e.type())
	{

	case Event::Type::MOUSE_BUTTON_PRESS:
		// ignore other mouse clicks except left mouse click
		if (e.key.keyCode != GLFW_MOUSE_BUTTON_LEFT) return;
		m_mousePressed = true;
		m_lastMousePos = { e.mouse.x, e.mouse.y };
		break;

	case Event::Type::MOUSE_BUTTON_RELEASE:
		// ignore other mouse clicks except left mouse click
		if (e.key.keyCode != GLFW_MOUSE_BUTTON_LEFT) return;
		m_mousePressed = false;
		break;

	case Event::Type::KEY_PRESS:
	case Event::Type::KEY_RELEASE:
		// on press a flag is set indicating direction
		// on release, that flag is removed
		setMovingDirection(e.key.keyCode, e.type() == Event::Type::KEY_PRESS);
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

void Camera::changeOrientation(double mouseX, double mouseY)
{
	// mouse movement doesnt depend on time so there is no deltatime

	// is mouse was not clicked then camera orientation does not change
	if (!m_mousePressed) {
		return;
	}
	
	// view matrix needs update
	m_needsUpdate = true;

	// TODO: maybe dont hardcode this
	const float SENSIVITY = 0.15f;

	float deltaX = m_currentMousePos.x - m_lastMousePos.x;
	// mouse y starts from top left so it is negated
	float deltaY = -(m_currentMousePos.y - m_lastMousePos.y);

	m_lastMousePos = m_currentMousePos;

	// multiply by sensitivity so mouse movement is not too much
	deltaX *=  SENSIVITY;
	deltaY *=  SENSIVITY;

	// pitch (up/down)
	glm::mat3 pitchRotateMatrix = glm::rotate(glm::radians(deltaY), m_right);
	
	// check if looking straight down/up
	// get the dot product absolute value and compare with 1.0 (means 0 degrees)
	if (1.0f - abs(glm::dot(pitchRotateMatrix * m_direction, m_UP)) > 0.005f) {
		m_direction = pitchRotateMatrix * m_direction;
	}

	// yaw (left/right)
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
	// TODO: maybe dont hardcode this
	const float SPEED = 2.5f;

	// update axis based on mouse movement
	changeOrientation(0, 0);

	// update position based on keys pressed
	if (m_moveDirection & CAMERA_MOVE_FORWARD) {
		m_position += float(deltaTime * SPEED) * m_direction;
		m_needsUpdate = true;
	}
	if (m_moveDirection & CAMERA_MOVE_BACKWARD) {
		m_position -= float(deltaTime * SPEED) * m_direction;
		m_needsUpdate = true;
	}
	if (m_moveDirection & CAMERA_MOVE_LEFT) {
		m_position -= float(deltaTime * SPEED) * m_right;
		m_needsUpdate = true;
	}
	if (m_moveDirection & CAMERA_MOVE_RIGHT) {
		m_position += float(deltaTime * SPEED) * m_right;
		m_needsUpdate = true;
	}
	if (m_moveDirection & CAMERA_MOVE_UP) {
		m_position += float(deltaTime * SPEED) * m_cameraUp;
		m_needsUpdate = true;
	}
	if (m_moveDirection & CAMERA_MOVE_DOWN) {
		m_position -= float(deltaTime * SPEED) * m_cameraUp;
		m_needsUpdate = true;
	}

	if (m_needsUpdate) {
		// manually calculate view matrix, the axis vectors are already calculated
		m_matrix = glm::mat4(
			glm::vec4(m_right.x, m_cameraUp.x, -m_direction.x, 0.0f),
			glm::vec4(m_right.y, m_cameraUp.y, -m_direction.y, 0.0f),
			glm::vec4(m_right.z, m_cameraUp.z, -m_direction.z, 0.0f),
			glm::vec4(
				-glm::dot(m_right, m_position), 
				-glm::dot(m_cameraUp,m_position), 
				-glm::dot(-m_direction, m_position),
				1
			)
		);
		/*m_matrix = glm::lookAt(m_position, m_position + m_direction, m_UP);*/
		m_needsUpdate = false;
	}

	/*printf("Camera at %f %f %f looking at %f %f %f \n",
		m_position.x, m_position.y, m_position.z,
		(m_position + m_direction).x, (m_position + m_direction).y, (m_position + m_direction).z);*/
}
