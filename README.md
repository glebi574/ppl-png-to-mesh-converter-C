# ppl-png-to-mesh-converter-C
This program converts PNG images to PewPew Live meshes.  
To use it drag and drop PNG image in program's console or just write it's path to start processing. Program itself only outputs transformed PNG image in form of pixels. You have to write your own interpreter to mesh, or use the existing ones, which are quite optimized already.
settings.lua must contain array `engines` with strings, representing names of all files, that will be called to generate output.  
