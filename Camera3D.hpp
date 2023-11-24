#ifndef	__CAMERA3D_CLASS__
# define __CAMERA3D_CLASS__

# include "Vertex.hpp"

class Camera3D
{
private:
	glm::vec3	pos;
	glm::vec3	front;
	glm::vec3	up;
public:
	Camera3D();
	void	rotate(glm::radians angle);
	void	move(glm::vec3 direction);

	detail::tmat4x4<T>	lookAt();
};


#endif