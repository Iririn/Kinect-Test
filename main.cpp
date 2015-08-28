//
//  main.cpp
//  Kinect
//
//  Created by IRIN on 27/08/2015.
//  Copyright (c) 2015 IRIN. All rights reserved.
//

#include <iostream>
#include <string>

#include <ni2/OpenNI.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace openni;
using namespace cv;
using namespace std;


void CheckOpenNIError(Status result, string status)
{
    if(result != STATUS_OK)
    {
        cerr << status << "Error: " << OpenNI::getExtendedError() << endl;
    }
}



int main(int argc, const char * argv[])
{
    Status result = STATUS_OK;
    
    // Define OpenNI2 image
    VideoFrameRef oniDepthImg;
    VideoFrameRef oniColorImg;
    
    // Define OpenCV image
    Mat cvDepthImg;
    Mat cvColorImg;
    Mat cvFusionImg;
    
    namedWindow("Depth");
    namedWindow("Color");
    //cv::namedWindow("Fusion");
    
    char key = 0;
    
    // Initialize OpenNI2
    result = OpenNI::initialize();
    CheckOpenNIError(result, "Initialize context");
    
    // Open device
    Device dev;
    result = dev.open(ANY_DEVICE);
    
    
    
    // Create depth stream, set video mode and start stream
    VideoStream oniDepthStream;
    result = oniDepthStream.create(dev, SENSOR_DEPTH);
    
    VideoMode modeDepth;
    modeDepth.setResolution(640, 480);
    modeDepth.setFps(30);
    modeDepth.setPixelFormat(PIXEL_FORMAT_DEPTH_1_MM);
    oniDepthStream.setVideoMode(modeDepth);
    
    result = oniDepthStream.start();
    
    
    // Create color stream, set video mode, set registration mode and start stream
    VideoStream oniColorStream;
    result = oniColorStream.create(dev, SENSOR_COLOR);
    
    VideoMode modeColor;
    modeColor.setResolution(640, 480);
    modeColor.setFps(30);
    modeColor.setPixelFormat(PIXEL_FORMAT_RGB888);
    oniColorStream.setVideoMode(modeColor);
    
    if(dev.isImageRegistrationModeSupported(IMAGE_REGISTRATION_DEPTH_TO_COLOR))
    {
        dev.setImageRegistrationMode(IMAGE_REGISTRATION_DEPTH_TO_COLOR);
    }
    
    result = oniColorStream.start();
    
    
    
    // Start loop
    while(key != 27)
    {
        // Read color frame
        if(oniColorStream.readFrame(&oniColorImg) == STATUS_OK)
        {
            // Convert color frame data into OpenCV type
            Mat cvRGBImg(oniColorImg.getHeight(), oniColorImg.getWidth(), CV_8UC3, (void*)oniColorImg.getData());
            cvtColor(cvRGBImg, cvColorImg, CV_RGB2BGR);
            imshow("Color", cvColorImg);
        }
        
        // Read depth frame
        if(oniDepthStream.readFrame(&oniDepthImg) == STATUS_OK)
        {
            // Convert depth frame data into OpenCV type
            Mat cvRawImg16U(oniDepthImg.getHeight(), oniDepthImg.getWidth(), CV_16UC1, (void*)oniDepthImg.getData());
            cvRawImg16U.convertTo(cvDepthImg, CV_8U, 255.0 / oniDepthStream.getMaxPixelValue());
            cvtColor(cvDepthImg, cvFusionImg, CV_GRAY2BGR);
            imshow("Depth", cvDepthImg);
        }
        
        
        addWeighted(cvColorImg, 0.5, cvFusionImg, 0.5, 0, cvFusionImg);
        imshow("Fusion", cvFusionImg);
  
        key = waitKey(20);
    }
    
    
    // CV destroy
    destroyAllWindows();
    
    // OpenNI2 destroy
    oniDepthStream.destroy();
    oniColorStream.destroy();
    dev.close();
    OpenNI::shutdown();
    
    return 0;
}
