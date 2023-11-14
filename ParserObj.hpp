/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserObj.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qcherel <qcherel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 12:02:20 by qcherel           #+#    #+#             */
/*   Updated: 2023/11/14 12:31:54 by qcherel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PARSEROBJ_CLASS__
#define __PARSEROBJ_CLASS__

#include <optional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <limits> 
#include <algorithm>
#include <cstring>
#include <vector>
#include <set>
#include <array>
#include <chrono>

#include "Model3D.hpp"

# define	GLM_FORCE_RADIANS
# define GLM_FORCE_DEPTH_ZERO_TO_ONE
# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>

enum	e_typeParse {
	VERTEX,
	VERTEX_TEXTURE,
	VERTEX_NORMAL,
	FACE,
	OBJECTNAME,
	GROUP,
	USEMTL,
	MTLLIB,
	SMOOTH_SHADING,
	COMMENT
};

class	ParserObj {
	private: 
		static int						getLineType(const	std::string	line);

		static glm::vec3				ParseVertex(const std::string	line);
		static glm::vec3				ParseVertexNormal(const std::string line);
		static glm::vec2				ParseVertexText(const std::string line);

		static std::vector<uint32_t>	ParseFace(Model3D&, const std::string);

	public:

		static Model3D					parseFile(const std::string filename);


};

#endif