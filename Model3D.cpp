/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Model3D.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qcherel <qcherel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 12:01:54 by qcherel           #+#    #+#             */
/*   Updated: 2023/11/16 15:43:55 by qcherel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Model3D.hpp"

Model3D::Model3D (void) {
	
}

Model3D::Model3D (const std::string objName) {
	this->name = objName;
}

Model3D::Model3D (const std::string objName, const std::string objMtllib, std::vector<Vertex>	objVertices, std::vector<uint32_t> objIndices) {
	this->name = objName;
	this->mtllib = objMtllib;
	this->vertices = objVertices;
	this->indices = objIndices;
}

Model3D::Model3D (Model3D const & src) {
	*this = src;
}

Model3D & Model3D::operator=(Model3D const & rhs) {
	if (this != &rhs) {
		this->name = rhs.name;
		this->mtllib = rhs.mtllib;
		this->vertices = rhs.vertices;
		this->indices = rhs.indices;
	}
	return *this;
}

Model3D::~Model3D (void) {
}

void	Model3D::addVertex(const Vertex& vertex) {
	vertices.push_back(vertex);
}

void	Model3D::addVertexText(const glm::vec2& vertexText) {
	verticesText.push_back(vertexText);
}

void	Model3D::addVertexNormal(const glm::vec3& vertexNorm) {
	verticesNorm.push_back(vertexNorm);
}

void	Model3D::setMtllib(const std::string mtlLib) {
	this->mtllib = mtlLib;
}

void	Model3D::addFace(std::vector<uint32_t>& vert, std::vector<uint32_t>& vertText, std::vector<uint32_t>& vertNorm) {
	if (vert.size() < 3)
		throw std::invalid_argument("Face with less than 3 vertices in .obj file!");
	this->indices.insert(indices.end(), vert.begin(), vert.begin() + 2);
	vert.erase(vert.begin() + 1);
	if (!vertText.empty()) {
		this->indicesText.insert(indicesText.end(), vertText.begin(), vertText.begin() + 2);
		vertText.erase(vertText.begin() + 1);
	}
	if (!vertNorm.empty()) {
		this->indicesText.insert(indicesText.end(), vertNorm.begin(), vertNorm.begin() + 2);
		vertNorm.erase(vertNorm.begin() + 1);
	}
}
