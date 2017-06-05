#!/usr/bin/env python

import sys;
sys.path.append('/Users/mereweth/snappy/tsdf_catkin_ws/devel/.private/voxblox/lib/')

import voxbloxpy
dir(voxbloxpy.EsdfMap)

map = voxbloxpy.EsdfMap('/Users/mereweth/Desktop/esdf_map.proto')

import numpy as np

query = np.matrix([[0,0,0.1],
                   [0.1,0,0],
                   [0.1,0.1,0],
                   [0,0.1,0]], dtype='double').T

grad = np.matrix(np.zeros(np.shape(query), dtype='double'))

dist = np.matrix(np.zeros((np.shape(query)[1], 1), dtype='double'))

obs = np.matrix(np.zeros((np.shape(query)[1], 1), dtype='int32'))

map.isObserved(query, obs)
map.getDistanceAtPosition(query, dist, obs)
map.getDistanceAndGradientAtPosition(query, dist, grad, obs)

try:
  import IPython; IPython.embed()

except:
  import code
  code.interact(local=dict(globals(), **locals()))
