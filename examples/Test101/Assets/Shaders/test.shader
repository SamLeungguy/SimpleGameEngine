Shader "Test"      //optional name, maybe a path to that shader
{
    Properties{Color4f     color = {1.0, 1.0, 1.0, 1.0}
        float4x4    MVP
        int         num   = 1
    }

    Premutation
    {
        bool    xxx_enable  true
        int     xxx_const   1
    }

    Pass "Pass0"    //optional name
    {
        Queue       "Transparent"
        Cull        "None"

        BlendRGB    Add One OneMinusSrcAlpha
        BlendAlpha  Add One OneMinusSrcAlpha

        DepthTest   Always
        DepthWrite  false

        VsFunc      vs_main
        PsFunc      ps_main
    }
}

struct appdata
{
    float4 position : POSITION;
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

struct mydata
{
    float v0;
    float2 v1;
    float3 v2;
    float3 v3[4];

};

Texture2D simpleTexture    ;//: register(t0);
SamplerState simpleSampler ;//: register(s0);

cbuffer hello : register(b0)
{
    mydata data;
    float v1_0;
    float2 v1_1;

};

v2f vs_main(appdata v)
{
    v2f output;

    output.position = v.position;
    output.uv = v.uv;
    output.normal = v.normal;

    output.position.z = data.v0 * data.v3[3] * data.v1.x * v1_0 * v1_1 ;
    
    return output;
}

float4 ps_main(v2f i) : SV_TARGET
{
    float4 texelColor = simpleTexture.Sample(simpleSampler, i.uv);

    return float4(i.uv.x, i.uv.y, i.position.z * data.v0 * data.v1.x * data.v2.x * texelColor.a, 1.0f);
    return float4(0.2f, 1.0, 1.0, 1.0);
    //return position;
    return float4( i.uv.x, i.normal.y, i.position.z, 1.0 );
}