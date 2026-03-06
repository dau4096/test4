"graphics.py"

if (__name__ == "__main__"):
	#If run directly, for debugging.
	import constants as C;
else:
	#If imported by main;
	from src import constants as C;




def init() -> None:
	pass; #Initialise values, create shaders, etc.


def drawFrame(cameraID:int) -> None:
	pass; #TBA