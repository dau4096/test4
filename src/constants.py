"constants.py"

import glm;

######## CONSTANT VALUES ########
RESOLUTION:tuple[int,int] = (960, 540);
FILE_PATH:str = "stages/t0.scene.xml";
ENV_TEXTURE_SHEET:str = "base";

INF:float = float('inf');
HZ:float = 60.0;
DT:float = 1.0 / HZ;

GRAVITY_ACCEL:float = 0.1 * DT;
AIR_DRAG:float = 0.9975
CURSOR_SPEED:float = 0.0025;
MOVE_SPEED:float = 0.1;
PLAYER_BB_DIMENSIONS:glm.vec3 = glm.vec3(0.25, 0.25, 1.75);

DEFAULT_SKY_COLOUR:glm.vec3 = glm.vec3(0.0, 0.0, 0.0);
######## CONSTANT VALUES ########

