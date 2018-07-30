#version 330

uniform vec3 shapeOffset;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projMatrix;

in vec3 vertPos;
in vec3 vertNormal;

in vec2 vertUV;
out vec2 texCoord = vertUV;

out vec3 worldCoord;
out vec3 eyeCoord;
out vec3 Normal;

void main() {
    vec4 position = vec4(vertPos + shapeOffset, 1.0f);

    vec4 worldPos = modelMatrix * position;
    vec4 eyePos = viewMatrix * worldPos;
    vec4 clipPos = projMatrix * eyePos;

    worldCoord = worldPos.xyz;
    eyeCoord = eyePos.xyz;

    gl_Position = clipPos;
//    gl_Position = projMatrix * modelMatrix * position;

	Normal = normalize(mat3(viewMatrix * modelMatrix) * vertNormal);
}
