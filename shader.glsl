#version 330

uniform vec2 iResolution;
uniform float iTime;

in vec2 fragCoord;
out vec3 fragColor;

// Returns 1.0 when the uv is inside the quad
// Parameter pos indicates the center of the rect
float rect(vec2 uv, vec2 pos, vec2 size)
{
    return 1.0 - clamp(length(max(abs(uv - pos)-size, 0.0))*800.0, 0.0, 1.0);
}

void main()
{
    // Let's work with Y=0 on top and a space [0.0, 1.0]
    vec2 uv = fragCoord.xy;
    uv.y = 1.0 - uv.y;
    
    // Correct Aspect Ration    
    uv.x *= iResolution.x / iResolution.y;
    vec3 col = vec3(0.0);
    
    // Calculate rectangles    
    float i = floor(uv.x / 0.3);
    float j = floor(uv.y / 0.35);   
    
    // Calculate if the pixel belongs to the rect or not    
    vec2 center    = vec2(0.14, 0.16) + vec2(0.3, 0.33) * vec2(i,j);
    vec2 size1     = vec2(0.12);
    vec2 size2     = max(vec2(0.02,0.02), size1 * 0.9 * (0.5 + 0.5 * sin(iTime * 0.2 * center.x + center.x)));
    float rect1    = rect(uv, center, size1);
    float rect2    = rect(uv, center, size2);
    
    // Calculate the final color
    vec3  col1     = 0.5 + 0.5*cos(iTime * 0.1 + 5.5*(center.x+center.y) + vec3(1.57, 0.0, 3.14) );
    vec3  col2     = 0.5 + 0.5*cos(iTime * 0.5  + 5.5*(center.x) + vec3(1.57, 0.0, 3.14) );
    col += mix(col1 * (rect1), col2, (rect1 * rect2));
    
    // Add background color
    col += step(col, vec3(0.0)) * vec3(0.9);
    
    // Output the color to screen
    fragColor = col;
}