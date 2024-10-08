
#version 460

// input
layout (location = 0) in vec3 fragPosition;
layout (location = 1) in vec4 fragColor;
layout (location = 2) in vec2 fragTexCoord;
layout (location = 3) in mat3 fragTBN;


// output
layout (location = 0) out vec4 outColor;


// samplers
layout(set = 2, binding = 0) uniform sampler2D textureImage;


void main() {

    vec3 diffuse = texture( textureImage, fragTexCoord).rgb;
    
    outColor = fragColor * vec4( diffuse, 1);    
}