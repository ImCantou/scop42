#include "Camera3D.hpp"

Camera3D::Camera3D(): pos(glm::vec3(0.0f, 0.0f, 3.0f)), front(glm::vec3(0.0f, 0.0f, -1.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)) {

}

void	Camera3D::move(glm::vec3 direction) {

}

void	Camera3D::rotate(glm::radians angle) {

}


detail::tmat4x4<T> Camera3D::lookAt() {
	return (glm::lookAt(pos, pos - front, up));
}