/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Model3D.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qcherel <qcherel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 12:00:19 by qcherel           #+#    #+#             */
/*   Updated: 2024/01/25 14:45:53 by qcherel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <vector>
# include <fstream>
# include <sstream>
# include "Vertex.hpp"

class Model3D {

	private:
		std::string				name;
		std::string				mtllib;
		std::vector<glm::vec3>	verticesPos;
		std::vector<glm::vec2>	verticesText;
		std::vector<glm::vec3>	verticesNorm;
		std::vector<uint32_t>	indices;
		std::vector<uint32_t>	indicesText;
		std::vector<uint32_t>	indicesNorm;

		std::vector<Vertex>		_mesh;		
		std::vector<uint32_t>	_meshIndices;


		std::string				textureFile;

	public:
		Model3D(void);
		Model3D(const Model3D & src);
		Model3D(const std::string);
		Model3D(const std::string, const std::string, std::vector<glm::vec3>, std::vector<uint32_t>);
		~Model3D(void);

		Model3D & operator=(Model3D const & rhs);

		//Setters
		void	addVertexPos(const glm::vec3& vertexPos);
		void	addVertexText(const glm::vec2& vertexText);
		void	addVertexNormal(const glm::vec3& vertexNorm);

		void	addFace( std::vector<uint32_t>&,  std::vector<uint32_t>&,  std::vector<uint32_t>&);

		void	setMtllib(const std::string);

		//Getters
		const   std::vector<Vertex>&	getMesh() const;
		const   std::vector<glm::vec2>&	getVerticesText() const;
		const   std::vector<glm::vec3>&	getVerticesNorm() const;
		const   std::vector<glm::vec3>&	getVerticesPos() const;
		const	std::vector<uint32_t>&	getIndices() const;
		const	std::vector<uint32_t>&	getIndicesText() const;
		const	std::vector<uint32_t>&	getIndicesNorm() const;
		const	std::vector<uint32_t>&	getMeshIndices() const;

		const	std::string				getTextureFile() const;

		//Create Mesh
		void	createMesh();
};
