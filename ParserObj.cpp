/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserObj.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qcherel <qcherel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 12:02:25 by qcherel           #+#    #+#             */
/*   Updated: 2023/11/28 12:08:44 by qcherel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParserObj.hpp"

Model3D						ParserObj::parseFile(const std::string filename) {
	
	Model3D					object;			
	std::ifstream			in(filename, std::ios::in);
	Model3D					*currentModel = nullptr;
	std::string				line;
	 
	if (!in)
		throw	std::runtime_error("failed to open .obj file!");


	while(std::getline(in, line)) {
		switch(ParserObj::getLineType(line)) {
			case VERTEX :
			 	object.addVertex(Vertex(ParseVertex(line)));
			 	break;
			case VERTEX_TEXTURE :
				object.addVertexText(ParseVertexText(line));
			 	break;
			case VERTEX_NORMAL : 
				object.addVertexNormal(ParseVertexNormal(line));
				break;
			case FACE :
				ParseFace(object, line);
				break ;
			case MTLLIB :
				object.setMtllib(line.substr(7));
				break;
			default :
				break;
		}
	}
	return object;
}

glm::vec3					ParserObj::ParseVertex(const std::string line) {
	std::istringstream	v(line.substr(2));
	float	x , y , z;
	v >> x;
	v >> y;
	v >> z;
	return (glm::vec3(x, y, z));
}

glm::vec2					ParserObj::ParseVertexText(const std::string line) {
	std::istringstream	v(line.substr(3));
	float	x , y;
	v >> x;
	v >> y;
	return (glm::vec2(x, y));
}


glm::vec3					ParserObj::ParseVertexNormal(const std::string line) {
	std::istringstream	v(line.substr(3));
	float	x , y, z;
	v >> x;
	v >> y;
	v >> z;
	return (glm::vec3(x, y, z));
}

void		ParserObj::ParseFace(Model3D& model, const std::string line) {
	std::istringstream		v(trim_copy(line).substr(2));
	std::vector<uint32_t>	vert, vertText, vertNorm;
	uint32_t				current;
	
	while(!v.eof()) {
		v >> current;
		vert.push_back(--current);
		if (v.get() == '/') {
			if (v.peek() == '/') {
				v.get();
				v >> current;
				vertNorm.push_back(--current);
			}
			else {
				v >> current;
				vertText.push_back(--current);
				if (v.get() == '/') {
					v >> current;
					vertNorm.push_back(--current);
				}
			}
		}
	}
	if ((!vertText.empty() && vertText.size() != vert.size()) || (!vertNorm.empty() && vertNorm.size() != vert.size()))
	{
		throw std::invalid_argument("File .obj has an invalid face format");
	}
	if (!checkExistingVertexInFace(model, vert, vertText, vertNorm)) {
		throw std::invalid_argument("File .obj has invalid Vertex indices in face");		
	}

	
	model.addFace(vert, vertText, vertNorm);
	// std::cin.getline(line);
}

bool						ParserObj::checkExistingVertexInFace(const Model3D& model, const std::vector<uint32_t>& vert, const std::vector<uint32_t>& vertText, const std::vector<uint32_t>& vertNorm) {
	uint32_t size = model.getVertices().size();	
	for ( uint32_t index : vert) {
		if (index >= size)
			return false;
	}

	size = model.getVerticesText().size();
	if (!model.getVerticesText().empty() &&!vertText.empty()) {
		for ( uint32_t index : vertText) {
			if (index >= size)
				return false;
		}
	}
	
	size = model.getVerticesNorm().size();
	if (!model.getVerticesNorm().empty() && !vertNorm.empty()) {
		for ( uint32_t index : vertNorm) {
			if (index >= size)
				return false;
		}
	}
	
	return true;
}

int							ParserObj::getLineType(const std::string line) {
	if(line.substr(0, 2) == "v ")
		return VERTEX;
	else if(line.substr(0, 3) == "vt ")
		return VERTEX_TEXTURE;
	else if(line.substr(0, 3) == "vn ")
		return VERTEX_NORMAL;
	else if(line.substr(0, 2) == "f ")
		return FACE;	
	else if(line.substr(0, 2) == "o ")
		return OBJECTNAME;
	else if(line.substr(0, 2) == "s ")
		return SMOOTH_SHADING;
	else if(line.substr(0, 2) == "g ")
		return GROUP;
	else if(line.substr(0, 1) == "#")
		return COMMENT;
	else if(line.substr(0, 7) == "usemtl ")
		return USEMTL;
	else if(line.substr(0, 7) == "mtllib ")
		return MTLLIB;
	else 
		return -1;
}
