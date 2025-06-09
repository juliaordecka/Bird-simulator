#version 330

in vec3 worldPos;
out vec4 pixelColor;

void main(void) {
    vec3 dir = normalize(worldPos);
    float skyGradient = (dir.y + 1.0) * 0.5;
    
    vec3 skyColor = mix(
        vec3(0.5, 0.7, 1.0),
        vec3(0.1, 0.3, 0.8),
        skyGradient
    );
    
    pixelColor = vec4(skyColor, 1.0);
}