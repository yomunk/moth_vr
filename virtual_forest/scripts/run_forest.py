#!/usr/bin/python2

import os
import sys

import numpy as np
import pandas as pd

from random import randint
from math import pi, sin, cos

from direct.showbase.ShowBase import ShowBase
from direct.task import Task

from panda3d.core import *

import zmq
import re

class VirtualForest(ShowBase):
    def __init__(self, forest):

#        self.controller = Controller()

        ctx = zmq.Context.instance()
        req = ctx.socket(zmq.REQ)

        req.connect("tcp://localhost:5560")
        self.req = req

        ctx2 = zmq.Context.instance()
        publocation = ctx2.socket(zmq.PUB)

        publocation.bind("tcp://*:5565")
        self.publocation = publocation;


        self.running = False

        self.yaw_control=0
        self.flight_speed=4

        self.accept("c", self.center)

        ShowBase.__init__(self)
        base.disableMouse()

        self.makeForestFloor()

        self.forest =  []
        for f in forest:
            tree = self.loader.loadModel("../models/tree")
            tree.reparentTo(self.render)
            tree.setScale(2*f['r'], 2*f['r'], 1)
            tree.setPos(f['x'], f['y'], -10)
            tree.setColor(0,0,0,1)
            self.forest.append(tree)


        self.camLens.setNear(0.1)
        self.camLens.setFar(20)
        self.setBackgroundColor(1,1,1)

        self.camera.setH(np.random.random()*360 - 180)

        self.fog = Fog('linearFog')
        self.fog.setColor(1,1,1)
        self.fog.setLinearRange(10, 20)
        render.setFog(self.fog)

        self.taskMgr.add(self.motionTask, "MotionTask")

    def makeForestFloor(self):
        node = GeomNode('gnode')
        geoms = []
        textures_count = 3
        # Creating a separate geom for the each textures
        # and prepearing the vertex data format for this geom
        for i in xrange(textures_count):
            gvd = GeomVertexData('name', GeomVertexFormat.getV3t2(), Geom.UHStatic)
            geom = Geom(gvd)
            prim = GeomTriangles(Geom.UHStatic)
            vertex = GeomVertexWriter(gvd, 'vertex')
            texcoord = GeomVertexWriter(gvd, 'texcoord')
            tex = loader.loadTexture('../textures/%i.png' % (i+1))
            tex.setMagfilter(Texture.FTLinearMipmapLinear)
            tex.setMinfilter(Texture.FTLinearMipmapLinear)
            geoms.append({'geom':geom,
                          'prim':prim,
                          'vertex':vertex,
                          'texcoord':texcoord,
                          'index':0,
                          'gvd':gvd,
                          'texture':tex})

        # Making and filling the mesh data
        tilesize=2
        for x in xrange(-100,100, tilesize):
            for y in xrange(-100,100, tilesize):
                t_img = randint(0,textures_count - 1) # assign with random texture
                i = geoms[t_img]['index']
                geoms[t_img]['vertex'].addData3f(x, y, -2)
                geoms[t_img]['texcoord'].addData2f(0, 0)
                geoms[t_img]['vertex'].addData3f(x, y+tilesize, -2)
                geoms[t_img]['texcoord'].addData2f(0, 1)
                geoms[t_img]['vertex'].addData3f(x+tilesize, y+tilesize, -2)
                geoms[t_img]['texcoord'].addData2f(1, 1)
                geoms[t_img]['vertex'].addData3f(x+tilesize, y, -2)
                geoms[t_img]['texcoord'].addData2f(1, -2)
                # d: index displace, becouse we use one vertex pool for the all geoms
                d = i * (textures_count + 1)
                geoms[t_img]['prim'].addVertices(d, d + 2, d + 1)
                geoms[t_img]['prim'].addVertices(d, d + 3, d + 2)
                geoms[t_img]['index'] += 1

        # Close a primitive, add them to the geom and add the texture attribute
        for i in xrange(3):
            geoms[i]['prim'].closePrimitive()
            geoms[i]['geom'].addPrimitive(geoms[i]['prim'])
            node.addGeom(geoms[i]['geom'])
            node.setGeomState(i, node.getGeomState(i).addAttrib(TextureAttrib.make(geoms[i]['texture'])))

        terrain = self.render.attachNewNode(node)
        terrain.analyze()


    def motionTask(self, task):

        self.req.send(b"abdo")#torque
        response = self.req.recv()
        print(response)
        dt = globalClock.getDt()

        if(response[:3] != "STO" ):
          self.flight_speed = 4
          self.yaw_control =(float(re.findall(r'-?\d+.\d+', response)[0]))
        else:
          self.yaw_control = 0
          self.flight_speed = 0
          self.center()

        self.camera.setH(self.camera, self.yaw_control*dt)
        self.camera.setY(self.camera, self.flight_speed*dt)

        #self.pose.x = self.camera.getX()
        #self.pose.y = self.camera.getY()
        #self.pose.theta = np.radians(self.camera.getH())
        CAMERAX = self.camera.getX()
        CAMERAY = self.camera.getY()
        HEADING = np.radians(self.camera.getH())
        #print(CAMERAX, CAMERAY, HEADING)
        self.publocation.send("CameraX %f" %  (CAMERAX))
        self.publocation.send("CameraY %f"% (CAMERAY))
        self.publocation.send("HEADING %f"% HEADING)

        return Task.cont

    def center(self):
        self.camera.setX(0)
        self.camera.setY(0)

if __name__=='__main__':
    f = np.loadtxt('../forests/forest.csv', delimiter=',', skiprows=1,
            dtype=[('x', 'f4'), ('y', 'f4'), ('r', 'f4')])

    app = VirtualForest(f)
    app.run()
