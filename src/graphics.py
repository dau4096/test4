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


	for (i, texName) in enumerate(("hostiles", "sprites", "transparent")): #T.Dynamic texture sets.
		if (texName not in textures.keys()):
			textures[texName] = gl.load_texture(f"textures/{texName}.sheet.png", texName);
		gl.add_texture(shaders["dynamic"], textures[texName], i);





def getUV(object:[T.Static|T.Dynamic], triIdx:int, vIdx:int) -> tuple[float, float]:
	texStr:str = "00"; #String rep, [00] → (0, 0) / [F0] → (15, 0) / HEX
	uvIdx:int = (0,1,2, 2,3,0)[vIdx%6];

	if (type(object) in (
		T.Quad, T.Tri, T.Sprite, T.Item,
	)): #Objects with 1 texture ("main")
		texStr = object.textures["main"];
	elif (type(object) in (
		T.CubeStatic, T.CubePath, T.CubePhysics,
	)): #Object with 3 textures ("low", "side", "top")
		texName:str = "side";
		if (triIdx<2): texName = "low";
		elif (triIdx<4): texName = "top";
		texStr = object.textures[texName];
	else:
		texStr = "00"; #Unknown, just use some default [00]/(0, 0) UV.\


	#Convert string rep into int rep.
	texID:glm.ivec2 = glm.ivec2(
		int(texStr[1], 16), int(texStr[0], 16)
	);

	#16x16 textures per sheet.
	LOW:glm.vec2  = glm.vec2(texID) / 16.0;
	HIGH:glm.vec2 = glm.vec2(texID + 1) / 16.0;
	return (
		(LOW.x,  1.0-LOW.y),
		(HIGH.x, 1.0-LOW.y),
		(HIGH.x, 1.0-HIGH.y),
		(LOW.x,  1.0-HIGH.y),
	)[uvIdx];


def addEnvironment(environment:list[T.Static]) -> None:
	vertices:list[float] = [];
	indices:list[int] = [];
	base:int = 0;

	for object in environment:
		if (type(object) == T.Light): continue; #No mesh to draw.

		triIdx = 0;
		for t in range(
			0, len(object.indices), 3
		): #Process triangles
			for i in range(3):
				vIDX = object.indices[t+i];
				V = object.vertices[vIDX];

				vertices.extend(list(V)); #X/Y/Z
				vertices.extend(getUV(object, triIdx, i + (3*triIdx))); #U/V

			triIdx += 1;
			indices.extend([base, base+1, base+2]);
			base += 3;

	gl.add_vao(shaders["environment"], gl.POS_UV2D, vertices, indices);




def updateDynamic(dynamic:list[T.Dynamic]) -> None:
	#Update the VAO of dynamic objects.
	pass; #TBA




def drawFrame(player:T.Player) -> None:
	#Draw environmental & dynamic objects.
	viewMat:glm.mat4 = player.getViewMatrix();
	pvmMatrix:glm.mat4 = projMat * viewMat * modlMat;

	#Draw environment triangles
	gl.add_uniform_value(shaders["environment"], "pvmMatrix", pvmMatrix);
	gl.run(shaders["environment"]);