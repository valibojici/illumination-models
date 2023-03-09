#pragma once
#include "EventHandler.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLFW/glfw3.h"

class Camera : public EventHandler
{
private:
	enum MoveDirection {
		CAMERA_MOVE_FORWARD = (1 << 0),
		CAMERA_MOVE_BACKWARD = (1 << 1),
		CAMERA_MOVE_LEFT = (1 << 2),
		CAMERA_MOVE_RIGHT = (1 << 3),
		CAMERA_MOVE_UP = (1 << 4),
		CAMERA_MOVE_DOWN = (1 << 5),
		CAMERA_MOVE_NONE = 0
	};
	// camera current moving direction (can be multiple)
	unsigned int m_moveDirection = CAMERA_MOVE_NONE;
	
	// time in seconds of the last mouse move event
	double m_lastMouseMoveTime = 0;

	// true if mouse left button was pressed and camera can move
	bool m_mousePressed = false;

	// indicates if view matrix need to be recalculated
	bool m_needsUpdate = true;

	// last mouse position (after click)
	glm::vec2 m_lastMousePos = { 0.0f,0.0f };
	// current mouse position (after click)
	glm::vec2 m_currentMousePos = { 0.0f,0.0f };

	// view matrix
	glm::mat4 m_matrix = glm::mat4(1.0f);
	
	// camera position
	glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 1.0f);

	// camera view direction (where the camera is pointing to)
	glm::vec3 m_direction = glm::vec3(0.0f, 0.0f, -1.0f);

	// right vector of the camera
	glm::vec3 m_right = glm::vec3(1.0f, 0.0f, 0.0f);

	// up vector of the camera
	glm::vec3 m_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// world UP vector
	glm::vec3 m_UP = glm::vec3(0.0f, 1.0f, 0.0f);
	
	void handleEvent(const Event& e);

	// helper method to set/unset flags
	void setMovingDirection(unsigned int key, bool pressed);

	/// <summary>
	/// Change camera view direction based on mouse coords.
	/// </summary>
	void changeOrientation(double mouseX, double mouseY);
public:
	Camera(
		const glm::vec3& position = glm::vec3(0.0f, 0.0f, 1.0f), 
		const glm::vec3& target = glm::vec3(0.0f), 
		const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f)
	);
	/// <summary>
	/// Get the camera view matrix (to update it, call update() method)
	/// </summary>
	inline const glm::mat4& getMatrix() const { return m_matrix; }
	
	// TODO: maybe move this in the handler
	/// <summary>
	/// Recalculates camera matrix based on the position and orientation
	/// </summary>
	/// <param name="deltaTime">: time in seconds between frames</param>
	void update(double deltaTime = 1.0);

	/// <summary>
	/// Get the camera position vector.
	/// </summary>
	inline const glm::vec3& getPosition() const { return m_position; }

	/// <summary>
	/// Set the camera position vector.
	/// </summary>
	void setPosition(const glm::vec3& pos);

	/// <summary>
	/// Set the camera view target (position+direction)
	/// </summary>
	void setTarget(const glm::vec3& target);
};

