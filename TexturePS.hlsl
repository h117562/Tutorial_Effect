Texture2D diffuseTexture;
SamplerState SampleType;

struct Pin
{
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

float4 main(Pin input) : SV_TARGET
{
    float4 textureColor = diffuseTexture.Sample(SampleType, input.texcoord);

	return textureColor;
}