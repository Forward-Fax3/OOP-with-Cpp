struct VertexData
{
	static float2[] data = {
		{ 1.0,  1.0},
        { 1.0, -1.0},
		{-1.0,  1.0},
		{-1.0, -1.0}
	};

    static int[] indices = {
        0, 1, 2,
        1, 3, 2
    };
};

cbuffer image
{
    float numberOfSamples;
    float scaleImage;
    float2 imagePositionOffset;
    bool toggleUV;
    Sampler2D<float4> texture;
};

struct VertexStageOutput
{
    float4 sv_position : SV_Position;
    float2 uv : UV;
};

[shader("vertex")]
VertexStageOutput vertexMain(uint VetexIndex : SV_VertexID)
{
    VertexStageOutput output;
    float2 vetData = VertexData.data[VertexData.indices[VetexIndex]];
    output.sv_position = float4(vetData, 1.0);
    output.uv = (vetData + 1.0f) * 0.5f;

    return output;
}

[shader("fragment")]
float4 fragmentMain(float2 uv : UV) : SV_Target
{
    float4 output;
    if (toggleUV)
    {
        float2 scaleduv = (uv * scaleImage) + imagePositionOffset;
        output = float4(scaleduv - floor(scaleduv), 0.0f, 1.0f) / numberOfSamples;
    }
    else
    {
        output = texture.Sample((uv * scaleImage) + imagePositionOffset);
        output = float4(float3(output.x, output.y, output.z) / numberOfSamples, output.w);
    }
    return output;
}
