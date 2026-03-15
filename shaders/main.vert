#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in float inLight;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out float fragLight;

layout(push_constant) uniform PushConstants {
    mat4 model;
    mat4 view;
    mat4 proj;
} push;

void main() {
    gl_Position = push.proj * push.view * push.model * vec4(inPosition, 1.0);
    fragTexCoord = inTexCoord;
    fragLight = inLight;
}
