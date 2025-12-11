#version 460
layout(row_major) uniform;
layout(row_major) buffer;

#line 21 0
struct SLANG_ParameterGroup_image_natural_0
{
    float numberOfSamples_0;
    float scaleImage_0;
    vec2 imagePositionOffset_0;
    bool toggleUV_0;
};


#line 22
layout(binding = 0)
layout(std140) uniform block_SLANG_ParameterGroup_image_natural_0
{
    float numberOfSamples_0;
    float scaleImage_0;
    vec2 imagePositionOffset_0;
    bool toggleUV_0;
}image_0;

#line 977 1
layout(binding = 1)
uniform sampler2D image_texture_0;


#line 977
layout(location = 0)
out vec4 entryPointParam_fragmentMain_0;


#line 977
layout(location = 0)
in vec2 uv_0;


#line 43 0
void main()
{

#line 43
    vec4 output_0;


    if(image_0.toggleUV_0)
    {
        vec2 scaleduv_0 = uv_0 * image_0.scaleImage_0 + image_0.imagePositionOffset_0;

#line 48
        output_0 = vec4(scaleduv_0 - floor(scaleduv_0), 0.0, 1.0) / image_0.numberOfSamples_0;

#line 46
    }
    else
    {

#line 53
        vec4 output_1 = (texture((image_texture_0), (uv_0 * image_0.scaleImage_0 + image_0.imagePositionOffset_0)));

#line 53
        output_0 = vec4(vec3(output_1.x, output_1.y, output_1.z) / image_0.numberOfSamples_0, output_1.w);

#line 46
    }

#line 46
    entryPointParam_fragmentMain_0 = output_0;

#line 46
    return;
}

