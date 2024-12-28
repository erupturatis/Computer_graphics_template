#version 410 core

in vec2 TexCoords; // From vertex shader
out vec4 FragColor;

uniform sampler2D fragPosLightSpaceMap; // Texture with FragPosLightSpace values

void main()
{
    vec4 fragPosLightSpace = texture(fragPosLightSpaceMap, TexCoords);
    FragColor = vec4(vec3(fragPosLightSpace.z * 0.5 + 0.5), 1.0);
}
