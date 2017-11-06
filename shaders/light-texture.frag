#version 330

#define MAX_LIGHTS 5

smooth in vec4 vertex;
smooth in vec3 normal;
smooth in vec2 uv;

out vec4 fragColour;

uniform struct Light {
    vec4 position;
    vec3 ambient;    
    vec3 diffuse;   
    vec3 specular;    
    vec3 coneDirection;
    float coneAngle;    
    float attenuation;    
    float intensity;
} lights[MAX_LIGHTS];

uniform int numLights;

uniform vec3 mtl_specular; 
uniform float shininess; 

uniform sampler2D texMap;

//Implements blinn phong lighting
//Has the ability to do point lights, directional lights and spotlights
vec3 phongPointLight(Light light, in vec3 appliedTexture, in vec4 position, in vec3 norm)
{
    if(light.coneAngle == 0.0) { // No lighting wanted since angle is 0
       // return vec3(1, 1, 1);
    }
    vec3 lightDir;
    float attenuation = 1.0;

    if(light.position.w == 0) { //For directional lights
        lightDir = normalize(-light.coneDirection);
        attenuation = 1.0;
    } 
    else { //Spotlights
        lightDir = normalize(vec3(light.position - position));

        float distanceToLight = length(light.position.xyz - position.xyz);
        attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));
        float lightToSurfaceAngle = degrees(acos(dot(-lightDir, light.coneDirection)));
        if(lightToSurfaceAngle > light.coneAngle){
            attenuation = 0.05;
        }
    }

    //Calculate lighting 
    float lambertian = max(dot(lightDir, norm), 0.0);
    float spec = 0.0;

    if(lambertian > 0.0) {
        vec3 viewDir = normalize(vec3(position));
        vec3 reflectDir = reflect(-lightDir, norm);  
        //vec3 halfwayDir = normalize(lightDir + viewDir);
        float specAngle = max(dot(viewDir, reflectDir), 0.0);
        spec = pow(specAngle, shininess);
    }

    vec3 ambient = light.ambient * light.intensity * appliedTexture;
    vec3 diffuse = lambertian * appliedTexture * light.diffuse;
    vec3 specular = spec * light.specular * mtl_specular;
    return ambient + (diffuse + specular) * attenuation;
}

void main(void) 
{
    vec3 lightCalculation = vec3(0);
    vec4 appliedTexture = texture(texMap, uv);  

    for(int i = 0; i < numLights; i++) {
        lightCalculation += phongPointLight(lights[i], appliedTexture.rgb, vertex, normalize(normal));
    }
    vec3 gamma = vec3(1.0/1.0);
    fragColour = vec4(pow(lightCalculation, gamma), appliedTexture.a);
}
