"loader.py"

from dataclasses import dataclass;

if (__name__ == "__main__"):
	#If run directly, for debugging.
	import constants as C;
else:
	#If imported by main;
	from src import constants as C;



#Pseudo-struct style class for storing stage data.
@dataclass
class Stage:
	filePath:str;
	objects:list; #List of all objects in the stage.




def loadFile(filePath:str) -> Stage|None:
	#Returns None & throws exception if stage could not be loaded.
	return None;