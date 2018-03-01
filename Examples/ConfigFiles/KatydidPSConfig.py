#!/usr/bin/env python

'''
Note that this script assumes that NymphPy and KatydidPy are in your PYTHONPATH.
This is *probably a bad assumption* because they are installed to cmake's build PREFIX/lib.
You can add that to your PYTHONPATH, or symbolic link the files into your path, or set a build target
somewhere in your PYTHONPATH (probably you'll want to target a virtualenv, if you're using
this then you're probably developing and don't want this going into a system-level install. A virtualenv will take care of updating environment variables).
'''

import json

import NymphPy
import KatydidPy

pt = NymphPy.KTProcessorToolbox('pt')
# create processors
pt.AddProcessor("egg-processor", "egg")
pt.AddProcessor("forward-fftw", "fft")
pt.AddProcessor("convert-to-power", "to-ps")
pt.AddProcessor('basic-root-writer', "writer")
# make connections
pt.MakeConnection("egg:header", "fft:header", 0)
pt.MakeConnection("egg:ts", "fft:ts-fftw", 1)
pt.MakeConnection("fft:fft", "to-ps:fs-fftw-to-psd", 2)
pt.MakeConnection("to-ps:psd", "writer:ps", 3)
# add to run queue
pt.PushBackToRunQueue("egg")
# configure processors
egg_conf = {'filename': '', # put a valid path here
            'egg-reader': 'rsamat',
            'slice-size': 16384,
            'number-of-slices': 2,
           }
pt.ConfigureProcessors(json.dumps({'egg':egg_conf}))

fft_conf = {'transform-flag': 'ESTIMATE'}
pt.ConfigureProcessors(json.dumps({'fft':fft_conf}))

writer_conf = {'output-file': '', # put a valid path here
               'file-flag': 'recreate'
              }
pt.ConfigureProcessors(json.dumps({'writer':writer_conf}))

pt.Run()
