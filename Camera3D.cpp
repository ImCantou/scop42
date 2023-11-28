#include "Camera3D.hpp"

Camera3D::Camera3D(): pos(glm::vec3(0.0f, 3.0f, 0.0f)), front(glm::vec3(0.0f, -1.0f, 0.0f)), up(glm::vec3(0.0f, 0.0f, 1.0f)) {

}

void	Camera3D::move(CameraCommands command, float speed) {
	std::cout << "pos: (" << pos.x << " , " << pos.y << " , " << pos.z << ")" << std::endl;
	std::cout << "front: (" << front.x << " , " << front.y << " , " << front.z << ")" << std::endl;
	std::cout << "up: (" << up.x << " , " << up.y << " , " << up.z << ")" << std::endl;
	switch (command)
	{
	case CAM_TRANSLATE_FORWARDS:
		this->pos += speed * this->front;
		break;
	case CAM_TRANSLATE_BACKWARDS:
		this->pos -= speed * this->front;
		break;
	case CAM_TRANSLATE_LEFT:
		this->pos -= speed * glm::normalize(glm::cross(this->front, this->up));
		break;
	case CAM_TRANSLATE_RIGHT:
		this->pos += speed * glm::normalize(glm::cross(this->front, this->up));
		break;
	default:
		break;
	}
}

void	Camera3D::rotate(double xoffset, double yoffset) {
	static float yaw   = -90.0f;
	static float pitch =  0.0f;

	pitch += yoffset;
	yaw -= xoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	std::cout << "pitch: " << pitch << std::endl << "yaw: " << yaw << std::endl;
	glm::vec3 tmp;
    tmp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    tmp.z = sin(glm::radians(pitch));
    tmp.y = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	
	front = glm::normalize(tmp);
}


struct glm::mat<4, 4, glm::f32, glm::packed_highp> Camera3D::lookAt() {
	return (glm::lookAt(pos, pos + front, up));
}