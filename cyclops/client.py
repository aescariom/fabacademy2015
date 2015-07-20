# Cyclops machine
# Alejandro Escario Méndez
# June 2015

#------IMPORTS-------
from pygestalt import nodes
from pygestalt import interfaces
from pygestalt import machines
from pygestalt import functions
from pygestalt.machines import elements
from pygestalt.machines import kinematics
from pygestalt.machines import state
from pygestalt.utilities import notice
from pygestalt.publish import rpc	#remote procedure call dispatcher
import time
import io
import socket
import sys


#------VIRTUAL MACHINE------
class virtualMachine(machines.virtualMachine):
	
	def initInterfaces(self):
		if self.providedInterface: self.fabnet = self.providedInterface		#providedInterface is defined in the virtualMachine class.
		else: self.fabnet = interfaces.gestaltInterface('FABNET', interfaces.serialInterface(baudRate = 115200, interfaceType = 'ftdi', portName = '/dev/tty.usbserial-FTXW6YE6'))
		
	def initControllers(self):
		self.x1AxisNode = nodes.networkedGestaltNode('X1 Axis', self.fabnet, filename = '086-005a.py', persistence = self.persistence)
		self.x2AxisNode = nodes.networkedGestaltNode('X2 Axis', self.fabnet, filename = '086-005a.py', persistence = self.persistence)
		self.yAxisNode = nodes.networkedGestaltNode('Y Axis', self.fabnet, filename = '086-005a.py', persistence = self.persistence)
		self.zAxisNode = nodes.networkedGestaltNode('Z Axis', self.fabnet, filename = '086-005a.py', persistence = self.persistence)

		self.xyNode = nodes.compoundNode(self.x1AxisNode, self.x2AxisNode, self.yAxisNode, self.zAxisNode)

	def initCoordinates(self):
		self.position = state.coordinate(['mm', 'mm', 'mm', 'mm'])
	
	def initKinematics(self):
		self.x1Axis = elements.elementChain.forward([elements.microstep.forward(4), elements.stepper.forward(1.8), elements.leadscrew.forward(8), elements.invert.forward(False)])
		self.x2Axis = elements.elementChain.forward([elements.microstep.forward(4), elements.stepper.forward(1.8), elements.leadscrew.forward(8), elements.invert.forward(False)])
		self.yAxis = elements.elementChain.forward([elements.microstep.forward(4), elements.stepper.forward(1.8), elements.leadscrew.forward(8), elements.invert.forward(False)])
		self.zAxis = elements.elementChain.forward([elements.microstep.forward(4), elements.stepper.forward(1.8), elements.leadscrew.forward(8), elements.invert.forward(False)])		
		self.stageKinematics = kinematics.direct(4)	#direct drive on all axes
	
	def initFunctions(self):
		self.move = functions.move(virtualMachine = self, virtualNode = self.xyNode, axes = [self.x1Axis, self.x2Axis, self.yAxis, self.zAxis], kinematics = self.stageKinematics, machinePosition = self.position,planner = 'null')
		self.jog = functions.jog(self.move)	#an incremental wrapper for the move function
		pass
		
	def initLast(self):
		#self.machineControl.setMotorCurrents(aCurrent = 0.8, bCurrent = 0.8, cCurrent = 0.8)
		#self.xNode.setVelocityRequest(0)	#clear velocity on nodes. Eventually this will be put in the motion planner on initialization to match state.
		pass
	
	def publish(self):
		#self.publisher.addNodes(self.machineControl)
		pass
	
	def getPosition(self):
		return {'position':self.position.future()}
	
	def setPosition(self, position  = [None]):
		self.position.future.set(position)

	def setSpindleSpeed(self, speedFraction):
		#self.machineControl.pwmRequest(speedFraction)
		pass

#------IF RUN DIRECTLY FROM TERMINAL------
if __name__ == '__main__':
    stages = virtualMachine(persistenceFile = "test.vmp")
    stages.xyNode.setVelocityRequest(3);
    
    HOST = 'localhost'
    PORT = 3333      
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
    s.connect((HOST, PORT))
    while True:
        s.send('1') # ready to read
        data = s.recv(255)
        if len(data) > 0 and data != "-1":
            li = data.split(";")
            float_list = [float(i) for i in li if i]
            
            move = [float_list[0], float_list[0], float_list[1], float_list[2]]
            
            print 'Move to: ', move
            stages.move(move, 0)
            status = stages.x1AxisNode.spinStatusRequest()
            while status['stepsRemaining'] > 0:
                time.sleep(0.001)
                status = stages.x1AxisNode.spinStatusRequest()
    s.close()