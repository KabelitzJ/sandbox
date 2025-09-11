#define MAX_IMAGE_ARRAY_SIZE 64

struct fs_in {
  float4 sv_position : SV_Position;
}; // struct fs_in

struct fs_out {
  float2 color : SV_Target0;
}; // struct fs_out

[shader("fragment")]
fs_out main(fs_in in) {
  fs_out out;

  out.color = float2(in.sv_position.z, 0.0);

  return out;
}
