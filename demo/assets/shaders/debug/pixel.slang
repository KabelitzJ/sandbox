struct PSIn {
  float4 color : TEXCOORD0;
}; // struct PSIn

struct PSOut {
  float4 color : SV_Target0;
}; // struct PSOut

[shader("pixel")]
PSOut PSMain(PSIn IN) : SV_Target0 {
  PSOut OUT;

  OUT.color = IN.color;

  return OUT;
}