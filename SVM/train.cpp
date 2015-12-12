#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <limits.h>
#include <math.h>       /* pow */
#include <float.h>

#define COLORS_C 9
#define LABEL_TOTAL 7
#define PER_LABEL 5

using namespace cv;

int colors[COLORS_C][3] = {{255,255,255}, {0,0,255}, {0,255,0}, {255,0,0}, {0,128,255}, {0,255,255}, {255,0,255}, {255,255,0}, {0,0,0}};
std::string flagname[LABEL_TOTAL] = {"kosovo", "france", "brazil", "china", "germany", "usa", "albania"};

int color_index(Point3_<uchar>* p){
	double distance = DBL_MAX;
	int min_idx = 0;
	for(int i=0; i<COLORS_C; i++){
		int b = p->x;
		int g = p->y;
		int r = p->z;

		double dist = std::sqrt(std::pow(0.114*(colors[i][0] - b),2) + std::pow(0.587*(colors[i][1] - g),2) + std::pow((colors[i][2] - r)*0.299,2));
		if(dist < distance){
			distance = dist;
			min_idx = i;
		}
	}
	return min_idx;
}
int main(int argc, char* args[])
{	
	CvSVM SVM;
	SVM.load("trainedSVM");
	if(!SVM.get_support_vector_count()){
		int labels[LABEL_TOTAL*PER_LABEL];
		for(int i=0; i<LABEL_TOTAL; i++){
			for(int j=0; j<PER_LABEL; j++){
				labels[i*PER_LABEL + j] = i;
			}
		}
		Mat labelsMat(LABEL_TOTAL*PER_LABEL, 1, CV_32SC1, labels);


		// read images 
		// read their corresponding colors
		float *trainingData[LABEL_TOTAL*PER_LABEL];

		std::string dir("training/");
		for(int i=0; i<LABEL_TOTAL; i++){
			for(int j=0; j<PER_LABEL; j++){
				std::string filename = dir + flagname[i] + std::to_string(j+1) + std::string(".jpg");
				std::cout << filename << std::endl;
				Mat img = imread(filename);
				std::cout << img.rows << " " << img.cols << std::endl;
				float color_per_img[COLORS_C] = {0};
				for(int r=0; r<img.rows; r++){
					for(int c=0;c<img.cols; c++){
						Point3_<uchar>* p = img.ptr<Point3_<uchar> >(r,c);
						int ind = color_index(p);
						color_per_img[ind]++;
					}
				}
				for(int r=0;r<COLORS_C;r++){
					color_per_img[r] /= img.rows*img.cols;
					color_per_img[r] *=100;
					std::cout << color_per_img[r] << std::endl;
				}
				trainingData[i*PER_LABEL+j] = color_per_img;
			}
		} 

		Mat trainingDataMat(LABEL_TOTAL*PER_LABEL, COLORS_C, CV_32FC1, trainingData);

		std::cout << " training " << std::endl;

		// Set up SVM's parameters
		CvSVMParams params;
		params.svm_type    = CvSVM::C_SVC;
		params.kernel_type = CvSVM::LINEAR;
		params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 10500, 1e-6);

		// Train the SVM
		SVM.train_auto(trainingDataMat, labelsMat, Mat(), Mat(), params);

		std::cout << "trained" << std::endl;
		SVM.save("trainedSVM");
	}else{
		Mat img = imread(std::string("training/") + std::string(args[1]));
		std::cout << img.rows << " " << img.cols << std::endl;
		float color_per_img[COLORS_C] = {0};
		for(int r=0; r<img.rows; r++){
			for(int c=0;c<img.cols; c++){
				Point3_<uchar>* p = img.ptr<Point3_<uchar> >(r,c);
				int ind = color_index(p);
				color_per_img[ind]++;
			}
		}
		for(int r=0;r<COLORS_C;r++){
			color_per_img[r] /= img.rows*img.cols;
			color_per_img[r] *=100;
			std::cout << color_per_img[r] << std::endl;
		}
		
		Mat test(COLORS_C, 1, CV_32FC1, color_per_img);
		std::cout << SVM.predict(test) << std::endl;
	}
}
