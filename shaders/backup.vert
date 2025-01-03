#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 fPosition;
out vec3 fNormal;
out vec2 fTexCoords;
out vec4 fragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    vec4 worldPos = model * vec4(vPosition, 1.0f);
    gl_Position = projection * view * worldPos;

    fPosition = vPosition;
    fNormal = vNormal;
    fTexCoords = vTexCoords;

    fragPosLightSpace = lightSpaceMatrix * worldPos;
}
