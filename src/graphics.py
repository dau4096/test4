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




def init() -> None:
	pass; #Initialise values, create shaders, etc.


def addEnvironment(environment:list[T.Static]) -> None:
	#Add all environmental objects to that VAO.
	pass; #TBA

def updateDynamic(dynamic:list[T.Dynamic]) -> None:
	#Update the VAO of dynamic objects.
	pass; #TBA


def drawFrame(player:T.Player) -> None:
	#Draw environmental & dynamic objects.
	pass; #TBA