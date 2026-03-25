"graphics.py"

import gl; #Import custom OpenGL wrapper module. [https://github.com/dau4096/py-graphics-module]
import glm;

if (__name__ == "__main__"):
	#If run directly, for debugging.
	import constants as C;
	import types as T;
else:
	#If imported by main;
	from src import constants as C;
	from src import types as T;



global shaders, textures;
shaders:dict[str, int] = {}; #To store all shaders.
textures:dict[str, int] = {}; #To store all textures.


#Constant Matrices
global projMat, modlMat;
projMat = glm.mat4(0.0);
modlMat = glm.mat4(0.0);






def init(cameraID:int) -> None:
	#Initialise values, matrices, textures and shaders.
	global projMat, modlMat;
	projMat = glm.mat4(gl.get_matrix(gl.PERSPECTIVE, cameraID));
	modlMat = glm.mat4(gl.get_matrix(gl.IDENTITY)); #Identity for now.

	global shaders;
	shaders["environment"] = gl.load_shader(gl.WORLDSPACE, vertex="src/shaders/worldspace.vert", fragment="src/shaders/uv.3D.frag");
	shaders["dynamic"] = gl.load_shader(gl.WORLDSPACE, vertex="src/shaders/worldspace.vert", fragment="src/shaders/uv.3D.frag");


	#Environment texture sheet
	textures[C.ENV_TEXTURE_SHEET] = gl.load_texture(f"textures/{C.ENV_TEXTURE_SHEET}.sheet.png", C.ENV_TEXTURE_SHEET);
	gl.add_texture(shaders["environment"], textures[C.ENV_TEXTURE_SHEET], 0);


	for (i, texName) in enumerate((C.ENV_TEXTURE_SHEET, "hostiles", "sprites", "transparent")): #T.Dynamic texture sets.
		if (texName not in textures.keys()):
			textures[texName] = gl.load_texture(f"textures/{texName}.sheet.png", texName);
		gl.add_texture(shaders["dynamic"], textures[texName], i);







CACHE:dict[str, tuple[float,float]] = {}; #Cache the pre-calculated UVs.
def getUV(obj:[T.Static|T.Dynamic], triIdx:int, vIdx:int) -> tuple[float, float]:
	global CACHE;
	texStr:str = "00"; #String rep, [00] → (0, 0) / [F0] → (15, 0) / HEX
	uvIdx:int = (0,1,2, 2,3,0)[vIdx%6];

	if (isinstance(obj, (
		T.Quad, T.Tri, T.Sprite, T.Item,
	))): #Objects with 1 texture ("main")
		texStr = obj.textures["main"];
	elif (isinstance(obj, (
		T.CubeStatic, T.CubePath, T.CubePhysics,
	))): #Object with 3 textures ("low", "side", "top")
		texName:str = "side";
		if (triIdx<2): texName = "low";
		elif (triIdx<4): texName = "top";
		texStr = obj.textures[texName];
	else:
		texStr = "00"; #Unknown, just use some default [00]/(0, 0) UV.\


	key:int = (vIdx << 8) | int(texStr, 16);
	if (key in CACHE): return CACHE[key]; #Skip recalc, already prepped for this face/texture.


	#Convert string rep into int rep.
	texID:glm.ivec2 = glm.ivec2(
		int(texStr[1], 16), int(texStr[0], 16)
	);

	#16x16 textures per sheet.
	OFFSET:float = 0.0;#1.0 / (16.0 * 128); #1 pixel, in a 16 texture grid (Each texture is 128x128)
	LOW:glm.vec2  = (glm.vec2(texID) / 16.0) + OFFSET;
	HIGH:glm.vec2 = (glm.vec2(texID + 1) / 16.0) - OFFSET;
	UV:tuple[float,float] = (
		(LOW.x,  1.0-HIGH.y),
		(HIGH.x, 1.0-HIGH.y),
		(HIGH.x, 1.0-LOW.y),
		(LOW.x,  1.0-LOW.y),
	)[uvIdx];
	CACHE[key] = UV;
	return UV;





def addEnvironment(environment:list[T.Static]) -> None:
	vertices:list[float] = [];
	indices:list[int] = [];
	base:int = 0;

	for obj in environment:
		if (type(obj) == T.Light): continue; #No mesh to draw.

		triIdx = 0;
		for t in range(
			0, len(obj.indices), 3
		): #Process triangles
			for i in range(3):
				vIDX = obj.indices[t+i];
				V = obj.vertices[vIDX];

				vertices.extend(list(V)); #X/Y/Z
				vertices.extend(getUV(obj, triIdx, i + (3*triIdx))); #U/V

			triIdx += 1;
			indices.extend([base, base+1, base+2]);
			base += 3;

	gl.add_vao(shaders["environment"], gl.POS_UV2D, vertices, indices);






def updateDynamic(dynamic:list[T.Dynamic]) -> None:
	#Update the VAO of dynamic objs.
	vertices:list[float] = [];
	indices:list[int] = [];
	base:int = 0;

	for obj in dynamic:
		if (isinstance(obj, T.Trigger)): continue; #Should not be rendered.

		triIdx = 0;
		for t in range(
			0, len(obj.indices), 3
		): #Process triangles
			for i in range(3):
				vIDX = obj.indices[t+i];
				V = obj.vertices[vIDX];

				vertices.extend(list(V)); #X/Y/Z
				vertices.extend(getUV(obj, triIdx, i + (3*triIdx))); #U/V

			triIdx += 1;
			indices.extend([base, base+1, base+2]);
			base += 3;

	gl.add_vao(shaders["dynamic"], gl.POS_UV2D, vertices, indices);








def drawFrame(player:T.Player) -> None:
	#Draw environmental & dynamic objs.
	viewMat:glm.mat4 = player.getViewMatrix();
	pvmMatrix:glm.mat4 = projMat * viewMat * modlMat;

	#Draw environment triangles
	gl.add_uniform_value(shaders["environment"], "pvmMatrix", pvmMatrix);
	gl.run(shaders["environment"]);

	#Draw dynamic obj triangles
	gl.add_uniform_value(shaders["dynamic"], "pvmMatrix", pvmMatrix);
	gl.run(shaders["dynamic"]);