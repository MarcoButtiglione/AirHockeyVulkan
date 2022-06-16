#version 450

layout(set = 0, binding = 0) uniform globalUniformBufferObject {
	mat4 view;
	mat4 proj;
	vec3 lightDir;
	vec3 lightPos;
	vec3 lightColor;
	vec4 lightParams;
	vec3 eyePos;
} gubo;

layout(set=1, binding = 1) uniform sampler2D texSampler;


layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

/*
• vec3 lightDir -> the direction of the light.
• vec3 lightPos -> the position of the light.
• vec3 lightColor -> the basic color of the light.
• vec4 lightParams -> the other parameters of the spot light.
• vec3 eyePos -> the position of the viewer.
The members of lightParams are the following:
• lightParams.x -> a float component containing the cosine of the inner angle.
• lightParams.y -> a float component containing the cosine of the outer angle.
• lightParams.z -> a float component containing the denominator exponent b.
• lightParams.w -> a float component containing the basic distance g.
*/
vec3 AmbientLighting (vec3 la, vec3 ma){
	/*
	AmbientLighting = Lamb

	la = light ambient color
	ma = ambient light reflection color
	*/
	vec3 Lamb = la * ma;
	return Lamb;
}
vec3 LambertReflection( vec3 md, vec3 lx, vec3 nx){
	/*
	LambertReflection = fDiffuse

	md = diffuse color of the object
	lx = light direction
	nx = unitary normal vector to the surface (normalize(fragNorm))

	*/
	vec3 fDiffuse =md*max(dot(lx,nx),0.0f);
	return fDiffuse;
}
vec3 PhongReflection(vec3 ms, vec3 wr, vec3 lx, vec3 nx, float gamma)  {
	/*
	PhongReflection = fSpecular

	ms = specular color of the object
	wr = direction from which the object is being observed (vec3 EyeDir = normalize(gubo.eyePos.xyz - fragPos);)
	lx = light direction
	nx = unitary normal vector to the surface (normalize(fragNorm))
	gamma = specular power, diffusion of the highlight regions (greater is gamma,
	smaller is the highlight, and more shiny the object appears to be)
	*/
	
	vec3 fSpecular = ms * pow(clamp(dot(wr,-reflect(lx,nx)),0.0f,1.0f),gamma);
	return fSpecular;
}

vec3 spot_light_color(vec3 l, float g ,vec3 p, vec3 x,float beta, vec3 d, float cOut, float cIn) {
	/*
	spot_light_color = L(l,lx)

	l = light color
	g = target distance (the distance at which the light reduction is exactly 1)
	p = light position
	x = the rendered point (fragPos)
	beta = decay factor
	d = direction of the spot light
	cOut = outer cones
	cIn = inner cones
	*/

	vec3 L = l*pow(float(g/length(p-x)),float(beta))*clamp((dot(normalize(p-x),d)-cOut)/(cIn-cOut),0.0f,1.0f) ;

	return L;
}
vec3 spot_light_dir(vec3 p, vec3 x) {
	/*
	spot_light_dir = lx

	p = light position
	x = the rendered point (fragPos)
	*/

	vec3 lx =normalize(p-x);
	return lx ;
}


void main() {
	vec3 Norm = normalize(fragNorm);

	vec3 EyeDir = normalize(gubo.eyePos.xyz - fragPos);
	//Diffuse color
	vec3 DifCol = texture(texSampler, fragTexCoord).rgb;
	//Specular color
	vec3 SpecCol = vec3(1.0f,1.0f,1.0f);
	//Ambient color
	vec3 AmbCol = texture(texSampler, fragTexCoord).rgb;
	//Specular power 
	float gamma =  10.0f;

	//Light model
	vec3 Lspot1 = spot_light_color(
		gubo.lightColor,
		gubo.lightParams.w,
		gubo.lightPos,
		fragPos,
		gubo.lightParams.z,
		gubo.lightDir,
		gubo.lightParams.y,
		gubo.lightParams.x
	);
	vec3 Lspot2 = spot_light_color(
		gubo.lightColor,
		gubo.lightParams.w,
		gubo.lightPos+vec3(-0.55f,0.0f,0.0f),
		fragPos,
		gubo.lightParams.z,
		gubo.lightDir,
		gubo.lightParams.y,
		gubo.lightParams.x
	);
	vec3 Lspot3 = spot_light_color(
		gubo.lightColor,
		gubo.lightParams.w,
		gubo.lightPos+vec3(0.55f,0.0f,0.0f),
		fragPos,
		gubo.lightParams.z,
		gubo.lightDir,
		gubo.lightParams.y,
		gubo.lightParams.x
	);

	//Light direction
	vec3 lx1 = spot_light_dir(
		gubo.lightPos,
		fragPos
	);
	vec3 lx2 = spot_light_dir(
		gubo.lightPos+vec3(-0.55f,0.0f,0.0f),
		fragPos
	);
	vec3 lx3 = spot_light_dir(
		gubo.lightPos+vec3(0.55f,0.0f,0.0f),
		fragPos
	);
	

	//BRDF functions
	vec3 fDiffuse1 = LambertReflection(
		DifCol,
		lx1,
		Norm
	);
	vec3 fDiffuse2 = LambertReflection(
		DifCol,
		lx2,
		Norm
	);
	vec3 fDiffuse3 = LambertReflection(
		DifCol,
		lx3,
		Norm
	);
	
	vec3 fSpecular1 = PhongReflection(
		SpecCol,
		EyeDir,
		lx1,
		Norm,
		gamma
	);
	vec3 fSpecular2 = PhongReflection(
		SpecCol,
		EyeDir,
		lx2,
		Norm,
		gamma
	);
	
	vec3 fSpecular3 = PhongReflection(
		SpecCol,
		EyeDir,
		lx3,
		Norm,
		gamma
	);
	
	//AmbientLighting
	vec3 Lamb = AmbientLighting(
		gubo.lightColor*0.2f,
		AmbCol
	);

	//Rendering equation
	vec3 L = (Lspot1*(fDiffuse1+fSpecular1)+Lspot2*(fDiffuse2+fSpecular2)+Lspot3*(fDiffuse3+fSpecular3))+Lamb;


	outColor = vec4(clamp(L,vec3(0.0f), vec3(1.0f)), 1.0f);	
}

