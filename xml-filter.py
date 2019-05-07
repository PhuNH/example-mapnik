#!/usr/bin/env python2
# -*- coding: utf-8 -*-
"""
Created on Fri Apr 26 11:11:28 2019

@author: phunh
"""
import mapnik
import os

m = mapnik.Map(1200,800)
style = 'xml-filter.xml'
mapnik.load_map(m,style)

m.zoom_all()
mapnik.render_to_file(m,'../output-mapnik/xml-filter.png')
os.system('xdg-open ../output-mapnik/xml-filter.png')