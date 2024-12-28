#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec4 fragPosLightSpace;
in vec2 fTexCoords;

out vec4 fColor;

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

uniform bool applyFog;

float ambientStrength = 0.2f;
float specularStrength = 0.5f;
float shininess = 32.0f;

float computeFog()
{
    float fogDensity = 0.01f;
    float fragmentDistance = length(fPosEye.xyz);
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
    return clamp(fogFactor, 0.0f, 1.0f);
}

void main()
{
    vec3 cameraPosEye = vec3(0.0f);

    vec3 normalEye = normalize(fNormal);
    vec3 lightDirN = normalize(lightDir);
    vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);

    vec3 ambient = ambientStrength * lightColor;
    vec3 diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    vec3 reflection = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
    vec3 specular = specularStrength * specCoeff * lightColor;

    vec3 baseColor = vec3(0.9f, 0.35f, 0.0f);

    ambient *= texture(diffuseTexture, fTexCoords).rgb;
    diffuse *= texture(diffuseTexture, fTexCoords).rgb;
    specular *= texture(specularTexture, fTexCoords).rgb;

    // ------------------------------
    // SHADOW CALCULATIONS
    float shadow;
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	float currentDepth = normalizedCoords.z;

    float bias = max(0.05 * (1.0 - dot(normalEye, lightDirN)), 0.005);
    shadow = (currentDepth-bias) > closestDepth ? 1.0f : 0.0f;
	if (normalizedCoords.z > 1.0f)
		shadow = 0.0f;
    // ------------------------------

    vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
    //vec3 color = min((ambient + diffuse) + specular, 1.0f);

    if (applyFog) {
        float fogFactor = computeFog();
        vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
        color = mix(fogColor.rgb, color, fogFactor);
    }

    fColor = vec4(color, 1.0f);
}
