
#include "utility.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"


vk::VertexInputBindingDescription Vertex::getBindingDescription() {
	
    auto bindingDescription = vk::VertexInputBindingDescription{}
        .setBinding( 0 )
        .setStride( sizeof(Vertex) )
        .setInputRate( vk::VertexInputRate::eVertex );
    
    return bindingDescription;
}

std::vector<vk::VertexInputAttributeDescription> Vertex::getAttributeDescriptions() {

    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
    attributeDescriptions.resize( 5);
    
    attributeDescriptions[0]
        .setLocation( 0 )
        .setBinding( 0 )
        .setFormat( vk::Format::eR32G32B32Sfloat )
        .setOffset( offsetof(Vertex, position) );
    attributeDescriptions[1]
        .setLocation( 1 )
        .setBinding( 0 )
        .setFormat( vk::Format::eR32G32B32A32Sfloat )
        .setOffset( offsetof(Vertex, color) );
    attributeDescriptions[2]
        .setLocation( 2 )
        .setBinding( 0 )
        .setFormat( vk::Format::eR32G32Sfloat )
        .setOffset( offsetof(Vertex, texCoord) );
    attributeDescriptions[3]
        .setLocation( 3 )
        .setBinding( 0 )
        .setFormat( vk::Format::eR32G32B32Sfloat )
        .setOffset( offsetof(Vertex, normal) );
    attributeDescriptions[4]
        .setLocation( 4 )
        .setBinding( 0 )
        .setFormat( vk::Format::eR32G32B32Sfloat )
        .setOffset( offsetof(Vertex, tangent) );
    
    return attributeDescriptions;
}


// load model data
void LoadMesh( std::string_view meshPath, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices) {

    // load mesh data from file
    tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warning, error;
		
	if( !tinyobj::LoadObj( &attrib, &shapes, &materials, &warning, &error, meshPath.data()) ) 
	{
		throw std::runtime_error(warning + error);
	}
		
    // process vertex and index data
    vertices.clear(); indices.clear();
	std::unordered_map<Vertex, uint32_t> uniqueVertices;
	for (const auto &shape : shapes) 
	{
		for(const auto &index : shape.mesh.indices) 
		{
			Vertex vertex{};				
			vertex.position = {
				attrib.vertices[3*index.vertex_index + 0],
				attrib.vertices[3*index.vertex_index + 1],
				attrib.vertices[3*index.vertex_index + 2]
			};
			vertex.texCoord = {
				attrib.texcoords[2*index.texcoord_index + 0],
				1.0f - attrib.texcoords[2*index.texcoord_index + 1]
			};
			vertex.normal = {
				attrib.normals[3*index.normal_index + 0],
				attrib.normals[3*index.normal_index + 1],
				attrib.normals[3*index.normal_index + 2]
			};
			vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};			
				
			if (uniqueVertices.find(vertex) == uniqueVertices.end()) 
			{
				uniqueVertices[vertex] = static_cast<uint32_t>( vertices.size());
				vertices.push_back( vertex);
			}
				
			indices.push_back( uniqueVertices[vertex]);
		}
	}

	// calculate tangents
	for (size_t i = 0 ; i < indices.size(); i += 3) 
	{
		Vertex& v0 = vertices[ indices[i]];
		Vertex& v1 = vertices[ indices[i+1]];
		Vertex& v2 = vertices[ indices[i+2]];

		glm::vec3 edge1 = v1.position - v0.position;
		glm::vec3 edge2 = v2.position - v0.position;

		float deltaU1 = v1.texCoord.x - v0.texCoord.x;
		float deltaV1 = v1.texCoord.y - v0.texCoord.y;
		float deltaU2 = v2.texCoord.x - v0.texCoord.x;
		float deltaV2 = v2.texCoord.y - v0.texCoord.y;

		float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);
		glm::vec3 tangent = f * (deltaV2 * edge1 - deltaV1 * edge2);
		
		v0.tangent += tangent;
		v1.tangent += tangent;
		v2.tangent += tangent;
	}
}


// load image data
std::vector<uint8_t> LoadImage( std::string_view imagePath, uint32_t &width, uint32_t &height) {

    // load image data from file
	int w, h, c;
	stbi_uc *pixels = stbi_load( imagePath.data(), &w, &h, &c, STBI_rgb_alpha);
	if (!pixels)
	{
		throw std::runtime_error("Failed to load texture image!");
	}

    size_t imageSize = w * h * 4;
    std::vector<uint8_t> imageData;
    imageData.resize( imageSize);
    memcpy( imageData.data(), pixels, imageSize);
    width = w; height = h;

    stbi_image_free( pixels);

    return imageData;
}