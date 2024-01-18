/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Model3D.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qcherel <qcherel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 12:00:19 by qcherel           #+#    #+#             */
/*   Updated: 2024/01/18 12:32:23 by qcherel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <vector>
# include "Vertex.hpp"

class Model3D {

	private:
		std::string				name;
		std::string				mtllib;
		std::vector<Vertex>		vertices;
		std::vector<glm::vec2>	verticesText;
		std::vector<glm::vec3>	verticesNorm;
		std::vector<uint32_t>	indices;
		std::vector<uint32_t>	indicesText;
		std::vector<uint32_t>	indicesNorm;
		
		std::string				textureFile;

	public:
		Model3D(void);
		Model3D(const Model3D & src);
		Model3D(const std::string);
		Model3D(const std::string, const std::string, std::vector<Vertex>, std::vector<uint32_t>);
		~Model3D(void);

		Model3D & operator=(Model3D const & rhs);

		//Setters
		void	addVertex(const Vertex& vertex);
		void	addVertexText(const glm::vec2& vertexText);
		void	addVertexNormal(const glm::vec3& vertexNorm);

		void	addFace( std::vector<uint32_t>&,  std::vector<uint32_t>&,  std::vector<uint32_t>&);

		void	setMtllib(const std::string);

		//Getters
		const   std::vector<Vertex>&	getVertices() const;
		const   std::vector<glm::vec2>&	getVerticesText() const;
		const   std::vector<glm::vec3>&	getVerticesNorm() const;
		const	std::vector<uint32_t>&	getIndices() const;

		const	std::string				getTextureFile() const;

		//Rotate Model
		// void	rotate(glm::vec3 axis, float angle);
};

