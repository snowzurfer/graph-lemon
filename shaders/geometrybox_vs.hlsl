struct InputType {
    float4 position : POSITION;
};

InputType main(InputType input) {
	// No vertices to process so Vertex shader pass values onto next stage.
	// You could manipulate the points in the mesh before passing them on.
	return input;
}
