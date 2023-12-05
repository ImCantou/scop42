#ifndef	__CAMERA3D_CLASS__
# define __CAMERA3D_CLASS__

# include "Vertex.hpp"

enum	CameraCommands {
	CAM_TRANSLATE_FORWARDS,
	CAM_TRANSLATE_BACKWARDS,
	CAM_TRANSLATE_LEFT,
	CAM_TRANSLATE_RIGHT

};

class Camera3D
{
private:
	glm::vec3	pos;
	glm::vec3	front;
	glm::vec3	up;
	float		fov;
public:
	Camera3D();
	void	rotate(double xoffset, double yoffset);
	void	move(CameraCommands command , float speed);
	void	zoom(float offset);

	struct glm::mat<4, 4, glm::f32, glm::packed_highp>	lookAt();
	float	getFov();
};


#endif