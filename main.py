"main.py"

import glm; #For the TvecN, and matNM types.
import gl; #Import custom OpenGL wrapper module. [https://github.com/dau4096/py-graphics-module]


#Import other files;
from src import constants as C;
from src import types as T;
from src import graphics as R;
from src import physics as P;
from src import loader as L;


def main() -> None:
	gl.set_output(gl.SILENT); #Debugging.
	gl.init(name="test4/rewrite26", resolution=C.RESOLUTION, version=(4, 6));
	gl.configure(gl.WORLDSPACE);

	stage:[L.Stage|None] = L.loadFile(C.FILE_PATH);
	if (stage is None):
		raise ValueError("Could not load stage.");
	#print("Loaded:", stage);
	R.init(stage.player.cameraID);


	R.addEnvironment(stage.environment); #Static objects.
	while (
		 gl.is_window_open() and
		(not gl.is_key_held(gl.KEY_ESCAPE))
	): #While window should stay open;
		#Handle inputs.
		gl.poll_events();
		stage.player.handleInputs();

		#Run Physics & logic
		P.updateLogic(stage.logicGates);
		P.updatePhysics(stage.dynamic, stage.environment, stage.player);

		#Render frame.
		R.updateDynamic(stage.dynamic); #Dynamic objects.
		R.drawFrame(stage.player); #Render the frame

		#Update window
		gl.update_window();


	gl.delete_camera(stage.player.cameraID);
	gl.terminate(); #Close after.



if (__name__ == "__main__"): main();
