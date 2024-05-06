#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragPosition;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

#define     MAX_LIGHTS              4
#define     LIGHT_DIRECTIONAL       0
#define     LIGHT_POINT             1

struct MaterialProperty {
    vec3 color;
    int useSampler;
    sampler2D sampler;
};

struct Light {
    int enabled;
    int type;
    vec3 position;
    vec3 target;
    vec4 color;
};

// Input lighting values
uniform Light lights[MAX_LIGHTS];
uniform vec4 ambient;
uniform vec3 viewPos;
uniform float fogDensity;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec3 lightDot = vec3(0.0);
    vec3 normal = normalize(fragNormal);
    vec3 viewD = normalize(viewPos - fragPosition);
    vec3 specular = vec3(0.0);

    // Cartoonish shading: Simplify the lighting to create flat areas of color
    float NdotL = dot(normal, -normalize(viewPos - fragPosition));
    NdotL = clamp(NdotL, 0.2, 1.0); // Clamp to create a more stylized look
    lightDot = vec3(NdotL);

    // Simplified specular highlights
    float specCo = pow(max(0.0, dot(viewD, reflect(-viewD, normal))), 12.0);
    specular = vec3(specCo);

    finalColor = (texelColor * (colDiffuse * vec4(lightDot, 1.0)));
    finalColor += texelColor * (ambient / 5.0); // Increase ambient for a softer look

    finalColor = pow(finalColor, vec4(1.0 / 2.2));

    float dist = length(viewPos - fragPosition);
    const vec4 fogColor = vec4(0.5, 0.4, 0.4, 1.0);
    const float fogDensity = 0.008;

    float fogFactor = 1.0 / exp((dist * fogDensity) * (dist * fogDensity));
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    finalColor = mix(fogColor, finalColor, fogFactor);
}
