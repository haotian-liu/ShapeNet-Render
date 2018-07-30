#version 330

uniform bool selected;
uniform vec3 lightDirection;
uniform float lightDistance;
uniform sampler2D textureSampler;
uniform bool hasTexture;

uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;

in vec3 worldCoord;
in vec3 eyeCoord;
in vec3 Normal;
in vec2 texCoord;

out vec4 FragColor;

void main() {
    float Shininess = Ns;

    vec3 color = texture(textureSampler, vec2(texCoord.x, 1 - texCoord.y)).rgb;
//    vec3 color = vec3(texture(textureSampler, texCoord));

    vec3 KaColor = Ka;
    vec3 KdColor = hasTexture ? color * Kd : Kd;
    vec3 KsColor = Ks;

    vec3 N = Normal;
    vec3 L = normalize(lightDirection - worldCoord);
    vec3 R = reflect(-L, N);
    vec3 E = normalize(eyeCoord);

    float NdotL = abs(dot(N, L));
    float EdotR = dot(-E, R);

    float diffuse = max(NdotL, 0.f) / lightDistance * 1.05;
    float specular = max(pow(EdotR, Shininess), 0.f) / lightDistance / 5;

    vec3 combined = vec3(KaColor + KdColor * diffuse + KsColor * specular);
//    combined = KdColor;
//    FragColor = vec4(selected ? vec3(combined.x, combined.yz + vec2(0.3f)) : combined, 1.f);
    FragColor = vec4(combined, 1.f);
}
