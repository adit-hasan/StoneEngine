#version 450

layout(binding = 0) uniform ModelViewProjectionMatrix
{
    mat4 Model;
    mat4 View;
    mat4 Projection;
} mvp;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColour;

layout(location = 0) out vec3 fragColour;

void main() {
    gl_Position = mvp.Projection * mvp.View * mvp.Model * vec4(inPosition, 0.0, 1.0);
    fragColour = inColour;
}