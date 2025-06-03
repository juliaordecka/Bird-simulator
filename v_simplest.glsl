uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

// Atrybuty - dopasowane do tego co przekazuje kod C++
in vec4 vertex;     // location 0
in vec4 normal;     // location 1  
in vec4 texCoord;   // location 2 - jako vec4, nie vec2!

// Zmienne interpolowane
out vec4 l;
out vec4 n;
out vec4 v;
out vec2 iTexCoord0; 
out vec2 iTexCoord1;

void main(void) {
    vec4 lp = vec4(0, 0, -6, 1); // pozycja światła, przestrzeń świata
    l = normalize(V * lp - V * M * vertex); // wektor do światła w przestrzeni oka
    v = normalize(vec4(0, 0, 0, 1) - V * M * vertex); // wektor do obserwatora w przestrzeni oka
    n = normalize(V * M * normal); // wektor normalny w przestrzeni oka
    
    // Używamy tylko x,y ze współrzędnych tekstury (vec4 -> vec2)
    iTexCoord0 = texCoord.xy;
    iTexCoord1 = (n.xy + 1) / 2;
    
    gl_Position = P * V * M * vertex;
}