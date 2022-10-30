--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return {
	shaders =
	{
		{ path = "Shaders/Vertex/standard.shader", arguments = { "vertex" } },
		{ path = "Shaders/Fragment/standard.shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/animatedColor.shader", arguments = { "fragment" } },

		{ path = "Shaders/Vertex/vertexInputLayout_mesh.shader", arguments = { "vertex" } },
	},
	meshes = {
		"Meshes/plane.mesh",
		"Meshes/cube.mesh",
		"Meshes/sphere.mesh",
		"Meshes/helix.mesh",
		"Meshes/chubby.mesh",
		"Meshes/ancient_temple.mesh",
	},
}
