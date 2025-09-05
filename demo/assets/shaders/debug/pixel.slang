struct ps_in {
  float4 color : TEXCOORD0;
}; // struct ps_in

struct ps_out {
  float4 color : SV_Target0;
}; // struct ps_out

[shader("pixel")]
ps_out ps_main(ps_in in) : SV_Target0 {
  ps_out out;

  out.color = in.color;

  return out;
}