#version 450

layout(set = 0, binding = 0) uniform globalUniformBufferObject {
	mat4 view;
	mat4 proj;
	//aggiunte per la luce
	vec3 lightDir;
	vec3 lightPos;
	vec3 lightColor;
	vec4 lightParams;
	vec3 eyePos;
} gubo;

layout(set=1, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragViewDir;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

vec3 AmbientLighting (vec3 la, vec3 ma){
	vec3 Lamb = la * ma;
	return Lamb;
}
vec3 BlinnReflection (vec3 ms, vec3 nx,vec3 lx,vec3 wr,  float gamma){
	vec3 fSpecular = ms * pow(clamp(dot(nx,normalize(lx+wr)),0.0f,1.0f),gamma);
	return fSpecular;
}

void main() {
  //PARTE COMMENTATA MA NECESSARIA

	/*const vec3  diffColor = texture(texSampler, fragTexCoord).rgb;
	const vec3  specColor = vec3(1.0f, 1.0f, 1.0f);
	const float specPower = 150.0f;
	const vec3  L = vec3(-0.4830f, 0.8365f, -0.2588f);

	vec3 N = normalize(fragNorm);
	vec3 R = -reflect(L, N);
	vec3 V = normalize(fragViewDir);

	// Lambert diffuse
	vec3 diffuse  = diffColor * max(dot(N,L), 0.0f);
	// Phong specular
	vec3 specular = specColor * pow(max(dot(R,V), 0.0f), specPower);
	// Hemispheric ambient
	vec3 ambient  = (vec3(0.1f,0.1f, 0.1f) * (1.0f + N.y) + vec3(0.0f,0.0f, 0.1f) * (1.0f - N.y)) * diffColor;

	outColor = vec4(clamp(ambient + diffuse + specular, vec3(0.0f), vec3(1.0f)), 1.0f);*/
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos.xyz - fragViewDir);
	//Diffuse color
	vec3 AmbCol = texture(texSampler, fragTexCoord).rgb;
	//Specular color
	vec3 SpecCol = texture(texSampler, fragTexCoord).rgb;
	//Specular power
	float gamma = texture(texSampler, fragTexCoord).a * 200.0f;

	//Light model
	vec3 L_directional = gubo.lightColor;
	vec3 lx_directional = gubo.lightDir;

	//AmbientLighting
	vec3 Lamb = AmbientLighting(
		gubo.lightColor,
		AmbCol
	);

	//BRDF functions
	vec3 fSpecular = BlinnReflection(
		SpecCol,
		Norm,
		lx_directional,
		EyeDir,
		gamma
	);

	//Rendering equation
	vec3 L = L_directional*(fSpecular)+Lamb;


	outColor = vec4(clamp(L,vec3(0.0f), vec3(1.0f)), 1.0f);
}
