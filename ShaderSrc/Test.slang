cbuffer image
{
//    Sampler2D<float4> texture;
//    float numberOfSamples;
    float4 colour;
};

struct VertexData
{
	static float2[] data = {
		{ 0.5,  0.5},
        { 0.5, -0.5},
		{-0.5,  0.5},
		{-0.5, -0.5}
	};

    static int[] indices = {
        0, 1, 2,
        1, 3, 2
    };
};

struct Fragment
{
    float4 colour;
}

struct VertexStageOutput
{
    float4 sv_position : SV_Position;
//    float2 uv : UV;
};

[shader("vertex")]
VertexStageOutput vertexMain(uint VetexIndex : SV_VertexID)
{
    VertexStageOutput output;
    float2 vetData = VertexData.data[VertexData.indices[VetexIndex]];
    output.sv_position = float4(vetData, 1.0);
//    output.uv = ((vetData * 2) + 1) / 2;

    return output;
}

[shader("fragment")]
//float4 fragmentMain(float2 uv : UV) : SV_Target
float4 fragmentMain() : SV_Target
{
    float4 output;
//    output = texture.Sample(uv) / numberOfSamples;
    output = colour;
    return output;
}
