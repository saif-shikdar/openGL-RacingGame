uniform sample2D sampler0;
in vec2 vTexCoord;
in vec3 n;
in vec4 p;
float rho = 0.01f;
vec3 fogColour = vec3(0.75f);

void main() {

	vec3 vColour = PhongModel(p, normalize(n));
	vec4 vTexColour = texture(sampler0, vTexCoord);
	vOutputColour = vTextColour*vec4(vColour, 1.0f);

	float d = length(p.xyz);
	float w = exp(-rho*d);
	vOutputColour.rgb = mix(fogColour, vOutputColour.rgb, w);
}