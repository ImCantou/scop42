/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserObj.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qcherel <qcherel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 12:02:20 by qcherel           #+#    #+#             */
/*   Updated: 2024/01/22 12:13:21 by qcherel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once 

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
#include <list>
#include <set>
#include <array>
#include <chrono>

# include "Model3D.hpp"
# include "str_utils.hpp"

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

		static void						ParseFace(Model3D&, const std::string);

		static bool						checkExistingVertexInFace(const Model3D&, const std::vector<uint32_t>& vert, const std::vector<uint32_t>& vertText, const std::vector<uint32_t>& vertNorm);

	public:

		static Model3D					parseFile(const std::string filename);

};
