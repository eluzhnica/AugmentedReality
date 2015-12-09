//
//  ViewController.m
//  openCViOSFaceTrackingTutorial
//
//  Created by Evangelos Georgiou on 16/03/2013.
//  Copyright (c) 2013 Evangelos Georgiou. All rights reserved.
//

#include <opencv2/opencv.hpp>

#define CHESSBOARD_WIDTH 6
#define CHESSBOARD_HEIGHT 5

#include "ViewController.h"

NSString* const faceCascadeFilename = @"haarcascade_frontalface_alt2";

@interface ViewController (){
    Mat display;
    vector<cv::Point2f> src;			// Source Points basically the 4 end co-ordinates of the overlay image
    vector<cv::Point2f> dst;			// Destination Points to transform overlay image
    Mat lastImg;
    
}
@end

@implementation ViewController

@synthesize videoCamera;

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    self.videoCamera = [[CvVideoCamera alloc] initWithParentView:imageView];
    self.videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
    self.videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPreset640x480;
    self.videoCamera.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationPortrait;
    self.videoCamera.defaultFPS = 30;
    self.videoCamera.grayscaleMode = NO;
    self.videoCamera.delegate = self;
    
    UIImage *image = [UIImage imageNamed:@"shingani.jpg"];
    display = [self cvMatFromUIImage:image];
    
    src.push_back(cv::Point2f(0,0));
    src.push_back(cv::Point2f(display.cols,0));
    src.push_back(cv::Point2f(display.cols, display.rows));
    src.push_back(cv::Point2f(0, display.rows));
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Protocol CvVideoCameraDelegate

#ifdef __cplusplus
- (void)processImage:(Mat&)img;
{
    cv::Size board_size(CHESSBOARD_WIDTH-1, CHESSBOARD_HEIGHT-1);
    
    vector<cv::Point2f> corners;
    
    Mat cpy_img(img.rows, img.cols, img.type());
    Mat neg_img(img.rows, img.cols, img.type());
    Mat gray;
    Mat blank(display.rows, display.cols, display.type());
    
    cvtColor(img, gray, CV_BGR2GRAY);
    
    bool found = false;
    
    if(dst.size() == 0){
        std::cout << "WTF DO YOUR JOB, BIATCH!" << std::endl;
    }else{
        std::cout << "WTF MAN " <<  dst.size() <<std::endl;
    }
    if(dst.size() == 0){
        bool flag = findChessboardCorners(img, board_size, corners);

        if(flag == 1){
            // This function identifies the chessboard pattern from the gray image, saves the valid group of corners
            cornerSubPix(gray, corners, cv::Size(11,11), cv::Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
            found = true;
            
            dst.push_back(corners[0]);
            dst.push_back(corners[CHESSBOARD_WIDTH-2]);
            dst.push_back(corners[(CHESSBOARD_WIDTH-1)*(CHESSBOARD_HEIGHT-1)-1]);
            dst.push_back(corners[(CHESSBOARD_WIDTH-1)*(CHESSBOARD_HEIGHT-2)]);
        }
    }else{
        vector<uchar> status;
        Mat error;
        cv::calcOpticalFlowPyrLK(lastImg, gray, dst, corners, status, error);
        dst.clear();
        
        found = true;
        
        for(int i=0; i<status.size();i++){
            if(status[i] == 0){
                found = false;
                break;
            }
        }
        
        if(found){
            dst = corners;
        }
    }
    
    if(found){
        
        
        // Compute the transformation matrix,
        // i.e., transformation required to overlay the display image from 'src' points to 'dst' points on the image
        Mat warp_matrix = getPerspectiveTransform(src, dst);
        
        blank = cv::Scalar(0);
        neg_img = cv::Scalar(0);								// Image is white when pixel values are zero
        cpy_img = cv::Scalar(0);								// Image is white when pixel values are zero
        
        bitwise_not(blank,blank);
        
        // Note the jugglery to augment due to OpenCV's limitation passing two images of DIFFERENT sizes while using "cvWarpPerspective"
        
        warpPerspective(display, neg_img, warp_matrix, cv::Size(neg_img.cols, neg_img.rows));	// Transform overlay Image to the position	- [ITEM1]
        warpPerspective(blank, cpy_img, warp_matrix, cv::Size(cpy_img.cols, neg_img.rows));		// Transform a blank overlay image to position
        bitwise_not(cpy_img, cpy_img);							// Invert the copy paper image from white to black
        bitwise_and(cpy_img, img, cpy_img);						// Create a "hole" in the Image to create a "clipping" mask - [ITEM2]
        bitwise_or(cpy_img, neg_img, img);						// Finally merge both items [ITEM1 & ITEM2]
    }

    lastImg = gray;
}
#endif

- (cv::Mat)cvMatFromUIImage:(UIImage *)image
{
    CGColorSpaceRef colorSpace = CGImageGetColorSpace(image.CGImage);
    CGFloat cols = image.size.width;
    CGFloat rows = image.size.height;
    
    cv::Mat cvMat(rows, cols, CV_8UC4); // 8 bits per component, 4 channels (color channels + alpha)
    
    CGContextRef contextRef = CGBitmapContextCreate(cvMat.data,                 // Pointer to  data
                                                    cols,                       // Width of bitmap
                                                    rows,                       // Height of bitmap
                                                    8,                          // Bits per component
                                                    cvMat.step[0],              // Bytes per row
                                                    colorSpace,                 // Colorspace
                                                    kCGImageAlphaNoneSkipLast |
                                                    kCGBitmapByteOrderDefault); // Bitmap info flags
    
    CGContextDrawImage(contextRef, CGRectMake(0, 0, cols, rows), image.CGImage);
    CGContextRelease(contextRef);
    
    return cvMat;
}

#pragma mark - UI Actions

- (IBAction)startCamera:(id)sender
{
    [self.videoCamera start];
}

- (IBAction)stopCamera:(id)sender
{
    [self.videoCamera stop];
}

@end
