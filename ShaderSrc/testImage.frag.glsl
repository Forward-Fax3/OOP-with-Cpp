#version 460
layout(row_major) uniform;
layout(row_major) buffer;

#line 51 0
struct SLANG_ParameterGroup_image_natural_0
{
    vec3 colour_0;
};


#line 1
layout(binding = 0)
layout(std140) uniform block_SLANG_ParameterGroup_image_natural_0
{
    vec3 colour_0;
}image_0;

#line 47
layout(location = 0)
out vec4 entryPointParam_fragmentMain_0;


#line 47
void main()
{

#line 47
    entryPointParam_fragmentMain_0 = vec4(image_0.colour_0, 1.0);

#line 47
    return;
}

