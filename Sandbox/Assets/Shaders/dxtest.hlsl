/* vertex attributes go here to input to the vertex shader */
struct vs_in
{
    float3 position_local : POS;
    float4 color_local : COL;
};

/* outputs from vertex shader go here. can be interpolated to pixel shader */
struct vs_out
{
    float4 position_clip : SV_POSITION; // required output of VS
    float4 color_clip : COLOR;
};

cbuffer vs_mat : register(b0)
{
    float4x4 ProjectionView;
};

vs_out VSMain(vs_in input)
{
    float4 pos = float4(input.position_local, 1.0);

    vs_out output = (vs_out)0; // zero the memory first
    output.position_clip = mul(ProjectionView, pos);
    output.color_clip = input.color_local; 
    return output;
}

float4 PSMain(vs_out input) : SV_TARGET
{
  return input.color_clip; // must return an RGBA colour
}