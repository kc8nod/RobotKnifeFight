import cmath
import math
import json
from math import sin, cos, radians


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
        return "%s x=%.2f y=%.2f a=%.2f (%3d)" % (self.__class__.__name__, self.xpos, self.ypos, self.angle, math.degrees(self.angle))
        
class CameraPosition(Position):

    camera_x_max = 1.0
    camera_y_max = 1.0


    def set_tuio(self, src):
        self.sessionid  = src.sessionid
        self.id         = src.id
        self.xpos       = src.xpos * self.camera_x_max
        self.ypos       = (1.0 - src.ypos) * self.camera_y_max
        self.angle      = radians(-src.angle )
        self.xmot       = src.xmot * self.camera_x_max
        self.ymot       = (1.0 - src.ymot) * self.camera_y_max 
        self.rot_vector = -src.rot_vector 
        self.mot_accel  = src.mot_accel 
        self.rot_accel  = -src.rot_accel 




class ArenaPosition(Position):
    
    scale = 1.0
    x_offset = 0
    y_offset = 0
    angle_offset = 0

    def command_str():
        heading = int( self.angle / (2 * pi()) * 16)
        "pos %d %d %d %d" * (self.id, self.xpos, self.ypos, heading)

    def set_camera(self, camera_pos):
    
        self.id = camera_pos.id
    
        self.xpos = (  camera_pos.xpos * cos(self.angle_offset) * self.scale 
                     + camera_pos.ypos * sin(self.angle_offset) * self.scale
                     + self.x_offset)
                    
        self.ypos = (  camera_pos.ypos * cos(self.angle_offset) * self.scale
                     - camera_pos.xpos * sin(self.angle_offset) * self.scale
                     + self.y_offset)
                    
        self.angle = camera_pos.angle + self.angle_offset 

        
    @classmethod
    def calibrate(cls, camera_pos1, field_pos1, camera_pos2, field_pos2):
        cam_dist, cam_ang = camera_pos1.distance_angle(camera_pos2)
        fld_dist, fld_ang = field_pos1.distance_angle(field_pos2)
        
        print camera_pos1
        print camera_pos2
        print "dist: %.3f angle: %.3f (%3d)" %(cam_dist, cam_ang, math.degrees(cam_ang))
        
        print field_pos1
        print field_pos2
        print "dist: %.3f angle: %.3f (%3d)" %(fld_dist, fld_ang, math.degrees(fld_ang))
        
        cls.angle_offset = fld_ang - cam_ang
        
        cls.scale = fld_dist / cam_dist
    
        cls.x_offset = field_pos1.xpos - camera_pos1.xpos * cos(cls.angle_offset) * cls.scale - camera_pos1.ypos * sin(cls.angle_offset) * cls.scale

        cls.y_offset = field_pos1.ypos - camera_pos1.ypos * cos(cls.angle_offset) * cls.scale - camera_pos1.xpos * sin(cls.angle_offset) * cls.scale




def write_config(filename='arena_config.json'):
    config = {
                'scale'        : ArenaPosition.scale,
                'x_offset'     : ArenaPosition.x_offset,
                'y_offset'     : ArenaPosition.y_offset,
                'angle_offset' : ArenaPosition.angle_offset
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
        


if __name__ == '__main__':

    c1 = CameraPosition(1,1)
    a1 = ArenaPosition(0, 0)
    
    c2 = CameraPosition(3,3)
    a2 = ArenaPosition(2,2)
    
    ArenaPosition.calibrate(c1, a1, c2, a2)
    
    c3 = CameraPosition(2,2)
    a3 = ArenaPosition()
    
    a3.set_camera(c3)
    
    assert a3.xpos == 1
    assert a3.ypos == 1

    print
    
    c1 = CameraPosition(1,1)
    a1 = ArenaPosition(0, 0)
    
    c2 = CameraPosition(3,3)
    a2 = ArenaPosition(200,200)
    
    ArenaPosition.calibrate(c1, a1, c2, a2)
    
    c3 = CameraPosition(2,2)
    a3 = ArenaPosition()
    
    a3.set_camera(c3)
    
    assert a3.xpos == 100
    assert a3.ypos == 100

    print    
    
    c1 = CameraPosition(1, 1)
    a1 =  ArenaPosition(2, 0)
    
    c2 = CameraPosition(3, 3)
    a2 =  ArenaPosition(0, 2)
    
    ArenaPosition.calibrate(c1, a1, c2, a2)
    
    print "ArenaPosition.scale        %.2f" % ArenaPosition.scale       
    print "ArenaPosition.x_offset     %.2f" % ArenaPosition.x_offset    
    print "ArenaPosition.y_offset     %.2f" % ArenaPosition.y_offset    
    print "ArenaPosition.angle_offset %.2f" % math.degrees(ArenaPosition.angle_offset)
    assert ArenaPosition.angle_offset == math.radians(90)
    
    c3 = CameraPosition(2,2)
    a3 = ArenaPosition()
    
    a3.set_camera(c3)
    
    print a3
    assert math.degrees(a3.angle) == 90             
    assert a3.ypos == 1
    assert a3.xpos == 1
    

    print "ok!"
    