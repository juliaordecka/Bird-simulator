#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

// Atrybuty - dok³adnie jak w oryginalnym
in vec4 vertex;     // location 0
in vec4 normal;     // location 1  
in vec4 texCoord;   // location 2

// Zmienne interpolowane
out vec2 iTexCoord0; 

void main(void) {
    // Przeka¿ wspó³rzêdne tekstury
    iTexCoord0 = texCoord.xy;
    
    // Standardowa transformacja wierzcho³ka
    gl_Position = P * V * M * vertex;
}