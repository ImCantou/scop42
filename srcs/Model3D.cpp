/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Model3D.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qcherel <qcherel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 12:01:54 by qcherel           #+#    #+#             */
/*   Updated: 2024/01/25 14:52:03 by qcherel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Model3D.hpp"

Model3D::Model3D (void) {
	
}

Model3D::Model3D (const std::string objName) {
	this->name = objName;
}

Model3D::Model3D (const std::string objName, const std::string objMtllib, std::vector<glm::vec3>	objVertices, std::vector<uint32_t> objIndices) {
	this->name = objName;
	this->mtllib = objMtllib;
	this->verticesPos = objVertices;
	this->indices = objIndices;
}

Model3D::Model3D (Model3D const & src) {
	*this = src;
}

Model3D & Model3D::operator=(Model3D const & rhs) {
	if (this != &rhs) {
		this->name = rhs.name;
		this->mtllib = rhs.mtllib;
		this->verticesPos = rhs.verticesPos;
		this->verticesText = rhs.verticesText;
		this->verticesNorm = rhs.verticesNorm;
		this->_mesh = rhs._mesh;
		this->_meshIndices = rhs._meshIndices;
		this->indices = rhs.indices;
		this->indicesText = rhs.indicesText;
		this->indicesNorm = rhs.indicesNorm;
		this->textureFile = rhs.textureFile;
	}
	return *this;
}

Model3D::~Model3D (void) {
}

void	Model3D::addVertexPos(const glm::vec3& vertexPos) {
	verticesPos.push_back(vertexPos);
}

void	Model3D::addVertexText(const glm::vec2& vertexText) {
	verticesText.push_back(vertexText);
}

void	Model3D::addVertexNormal(const glm::vec3& vertexNorm) {
	verticesNorm.push_back(vertexNorm);
}


const std::vector<uint32_t>&	Model3D::getIndices() const {
	return (this->indices);
}

const std::vector<uint32_t>&	Model3D::getIndicesText() const {
	return (this->indicesText);
}

const std::vector<uint32_t>&	Model3D::getIndicesNorm() const {
	return (this->indicesNorm);
}

const std::vector<uint32_t>&	Model3D::getMeshIndices() const {
	return (this->_meshIndices);
}

const std::vector<Vertex>&	Model3D::getMesh() const {
	return (this->_mesh);
}

const std::vector<glm::vec2>&	Model3D::getVerticesText() const {
	return (this->verticesText);
}

const std::vector<glm::vec3>&	Model3D::getVerticesNorm() const {
	return (this->verticesNorm);
}

const std::vector<glm::vec3>&	Model3D::getVerticesPos() const {
	return (this->verticesPos);
}

const	std::string				Model3D::getTextureFile() const {
	return (this->textureFile);
}


void	Model3D::setMtllib(const std::string mtlLib) {
	this->mtllib = "resources/" + mtlLib;
	std::ifstream			in(this->mtllib, std::ios::in);
	std::string				line;

	if (!in) {
		std::cout << "Couldn't open mtllib file..." << std::endl << "Setting texture to default." << std::endl;
		this->textureFile = "";
		return ;
	}

	while (std::getline(in, line)) {
		if(line.substr(0, 7) == "map_Kd ") {
			this->textureFile = std::string(line.substr(7));
			return ;
		}
	}
	this->textureFile = "";
}

void	Model3D::addFace(std::vector<uint32_t>& vert, std::vector<uint32_t>& vertText, std::vector<uint32_t>& vertNorm) {
	if (vert.size() < 3)
		throw std::invalid_argument("Face with less than 3 vertices in .obj file!");
	this->indices.insert(indices.end(), vert.begin(), vert.begin() + 3);
	vert.erase(vert.begin() + 1);
	if (!vertText.empty()) {
		this->indicesText.insert(indicesText.end(), vertText.begin(), vertText.begin() + 3);
		vertText.erase(vertText.begin() + 1);
	}
	if (!vertNorm.empty()) {
		this->indicesNorm.insert(indicesNorm.end(), vertNorm.begin(), vertNorm.begin() + 3);
		vertNorm.erase(vertNorm.begin() + 1);
	}
	if (vert.size() >= 3)
		this->addFace(vert, vertText, vertNorm);
	else
	{
		vert.clear();
		vertText.clear();
		vertNorm.clear();
	}
}

void	Model3D::createMesh() {
	if ((verticesText.empty() && verticesNorm.empty()) || textureFile.empty()) {
		for (glm::vec3 vert : verticesPos)
			_mesh.push_back(Vertex(vert));
		_meshIndices = indices;
		return ;
	}
	
	for(uint32_t i = 0; i < indices.size(); i++) {
		// std::cout << indicesText.size() << std::endl;
		_mesh.push_back(Vertex(verticesPos[indices[i]], i < indicesText.size() ? verticesText[indicesText[i]] : glm::vec2(-1.0, -1.0),i < indicesNorm.size() ? verticesNorm[indicesNorm[i]] : glm::vec3(1.0, 0.0, 0.0)));
		_meshIndices.push_back(i);
	}
}
