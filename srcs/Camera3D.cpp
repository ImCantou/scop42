#include "Camera3D.hpp"

Camera3D::Camera3D(): pos(glm::vec3(0.0f, 3.0f, 0.0f)), front(glm::vec3(0.0f, -1.0f, 0.0f)), up(glm::vec3(0.0f, 0.0f, 1.0f)), right(glm::vec3(0.0f, 0.0f, 1.0f)), fov(45.0f), rotations(0.0f, 0.0f, 0.0f) {

}

void	Camera3D::move(CameraCommands command, float speed) {
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
	// static float yaw   = -90.0f;
	// static float pitch =  0.0f;
// 
	// pitch += yoffset;
	// yaw -= xoffset;
	// if (yaw > 180.0f)
		// yaw -= 360.0f;
	// else if (yaw < -180.0f)
		// yaw += 360.0f;
	// if (pitch > 89.0f)
		// pitch = 89.0f;
	// if (pitch < -89.0f)
		// pitch = -89.0f;

	// std::cout << "pitch: " << pitch << std::endl << "yaw: " << yaw << std::endl;
	// glm::vec3 tmp(front);
    // tmp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    // tmp.z = sin(glm::radians(pitch));
    // tmp.y = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	// 
	// front = glm::normalize(tmp);




	// glm::vec3	cross_y = glm::cross(this->front, this->up);


	// if (yoffset) {
	// 	float	cosangle = cos(glm::radians(yoffset));
	// 	float	sinangle = sin(glm::radians(yoffset));

	// 	this->front = this->front * cosangle + glm::cross(cross_y, this->front) * sinangle + this->front * glm::dot(cross_y, this->front) * (1 - cosangle);
	// 	this->up = this->up * cosangle + glm::cross(cross_y, this->up) * sinangle + this->up * glm::dot(cross_y, this->up) * (1 - cosangle);
		
	// 	this->front = glm::normalize(this->front);
	// 	this->up = glm::normalize(this->up);
	// }
	// if (xoffset) {
	// 	float	cosangle = cos(glm::radians(-xoffset));
	// 	float	sinangle = sin(glm::radians(-xoffset));

	// 	this->front = this->front * cosangle + glm::cross(this->up, this->front) * sinangle + this->front * glm::dot(this->up, this->front) * (1 - cosangle);

	// 	this->front = glm::normalize(this->front);
	// }




	// glm::vec3 right = glm::normalize(glm::cross(front, up));
	// xoffset *= 0.25;
	// yoffset *= 0.25;


	// front = glm::rotate(glm::mat4(1.0f), static_cast<float>(yoffset), right) * glm::vec4(front, 0.0f);
	// front = glm::rotate(glm::mat4(1.0f), static_cast<float>(xoffset), -up) * glm::vec4(front, 0.0f);
	
	// // up = glm::rotate(glm::mat4(1.0f), static_cast<float>(xoffset), -up) * glm::vec4(up, 0.0f);
	// up = glm::rotate(glm::mat4(1.0f), static_cast<float>(yoffset), right) * glm::vec4(up, 0.0f);


	// up = glm::normalize(up);
	// front = glm::normalize(front);

	rotations.x += -xoffset ;
	rotations.y += yoffset ;
	// if (rotations.z > 180.0f)
	// 	rotations.z -= 360.0f;
	// else if (rotations.z < -180.0f)
	// 	rotations.z += 360.0f;
	// if (rotations.x > 89.0f)
	// 	rotations.x = 89.0f;
	// if (rotations.x < -89.0f)
	// 	rotations.x = -89.0f;

	// glm::quat	qPitch	= glm::angleAxis(glm::radians(rotations.x), glm::vec3(1, 0, 0));
	// glm::quat	qYaw	= glm::angleAxis(glm::radians(rotations.z), glm::vec3(0, 0, 1));
	// glm::quat	qRoll	= glm::angleAxis(glm::radians(rotations.y), glm::vec3(0, 1, 0));

	// glm::quat	orientation = qPitch * qYaw;
	// orientation = glm::normalize(orientation);

	// glm::mat4 rotate = glm::mat4_cast(orientation);
	// glm::mat4 translate = glm::mat4(1.0f);
	// translate = glm::translate(translate, - this->pos);

	// glm::mat4 viewMatrix = rotate * translate;

	// this->front = glm::normalize(glm::vec3(viewMatrix * glm::vec4(this->front, 0.0f)));
	// this->right = glm::normalize(glm::vec3(viewMatrix * glm::vec4(this->right, 0.0f)));

	// this->up = glm::cross(front, right);	

	this->front = glm::vec3(cos(glm::radians(rotations.y)) * sin(glm::radians(rotations.x)),
    						sin(glm::radians(rotations.y)),
    						cos(glm::radians(rotations.y)) * cos(glm::radians(rotations.x)));
	
	this->right = glm::vec3(sin(glm::radians(rotations.x) - glm::radians(90.0f)),
							0,
    						cos(glm::radians(rotations.x) - glm::radians(90.0f)));

	this->up = glm::cross(right, front);
}


struct glm::mat<4, 4, glm::f32, glm::packed_highp> Camera3D::lookAt() {
	return (glm::lookAt(pos, pos + front, up));
}

float	Camera3D::getFov() {
	return fov;
}

void	Camera3D::zoom(float offset) {
	fov -= offset;
	if (fov <= 1.0f)
		fov = 1.0f;
	else if (fov >= 45.0f)
		fov = 45.0f;
}