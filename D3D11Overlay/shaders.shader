struct VS_INPUT {
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct VS_OUTPUT {
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

VS_OUTPUT VShader(VS_INPUT input) {
    VS_OUTPUT output;
    output.pos = float4(input.pos, 1.0);
    output.color = input.color;
    return output;
}

struct PS_INPUT {
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

struct PS_OUTPUT {
    float4 color : SV_TARGET;
};

PS_OUTPUT PShader(PS_INPUT input) {
    PS_OUTPUT output;
    output.color = input.color;
    return output;
}