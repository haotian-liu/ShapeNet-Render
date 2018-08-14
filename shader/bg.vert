#version 330

in vec2 xy;
out vec2 uv;

void main() {
    uv = xy / 2 + vec2(.5f);
    gl_Position = vec4(xy, .5f, 1.f);
}