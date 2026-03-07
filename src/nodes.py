"nodes.py"

from dataclasses import dataclass;
import xml.etree.ElementTree as ET;
import glm;

if (__name__ == "__main__"):
	#If run directly, for debugging.
	import constants as C;
else:
	#If imported by main;
	from src import constants as C;


def vec3(v:str) -> glm.vec3: return glm.vec3([float(x) for x in v.split(",")][:3]);

@dataclass
class Node:
	name:str;			   #Name given to refer to this node
	position:glm.vec3;     #3D position
	connections:list[str]; #List of other node names.

	@classmethod
	def fromXML(cls, XML:ET.Element):
		attr:dict[str,str] = XML.attrib;
		return cls(attr["name"], vec3(attr["position"]), [x.strip() for x in attr["connections"].split(",")]);



#Nodegraph class
class Graph:
	def __init__(self):
		self.nodes:dict[str, Node] = {}; #Nodes keyed by their names.

	def _dijkstra(self) -> any:
		pass; #TBA

	def addNode(self, node:Node) -> None:
		self.nodes[node.name] = node;

	def findPathStr(self, startName:str, endName:str) -> list[str]:
		pass; #TBA

	def findPathPos(self, startPos:glm.vec3, endPos:glm.vec3) -> list[Node]:
		pass; #TBA


	def __repr__(self) -> str:
		return f"<Graph [#Nodes: {len(self.nodes)},    Names: {tuple(self.nodes.keys())}]>"
