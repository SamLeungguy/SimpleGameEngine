Shader "Test"//optional name, maybe a path to that shader
{
    Properties{//Color4f     color ={1.0, 1.0, 1.0, 1.0}
        //float4x4    MVP
        int         num   = 1//asdsdasd asdasd
        float2       asd7={1.0, 2.0f}// asdsad asd
        float3       asd8={1.0,2.0f, 3.0}// asdsad asd
        float4       asd9={1.0, 2.0,3.0, 4.0f}// asdsad asd
        //"" asd
        float       asd 
        float       asd1=122.0f///fgdfg
        float       asd2 =123.0f
        float       asd3= 124.0f//asdsad asd
        float       asd4= 125.0f// asdsad asd
        float       asd5// asdsad asd
        float       asd6// asdsad asd

        //bool        true1=true
        //floatx2
        //_EdgeSoftness("Edge Softness", range(0,1)) = 0.0
		//_EdgeColor("Edge Color", Color) = (1,0,0,1)
        //_MaskTex ("MaskTexture", 2D) = "white" {}
		//_Dissolve("Dissolve", range(0,1) ) = 0.0
		//_Mode("Mode [0:Normal] [1:Radial] [2:Rhombus]", Int) = 1
    }

    Premutation
    {
        bool    xxx_enable  true
        int     xxx_const   1
    }

    Pass "Pass0"    //optional name
    {
        Queue "Transparent"
        Cull        "None"

        BlendRGB    Add One OneMinusSrcAlpha
        BlendAlpha  Add One OneMinusSrcAlpha

        DepthTest   Always
        DepthWrite  false

        VsFunc      vs_main
        PsFunc      ps_main
    }

    Pass "Pass1"{//asdsad
        Queue       "Geometry"//asdsad
        Cull        "None"

        BlendRGB    Add One OneMinusSrcAlpha
        BlendAlpha  Add One OneMinusSrcAlpha

        DepthTest   Always
        DepthWrite  false

        VsFunc      vs1_main
        PsFunc      ps1_main
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
    half2 v1;
    float3 v2;
    float3 v3[4];
    //float4x4 mvp;
    //matrix vp;
};

Texture2D simpleTexture    ;//: register(t0);
SamplerState simpleSampler ;//: register(s0);

cbuffer hello : register(b0)
{
    mydata data;
    float v1_0;
    float2 v1_1;
    float4 v1_2[4];

};

v2f vs_main(appdata v)
{
    v2f output;

    output.position = v.position;
    output.uv = v.uv;
    output.normal = v.normal;
    //output.normal = mul(v.normal, data.mvp);
    //output.normal = mul(v.normal, data.vp);

    output.position.z = data.v0 * data.v3[3] * data.v1.x * v1_0 * v1_1 * v1_2[3];
    
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

v2f vs1_main(appdata v)
{
    v2f output;

    output.position = v.position;
    output.uv = v.uv;
    output.normal = v.normal;
    //output.normal = mul(v.normal, data.mvp);
    //output.normal = mul(v.normal, data.vp);

    output.position.z = data.v0 * data.v3[3] * data.v1.x * v1_0 * v1_1 * v1_2[3];
    
    return output;
}

float4 ps1_main(v2f i) : SV_TARGET
{
    float4 texelColor = simpleTexture.Sample(simpleSampler, i.uv);

    return float4(i.uv.x, i.uv.y, i.position.z * data.v0 * data.v1.x * data.v2.x * texelColor.a, 1.0f);
    return float4(0.2f, 1.0, 1.0, 1.0);
    //return position;
    return float4( i.uv.x, i.normal.y, i.position.z, 1.0 );
}