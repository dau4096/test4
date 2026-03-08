"dat-to-xml.py"
#Converts the (displeasing) .dat format of old into a nicer, XML representation.



def V4toV3(V4:str) -> str:
	return ", ".join(V4.split(", ")[:-1]);
def v3(v:str) -> str:
	#Swap from Y up to Z up.
	parts:list[str] = v.split(", ");
	partsFixed = [parts[0], parts[2], parts[1]];
	return ", ".join(partsFixed);


######## FORMATTING FUNCS ########
def SCENE(parts:list[str]) -> str: #0
	#0, SCENE, scene-wide data, Sounds & skybox // Void-colour, sound file to play ambiently.
	return f'<meta skyColour="{parts[0]}" ambientSound="{parts[1]}" />';


def PLAYER(parts:list[str]) -> str: #1
	#1, PLAYER, player data & items // Start position, direction, list of item names.
	return f'<player position="{v3(parts[0])}" angle="{parts[1]}" items="{parts[2].replace(":",",")}" />';


def CUBE_STATIC(parts:list[str]) -> str: #2
	#2, CUBE_STATIC // Centre location, Dimensions, bottom/side/top textures.
	textures:list[str] = [x.split(">")[-1] for x in parts[3].split("/")];
	return f'<cube-static position="{v3(parts[0])}" dimensions="{v3(parts[1])}" collision="{parts[2]=="T"}"> <textures low="{textures[0]}" side="{textures[1]}" top="{textures[2]}" /> </cube-static>';


def QUAD(parts:list[str]) -> str: #3
	#3, QUAD // TL coordinate, BL coordinate, BR coordinate, TR coordinate, texture.
	return f'<quad texture="{parts[5].split(">")[-1]}" collision="{parts[4]=="T"}"> <vertices v0="{v3(parts[0])}" v1="{v3(parts[1])}" v2="{v3(parts[2])}" v3="{v3(parts[3])}" /> </quad>';


def TRI(parts:list[str]) -> str: #4
	#4, TRI // TL coordinate, BL coordinate, BR coordinate, Collide-able, texture.
	return f'<tri texture="{parts[4].split(">")[-1]}" collision="{parts[3]=="T"}"> <vertices v0="{v3(parts[0])}" v1="{v3(parts[1])}" v2="{v3(parts[2])}" /> </tri>';


def SPRITE_STATIC(parts:list[str]) -> str: #5
	#5, SPRITE_STATIC, decoration // Centre location, texture.
	return f'<sprite position="{v3(parts[0])}" texture="{parts[1].split(">")[-1]}" />';


def ITEM(parts:list[str]) -> str: #6
	#6, ITEM, gives something and disappears // Centre location, "what to give" (health, ammo)
	return f'<item position="{v3(parts[0])}" type="{parts[1]}" />';


def TRIGGER(parts:list[str]) -> str: #7
	#7, TRIGGER, Do "thing" when player touches (01 is prev. scene, 02 is next scene, etc.) // Centre location, Dimensions, Trigger-flag.
	return f'<trigger position="{v3(parts[0])}" dimensions="{v3(parts[1])}" flag="{parts[2]}" />';


def INTERACTABLE(parts:list[str]) -> str: #8
	#8, INTERACTABLE, Like a button for player to use // TL coordinate, BL coordinate, BR coordinate, TR coordinate, Trigger-flag, texture.
	return f'<interactable flag="{parts[4]}" texture="{parts[5].split(">")[-1]}"> <vertices v0="{v3(parts[0])}" v1="{v3(parts[1])}" v2="{v3(parts[2])}" v3="{v3(parts[3])}" /> </interactable>';


def CUBE_PATH(parts:list[str]) -> str: #9
	#9, CUBE_PATH, Moving boxes for doors or similar // Position, Dimentions, Movement vector, Movement speed, Trigger-flag, bottom/side/top textures.
	textures:list[str] = [x.split(">")[-1] for x in parts[5].split("/")];
	return f'<cube-path position="{v3(parts[0])}" dimensions="{v3(parts[1])}" displacement="{v3(parts[2])}" speed="{parts[3]}" flag="{parts[4]}"> <textures low="{textures[0]}" side="{textures[1]}" top="{textures[2]}" /> </cube-path>';


def ENEMY(parts:list[str]) -> str: #A|10
	#A, ENEMY, fights back // Start location, Facing direction, type.
	return f'<enemy position="{v3(parts[0])}" direction="{v3(parts[1])}" type="{parts[2]}" />';


def CUBE_PHYSICS(parts:list[str]) -> str: #B|11
	#B, CUBE_PHYSICS, Such as a box // Position, Dimentions, bottom/side/top textures.
	textures:list[str] = [x.split(">")[-1] for x in parts[3].split("/")];
	return f'<cube-physics position="{v3(parts[0])}" dimensions="{v3(parts[1])}" mass="{parts[2]}"> <textures low="{textures[0]}" side="{textures[1]}" top="{textures[2]}" /> </cube-physics>';


def LIGHT(parts:list[str]) -> str: #C|12
	#C, LIGHT, Creates light at a point with intensity and colour. Can be toggled with the flag. // Location, Look-at position, Colour, Intensity, FOV, Max-distance, Toggle-flag.
	return f'<light position="{v3(parts[0])}" look-at="{v3(parts[1])}" colour="{V4toV3(parts[2])}" intensity="{parts[3]}" FOV="{parts[4]}" range="{parts[5]}" flag="{parts[6]}" />';


def NPC_PATH_NODE(parts:list[str]) -> str: #D|13
	#D, NPC_PATH_NODE, Used for Enemy pathfinding with Dijkstra's Algorithm. // Location, flag, Connections (To other Nodes).
	return f'<node position="{v3(parts[0])}" name="{parts[1]}" connections="{parts[2]}" />';


def LOGIC(parts:list[str]) -> str: #E|14
	#E, LOGIC, Logic gate for flag logic. // flagA TYPE flagB, outputFlag.
	operation:list[str] = parts[0].split(" ");
	if (len(operation) == 2): operation = [operation[1], operation[0], ""];
	return f'<logic-gate type="{operation[1]}" A="{operation[0]}" B="{operation[2]}" Q="{parts[1]}" />';
######## FORMATTING FUNCS ########




formatMap:tuple[callable] = (
	SCENE, PLAYER, CUBE_STATIC,
	QUAD, TRI, SPRITE_STATIC,
	ITEM, TRIGGER, INTERACTABLE,
	CUBE_PATH, ENEMY, CUBE_PHYSICS,
	LIGHT, NPC_PATH_NODE, LOGIC
);




def convertFile(name:str) -> None:
	#Read file
	datLines:list[str] = [];
	with open(f"{name}.dat", "r") as datFile:
		datLines = datFile.readlines();


	#Convert to XML.
	xmlLines:list[str] = ["<stage>",];
	for ln in datLines:
		#Loop through each line.
		ln = ln.strip();
		converted:str = ln;
		if (ln.startswith("//")): #Reformat comment lines;
			converted = f"<!-- {ln[2:]} -->";
		elif (ln.startswith(">")): #Texture sheet def line (Unknown purpose?)
			pass;
		elif (len(ln) > 0): #If not a comment/spacing line;
			parts:list[str] = [x.strip() for x in ln.split("|")];
			converted = formatMap[int(parts[0], 16) % 15](parts[1:]);

		xmlLines.append("    " + converted);
	xmlLines.append("</stage>");


	#Write back to file
	with open(f"{name}.xml", "w") as xmlFile:
		xmlFile.write("\n".join(xmlLines));



convertFile("t0.scene");