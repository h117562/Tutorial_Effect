Texture2D fireTexture : register(t0);
Texture2D noiseTexture : register(t1);
Texture2D alphaTexture : register(t2);
SamplerState SampleTypeWrap : register(s0);

cbuffer NoiseBuffer : register(b0)
{
    float3 padding;
	float frameTime;
}

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float4 finalColor;
    float noiseColor;
    float noiseColor2;
    float finalNoise;
    float2 noiseCoord;
    float2 noiseCoord2;


    noiseCoord = input.tex;
    noiseCoord.xy *= float2(0.8f, 0.7f);
    noiseCoord.xy += float2(0.1f, 0.5f) * frameTime;

    noiseCoord2 = input.tex;
    noiseCoord2.xy *= float2(1.0f, 0.5f);
    noiseCoord2.xy += float2(-0.2f, 0.3f) * frameTime;

    noiseColor = noiseTexture.Sample(SampleTypeWrap, noiseCoord).r;
    noiseColor2 = noiseTexture.Sample(SampleTypeWrap, noiseCoord2).r;

    finalNoise = noiseColor * noiseColor2;
    //finalNoise = saturate(finalNoise);

    finalColor = fireTexture.Sample(SampleTypeWrap, finalNoise);

    finalColor.a = finalNoise + input.tex.y - 0.3f;

    return finalColor;
}