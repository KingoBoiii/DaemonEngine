#type vertex
#version 450 core
layout (location = 0) in vec3 POSITION;
layout (location = 1) in vec4 COLOR;
layout (location = 2) in vec2 TEXCOORD;
layout (location = 3) in float TEXTUREID;

layout (std140, binding = 0) uniform Camera {
	mat4 ProjectionView;
};

out VertexData {
	vec3 Position;
	vec4 Color;
	vec2 TexCoord;
	float TexIndex;
} VS_OUT;

void main() {
	VS_OUT.Position = POSITION;
	VS_OUT.Color = COLOR;
	VS_OUT.TexCoord = TEXCOORD;
	VS_OUT.TexIndex = TEXTUREID;

	gl_Position = ProjectionView * vec4(POSITION, 1.0);
}


#type fragment
#version 450 core
layout (location = 0) out vec4 FragColor;

in VertexData {
	vec3 Position;
	vec4 Color;
	vec2 TexCoord;
	float TexIndex;
} FS_IN;

uniform sampler2D u_Textures[32];

void main() {
	vec4 texColor = FS_IN.Color;

	switch(int(round(FS_IN.TexIndex)))
	{
		case  0: texColor *= texture(u_Textures[ 0], FS_IN.TexCoord); break;
		case  1: texColor *= texture(u_Textures[ 1], FS_IN.TexCoord); break;
		case  2: texColor *= texture(u_Textures[ 2], FS_IN.TexCoord); break;
		case  3: texColor *= texture(u_Textures[ 3], FS_IN.TexCoord); break;
		case  4: texColor *= texture(u_Textures[ 4], FS_IN.TexCoord); break;
		case  5: texColor *= texture(u_Textures[ 5], FS_IN.TexCoord); break;
		case  6: texColor *= texture(u_Textures[ 6], FS_IN.TexCoord); break;
		case  7: texColor *= texture(u_Textures[ 7], FS_IN.TexCoord); break;
		case  8: texColor *= texture(u_Textures[ 8], FS_IN.TexCoord); break;
		case  9: texColor *= texture(u_Textures[ 9], FS_IN.TexCoord); break;
		case 10: texColor *= texture(u_Textures[10], FS_IN.TexCoord); break;
		case 11: texColor *= texture(u_Textures[11], FS_IN.TexCoord); break;
		case 12: texColor *= texture(u_Textures[12], FS_IN.TexCoord); break;
		case 13: texColor *= texture(u_Textures[13], FS_IN.TexCoord); break;
		case 14: texColor *= texture(u_Textures[14], FS_IN.TexCoord); break;
		case 15: texColor *= texture(u_Textures[15], FS_IN.TexCoord); break;
		case 16: texColor *= texture(u_Textures[16], FS_IN.TexCoord); break;
		case 17: texColor *= texture(u_Textures[17], FS_IN.TexCoord); break;
		case 18: texColor *= texture(u_Textures[18], FS_IN.TexCoord); break;
		case 19: texColor *= texture(u_Textures[19], FS_IN.TexCoord); break;
		case 20: texColor *= texture(u_Textures[20], FS_IN.TexCoord); break;
		case 21: texColor *= texture(u_Textures[21], FS_IN.TexCoord); break;
		case 22: texColor *= texture(u_Textures[22], FS_IN.TexCoord); break;
		case 23: texColor *= texture(u_Textures[23], FS_IN.TexCoord); break;
		case 24: texColor *= texture(u_Textures[24], FS_IN.TexCoord); break;
		case 25: texColor *= texture(u_Textures[25], FS_IN.TexCoord); break;
		case 26: texColor *= texture(u_Textures[26], FS_IN.TexCoord); break;
		case 27: texColor *= texture(u_Textures[27], FS_IN.TexCoord); break;
		case 28: texColor *= texture(u_Textures[28], FS_IN.TexCoord); break;
		case 29: texColor *= texture(u_Textures[29], FS_IN.TexCoord); break;
		case 30: texColor *= texture(u_Textures[30], FS_IN.TexCoord); break;
		case 31: texColor *= texture(u_Textures[31], FS_IN.TexCoord); break;
	}

	FragColor = texColor;
}


