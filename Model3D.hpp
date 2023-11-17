/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Model3D.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qcherel <qcherel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 12:00:19 by qcherel           #+#    #+#             */
/*   Updated: 2023/11/16 15:43:34 by qcherel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __MODEL3D_CLASS__
#define __MODEL3D_CLASS__

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
		const   std::vector<Vertex>&	vert() const;
		const   std::vector<Vertex>&	vertText() const;
		const   std::vector<Vertex>&	vertNorm() const;
};

#endif