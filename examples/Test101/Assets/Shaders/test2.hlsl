struct VOut
{
    float4 position : SV_POSITION;
    float2 tex0     : TEXCOORD0;
    float4 color    : COLOR;
};

VOut vs_main(float4 position : POSITION, float2 tex0 : TEXCOORD0, float4 color : COLOR)
{
    VOut output;

    output.position = position;
    output.tex0 = tex0;
    output.color = color;

    return output;
}


float4 ps_main(float4 position : SV_POSITION, float2 tex0 : TEXCOORD0, float4 color : COLOR) : SV_TARGET
{
    return float4(tex0.x, tex0.x, color.x, 1.0);
}