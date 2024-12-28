#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 FragPosLightSpace;

out vec4 fColor;

// Matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

// Directional light
uniform vec3 lightDir;
uniform vec3 lightColor;

// Point light
uniform vec3 lightPointLoc;
uniform vec3 lightPointColor;

// Shadow map
uniform sampler2D shadowMap;

// Textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

// Camera position
uniform vec3 viewPos;

// Components
vec3 ambient;
vec3 diffuse;
vec3 specular;

// Constants
const float shininess = 32.0;

void computeDirLight(vec3 normal, vec3 viewDir, float shadow)
{
    // Transform light direction to eye space
    vec3 lightDirEye = normalize(view * vec4(lightDir, 0.0)).xyz;

    float ambientStrengthDir = 0.2;
    float specularStrengthDir = 0.5; // Adjust as needed

    vec3 ambientDir = ambientStrengthDir * lightColor;

    float diff = max(dot(normal, lightDirEye), 0.0);
    vec3 diffuseDir = diff * lightColor;

    vec3 reflectDir = reflect(-lightDirEye, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specularDir = specularStrengthDir * spec * lightColor;

    // Apply shadow
    diffuseDir *= (1.0 - shadow);
    specularDir *= (1.0 - shadow);

    ambient += ambientDir;
    diffuse += diffuseDir;
    specular += specularDir;
}

void computePointLightFixedSpecular(vec3 normal, vec3 fragPosWorld, float shadow)
{
    vec3 lightPosWorld = lightPointLoc;
    vec3 lightDirWorld = lightPosWorld - fragPosWorld;
    float distance = length(lightDirWorld);
    vec3 lightDirNormalized = normalize(lightDirWorld);

    vec3 lightDirEye = normalize((view * vec4(lightDirNormalized, 0.0)).xyz);

    float attenuation = 1.0 / (distance * distance); // Simple quadratic attenuation

    float ambientStrengthPoint = 0.1;
    vec3 ambientPoint = ambientStrengthPoint * lightPointColor;

    float diff = max(dot(normal, lightDirEye), 0.0);
    vec3 diffusePoint = diff * lightPointColor;

    vec3 fixedViewDir = vec3(0.0, 0.0, 1.0); // Adjust as needed

    vec3 reflectDir = reflect(-lightDirEye, normal);
    float spec = pow(max(dot(fixedViewDir, reflectDir), 0.0), shininess);
    vec3 specularPoint = spec * lightPointColor; // You can multiply by a specular strength if desired

    ambientPoint *= attenuation;
    diffusePoint *= attenuation;
    specularPoint *= attenuation;

    diffusePoint *= (1.0 - shadow);
    specularPoint *= (1.0 - shadow);

    ambient += ambientPoint;
    diffuse += diffusePoint;
    specular += specularPoint;
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    // Transform fragment position to eye space
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 fragPosEye = fPosEye.xyz;

    // Transform fragment position to world space for point light calculation
    vec4 fPosWorld = model * vec4(fPosition, 1.0f);
    vec3 fragPosWorld = fPosWorld.xyz;

    // Transform normal to eye space
    vec3 normalEye = normalize(normalMatrix * fNormal);

    // View direction in eye space (assuming camera is at origin in eye space)
    vec3 viewDir = normalize(-fragPosEye);

    // Initialize lighting components
    ambient = vec3(0.0);
    diffuse = vec3(0.0);
    specular = vec3(0.0);

    // Calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace);

    // Compute lighting with shadow
    computeDirLight(normalEye, viewDir, shadow);
    computePointLightFixedSpecular(normalEye, fragPosWorld, shadow);

    // Combine lighting with textures
    vec3 diffuseTex = texture(diffuseTexture, fTexCoords).rgb;
    vec3 specularTex = texture(specularTexture, fTexCoords).rgb;

    // Final color calculation
    vec3 finalColor = (ambient + diffuse) * diffuseTex + specular * specularTex;
    finalColor = clamp(finalColor, 0.0, 1.0);

    fColor = vec4(finalColor, 1.0f);
}
