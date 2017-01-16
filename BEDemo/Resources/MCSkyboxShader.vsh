#version 300 es

layout (location=0) in vec3 position;
out vec3 TexCoords;

uniform mat4 boxViewMatrix;
uniform mat4 boxProjectionMatrix;

void main()
{
    gl_Position = boxProjectionMatrix * boxViewMatrix * vec4(position, 1.0);
    TexCoords = position;
}
