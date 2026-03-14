#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main()
{    
    // 1. Store the fragment position vector in the first G-Buffer texture
    gPosition = FragPos;
    
    // 2. Store the per-fragment normal into the G-Buffer
    gNormal = normalize(Normal);
    
    // 3. Store the color (Albedo)
    // Hardcoded for now, or use texture if available
    gAlbedo.rgb = vec3(1.0, 0.5, 0.2); // Orange
    gAlbedo.a = 1.0;
}
