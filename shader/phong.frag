#version 330

uniform vec3 LightDirection;
uniform sampler2D textureSampler;

in vec3 worldCoord;
in vec3 eyeCoord;
in vec3 Normal;
in vec2 texCoord;

out vec4 FragColor;

void main() {
    float Shininess = 1.f;
    float Strength = 1.f;

    vec3 color = vec3(texture(textureSampler, vec2(texCoord.x, 1.f - texCoord.y)));
//    vec3 color = vec3(texture(textureSampler, texCoord));

    vec3 KaColor = vec3(color);
    vec3 KdColor = vec3(0.8f);
    vec3 KsColor = vec3(0.8f);

    vec3 N = Normal;
    vec3 L = normalize(LightDirection - worldCoord);
    vec3 R = reflect(-L, N);
    vec3 E = normalize(eyeCoord);

    float NdotL = abs(dot(N, L));
    float EdotR = dot(-E, R);

    float diffuse = max(NdotL, 0.f);
    //float specular = max(pow(EdotR, Shininess), 0.f);
    float specular = 0.f;

//    FragColor = vec4(vec3(0.f), 1.f);
    FragColor = vec4(KaColor + KdColor * diffuse + KsColor * specular, 1.f);
}
