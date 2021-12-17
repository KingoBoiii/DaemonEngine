struct VS_INPUT
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};


VS_OUTPUT VSMain(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = input.Position;
	output.TexCoord = input.TexCoord;
	return output;
}

Texture2D Texture;
SamplerState Sampler;
float4 PSMain(VS_OUTPUT input) : SV_TARGET1
{
	return Texture.Sample(Sampler, input.TexCoord);
}