#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec4 fragPosLightSpace;
in vec2 fTexCoords;

out vec4 fColor;

// ------------------------------
// Uniforms for Directional Light
uniform vec3 lightDir;
uniform vec3 lightColor;

// ------------------------------
// Uniforms for Point Light
uniform vec3 lightPointLoc;
uniform vec3 lightPointColor;

// ------------------------------
// Textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

// Fog Toggle
uniform bool applyFog;

// ------------------------------
// Constants for Lighting
const float ambientStrength = 0.2;
const float specularStrength = 0.5;
const float shininess = 32.0;

// Constants for Point Light
const float pointLightConstant = 1.0;
const float pointLightLinear = 0.09;
const float pointLightQuadratic = 0.032;

// ------------------------------
// Function to Compute Fog
float computeFog()
{
    float fogDensity = 0.05;
    float fragmentDistance = length(fPosEye.xyz);
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
    return clamp(fogFactor, 0.0, 1.0);
}

void main()
{
    vec3 cameraPosEye = vec3(0.0); // Assuming camera is at the origin in eye space

    // Normalize the normal vector
    vec3 normalEye = normalize(fNormal);

    // ------------------------------
    // Directional Light Calculations
    vec3 lightDirN = normalize(lightDir);
    vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);

    vec3 ambientDir = ambientStrength * lightColor;

    float diffDir = max(dot(normalEye, lightDirN), 0.0);
    vec3 diffuseDir = diffDir * lightColor;

    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specDir = pow(max(dot(viewDirN, reflectDir), 0.0), shininess);
    vec3 specularDir = specularStrength * specDir * lightColor;

    vec3 diffuseTex = texture(diffuseTexture, fTexCoords).rgb;
    vec3 specularTex = texture(specularTexture, fTexCoords).rgb;

    ambientDir *= diffuseTex;
    diffuseDir *= diffuseTex;
    specularDir *= specularTex;
    // ------------------------------

    // ------------------------------
    // Point Light Calculations

    // Calculate vector from fragment to point light
    vec3 toLight = lightPointLoc - fPosEye.xyz;
    float distance = length(toLight);
    vec3 lightDirPoint = normalize(toLight);

    vec3 ambientPoint = 1.0f * ambientStrength * lightPointColor;

    float diffPoint = max(dot(normalEye, lightDirPoint), 0.0);
    vec3 diffusePoint = diffPoint * lightPointColor;

    vec3 reflectPoint = reflect(-lightDirPoint, normalEye);
    float specPoint = pow(max(dot(viewDirN, reflectPoint), 0.0), shininess);
    vec3 specularPoint = 5.0f * specularStrength * specPoint * lightPointColor;

    float attenuation = 1.0 / (pointLightConstant + pointLightLinear * distance +
                               pointLightQuadratic * (distance * distance));

    // Apply attenuation to point light components
    ambientPoint *= diffuseTex;
    diffusePoint *= diffuseTex;
    specularPoint *= specularTex;

    ambientPoint *= attenuation;
    diffusePoint *= attenuation;
    specularPoint *= attenuation;
    // ------------------------------

    // ------------------------------
    // Shadow Calculations for Directional Light
    float shadow;
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    normalizedCoords = normalizedCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
    float currentDepth = normalizedCoords.z;

    float bias = max(0.05 * (1.0 - dot(normalEye, lightDirN)), 0.005);
    shadow = (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
    if (normalizedCoords.z > 1.0)
        shadow = 0.0;
    // ------------------------------

    // ------------------------------
    // Combine Lighting Components

    vec3 lightingDir = ambientDir + ((1.0 - shadow) * diffuseDir) + ((1.0 - shadow) * specularDir);
    vec3 lightingPoint = ambientPoint + diffusePoint + specularPoint;
    // Total lighting
    vec3 color = vec3(0.0);
    color += lightingDir + lightingPoint;
    //color += lightingDir + lightingPoint;

    // Clamp color to [0,1] range
    color = min(color, vec3(1.0));
    // ------------------------------

    // ------------------------------
    // Apply Fog if Enabled
    if (applyFog) {
        float fogFactor = computeFog();
        vec4 fogColor = vec4(0.5, 0.5, 0.5, 1.0); // Adjust fog color as needed
        color = mix(fogColor.rgb, color, fogFactor);
    }
    // ------------------------------

    // Final Fragment Color
    fColor = vec4(color, 1.0);
}
