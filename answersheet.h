#ifndef ANSWERSHEET_H
#define ANSWERSHEET_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml.hpp>
#include <QSqlQuery>
#include <sqlite3.h>
#include <QSqlQueryModel>

enum eye_pose {LEFT, RIGHT };



class AnswerSheet
{
public:
    AnswerSheet(cv::Mat img);
    cv::Mat RemoveColors () ;
    int DetectEyes (int pad_rectangle) ;
    cv::Mat getImage();
    cv::Mat DrawChoices (int referenceEye,int distanceWidth,int distanceHeight,int choiceWidth,int choiceHeight,
                         int choiceNumber, int distanceChoiceChoice, int numberOfQuestions, int columnDistance,
                         int barcodeX_,int barcodeY_,int barcodeWidth_,int barcodeHeight_) ;
    cv::Mat ProcessImage (cv::Mat img_process);


private :
    int RemoveColorsDialog () ;
    int findSquares( const cv::Mat& image, std::vector<std::vector<cv::Point> >& squares,cv::Rect area );
    void drawSquares( cv::Mat& image, cv::vector<cv::Rect> rect_vector );
    void sortSquares( const std::vector<std::vector<cv::Point> >& squares ,
                             std::vector<cv::Rect>& rect_vector , eye_pose pose );
    double findAngle(const std::vector<std::vector<cv::Point> >& squares_left , const std::vector<std::vector<cv::Point> >& squares_right);
    void rotateImage(cv::Mat& image,double angle, std::vector<cv::Rect>& eyes_left , std::vector<cv::Rect>& eyes_right );
    void rotateRect(cv::Rect& rect, const cv::Mat & rot);
    std::string readChoices(cv::Mat& img_org ,cv::Mat& img ,std::vector<cv::Rect> left_eye_,std::vector<cv::Rect> right_eye_);


};

#endif // ANSWERSHEET_H
