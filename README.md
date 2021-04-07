# Visual Odometry

## What is visual odometry?
The goal of visual odometry is to estimate the camera's motion based on images that it is seeing. 
It can be performed with monocular or stereo camera systems. 
The problem with monocular systems is that they can only produce a result up to scale if no other constraints are specified. 
That means that you can only know the direction of translation but not the actual distance.

## My solution
I implemented visual odometry for android smartphones. 
That meant that I had certain limitations in what I could do. 
The system had to use only a single camera. 
Even though most smartphones have multiple cameras, matching between different lenses is difficult and outside of the scope for this project. 
Even if I did try to do that, the distance between cameras is too small for accurate measurements.
A common way to avoid these problems is to get distance information from another sensor. 
For example, monocular visual odometry is possible in a car because you can get the speed information from the speedometer. 
For my implementation, I decided to introduce a  constraint that the camera had to be parallel to the ground plane. 
Without further ado, let's go into the details. Most of the code is written in c++ using the Opencv library. 
The first and the hardest part was getting images from the camera and making a camera preview. 
Android's documentation for native camera management is almost nonexistent. 
The second part was creating a seamless camera calibration process. 
After that, I started to work on the main part. For detecting good features to track, I used a Shi-Tomasi corner detector. 
Extracted features were then tracked by the Lucas-Kanade method for optical flow. 
To get a rotation matrix and a translation vector, I estimated an affine transform between frames. 
Those vectors were then integrated over time to get the path. 

## Results
[![video](https://img.youtube.com/vi/WxPZNc1XAQ0/0.jpg)](https://www.youtube.com/watch?v=WxPZNc1XAQ0)

