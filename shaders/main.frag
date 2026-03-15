#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in float fragLight;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D texSampler;

layout(set = 0, binding = 1) uniform UniformBufferObject {
    vec3 globalLightDir;
    float ambientStrength;
} ubo;

void main() {
    vec4 texColor = texture(texSampler, fragTexCoord);
    if (texColor.a < 0.1) discard;
    
    // Simple directional light simulation
    float diffuse = max(dot(vec3(0.0, 1.0, 0.0), -ubo.globalLightDir), ubo.ambientStrength);

    outColor = vec4(texColor.rgb * fragLight * diffuse, texColor.a);
}
