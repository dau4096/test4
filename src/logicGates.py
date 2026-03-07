"logicGates.py"

import xml.etree.ElementTree as ET;

global flags;
flags:dict[str, bool] = { #Global dataset of all flags.
	#Default values;
	"CONST_ON":True, "CONST_OFF":False,
};

def getFlag(name:str) -> bool:
	if (name in flags): return flags[name];
	return False;

GATE_FUNC_MAP:dict[str, callable] = {
	"NOT":	(lambda A,B : not getFlag(A)),
	"AND":	(lambda A,B : getFlag(A) and getFlag(B))
};


def addFlag(name:str) -> str:
	if (name not in flags):	flags[name] = False;
	return name;


class LogicGate:
	def __init__(self, type:str, inputA:str, inputB:str, output:str):
		self.type:str = type.upper();

		#Add operands and initialise their flag entries.
		self.A:str = inputA;
		self.B:str = inputB;
		self.Q:str = output;

		addFlag(self.A);
		addFlag(self.B);
		addFlag(self.Q);

		if (self.type in GATE_FUNC_MAP.keys()):
			self._func:callable = GATE_FUNC_MAP[self.type];
		else:
			raise KeyError(f"Unknown LogicGate type: {self.type}");

	def __repr__(self) -> str:
		if (self.B != ""): #Has 2 operands.
			return f"<LogicGate [{self.A} {self.type} {self.B} → {self.Q}]>";
		else: #Only has 1.
			return f"<LogicGate [{self.type} {self.A} → {self.Q}]>";

	@classmethod
	def fromXML(cls, XML:ET.Element):
		attr:dict[str,str] = XML.attrib;
		B:str = "" if ("B" not in attr) else attr["B"]; #Allow missing B, as gates like NOT do not require it.
		return cls(attr["type"], attr["A"], B, attr["Q"]);

	def eval(self) -> bool:
		global flags;
		flags[self.Q] = self._func(self.A, self.B);
		return flags[self.Q];
