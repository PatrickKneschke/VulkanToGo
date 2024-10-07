#pragma once

#include "vk_core.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <vector>


// single mesh vertex data
struct Vertex {

	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texCoord;
	glm::vec3 normal;
	glm::vec3 tangent;

	static vk::VertexInputBindingDescription getBindingDescription();	
	static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
	
	bool operator ==(const Vertex &other) const {
		
		return position == other.position && color == other.color && texCoord == other.texCoord
				&& normal == other.normal && tangent == other.tangent;
	}
};

// custom hash for Vertex type
namespace std {
	template<> 
	struct hash<Vertex> {
			
		size_t operator()(Vertex const &vertex) const {
				
			return ((((hash<glm::vec3>()(vertex.position) ^
				      (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
					  (hash<glm::vec2>()(vertex.texCoord) << 1) >> 1) ^
					  (hash<glm::vec3>()(vertex.normal) << 1) >> 1) ^
					  (hash<glm::vec3>()(vertex.tangent) << 1);
		}
	};
}


// load .obj file
void LoadMesh(std::string_view meshPath, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices);


// load image
std::vector<uint8_t> LoadImage(std::string_view imagePath, uint32_t &width, uint32_t &height);

