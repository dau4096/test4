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



def updateLogic(logicGates:list[G.LogicGate]) -> None:
	#Updates every logicGate.
	return logicGates; #TBA


PHYSICS_OBJECTS:tuple[T.Dynamic] = (
	T.Player, T.Item, T.Enemy, T.CubePhysics #All have cuboidal physics boxes.
);

def updatePlayer(dynamic:list[T.Dynamic], environment:list[T.Static], player:T.Player) -> None:
	#Run player physics, and such.
	pass; #TBA


def updatePhysics(dynamic:list[T.Dynamic], environment:list[T.Static], player:T.Player) -> None:
	#Takes in and updates the dynamic objs & the player.
	for obj in dynamic:
		if (isinstance(obj, PHYSICS_OBJECTS)):
			#If obj should have physics simulated;
			obj.velocity.z = -C.GRAVITY_ACCEL;
			obj.velocity *= C.AIR_DRAG;
			BB:T.BoundingBox = obj.getBoundingBox();

			for env in environment:
				#Check AABB collision against each env object;
				collision:T.Intersection = env.intersects(BB);
				if (collision.isIntersecting):
					#Fix this collision.
					if (collision.intersect.z > 0.1): print(collision.intersect, obj.position);
					obj.position -= collision.intersect;
					obj.velocity = glm.vec3(0.0, 0.0, 0.0); #Zero the v for now. Will replace with something more sophisticated later.
			
			obj.position += obj.velocity;
					

		#Update every obj.
		obj.update(player);

	updatePlayer(dynamic, environment, player);