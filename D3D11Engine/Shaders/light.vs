cbuffer MatrixBuffer {
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer {
	float3 cameraPosition;
	float padding;
};

struct VertexInputType {
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

PixelInputType LightVertexShader(VertexInputType input) {
	PixelInputType output;
	float4 worldPosition;

	input.position.w = 1.0f;

	// 위치 계산
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

	// 노멀 벡터는 월드 매트릭스만 곱해줍니다.
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	// 뷰 방향 계산합니다.
	worldPosition = mul(input.position, worldMatrix);
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	output.viewDirection = normalize(output.viewDirection);

	return output;
}