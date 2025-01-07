cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
}

struct Vin
{
    float4 pos : POSITION;
    float2 tex : TEXCOORD;
};

struct VOut
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

VOut main(Vin input)
{ 
	VOut output;

	output.pos = mul(input.pos, World);
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);
	output.tex = input.tex;

	return output;
}