#version 300 es

//varying variables use to pass value between vertex & fragment shader
in lowp vec3 vertexcolor;
in lowp vec2 texturecoord;
in lowp vec3 calculatedNormal;
in lowp vec3 modelPosition;
in lowp vec3 viewPosition;

out lowp vec4 FragColor;

//Android GLSL doesn't support struct
uniform lowp vec3 light_ambient;
uniform lowp vec3 light_diffuse;
uniform lowp vec3 light_specular;
uniform lowp vec3 light_color;
uniform lowp vec3 light_position;

uniform lowp vec3  material_ambient;
uniform lowp vec3  material_diffuse;
uniform lowp vec3  material_specular;
uniform lowp float material_dissolve;
uniform lowp float material_shininess;

//texture sampling must in fragment shader
uniform sampler2D texsampler;

void main()
{
    lowp vec3 lightDir = normalize(light_position - modelPosition);
    
    //Ambient Light
    lowp vec3 ambient = (light_ambient * light_color) * material_ambient;
    
    //Diffuse Light
    lowp float diffuseDot = max(0.0, dot(normalize(calculatedNormal), normalize(light_position)));
    lowp vec3  diffuse    = (light_diffuse * light_color) * (diffuseDot * material_diffuse);
    
    //Specular Light
    lowp vec3 viewDir  = normalize(viewPosition - modelPosition);
    lowp vec3 reflectDir = reflect(-lightDir, calculatedNormal);
    
    lowp float specularDot = max(0.0, dot(viewDir, reflectDir));
    lowp float specExp = pow(specularDot, material_shininess);//32 material.shininess
    
    lowp vec3 specular = (light_specular * light_color) * (specExp * material_specular);
    
    //Color Output
    FragColor = vec4((ambient + diffuse + specular) * vertexcolor, material_dissolve);
}
