#version 410 core

layout(location = 0) in vec3 aPos; // Quad positions
layout(location = 1) in vec2 aTexCoords; // Texture coordinates

out vec2 TexCoords; // Pass to fragment shader

void main()
{
    TexCoords = aTexCoords; // Pass texture coordinates
    gl_Position = vec4(aPos, 1.0); // Standard quad projection
}
