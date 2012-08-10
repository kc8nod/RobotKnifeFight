import cmath
import math
import json
from math import sin, cos, radians

# BOT_ID is the index of the first dimension.
# a robot's main fiducial id is element 0 .
# the robot's death fiducial id is element 1 .
# the robot's alive status is elemt 2. (0=dead, 1=alive)
robot_ids = [[0,1,1],[2,3,1],[4,5,1],[6,7,1]]

class Position(object):
    def __init__(self, x=0, y=0, a=0, id=None):
        self.set(x, y, a)
        self.id = id
        
    def set(self, x, y, a=0):
        self.xpos = x
        self.ypos = y
        self.angle = a

    def distance_angle(self, dest):
        s = complex(self.xpos,  self.ypos)
        d = complex(dest.xpos, dest.ypos)
        return cmath.polar(d - s)

    def __str__(self):
        return "%s x=%.2f y=%.2f a=%.2frad (%3ddeg)" % (self.__class__.__name__, self.xpos, self.ypos, self.angle, math.degrees(self.angle))
        
        
        
class CameraPosition(Position):

    camera_x_max = 1280
    camera_y_max = 720

    def set_tuio(self, src):
        self.sessionid  = src.sessionid
        self.id         = src.id
        self.xpos       = src.xpos * self.camera_x_max
        self.ypos       = self.camera_y_max - (src.ypos * self.camera_y_max)
        self.angle      = src.angle
        self.xmot       = src.xmot * self.camera_x_max
        self.ymot       = self.camera_y_max - (src.ymot * self.camera_y_max) 
        self.rot_vector = src.rot_vector 
        self.mot_accel  = src.mot_accel 
        self.rot_accel  = src.rot_accel 




class ArenaPosition(Position):
    
    scale = 1.0
    x_offset = 0
    y_offset = 0
    angle_offset = 0
    alive_status = 0

    def command_str(self):
        heading = int( self.angle / (2 * math.pi) * 16)
        return "pos " + str(self.id) + " %d %d %d %d" % (self.xpos, self.ypos, heading, self.alive_status)        

    def set_camera(self, camera_pos):
        # Look up BOT_ID based on fiducial id detected by the camera
        self.id = -128 #id not found, yet.
        for fiducial in robot_ids:
            #print fiducial
            if fiducial[0]==camera_pos.id:      # if fiducial matches the main id set as BOT_ID
                self.id = camera_pos.id
            elif fiducial[1]==camera_pos.id:    # if fiducial matches the death id set objects id to be ignored (-1)
                self.id = -1
                #fiducial[2] = 0 # mark as dead #this seems to breaking stuff
        if self.id == -128: #still not found.
            self.id = camera_pos.id
            
        self.alive_status = fiducial[2]     
        
        self.xpos = ((camera_pos.xpos-self.x_offset)*cos(self.angle_offset)*self.scale+(camera_pos.ypos-self.y_offset)*sin(self.angle_offset)*self.scale)
        
        self.ypos = (-(camera_pos.xpos-self.x_offset)*sin(self.angle_offset)*self.scale+(camera_pos.ypos-self.y_offset)*cos(self.angle_offset)*self.scale)
        
        self.angle = camera_pos.angle - self.angle_offset 

        
    @classmethod
    def calibrate(cls, camera_pos1, field_pos1, camera_pos2, field_pos2):
        cam_dist, cam_ang = camera_pos1.distance_angle(camera_pos2)
        fld_dist, fld_ang = field_pos1.distance_angle(field_pos2)
        
        cls.angle_offset = cam_ang - fld_ang
        
        cls.scale = fld_dist / cam_dist
    
        cls.x_offset = camera_pos1.xpos + (field_pos1.xpos * cos(cls.angle_offset) - field_pos1.ypos * sin(cls.angle_offset)) / cls.scale

        cls.y_offset = camera_pos1.ypos + (field_pos1.ypos * cos(cls.angle_offset) + field_pos1.xpos * sin(cls.angle_offset)) / cls.scale




def write_config(filename='arena_config.json'):
    config = {
                'scale'        : ArenaPosition.scale,
                'x_offset'     : ArenaPosition.x_offset,
                'y_offset'     : ArenaPosition.y_offset,
                'angle_offset' : ArenaPosition.angle_offset,
                'camera_x_max' : CameraPosition.camera_x_max,
                'camera_y_max' : CameraPosition.camera_y_max
             }

    with open(filename, 'w') as fh:
         json.dump(config, fh, indent=4)



def read_config(filename='arena_config.json'):
    with open(filename, 'r') as fh:
        config = json.load(fh)
        
        ArenaPosition.scale        = config['scale']
        ArenaPosition.x_offset     = config['x_offset']
        ArenaPosition.y_offset     = config['y_offset']
        ArenaPosition.angle_offset = config['angle_offset']
        CameraPosition.camera_x_max = config['camera_x_max']
        CameraPosition.camera_y_max = config['camera_y_max']
        


if __name__ == '__main__':

    def assert_f_eq(a,b):
        delta = a-b
        if abs(delta) > 1.0e-9:
            raise AssertionError, "%f != %f  difference: %e" % (a, b, delta)
        

    def test(c1, a1, c2, a2, c3, a3_expected):
        print "calibrate:"
        print "c1: " + str(c1)
        print "c2: " + str(c2)
        #print "dist: %.3f angle: %.3f (%3d)" %(cam_dist, cam_ang, math.degrees(cam_ang))
        
        print "a1: " + str(a1)
        print "a2: " + str(a1)
        #print "dist: %.3f angle: %.3f (%3d)" %(fld_dist, fld_ang, math.degrees(fld_ang))
        
        ArenaPosition.calibrate(c1, a1, c2, a2)
        
        print "ArenaPosition.scale        %.2f" % ArenaPosition.scale       
        print "ArenaPosition.x_offset     %.2f" % ArenaPosition.x_offset    
        print "ArenaPosition.y_offset     %.2f" % ArenaPosition.y_offset    
        print "ArenaPosition.angle_offset %.2f" % math.degrees(ArenaPosition.angle_offset)
        
        a3 = ArenaPosition()
        a3.set_camera(c3)
        
        print "test:"
        print c3
        print a3
        assert_f_eq(a3.angle, a3_expected.angle)              
        assert_f_eq(a3.ypos,  a3_expected.ypos)
        assert_f_eq(a3.xpos,  a3_expected.xpos)
    

    print "\n\ncase 1"
    
    test(CameraPosition(1,1),  ArenaPosition(0,0),
         CameraPosition(3, 3), ArenaPosition(2, 2),
         CameraPosition(2, 2), ArenaPosition(1, 1, 0)
        )

    print "\n\ncase 2"
    
    test(CameraPosition(1,1),     ArenaPosition(0,0),
         CameraPosition(3, 3),    ArenaPosition(200, 200),
         CameraPosition(2, 2),    ArenaPosition(100, 100, 0)
        )

    print "\n\ncase 3"
    
    test(CameraPosition(0,0),     ArenaPosition(0,0),
         CameraPosition(4, 4),    ArenaPosition(2, 2),
         CameraPosition(2, 2),    ArenaPosition(1, 1, 0)
        )


    print "\n\ncase 4"
    
    test(CameraPosition(0,0),     ArenaPosition(0,0),
         CameraPosition(2, 0),    ArenaPosition(0, 2),
         CameraPosition(1,-1),    ArenaPosition(1, 1, math.radians(90))
        )

    print "\n\ncase 5"

    test(CameraPosition(1, 3),    ArenaPosition(0, 0),
         CameraPosition(3, 1),    ArenaPosition(2, 2),
         CameraPosition(2, 2),    ArenaPosition(1, 1, math.radians(90))
        )
    
    print "\n\ncase 6"

    test(CameraPosition(1, 1),    ArenaPosition(0, 0),
         CameraPosition(1, 3),    ArenaPosition(2, 2),
         CameraPosition(1, 2, math.radians(50)),    ArenaPosition(1, 1, math.radians(5))
        )
    
    
    print "ok!"
    
