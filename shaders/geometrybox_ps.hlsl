

struct InputType {
  float4 position : SV_POSITION;
};

float4 main(InputType input) : SV_TARGET {
  return float4(1.f, 1.f, 1.f, 1.f);
}

