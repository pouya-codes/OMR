#ifndef ANSWERSHEET_H
#define ANSWERSHEET_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml.hpp>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QFileDialog>
#include <QDir>
#include <QObject>
#include <QString>
#include <thread>
#include <mutex>

enum eye_pose {LEFT, RIGHT,BOTH };



class AnswerSheet
{
    struct OMRResult
    {
        std::string choices;
        int blackness;
        QString choiceAreas ;
    };
public:
    AnswerSheet(cv::Mat img);
    cv::Mat RemoveColors () ;
    void DetectEyes (int pad_rectangle, int darkness_threshold,int min_squre, int max_squre) ;
    cv::Mat getImage();
    cv::Mat DrawChoices (int referenceEye,int distanceWidth,int distanceHeight,int choiceWidth,int choiceHeight,
                         int choiceNumber, int distanceChoiceChoice, int numberOfQuestions, int columnDistance,
                         int barcodeX_,int barcodeY_,int barcodeWidth_,int barcodeHeight_,bool row_question_order_,int rowDistance_,
                         bool has_code_,int code_refrenceEye_,int code_numCode_,int code_distanceWidth_,int code_distanceHeight_, int code_distanceChoice_) ;
    cv::Mat ProcessImage (cv::String imagePath,QString table_name,std::string out_path_orginal,std::string out_path_processd,std::string out_path_error,int thread_no, bool remove_processed_file);
    bool createTable(QString tableName) ;
    bool deleteTable(QString tableName) ;
    bool clearTable(QString tableName) ;
    bool clearOmitedColors() ;
    void openDB(QString dbName);


private :
    std::mutex mtx;
    void omitColors(cv::Mat& img);
    void RemoveColorsDialog () ;
    int findSquares( const cv::Mat& image, std::vector<std::vector<cv::Point> >& squares,cv::Rect area );
    void drawSquares( cv::Mat& image, cv::vector<cv::Rect> rect_vector , cv::Scalar color =cv::Scalar(0,255,0) );
    void sortSquares( const std::vector<std::vector<cv::Point> >& squares ,
                      std::vector<cv::Rect>& rect_vector , eye_pose pose );
    double findAngle(const std::vector<std::vector<cv::Point> >& squares_left , const std::vector<std::vector<cv::Point> >& squares_right);
    void rotateImage(cv::Mat& image,double angle, std::vector<cv::Rect>& eyes_left , std::vector<cv::Rect>& eyes_right );
    void rotateImage(cv::Mat& image,double angle);
    void rotateRect(cv::Rect& rect, const cv::Mat & rot);
    AnswerSheet::OMRResult readChoices(cv::Mat& img_org ,cv::Mat& img ,std::vector<cv::Rect> left_eye_,std::vector<cv::Rect> right_eye_);



};

#endif // ANSWERSHEET_H
