--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return {
	shaders =
	{
		{ path = "Shaders/Vertex/lambert.shader", arguments = { "vertex" } },
		{ path = "Shaders/Fragment/lambert.shader", arguments = { "fragment" } },

		{ path = "Shaders/Vertex/vertexInputLayout_mesh.shader", arguments = { "vertex" } },
	},
	meshes = {
		--"Meshes/plane.mesh",
		--"Meshes/cube.mesh",
		--"Meshes/helix.mesh",
		--"Meshes/teapot.mesh",
		--"Meshes/Taser.mesh",
		"Meshes/backpack.mesh",
	},
}
