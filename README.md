# AcidCam2.Linux

Command line version of Acid Cam for Linux

To compile use

./configure

pass a path to configure if OpenCV 3 is installed somewhere else example

./configure --prefix=/opt/local

Command line arguments:

-v list.txt [Image list text file]
-t translation_speed [Transition variable]
-l input_file.avi [Input file name (avi)]
-d device_num [Capture Device Index]
-w width [Capture Device Width]
-h height [Capture Device Height]
-f fps [Record frames per second]
-p alpha [Pass 2 blend alpha]
-o output.avi [Output filename]
-s seed [Random seed value]
-x [Disable recording]
-i image_file.jpg [Image for blending functions]
-a path [Save prefix]

Keys

'a' take uncompressed snapshot 
's' take compressed lossless png
'x' reset alpha
'l' filter increased (Next filter)
'o' filter decreased (Next filter)
