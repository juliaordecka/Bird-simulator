#version 330

uniform mat4 P;
uniform mat4 V;

layout(location = 0) in vec3 vertex;

out vec3 worldPos;

void main(void) {
    mat4 rotView = mat4(mat3(V));
    vec4 pos = P * rotView * vec4(vertex, 1.0);
    gl_Position = pos.xyww;
    worldPos = vertex;
}