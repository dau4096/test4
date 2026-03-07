"loader.py"

import xml.etree.ElementTree as ET;
from dataclasses import dataclass;
import gl; #Import custom OpenGL wrapper module. [https://github.com/dau4096/py-graphics-module]

if (__name__ == "__main__"):
	#If run directly, for debugging.
	import constants as C;
	import types as T;
	import nodes as N;
	import logicGates as G;
else:
	#If imported by main;
	from src import constants as C;
	from src import types as T;
	from src import nodes as N;
	from src import logicGates as G;



#Pseudo-struct style class for storing stage data.
@dataclass
class Stage:
	filePath:str;
	environment:list[T.Static];   #List of all static objects in the stage.
	dynamic:list[T.Dynamic];      #List of all dynamic objects in the stage.
	graph:N.Graph;                #Graph to be pathfind(ed)-through.
	logicGates:list[G.LogicGate]; #List of all logic gates in the stage.
	player:T.Player;			  #Player instance for this stage.

	def __repr__(self) -> str:
		return f"""<Stage [
	FilePath: \"{self.filePath}\",
	Number of T.Static derived: {len(self.environment)},    Number of T.Dynamic derived: {len(self.dynamic)},
	NodeGraph: {self.graph},
	Gates: (
        {'\n        '.join([repr(lG) for lG in self.logicGates])}
	),
	Player: {self.player}
]>""";



#Maps each XML tag type to a class/typedef.
TYPE_MAP:dict[str,[T.Static|T.Dynamic]] = {
	"scene": T.Scene, "player": T.Player,
	"cube-static": T.CubeStatic,
	"quad": T.Quad, "tri": T.Tri,
	"sprite": T.Sprite, "item": T.Item,
	"trigger": T.Trigger,
	"interactable": T.Interactable,
	"cube-path": T.CubePath, "enemy": T.Enemy,
	"cube-physics": T.CubePhysics,
	"light": T.Light, "node": N.Node,
	"logic-gate": G.LogicGate
};



#Environment types - No updates per frame/tick.
ENVIRONMENT_TAGS:tuple[str] = (
	"cube-static", "quad", "tri", "light"
);
#Dynamic types - Updates every frame/tick.
DYNAMIC_TAGS:tuple[str] = (
	"item", "cube-path", "sprite", "enemy",
	"cube-physics", "interactable", "trigger"
);



def createObject(objectXML:ET.Element) -> [T.Static|T.Dynamic]:
	try:
		return TYPE_MAP[objectXML.tag].fromXML(objectXML);
	except KeyError as e:
		print(f"Missing value for {objectXML.tag} : {e}");
		return;


def loadFile(filePath:str) -> Stage|None:
	#Returns None & throws exception if stage could not be loaded.
	rootNode:ET.Element = None;
	try:
		tree:ET = ET.parse(filePath);
		rootNode = tree.getroot();
	except FileNotFoundError:
		print(f"Could not open file: {filePath}");
		return None; #Could not read file.


	environment:list[[T.Static]] = [];
	dynamic:list[[T.Dynamic]] = [];
	graph:N.Graph = N.Graph(); #Create empty graph.
	logicGates:list[G.LogicGate] = [];
	playerInitial:T.PlayerInitial = None;
	for objectXML in rootNode:
		#Process each node in the file.
		tag:str = objectXML.tag;
		if (tag in ENVIRONMENT_TAGS): environment.append(createObject(objectXML));	#Static nonupdating objects
		elif (tag in DYNAMIC_TAGS): dynamic.append(createObject(objectXML));		#Movable/updating objects
		elif (tag == "logic-gate"): logicGates.append(createObject(objectXML));		#Logic gates
		elif (tag == "node"): graph.addNode(createObject(objectXML));				#Pathfinding graph nodes.
		elif (tag == "player"): playerInitial = T.PlayerInitial.fromXML(objectXML);	#Player start values.
		else: #Unknown
			print(f"Unknown node type: {tag}");
			return None;

	cameraID:int = gl.create_camera(fov_deg=70.0, near_z=0.1, far_z=100.0);
	player:T.Player = T.Player(cameraID, playerInitial);

	return Stage(filePath, environment, dynamic, graph, logicGates, player);