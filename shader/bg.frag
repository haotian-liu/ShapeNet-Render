#version 330

uniform sampler2D textureSampler;

in vec2 uv;
out vec4 FragColor;

void main() {
    FragColor = vec4(
        texture(textureSampler, vec2(uv.x, 1 - uv.y)).rgb,
        1.f
    );
}
