#version 330

uniform sampler2D textureMap0; 

// Zmienne interpolowane
in vec2 iTexCoord0;

// Zmienna wyjœciowa
out vec4 pixelColor;

void main(void) {
    // Pobierz kolor z tekstury - u¿ywaj texture() nie texture2D()
    vec4 textureColor = texture(textureMap0, iTexCoord0);
    
    // SprawdŸ czy tekstura siê za³adowa³a
    if (textureColor.a < 0.1) {
        // Fallback - jeœli brak tekstury, u¿yj zielonego koloru
        pixelColor = vec4(0.2, 0.8, 0.3, 1.0);
    } else {
        // Lekko rozjaœnij teksturê
        pixelColor = vec4(textureColor.rgb * 1.1, 1.0);
    }
}