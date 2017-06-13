#include "mainwindow.h"
#include <QApplication>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml.hpp>


//#include <iostream>
//#include <math.h>
//#include <string.h>
//#include <dirent.h>
//#include <string.h>
//#include <boost/filesystem.hpp>
//#include <boost/algorithm/string.hpp>
//#include <stdlib.h>
//#include <fstream>

//#include <zbar.h>



//using namespace cv;
//using namespace std;
//using namespace zbar;

//#define PI 3.14159265

////ImageScanner scanner;

//string input_path = "/home/pouya/Qom/Adabi/qom.adabi.XSM/" ;
//string result_file = "error_test.txt" ;
//string result_file_error = "test_error.txt" ;

//std::string olampiad_name = "test_error";


//int thresh = 30;
//int color_omit_range = 30 ;
//const char* wndname = "Square Detection Demo";
//double resize_factor = 0.6 ;

//Mat image;
//std::vector<std::vector<cv::Vec3b>> omit_colors ;
//cv::Rect right_rect ;
//cv::Rect left_rect  ;
//int image_height ;
//int image_width;
//enum eye_pose {LEFT, RIGHT };

//int pad_rect = 90;
//int num_squre = 30 ;
////zist computer bonyadi shimi riyazi nojum
////int eye_number = 10 ;
////cv::Rect choice_area = cv::Rect(0,0,40,28) ;
////cv::Point choice_1 = cv::Point(110,55) ;
////cv::Point choice_2 = cv::Point(175,55);

////adabi
//int eye_number = 6 ;
//cv::Rect choice_area = cv::Rect(0,0,40,28) ;
//cv::Point choice_1 = cv::Point(107,52) ;
//cv::Point choice_2 = cv::Point(172,52);

//static double angle( Point pt1, Point pt2, Point pt0 )
//{
//    double dx1 = pt1.x - pt0.x;
//    double dy1 = pt1.y - pt0.y;
//    double dx2 = pt2.x - pt0.x;
//    double dy2 = pt2.y - pt0.y;
//    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
//}

//static int findSquares( const Mat& image, vector<vector<Point> >& squares,cv::Rect area )
//{
//    squares.clear();

//    cv::Mat thr ;
//    cv::Mat roi_image= image(area) ;


//    cv::cvtColor(roi_image,thr,cv::COLOR_BGR2GRAY) ;

//    cv::threshold(thr,thr,100,255,cv::THRESH_BINARY) ;

////    cv::imshow(wndname,thr);
////    cv::waitKey(0);

//    Mat pyr, timg,gray;

//    pyrDown(thr, pyr, Size(roi_image.cols/2, roi_image.rows/2));
//    pyrUp(pyr, timg, roi_image.size());

//    //    int erosion_size =1 ;

//    //    int erosion_type = MORPH_RECT;
//    //    Mat element = getStructuringElement( erosion_type,
//    //                                         Size( 2*erosion_size + 1, 2*erosion_size+1 ),
//    //                                         Point( erosion_size, erosion_size ) );
//    //    erode( timg, timg, element );

//    vector<vector<Point> > contours;


//    Canny(timg, gray, 0, thresh, 5);
//    dilate(gray, gray, Mat(), Point(-1,-1));

//    findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

//    vector<Point> approx;

//    for( size_t i = 0; i < contours.size(); i++ )
//    {
//        approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.10, true);
//        //        1000,1700
//        if( approx.size() == 4 &&
//                fabs(contourArea(Mat(approx))) > 250 && fabs(contourArea(Mat(approx))) < 500 &&
//                isContourConvex(Mat(approx)) )
//        {
//            double maxCosine = 0;

//            for( int j = 2; j < 5; j++ )
//            {

//                double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
//                maxCosine = MAX(maxCosine, cosine);
//            }

//            if( maxCosine < 0.3 ){
//                squares.push_back(approx);
//            }
//        }

//    }
//    return(squares.size());


//}



//static void drawSquares( Mat& image, vector<cv::Rect> rect_vector  )
//{
//    for( uint i = 0; i < rect_vector.size(); i++ )
//    {
//        cv::rectangle(image,rect_vector[i],Scalar(0,255,0),2);

//    }

//}

//static void sortSquares( const vector<vector<Point> >& squares ,
//                         vector<cv::Rect>& rect_vector , eye_pose pose )
//{
//    rect_vector.clear();
//    for( int i = squares.size()-1; i > -1; --i )
//    {
//        cv::Rect rect_area ;
//        int p_1 = 1 ;
//        int p_2 = 3;
//        if(squares[i][0].x <= squares[i][1].x && squares[i][0].y <= squares[i][1].y) {
//            p_1=0 ;
//            p_2=2 ;
//        }
//        if(pose==LEFT)
//            rect_area = cv::Rect(
//                        squares[i][p_1].x,
//                        squares[i][p_1].y,
//                        (int)fabs(squares[i][p_2].x-squares[i][p_1].x),
//                        (int)fabs(squares[i][p_2].y-squares[i][p_1].y)) ;
//        else
//            rect_area = cv::Rect(
//                        (image_width-pad_rect) + squares[i][p_1].x,
//                        squares[i][p_1].y,
//                        (int)fabs(squares[i][p_2].x-squares[i][p_1].x),
//                        (int)fabs(squares[i][p_2].y-squares[i][p_1].y)) ;

//        rect_vector.push_back(rect_area);

//    }

//}



//static double findAngle(const vector<vector<Point> >& squares_left , const vector<vector<Point> >& squares_right)
//{


//    int i = num_squre -1 ;
//    cv::Point p_1 = squares_left[i][1] ;
//    if(squares_left[i][0].x <= squares_left[i][1].x && squares_left[i][0].y <= squares_left[i][1].y) {
//        p_1 = squares_left[i][0] ;
//    }
//    cv::Point p_2 = squares_right[i][1] ;
//    if(squares_right[i][0].x <= squares_right[i][1].x && squares_right[i][0].y <= squares_right[i][1].y) {
//        p_2 = squares_right[i][0] ;
//    }
//    //    std::cout<<p_2 << "-" << p_1 << std::endl ;
//    return atan (double(p_2.y-p_1.y)/((p_2.x+image_width-pad_rect)-p_1.x) * 180 / PI);


//}

//static void omitColors() {

//    cv::Mat mask;
//    for (uint var = 0; var < omit_colors.size(); ++var) {
//        omit_colors[var][0];

//        cv::inRange(image,cv::Scalar(omit_colors[var][0][0],omit_colors[var][0][1],omit_colors[var][0][2]),
//                cv::Scalar(omit_colors[var][1][0],omit_colors[var][1][1],omit_colors[var][1][2]), mask);
//        image.setTo(cv::Scalar(255,255,255),mask);

//    }
//}
//static void rotateRect(cv::Rect& rect, const cv::Mat & rot)
//{
//    vector<Point2f> rot_roi_points;
//    vector<Point2f> roi_points = {
//        {rect.x,              rect.y},
//        {rect.x + rect.width, rect.y + rect.height}
//    };
//    transform(roi_points, rot_roi_points, rot);
//    cv::Rect result= cv::Rect((int) round(rot_roi_points[0].x),
//            (int) round(rot_roi_points[0].y),
//            (int) round(rot_roi_points[1].x-rot_roi_points[0].x),
//            (int) round(rot_roi_points[1].y-rot_roi_points[0].y)) ;
//    rect = result ;

//}
//static void rotateImage(Mat& image,double angle, vector<cv::Rect>& eyes_left , vector<cv::Rect>& eyes_right ) {
//    // get rotation matrix for rotating the image around its center
//    cv::Point2f center(image.cols/2.0, image.rows/2.0);
//    cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);

//    // determine bounding rectangle
//    cv::Rect bbox = cv::RotatedRect(center,image.size(), angle).boundingRect();
//    // adjust transformation matrix
//    rot.at<double>(0,2) += bbox.width/2.0 - center.x;
//    rot.at<double>(1,2) += bbox.height/2.0 - center.y;


//    cv::warpAffine(image, image, rot, bbox.size(),cv::INTER_LINEAR,cv::BORDER_CONSTANT,
//                   cv::Scalar(255, 255, 255));


//    for (uint idx = 0; idx < eyes_left.size(); ++idx) {
//        rotateRect(eyes_left[idx],rot) ;
//        rotateRect(eyes_right[idx],rot) ;

//    }


//}

//void mouseHandler (int event, int x, int y, int flags, void *userdata){
//    if(event==cv::EVENT_FLAG_LBUTTON) {
//        std::vector<cv::Vec3b> omit_color ;
//        cv::Vec3b color = image.at<cv::Vec3b>(y,x) ;
//        //        std::cout<< color << std::endl ;
//        omit_color.push_back(cv::Vec3b(
//                                 int(color[0])-color_omit_range < 0 ? 0 : int(color[0])-color_omit_range,
//                int(color[1])-color_omit_range < 0 ? 0 : int(color[1])-color_omit_range,
//                int(color[2])-color_omit_range < 0 ? 0 : int(color[2])-color_omit_range));
//        omit_color.push_back(cv::Vec3b(
//                                 int(color[0])+color_omit_range > 255 ? 255 : int(color[0])+color_omit_range,
//                int(color[1])+color_omit_range > 255 ? 255 : int(color[1])+color_omit_range,
//                int(color[2])+color_omit_range > 255 ? 255 : int(color[2])+color_omit_range));
//        omit_colors.push_back(omit_color);
//        omitColors() ;
//        cv::imshow(wndname,image);

//    }

//}

//int main(int argc, char** argv)

//{

//    namedWindow( wndname, 1 );
//    vector<vector<Point>> squares_left;
//    vector<vector<Point>> squares_right;

//    if(!boost::filesystem::exists("results/"+olampiad_name)) {
//        boost::filesystem::create_directory("results/"+olampiad_name) ;
//    }

//    std::ofstream ofs;
//    ofs.open (result_file.c_str(), std::ofstream::out | std::ofstream::app);
//    std::ofstream ofs_error;
//    ofs_error.open (result_file_error.c_str(), std::ofstream::out | std::ofstream::app);

//    DIR *dir;
//    struct dirent *ent;
//    bool first_run =true ;

//    int counter = 0 ;
//    if ((dir = opendir (input_path.c_str())) != NULL) {
//        while ((ent = readdir (dir)) != NULL) {

//            std::string temp = ent->d_name ;
//            std::string file_name = input_path + temp ;
////            std::cout << file_name << std::endl;
//            image = imread(file_name, 1);
//            if( image.empty() )
//            {
//                cout << "Couldn't load " << file_name << endl;
//                continue;
//            }

//            cv::resize(image,image,Size(0,0),0.5,0.5) ;
//            if(first_run) {
//                cv::Mat copy_image ;
//                image.copyTo(copy_image);
//                left_rect = Rect(0,0,pad_rect,image.rows);
//                right_rect = Rect(image.cols-pad_rect,0,pad_rect,image.rows);
//                image_width =  image.cols;
//                image_height = image.rows;
//                cv::setMouseCallback(wndname,mouseHandler) ;
//                cv::rectangle(image,right_rect,cv::Scalar(0,255,0),2);
//                cv::rectangle(image,left_rect,cv::Scalar(0,255,0),2);
//                cv::resize(image,image,cv::Size(0,0),resize_factor,resize_factor) ;
//                cv::imshow(wndname,image) ;
//                char c = (char)waitKey();
//                if( c == 27 ) {
//                    break ;

//                }
//                if(c== 'c') {
//                    copy_image.copyTo(image);
//                    cv::destroyWindow(wndname);
//                    first_run=false ;
//                }

//            }


//            if(image.cols!=image_width || image.rows!=image_height) {
//                cv::resize(image,image,Size(image_width,image_height)) ;
//            }

//            omitColors();

//            int a = findSquares(image, squares_left,left_rect);
//            int b = findSquares(image, squares_right,right_rect);

//            vector<Rect> left_eye;
//            vector<Rect> right_eye;

//            sortSquares(squares_left,left_eye,LEFT);
//            sortSquares(squares_right,right_eye,RIGHT);

//            if(a!=num_squre || b!=num_squre) {
//                std::cout <<a <<"-"<<b <<std::endl;
//                if(!boost::filesystem::exists("results/"+olampiad_name+"/"+temp))
//                    boost::filesystem::copy(file_name,"results/"+olampiad_name+"/"+temp) ;
//                drawSquares(image, left_eye);
//                drawSquares(image, right_eye);
//                vector<string> strs;
//                boost::split(strs,temp,boost::is_any_of("."));
//                std::string out_file = "results/"+olampiad_name+"/"+strs[0]+"_"+".jpg" ;
//                cv::imwrite(out_file,image);
//                ofs_error << strs[0] << "," << std::endl ;
////                cv::Mat resized ;
////                cv::resize(image,resized,cv::Size(0,0),resize_factor,resize_factor) ;
////                cv::imshow(wndname,resized) ;
////                char c = (char)waitKey(0);
////                if( c == 27 )
////                    break;
////                cv::destroyWindow(wndname);
//                continue;
//            }
//            double angle = findAngle(squares_left,squares_right);

//            //            std::cout << angle <<std::endl;

//            rotateImage(image,angle,left_eye,right_eye);



//            cv::Rect c1 = cv::Rect(left_eye[eye_number-1].x+choice_1.x,
//                    left_eye[eye_number-1].y+choice_1.y,
//                    choice_area.width,choice_area.height) ;
//            cv::Rect c2 = cv::Rect(left_eye[eye_number-1].x+choice_2.x,
//                    left_eye[eye_number-1].y+choice_2.y,
//                    choice_area.width,choice_area.height) ;

//            cv::Mat choice_1_pixels = image(c1) ;
//            cv::Mat choice_2_pixels = image(c2) ;

//            cv::Mat th1,th2 ;

//            cv::threshold(choice_1_pixels,th1,200,255,THRESH_BINARY_INV) ;
//            cv::threshold(choice_2_pixels,th2,200,255,THRESH_BINARY_INV) ;
//            std::string answer = "" ;
//            if(cv::sum(th1)[0]>80000) {
//                answer+= ",1" ;
//            }
//            if(cv::sum(th2)[0]>80000) {
//                answer+= ",2" ;
//            }
////            std::cout << answer << std::endl;

//            vector<string> strs;
//            boost::split(strs,temp,boost::is_any_of("."));

//            ofs << strs[0] << answer << std::endl;
////            std::cout << answer << std::endl;
//            counter++ ;
//            if(counter%500 == 0){
//                std::cout << counter << " files processed ..." << std::endl;
//                ofs.flush();
//                ofs_error.flush();
//            }


////            std::cout<< cv::sum(th1)[0] << "-" <<cv::sum(th2)[0] << std::endl ;

////            cv::rectangle(image,left_eye[eye_number-1],cv::Scalar(0,255,0),4) ;

////            cv::rectangle(image,c1,cv::Scalar(0,255,0),4) ;
////            cv::rectangle(image,c2,cv::Scalar(0,0,255),4) ;

////            cv::resize(image,image,cv::Size(0,0),resize_factor,resize_factor) ;
////            cv::imshow(wndname,image) ;
////            cv::waitKey(0) ;



//        }
//        closedir (dir);
//    } else {
//        /* could not open directory */
//        perror ("");
//        return EXIT_FAILURE;
//    }
//    ofs.close();
//    ofs_error.close();

//    std::cout << "finished" << std::endl ;

//    return 0;
//}



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    a.exec();
}
