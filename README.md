
#Augmented reality game with national flags

This implementation detects a national flag, classifies it and then it augments an image of a famous picture from that country on the flag. All this is done in real-time. The application is supposed to run on iOS devices therefore it is optimized for that architecture.

[![Alt text for your video](https://i.ytimg.com/vi/8YtrPASa8Yc/default.jpg)](https://youtu.be/8YtrPASa8Yc)


###Tools
OpenCV and Armadillo

###High Level approach
- Detection is done by locating quadrilaterals and then verify that in the recognition phase.
- Recognition of the flags is done by feature matching (FAST features, BRIEF descriptors) and using a trained SVM.
- Tracking is done using KLT optical flow.
- Using the points of quadrilateral (4) it is enough to fit a homography and augment the image.


###Thorough Approach
The application flow is: detecting and recognizing the flag, augmenting the corresponding picture and tracking the flag. The first two phases are interleaved, namely, the detection and recognition. We use OpenCV to quickly find the quadrilaterals in the image. We assume that the flag is occupying a significant part of the screen and this way we can filter out a lot of quadrilaterals. However, there are still ways that the detection can yield incorrect results. Because of this, the recognition phase is also a confirmation for the detection. For the recognition we use two steps: prediction using a trained SVM model and feature tracking. We will explain the reasons behind these shortly, but first let's discuss the SVM training. The idea is that flags can be mostly distinguished based on their colors, rather than their structure or layout. So, we train a non-linear SVM based on the percentage of a predefined set colors that make up the flag. For our purposes, we use only 9 different colors, namely: White, Red, Green, Blue, Orange, Yellow, Magenta, Cyan and Black. Then, each pixel is mapped to one of these colors to find out the percentage of each of the colors in the image. Mapping the RGB data to the colors involves a biological fact of the humans. We are not equally sensitive to Red, Green and Blue. So we use the shortest weighted euclidean distance, with weights of 0.114, 0.587, 0.299 for Blue, Green and Red, respectively.

For the features, we use FAST feature detector and BRIEF descriptors. These are known to be fast. FAST is only doing calculations within a radius from each pixel and this way it can be easily parallelized since it doesn't have a sequential nature. BRIEF, in our case, is used with 64 bit strings. Matching of these descriptors can be fast for two main reasons: The bit strings are short, therefore have a good locality (less cache thrashing) and matching has a parallel nature since it only checks for corresponding bit matches. However, it is worth mentioning that, these were points of design to be remembered if further improvements were needed when the game becomes more complex. We used OpenCV which at the time of writing does not provide GPU acceleration for the above mentioned points for non NVIDIA graphics cards. Hopefully this will change soon.

The reason we use two steps are the following: first, the detection phase can give a region which is not a valid flag. For instance, in the flag of Germany, each stripe can be considered a quadrilateral on its own. This gives a low confidentiality rate in the SVM so we can stop. Same holds when, the quadrilateral is bigger than the flag. This would be the case when the quadrilateral would include a part of the white paper where the flag is printed on. 

The second step of feature matching is needed to ensure that not only they have the same colors but they do also have the distinct objects. For instance, the flag of Bosnia and Hercegovina and the flag of Kosovo have the same colors, and roughly with the same percentages. In case of incorrect detection, the percentages can be exactly equal however their distinct features will not have significant matches. 

The tracking is done using KLT only on the corners of the quadrilateral. The detection is reasonably fast, so one question could be why not re-detect. We should not forget that it is reasonably fast using the assumption that the flag takes a significant portion of the screen. These are enough to estimate the homography needed for the augmentation.


###Results and efficiency
From the programatic timers we get that the frame rate for the detection part is on average 90fps while the tracking part is 110fps. These are not completely accurate numbers, since the timers are not guaranteed to be exact and the error in timers can accumulate. Below we can find the app in action.

![alt tag](http://s13.postimg.org/830girzxz/image_10.jpg)

![alt tag](http://s13.postimg.org/47x2g7gs7/image_11.jpg)

Worth mentioning is that, there was a 1.5 times increase in efficiency by only reordering the double loop order of accessing of the image pixels when trying to extract the colors. Of course, this was under special circumstances where there would be a lot of other apps open in the device. The reason was that Mat matrices are ordered differently, so it matters if we access by column or by row because of the cache.
Armadillo usage in perspective warping gave also about 1.5-2 times increase in efficiency. Generally, the assumption would be that we would get more, however the conversion between formats that needs to happen consumes a fair bit of time. Hopefully, if we move everything to Armadillo we will make the other parts faster, and the conversion would be re-used.




###Extensions
This will be soon extended to augment 3D objects of the famous objects. For this we will use available point clouds.
