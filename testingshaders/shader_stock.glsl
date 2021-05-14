#version 330

uniform vec2 iResolution;
uniform float iTime;

in vec2 fragCoord;
out vec3 fragColor;

void main()
{
	vec2 uv = vec2(fragCoord.x * iResolution.y / iResolution.x, fragCoord.y);
	fragColor = 0.5 + 0.5 * cos(iTime + uv.xyx + vec3(0, 2, 4));
	
}
