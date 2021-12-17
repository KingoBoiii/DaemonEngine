struct VS_INPUT
{
	float3 Position : POSITION;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
	float TextureIndex : TEXTUREID;
};

struct VS_OUTPUT
{
	float4 PositionCS : SV_POSITION;
	float4 Position : POSITION; 
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
	float TextureIndex : TEXTUREID;
};

cbuffer VS_MATRICES : register(b0)
{
	float4x4 ProjectionView;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
	float4 pos = float4(input.Position, 1.0);

	VS_OUTPUT output = (VS_OUTPUT)0;
	output.PositionCS = mul(ProjectionView, pos);
	output.Position = pos;
	output.Color = input.Color;
	output.TexCoord = input.TexCoord;
	output.TextureIndex = input.TextureIndex;
	return output;
}

Texture2D Textures[32] : register(t0);
SamplerState Samplers[32] : register(s0);

float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
	float4 color = input.Color;
	int textureId = (int)input.TextureIndex;
	switch(textureId) 
	{
		case  0: color *= Textures[ 0].Sample(Samplers[ 0], input.TexCoord); break;
		case  1: color *= Textures[ 1].Sample(Samplers[ 1], input.TexCoord); break;
		case  2: color *= Textures[ 2].Sample(Samplers[ 2], input.TexCoord); break;
		case  3: color *= Textures[ 3].Sample(Samplers[ 3], input.TexCoord); break;
		case  4: color *= Textures[ 4].Sample(Samplers[ 4], input.TexCoord); break;
		case  5: color *= Textures[ 5].Sample(Samplers[ 5], input.TexCoord); break;
		case  6: color *= Textures[ 6].Sample(Samplers[ 6], input.TexCoord); break;
		case  7: color *= Textures[ 7].Sample(Samplers[ 7], input.TexCoord); break;
		case  8: color *= Textures[ 8].Sample(Samplers[ 8], input.TexCoord); break;
		case  9: color *= Textures[ 9].Sample(Samplers[ 9], input.TexCoord); break;
		case 10: color *= Textures[10].Sample(Samplers[10], input.TexCoord); break;
		case 11: color *= Textures[11].Sample(Samplers[11], input.TexCoord); break;
		case 12: color *= Textures[12].Sample(Samplers[12], input.TexCoord); break;
		case 13: color *= Textures[13].Sample(Samplers[13], input.TexCoord); break;
		case 14: color *= Textures[14].Sample(Samplers[14], input.TexCoord); break;
		case 15: color *= Textures[15].Sample(Samplers[15], input.TexCoord); break;
		//case 16: color *= Textures[16].Sample(Samplers[16], input.TexCoord); break;
		//case 17: color *= Textures[17].Sample(Samplers[17], input.TexCoord); break;
		//case 18: color *= Textures[18].Sample(Samplers[18], input.TexCoord); break;
		//case 19: color *= Textures[19].Sample(Samplers[19], input.TexCoord); break;
		//case 20: color *= Textures[20].Sample(Samplers[20], input.TexCoord); break;
		//case 21: color *= Textures[21].Sample(Samplers[21], input.TexCoord); break;
		//case 22: color *= Textures[22].Sample(Samplers[22], input.TexCoord); break;
		//case 23: color *= Textures[23].Sample(Samplers[23], input.TexCoord); break;
		//case 24: color *= Textures[24].Sample(Samplers[24], input.TexCoord); break;
		//case 25: color *= Textures[25].Sample(Samplers[25], input.TexCoord); break;
		//case 26: color *= Textures[26].Sample(Samplers[26], input.TexCoord); break;
		//case 27: color *= Textures[27].Sample(Samplers[27], input.TexCoord); break;
		//case 28: color *= Textures[28].Sample(Samplers[28], input.TexCoord); break;
		//case 29: color *= Textures[29].Sample(Samplers[29], input.TexCoord); break;
		//case 30: color *= Textures[30].Sample(Samplers[30], input.TexCoord); break;
		//case 31: color *= Textures[31].Sample(Samplers[31], input.TexCoord); break;
	}
	return color;
}
