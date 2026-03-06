"main.py"

import glm; #For the TvecN, and matNM types.
import gl; #Import custom OpenGL wrapper module. [https://github.com/dau4096/py-graphics-module]


#Import other files;
from src import constants as C;
from src import graphics as R;
from src import loader as L;


def main() -> None:
	gl.set_output(gl.DEBUG); #Debugging.
	gl.init(name="test4/rewrite26", resolution=C.RESOLUTION, version=(4, 6));
	gl.configure(gl.WORLDSPACE);

	R.init();
	L.loadFile(C.FILE_PATH);

	cameraID:int = gl.create_camera(fov_deg=70.0, near_z=0.1, far_z=100.0);

	while (
		gl.is_window_open() and
		(not gl.is_key_held(gl.KEY_ESCAPE))
	): #While window should stay open;
		gl.poll_events();

		#Handle inputs.

		#Render frame.
		R.drawFrame(cameraID);

		gl.update_window();





if (__name__ == "__main__"): main();
