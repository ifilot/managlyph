#version 330 core

in vec3 vertex_direction_eyespace;
in vec3 lightdirection_eyespace;
in vec3 normal_eyespace;

uniform vec4  color;
uniform vec3  light_color;
uniform float diffuse_strength;
uniform float ambient_strength;
uniform float specular_strength;
uniform float shininess;

out vec4 fragColor;

void main()
{
    vec3 N = normalize(normal_eyespace);
    vec3 L = normalize(lightdirection_eyespace);
    vec3 V = normalize(vertex_direction_eyespace);

    // --- Ambient ---
    vec3 ambient = ambient_strength * light_color;

    // --- Diffuse ---
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = diffuse_strength * NdotL * light_color;

    // --- Specular ---
    vec3 H = normalize(L + V);
    float NdotH = max(dot(N, H), 0.0);
    float spec = pow(NdotH, shininess);
    vec3 specular = specular_strength * spec * light_color * (1.0 - color.rgb);

    // Combine lighting
    vec3 result = (ambient + diffuse) * color.rgb + specular;

    // ================= EDGE DARKENING =================
    const float edge_strength = 2;  // darkness amount
    const float edge_power    = 3.0;   // edge width/softness

    float rim = 1.0 - max(dot(N, V), 0.0);
    rim = pow(rim, edge_power);

    result *= (1.0 - rim * edge_strength);
    // ==================================================

    // Gamma correction
    result = pow(result, vec3(1.0/2.2));

    fragColor = vec4(result, color.a);
}
