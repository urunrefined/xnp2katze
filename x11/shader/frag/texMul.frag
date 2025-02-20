#version 450 core

layout(binding = 2) uniform sampler2D texSampler [16];

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

layout(location = 1) flat in uint texIndice;

void main() {
    outColor = texture(texSampler[texIndice], fragTexCoord);
}
