import cmath
import math

ARENA_SIZE_INCHES = (72.0, 48.0)
ARENA_SIZE_UNITS = (ARENA_SIZE_INCHES[0]*2.0, ARENA_SIZE_INCHES[1]*2.0)

NUM_HEADINGS = 16.0
DEG_PER_HEADING = 360.0 / NUM_HEADINGS


class Position(object):
    def __init__(self, x, y, a):
        self.xpos = x
        self.ypos = y
        self.angle = a

    def distance_angle(self, dest):
        s = complex(self.xpos,  self.ypos)
        d = complex(dest.xpos, dest.ypos)
        cmath.polar(d - s)

        
        
class CameraPosition(Position):

    camera_x_max = 1.0
    camera_y_max = 1.0
    

    def __init__(self, src):
        self.sessionid  = src.sessionid
        self.id         = src.id
        self.xpos       = src.xpos * camera_x_max
        self.ypos       = (1.0 - src.ypos) * camera_y_max
        self.angle      = radians(-src.angle )
        self.xmot       = src.xmot * camera_x_max
        self.ymot       = (1.0 - src.ymot) * camera_y_max 
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
    
        self.xpos = (  camera.xpos * cos(angle_offset) * scale 
                     + camera.ypos * sin(angle_offset) * scale
                     + x_offset)
                    
        self.ypos = (  camera.ypos * cos(angle_offset) * scale
                     - camera.xpos * sin(angle_offset) * scale
                     + y_offset)
                    
        self.angle = camera_pos.angle + angle_offset 

        
    @classmethod
    def calibrate(cls, camera_pos1, field_pos1, camera_pos2, field_pos2):
        cam_dist, cam_ang = distance_angle(camera_pos1, camera_pos2)
        fld_dist, fld_ang = distance_angle(field_pos1, field_pos2)
        
        angle_offset = fld_ang - cam_ang
        
        x_scale = fld_dist / cam_dist
    
        x_offset = field_pos1.xpos - camera_pos.xpos * cos(angle_offset) * scale - camera_pos.ypos * sin(angle_offset) * scale

        y_offset = field_pos1.ypos - camera_pos.ypos * cos(angle_offset) * scale - camera_pos.xpos * sin(angle_offset) * scale
