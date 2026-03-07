"physics.py"


import glm;

if (__name__ == "__main__"):
	#If run directly, for debugging.
	import constants as C;
	import logicGates as G;
	import types as T;
	import loader as L;
else:
	#If imported by main;
	from src import constants as C;
	from src import logicGates as G;
	from src import types as T;
	from src import loader as L;



def updateLogic(logicGates:list[G.LogicGate]) -> list[G.LogicGate]:
	#Updates every logicGate.
	return logicGates; #TBA


def updatePhysics(dynamic:list[T.Dynamic], player:T.Player) -> tuple[list[T.Dynamic], T.Player]:
	#Takes in and updates the dynamic objects & the player.
	return (dynamic, player); #TBA