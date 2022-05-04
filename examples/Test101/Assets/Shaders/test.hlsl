struct appdata
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
    float3 normal   : NORMAL;
    //float4 color : COLOR;
};

struct v2f
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
    float3 normal   : NORMAL;
    //float4 color : COLOR;
};

v2f vs_main(appdata v)
{
    v2f output;

    output.position = v.position;
    output.uv = v.uv;
    output.normal = v.normal;

    return output;
}


float4 ps_main(v2f i) : SV_TARGET
{
    i.position.w = 1.0;
    i.uv.x = 1.0;
    i.normal.x = 0.0;

    return float4( i.uv.x, i.normal.y, i.position.z, 1.0 );
}