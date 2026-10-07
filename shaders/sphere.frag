#version 460 core

in vec3 worldNormal;
layout(location = 0) out vec4 fragmentColor;

void main()
{
    vec3 N = normalize(worldNormal);
    vec3 L = normalize(vec3(1.0, 1.0, 1.0));

    float diffuse = max(dot(N, L), 0.0);
    vec3 baseColor = vec3(0.2, 0.7, 0.9);
    fragmentColor = vec4(baseColor * (0.2 + 0.8 * diffuse), 1.0);
}