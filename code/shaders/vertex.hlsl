void
main(float4 in_position      : POSITION,
     float4 in_colour        : COLOUR,
     out float4 out_colour   : COLOUR,
     out float4 out_position : SV_Position) {
    out_position = in_position;
    out_colour = in_colour;
}