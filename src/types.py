"types.py"

import glm;
import gl; #Import custom OpenGL wrapper module. [https://github.com/dau4096/py-graphics-module]
import math as maths;
from dataclasses import dataclass;

import xml.etree.ElementTree as ET;
if (__name__ == "__main__"):
	#If run directly, for debugging.
	import constants as C;
else:
	#If imported by main;
	from src import constants as C;


@dataclass
class Intersection:
	isIntersecting:bool;
	distance:float;
	direction:glm.vec3;


def toBool(v:str) -> bool: return v.lower() in ("t", "true");
def vec2(v:str) -> glm.vec2: return glm.vec2([float(x) for x in v.split(",")][:2]);
def vec3(v:str) -> glm.vec3: return glm.vec3([float(x) for x in v.split(",")][:3]);



#Player/Meta
@dataclass
class PlayerInitial:
	position:glm.vec3; #Initial Pos'n
	angle:glm.vec3;	#Initial Angle
	items:list;		#Initial Items

	@classmethod
	def fromXML(cls, XML:ET.Element):
		attr:dict[str,str] = XML.attrib;
		return cls(vec3(attr["position"]), vec3(attr["angle"]), [x.strip() for x in attr["items"].split(",")]);


class Player:
	def __init__(self, cameraID:int, init:PlayerInitial):
		self.cameraID:int = cameraID;
		self.position:glm.vec3 = init.position;
		self.angle:glm.vec3 = init.angle;
		self.initial:PlayerInitial = init;

	def getViewMatrix(self) -> glm.mat4:
		#Update gl camera.
		gl.set_new_camera_position(self.cameraID, self.position);
		gl.set_new_camera_angle(self.cameraID, self.angle);

		return glm.mat4(gl.get_matrix(gl.VIEW, self.cameraID));

	def handleInputs(self) -> None:
		#Read inputs, translate/rotate as necessary.
		#Translation
		forward:glm.vec3 = glm.vec3(maths.sin(self.angle.x), maths.cos(self.angle.x), 0.0) * C.MOVE_SPEED;
		right:glm.vec3 = glm.vec3(forward.y, -forward.x, 0.0);
		up:glm.vec3 = glm.vec3(0.0, 0.0, C.MOVE_SPEED);

		if (gl.is_key_held(gl.KEY_W)): self.position += forward;
		if (gl.is_key_held(gl.KEY_S)): self.position -= forward;
		if (gl.is_key_held(gl.KEY_D)): self.position += right;
		if (gl.is_key_held(gl.KEY_A)): self.position -= right;
		if (gl.is_key_held(gl.KEY_E)): self.position += up;
		if (gl.is_key_held(gl.KEY_Q)): self.position -= up;

		#Rotation
		if (gl.is_key_held(gl.KEY_1)): gl.show_cursor(); #Stop cursor control briefly.
		else:
			gl.hide_cursor();
			cursorDelta:glm.vec2 = glm.vec2(gl.get_cursor_movement());
			delta:glm.vec3 = glm.vec3(cursorDelta.x, -cursorDelta.y, 0.0);
			self.angle += delta * C.CURSOR_SPEED;
			self.angle.y = glm.clamp(self.angle.y, -maths.pi/2.0, maths.pi/2.0);


	def __repr__(self) -> str:
		return f"<Player [CameraID: {self.cameraID},    Pos: {tuple(self.position)},    Ang: {tuple(self.angle)}]>";




def getTextures(XML:ET.Element) -> dict[str,str]:
	for node in XML:
		if (node.tag == "textures"):
			#<objecttag attribs=""> <textures low="{textures[0]}" side="{textures[1]}" top="{textures[2]}" /> </objecttag>
			return node.attrib;

	return {};

def getVertices(XML:ET.Element) -> list[glm.vec3]:
	for node in XML:
		if (node.tag == "vertices"):
			#<objecttag attribs=""> <vertices v0="{v[0]}" v1="{v[1]}" ... vN="{v[N]}" /> </objecttag>
			return [vec3(x) for x in node.attrib.values()];

	return [];

def getCuboidVerticesIndices(position:glm.vec3, dimensions:glm.vec3) -> tuple[list[glm.vec3], list[int]]:
	#Offsets in range [-1 ←→ +1], will be scaled & translated later.
	#Could precalc offsets, but this feels clearer (?)
	vertices:list[glm.vec3] = [
		#Lower vertices
		glm.vec3(-1.0, -1.0, -1.0),
		glm.vec3( 1.0, -1.0, -1.0),
		glm.vec3( 1.0,  1.0, -1.0),
		glm.vec3(-1.0,  1.0, -1.0),

		#Upper vertices
		glm.vec3(-1.0, -1.0,  1.0),
		glm.vec3( 1.0, -1.0,  1.0),
		glm.vec3( 1.0,  1.0,  1.0),
		glm.vec3(-1.0,  1.0,  1.0),
	];

	scale:glm.vec3 = dimensions * 0.5;
	for i in range(8): #Scale, then translate the vertices.
		vertices[i].x *= scale.x;
		vertices[i].y *= scale.y;
		vertices[i].z *= scale.z;

		vertices[i].x += position.x;
		vertices[i].y += position.y;
		vertices[i].z += position.z;


	indices:list[int] = [
		#Horizontal faces
		0,1,2, 2,3,0, #Lower face
		4,5,6, 6,7,4, #Upper face

		#Vertical faces
		0,1,5, 5,4,0,
		1,2,6, 6,5,1,
		2,3,7, 7,6,2,
		3,0,4, 4,7,3,
	];

	return (vertices, indices);




















######## DYNAMIC TYPES ########
#Parent class for all dynamic object types. [Physics]
class Dynamic:
	def __init__(self, type:str, position:glm.vec3, vertices:list[glm.vec3], indices:list[int], textures:dict[str,str]):
		self.type = type;
		self.vertices:list[glm.vec3] = vertices;
		self.indices:list[int] = indices;
		self.textures:dict[str,str] = textures;

		self.position:glm.vec3 = position;
		self.velocity:glm.vec3 = glm.vec3(0.0, 0.0, 0.0);

	def __repr__(self): return f"<{self.__class__.__name__}>";

	@classmethod 
	def fromXML(cls, XML:ET.Element): raise NotImplementedError(f"{cls.__name__}.fromXML() method was not implemented.");

	def update(self, player:Player):
		#Update by 1 physics tick.
		self.velocity.z -= C.GRAVITY_ACCEL;
		self.velocity *= C.AIR_DRAG;
		self.position += self.velocity;



def getSpriteVertices(sPos:glm.vec3, sDim:glm.vec2, pPos:glm.vec3) -> list[glm.vec3]:
	delta:glm.vec3 = sPos - pPos;
	right:glm.vec2 = glm.normalize(glm.vec2(-delta.y, delta.x));
	left:glm.vec2 = -right;
	vertices:list[glm.vec3] = [
		#Lower verts
		glm.vec3(right.x, right.y, 0.0),
		glm.vec3( left.x,  left.y, 0.0),

		#Upper verts
		glm.vec3( left.x,  left.y, sDim.y),
		glm.vec3(right.x, right.y, sDim.y),
	];

	scale:glm.vec2 = sDim / 2.0;
	for v in vertices:
		#Width must be halved.
		v.x *= scale.x;
		v.y *= scale.x;

		#Translation
		v += sPos;

	return vertices;


class Sprite(Dynamic):
	def __init__(self, position:glm.vec3, dimensions:glm.vec2, texture:str):
		super().__init__(type(self), position, [], [0,1,2, 2,3,0], {"main": texture,});
		self.dimensions:glm.vec2 = dimensions;

	@classmethod
	def fromXML(cls, XML:ET.Element):
		attr:dict[str,str] = XML.attrib;
		dim:glm.vec2 = glm.vec2(1.0, 1.0) if ("dimensions" not in attr) else vec2(attr["dimensions"]);
		return cls(vec3(attr["position"]), dim, attr["texture"]);

	def update(self, player:Player):
		#Overload to update sprite billboarding.
		self.vertices = getSpriteVertices(self.position, self.dimensions, player.position);



class Item(Sprite):
	def __init__(self, position:glm.vec3, type:str):
		super().__init__(position, glm.vec2(1.0, 1.0), "00"); #Texture TBA
		self.type = type;

	@classmethod
	def fromXML(cls, XML:ET.Element):
		attr:dict[str,str] = XML.attrib;
		return cls(vec3(attr["position"]), attr["type"]);

	def update(self, player:Player):
		#Overload to update sprite billboarding, and check if should be collected.
		self.velocity.z -= C.GRAVITY_ACCEL;
		self.velocity *= C.AIR_DRAG;
		self.position += self.velocity;

		self.vertices = getSpriteVertices(self.position, self.dimensions, player.position);



class Enemy(Sprite):
	def __init__(self, position:glm.vec3, type:str):
		super().__init__(position, glm.vec2(1.0, 1.0), "00"); #Texture TBA
		self.type = type;

	@classmethod
	def fromXML(cls, XML:ET.Element):
		attr:dict[str,str] = XML.attrib;
		return cls(vec3(attr["position"]), attr["type"]);

	def update(self, player:Player):
		#Overload to update sprite billboarding & player interaction.
		self.velocity.z -= C.GRAVITY_ACCEL;
		self.velocity *= C.AIR_DRAG;
		self.position += self.velocity;

		self.vertices = getSpriteVertices(self.position, self.dimensions, player.position);



class CubePhysics(Dynamic):
	def __init__(self, position:glm.vec3, dimensions:glm.vec3, textures:list[str], mass:float):
		(vertices, indices) = getCuboidVerticesIndices(position, dimensions);
		super().__init__(type(self), position, vertices, indices, textures);
		self.mass:float = mass;
		self.dimensions = dimensions;

	@classmethod
	def fromXML(cls, XML:ET.Element):
		attr:dict[str,str] = XML.attrib;
		return cls(vec3(attr["position"]), vec3(attr["dimensions"]), getTextures(XML), float(attr["mass"]));

	def update(self, player:Player):
		#Update by 1 physics tick.
		self.velocity.z -= C.GRAVITY_ACCEL;
		self.velocity *= C.AIR_DRAG;
		self.position += self.velocity;
		
		(self.vertices, _) = getCuboidVerticesIndices(self.position, self.dimensions);



class Interactable(Dynamic):
	def __init__(self, vertices:list[glm.vec3], texture:str, flag:str):
		super().__init__(type(self), glm.vec3(0.0, 0.0, 0.0), vertices, [0,1,2, 2,3,0], {"main": texture,});
		self.flag:str = flag;
	
	@classmethod
	def fromXML(cls, XML:ET.Element):
		attr:dict[str,str] = XML.attrib;
		return cls(getVertices(XML), attr["texture"], attr["flag"]);

	def update(self, player:Player):
		#Custom update to check for player press.
		pass; #TBA



class Trigger(Dynamic):
	def __init__(self, position:glm.vec3, dimensions:glm.vec3, flag:str):
		(vertices, indices) = getCuboidVerticesIndices(position, dimensions);
		super().__init__(type(self), position, vertices, indices, {});
		self.flag:str = flag;

	@classmethod
	def fromXML(cls, XML:ET.Element):
		attr:dict[str,str] = XML.attrib;
		return cls(vec3(attr["position"]), vec3(attr["dimensions"]), attr["flag"]);

	def update(self, player:Player):
		#Checks if player is inside and sets the flag.
		pass; #TBA



class CubePath(Dynamic):
	def __init__(self, position:glm.vec3, dimensions:glm.vec3, textures:list[str], displacement:glm.vec3, speed:float, flag:str):
		(vertices, indices) = getCuboidVerticesIndices(position, dimensions);
		super().__init__(type(self), position, vertices, indices, textures);
		self.displacement:glm.vec3 = displacement;
		self.speed:float = max(speed, glm.length(displacement)); #Don't let it try move further than its max.
		self._t:float = 0.0; #Used to track progress along its displacement vector. Ranges from 0.0 (start pos) to |displacement| (end pos)
		self.flag:str = flag;
	
	@classmethod
	def fromXML(cls, XML:ET.Element):
		attr:dict[str,str] = XML.attrib;
		return cls(vec3(attr["position"]), vec3(attr["dimensions"]), getTextures(XML), vec3(attr["displacement"]), float(attr["speed"]), attr["flag"]);

	def update(self, player:Player):
		#Updates to check if flag is enabled, and move if so.
		pass; #TBA
######## DYNAMIC TYPES ########

























######## STATIC TYPES ########
#Parent class for all static object types. [Environmental]
class Static:
	def __init__(self, type:str, vertices:list[glm.vec3], indices:list[int], textures:dict[str,str], collision:bool):
		self.type = type;
		self.vertices:list[glm.vec3] = vertices;
		self.indices:list[int] = indices;
		self.textures:dict[str,str] = textures;
		self.collision:bool = collision;

	def __repr__(self): return f"<{self.__class__.__name__}>";

	@classmethod
	def fromXML(cls, XML:ET.Element): raise NotImplementedError(f"{cls.__name__}.fromXML() method was not implemented.");


class CubeStatic(Static):
	def __init__(self, position:glm.vec3, dimensions:glm.vec3, textures:list[str], collision:bool):
		(vertices, indices) = getCuboidVerticesIndices(position, dimensions);
		super().__init__(type(self), vertices, indices, textures, collision);
	
	@classmethod
	def fromXML(cls, XML:ET.Element):
		attr:dict[str,str] = XML.attrib;
		return cls(vec3(attr["position"]), vec3(attr["dimensions"]), getTextures(XML), toBool(attr["collision"]));

	def __contains__(self, other:type[Dynamic]) -> Intersection:
		#`if (dynamic in static): ...`
		#AABB bounding box checks.
		pass;


class Tri(Static):
	def __init__(self, vertices:list[glm.vec3], texture:str, collision:bool):
		super().__init__(type(self), vertices, [0,1,2], {"main": texture,}, collision);
	
	@classmethod
	def fromXML(cls, XML:ET.Element):
		attr:dict[str,str] = XML.attrib;
		return cls(getVertices(XML), attr["texture"], toBool(attr["collision"]));

	def __contains__(self, other:type[Dynamic]) -> Intersection:
		#`if (dynamic in static): ...`
		#Uses Seperating Axis Theorem.
		pass; #TBA


class Quad(Static):
	def __init__(self, vertices:list[glm.vec3], texture:str, collision:bool):
		super().__init__(type(self), vertices, [0,1,2, 2,3,0], {"main": texture,}, collision);
	
	@classmethod
	def fromXML(cls, XML:ET.Element):
		attr:dict[str,str] = XML.attrib;
		return cls(getVertices(XML), attr["texture"], toBool(attr["collision"]));

	def __contains__(self, other:type[Dynamic]) -> Intersection:
		#`if (dynamic in static): ...`
		#Uses Seperating Axis Theorem.
		pass; #TBA


class Light(Static):
	def __init__(self, position:glm.vec3, lookAt:glm.vec3, colour:glm.uvec3, intensity:float, FOV:float, dist:float, flag:str):
		super().__init__(type(self), [], [], [], False);
		self.position:glm.vec3 = position;
		self.lookAt:glm.vec3 = lookAt;

		self.colour:glm.vec3 = glm.vec3(colour) / 255.0;
		self.intensity:float = intensity;
		self.FOV:float = maths.radians(FOV);
		self.range:float = dist;

		self.flag:str = flag;

	@classmethod
	def fromXML(cls, XML:ET.Element):
		attr:dict[str,str] = XML.attrib;
		return cls(vec3(attr["position"]), vec3(attr["look-at"]), vec3(attr["colour"]), float(attr["intensity"]), float(attr["FOV"]), float(attr["range"]), attr["flag"]);

	def __contains__(self, other:type[Dynamic]) -> Intersection:
		return Intersection(False, 0.0, glm.vec3(0.0, 0.0, 0.0));
######## STATIC TYPES ########