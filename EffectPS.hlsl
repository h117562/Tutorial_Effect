Texture2D fireTexture : register(t0);
Texture2D noiseTexture : register(t1);
Texture2D alphaTexture : register(t2);
SamplerState SampleTypeWrap : register(s0);

cbuffer NoiseBuffer : register(b0)
{
	float frameTime;
	float3 speed;
	float3 scale;
    float padding;
}

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float4 fireColor;
    float noiseColor;
    float noiseColor2;
    float finalNoise;
    float2 noiseCoord;
    float2 noiseCoord2;

    float4 finalColor;

    noiseCoord = input.tex;
    noiseCoord *= float2(0.8f, 0.7f);
    noiseCoord.xy += float2(0.1f, 0.5f) * frameTime;

    noiseCoord2 = input.tex;
    noiseCoord2 *= float2(1.0f, 0.5f);
    noiseCoord2.xy += float2(-0.2f, 0.3f) * frameTime;

    noiseColor = noiseTexture.Sample(SampleTypeWrap, noiseCoord).r;
    noiseColor2 = noiseTexture.Sample(SampleTypeWrap, noiseCoord2).r;

    finalNoise = noiseColor * noiseColor2 * 2.0f;
    finalNoise = saturate(finalNoise);

    float alphaColor;

    alphaColor = alphaTexture.Sample(SampleTypeWrap, input.tex).r;

    fireColor = fireTexture.Sample(SampleTypeWrap, float2(finalNoise, finalNoise));

    fireColor.a = finalNoise;

    return fireColor;
}