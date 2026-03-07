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



def getUV(object:[T.Static|T.Dynamic], i:int) -> tuple[float, float]:
	return (
		(0.0, 0.0),
		(0.0, 1.0),
		(1.0, 1.0),
		(1.0, 0.0),
	)[i%4]; #TBA



def addEnvironment(environment:list[T.Static]) -> None:
	#Add all environmental objects to that VAO.
	vertices:list[glm.vec3] = [];
	indices:list[int] = [];

	base:int = 0;
	for object in environment:
		if (type(object) == T.Light): continue; #Don't try to add this, its not a "proper" T.Static derivative class.

		#Modify to add texture UV values.
		for (i, V) in enumerate(object.vertices):
			vertices.extend(list(V)); #X/Y/Z
			vertices.extend(getUV(object, i)); #U/V

		for I in object.indices:
			#Convert from "local" indices (start at 0) to the "global" indices (Counted for the entire vertices dataset)
			indices.append(base + I);
		base += len(object.vertices);

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