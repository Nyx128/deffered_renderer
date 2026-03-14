#version 460 core
// Matches C++ VertexGPU
struct Vertex {
    vec4 pos;
    vec4 normal;
    vec4 uv;
};

// Vertex Data
layout(std430, binding = 0) readonly buffer VertexBuffer {
    Vertex vertices[];
};

// Index Buffer
layout(std430, binding = 1) readonly buffer IndexBuffer {
    uint indices[];
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    // Index Pulling
    uint vertexIndex = indices[gl_VertexID];
    
    // Fetch
    Vertex v = vertices[vertexIndex];

    // Transform
    vec4 worldPos = model * vec4(v.pos.xyz, 1.0);
    FragPos = worldPos.xyz;
    
    // Normal Matrix
    Normal = mat3(transpose(inverse(model))) * v.normal.xyz;
    
    TexCoords = v.uv.xy;

    gl_Position = projection * view * worldPos;
}
