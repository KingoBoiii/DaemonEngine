struct VS_INPUT
{
	float3 Position : POSITION;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
	float TextureIndex : TEXTUREID;
	int EntityId : ENTITYID;
};

struct VS_OUTPUT
{
	float4 PositionCS : SV_POSITION;
	float4 Position : POSITION; 
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
	nointerpolation int TextureIndex : TEXTUREID;
	int EntityId : ENTITYID;
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
	output.TextureIndex = (int)input.TextureIndex;
	output.EntityId = input.EntityId;
	return output;
}

Texture2D Textures[16] : register(t0);
SamplerState Samplers[16] : register(s0);

struct PS_OUTPUT
{
	float4 Color : SV_TARGET0;
	int EntityId : SV_TARGET1;
};

PS_OUTPUT PSMain(VS_OUTPUT input)
{
	float4 color = input.Color;
	switch(input.TextureIndex)
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
	}

	PS_OUTPUT output = (PS_OUTPUT)0;
	output.Color = color;
	output.EntityId = input.EntityId;
	return output;
}
