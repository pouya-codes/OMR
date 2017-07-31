#include "answersheet.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QObject>
#include <zxing/common/Counted.h>
#include <zxing/Binarizer.h>
#include <zxing/MultiFormatReader.h>
#include <zxing/Result.h>
#include <zxing/ReaderException.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/Exception.h>
#include <zxing/common/IllegalArgumentException.h>
#include <zxing/BinaryBitmap.h>
#include <zxing/DecodeHints.h>
#include <zxing/qrcode/QRCodeReader.h>
#include <zxing/MultiFormatReader.h>
#include <zxing/MatSource.h>
#include <ctime>
#include <mutex>


using namespace zxing;
using namespace zxing::qrcode;



cv::Mat image,orginal_img ,removed_color_img,empty;
const char* wndname = "Remove Colors";
std::vector<std::vector<cv::Vec3b>> omit_colors ;
int image_width,image_height;
void mouseHandler (int event, int x, int y, int flags, void *userdata);
int color_omit_rang = 30 ;
int thresh_choice_value = 30;
int eye_number ;
int pad_rect , referenceEye, distanceWidth, distanceHeight, choiceWidth, choiceHeight,
choiceNumber,  distanceChoiceChoice, numberOfQuestions,  columnDistance ,
barcodeX,barcodeY,barcodeWidth,barcodeHeight;
cv::Size resize_size = cv::Size(1248,1755) ;
cv::Size orginal_size ;
Ref<Reader> reader;
QSqlDatabase db;
std::mutex mtx;

int temp_counter = 0 ;
int error_counter = 0 ;


std::vector<cv::Rect> left_eye,right_eye;
#define PI 3.14159265

bool AnswerSheet::createTable(QString tableName) {
    QSqlQuery query;
    return query.exec("create table " + tableName +
                      " (id integer PRIMARY KEY AUTOINCREMENT, "
                      "orginalFilePath varchar(300), "
                      "processedFilePath varchar(300), "
                      "code varchar(30) ,"
                      "answers varchar(300),date varchar(50),maxblackness integer) "
                      );
}


bool AnswerSheet::deleteTable(QString tableName) {
    QSqlQuery query;
    std::cout << query.exec("DROP TABLE " +tableName+";") << std::endl;
    return true ;
}



const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);


    strftime(buf, sizeof(buf), "%Y-%m-%d-%X", &tstruct);
    int millisec = rand();
    std::string time = std::string(buf) + std::to_string(millisec);
    return time;
}



AnswerSheet::AnswerSheet(cv::Mat img)
{
    orginal_size.width = img.cols ;
    orginal_size.height = img.rows ;
    img.copyTo(orginal_img);
    cv::resize(img,image,resize_size) ;
    image_width =  image.cols;
    image_height = image.rows;
    reader.reset(new MultiFormatReader);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("OMR_Results.db");
    bool ok = db.open();
    if(ok)
        std::cout <<"Database Opend" << std::endl ;
}

cv::Mat AnswerSheet::RemoveColors() {
    int res = RemoveColorsDialog();
    return image;

}


cv::Mat AnswerSheet::getImage() {
    return image ;
}

int AnswerSheet::DetectEyes (int pad_rectangle) {
    pad_rect =pad_rectangle ;
    cv::Rect left_rect = cv::Rect(0,0,pad_rect,image.rows);
    cv::Rect right_rect = cv::Rect(image.cols-pad_rect,0,pad_rect,image.rows);
    cv::vector<cv::vector<cv::Point>> squares_left,squares_right;
    int a = findSquares(image, squares_left,left_rect);
    int b = findSquares(image, squares_right,right_rect);

    if(a == b ) {
        eye_number =a ;

        sortSquares(squares_left,left_eye,LEFT);
        sortSquares(squares_right,right_eye,RIGHT);
        double angle = findAngle(squares_left,squares_right);
        rotateImage(image,angle,left_eye,right_eye);
        cv::resize(image,image,resize_size) ;

        int c = findSquares(image, squares_left,left_rect);
        int d = findSquares(image, squares_right,right_rect);
        if(c == d and c== a ) {
            sortSquares(squares_left,left_eye,LEFT);
            sortSquares(squares_right,right_eye,RIGHT);
            drawSquares(image, left_eye);
            drawSquares(image, right_eye);
        }


    }
    else {
        sortSquares(squares_left,left_eye,LEFT);
        sortSquares(squares_right,right_eye,RIGHT);
        drawSquares(image, left_eye);
        drawSquares(image, right_eye);
    }


    //


    //



    //    cv::Rect c1 = cv::Rect(left_eye[eye_number-1].x+choice_1.x,
    //            left_eye[eye_number-1].y+choice_1.y,
    //            choice_area.width,choice_area.height) ;
    //    cv::Rect c2 = cv::Rect(left_eye[eye_number-1].x+choice_2.x,
    //            left_eye[eye_number-1].y+choice_2.y,
    //            choice_area.width,choice_area.height) ;
}
cv::Mat AnswerSheet::DrawChoices (int referenceEye_,int distanceWidth_,int distanceHeight_,int choiceWidth_,int choiceHeight_,
                                  int choiceNumber_, int distanceChoiceChoice_, int numberOfQuestions_, int columnDistance_,
                                  int barcodeX_,int barcodeY_,int barcodeWidth_,int barcodeHeight_) {
    referenceEye =  referenceEye_;
    distanceWidth = distanceWidth_ ;
    distanceHeight = distanceHeight_ ;
    choiceWidth = choiceWidth_;
    choiceHeight = choiceHeight_;
    choiceNumber = choiceNumber_;
    distanceChoiceChoice = distanceChoiceChoice_;
    numberOfQuestions = numberOfQuestions_ ;
    columnDistance = columnDistance_;

    barcodeX =barcodeX_ ;
    barcodeY = barcodeY_;
    barcodeWidth = barcodeWidth_ ;
    barcodeHeight =barcodeHeight_;


    cv::Mat temp ;
    image.copyTo(temp);
    int ques_counter = 0 ;
    int row = 0; int column=0;
    while (ques_counter< numberOfQuestions) {
        for (int choice = 0; choice < choiceNumber; ++choice) {
            cv::Rect c1 = cv::Rect(left_eye[row+referenceEye].x+ distanceWidth+(choice * distanceChoiceChoice)+(column*columnDistance),
                    left_eye[row+referenceEye].y+distanceHeight  ,
                    choiceWidth,choiceHeight) ;
            cv::rectangle(temp,c1,cv::Scalar(255,0,0),2);
        }
        ques_counter++ ;
        row++ ;
        if(row+referenceEye == left_eye.size() ) {
            column++ ;
            row = 0;

        }
    }
    cv::rectangle(temp,cv::Rect(barcodeX,barcodeY,barcodeWidth,barcodeHeight),cv::Scalar(255,0,0),2);


    return temp ;

}

int AnswerSheet::RemoveColorsDialog() {
    cv::namedWindow( wndname, 1 );
    cv::setMouseCallback(wndname,mouseHandler) ;
    cv::imshow(wndname,image) ;
    char c = (char)cv::waitKey();
    if( c == 27 ) {
        cv::destroyAllWindows();
        return 0 ;

    }
    if(c== 'c') {
        cv::destroyAllWindows();
        return 1 ;
    }
}

void omitColors(cv::Mat& img) {

    cv::Mat mask;
    for (uint var = 0; var < omit_colors.size(); ++var) {
        omit_colors[var][0];

        cv::inRange(img,cv::Scalar(omit_colors[var][0][0],omit_colors[var][0][1],omit_colors[var][0][2]),
                cv::Scalar(omit_colors[var][1][0],omit_colors[var][1][1],omit_colors[var][1][2]), mask);
        img.setTo(cv::Scalar(255,255,255),mask);

    }
}
void mouseHandler (int event, int x, int y, int flags, void *userdata){
    if(event==cv::EVENT_FLAG_LBUTTON) {
        std::vector<cv::Vec3b> omit_color ;
        cv::Vec3b color = image.at<cv::Vec3b>(y,x) ;

        if (color!=cv::Vec3b(255,255,255)) {
            //            std::cout<< color << std::endl ;
            omit_color.push_back(cv::Vec3b(
                                     int(color[0])-color_omit_rang < 0 ? 0 : int(color[0])-color_omit_rang,
                    int(color[1])-color_omit_rang < 0 ? 0 : int(color[1])-color_omit_rang,
                    int(color[2])-color_omit_rang < 0 ? 0 : int(color[2])-color_omit_rang));
            omit_color.push_back(cv::Vec3b(
                                     int(color[0])+color_omit_rang > 255 ? 255 : int(color[0])+color_omit_rang,
                    int(color[1])+color_omit_rang > 255 ? 255 : int(color[1])+color_omit_rang,
                    int(color[2])+color_omit_rang > 255 ? 255 : int(color[2])+color_omit_rang));
            omit_colors.push_back(omit_color);
            omitColors(image) ;
            cv::imshow(wndname,image);
        }




    }

}

double angle( cv::Point pt1, cv::Point  pt2, cv::Point  pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

int AnswerSheet::findSquares( const cv::Mat& image, std::vector<std::vector<cv::Point> >& squares,cv::Rect area )
{
    squares.clear();

    cv::Mat thr ;
    cv::Mat roi_image= image(area) ;

    cv::cvtColor(roi_image,thr,cv::COLOR_BGR2GRAY) ;
    cv::threshold(thr,thr,80,255,cv::THRESH_BINARY) ;


    cv::Mat pyr, gray;

    pyrDown(thr, pyr, cv::Size(roi_image.cols/2, roi_image.rows/2));
    //    pyrDown(pyr, pyr, cv::Size(pyr.cols/2, pyr.rows/2));
    //    pyrUp(pyr, pyr, cv::Size(roi_image.cols/2, roi_image.rows/2));
    pyrUp(pyr, pyr, roi_image.size());


    int dilation_size = 3 ;

    cv::Mat element = getStructuringElement( cv::MORPH_RECT,
                                             cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                             cv::Point( dilation_size, dilation_size ) );


    cv::Canny(pyr, gray, 0, 30, 5);
    //    cv::dilate(gray, gray, cv::Mat(), cv::Point(-1,-1));
    cv::dilate(gray, gray, element);
    //    cv::Mat tttt ;
    //    cv::resize(gray,tttt,cv::Size(gray.cols/2, gray.rows/2)) ;
    //    cv::imshow(wndname,gray);
    //    cv::waitKey(0);


    cv::vector<cv::vector<cv::Point> > contours;
    cv::findContours(gray, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    cv::vector<cv::Point> approx;

    for( size_t i = 0; i < contours.size(); i++ )
    {
        cv::approxPolyDP(cv::Mat(contours[i])   , approx, arcLength(cv::Mat(contours[i]), true)*0.10, true);
        if( approx.size() == 4 &&
                fabs(contourArea(cv::Mat(approx))) > 200 && fabs(contourArea(cv::Mat(approx))) < 500 &&
                cv::isContourConvex(cv::Mat(approx)) )
        {
            double maxCosine = 0;

            for( int j = 2; j < 5; j++ )
            {

                double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                maxCosine = MAX(maxCosine, cosine);
            }

            if( maxCosine < 0.3 ){
                squares.push_back(approx);
            }
        }

    }
    return(squares.size());


}

void AnswerSheet::drawSquares( cv::Mat& image, std::vector<cv::Rect> rect_vector , cv::Scalar color )
{
    for( uint i = 0; i < rect_vector.size(); i++ )
    {
        cv::rectangle(image,rect_vector[i],color,2);
        cv::putText(image, std::to_string(i), cv::Point(rect_vector[i].x,rect_vector[i].y), CV_FONT_HERSHEY_SIMPLEX, 1,
                    cv::Scalar(0,0,255), 2, 1);

    }

}

void AnswerSheet::rotateImage(cv::Mat& image,double angle, std::vector<cv::Rect>& eyes_left , std::vector<cv::Rect>& eyes_right ) {
    // get rotation matrix for rotating the image around its center
    cv::Point2f center(image.cols/2.0, image.rows/2.0);
    cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);

    // determine bounding rectangle
    cv::Rect bbox = cv::RotatedRect(center,image.size(), angle).boundingRect();
    // adjust transformation matrix
    rot.at<double>(0,2) += bbox.width/2.0 - center.x;
    rot.at<double>(1,2) += bbox.height/2.0 - center.y;


    cv::warpAffine(image, image, rot, bbox.size(),cv::INTER_CUBIC,cv::BORDER_CONSTANT,
                   cv::Scalar(255, 255, 255));


    for (uint idx = 0; idx < eyes_left.size(); ++idx) {
        rotateRect(eyes_left[idx],rot) ;
        rotateRect(eyes_right[idx],rot) ;

    }


}

void AnswerSheet::sortSquares( const std::vector<std::vector<cv::Point> >& squares ,
                               std::vector<cv::Rect>& rect_vector , eye_pose pose )
{
    rect_vector.clear();
    for( int i = squares.size()-1; i >= 0; --i )
    {
        cv::Rect rect_area ;
        int min_x =  squares[i][0].x ;
        int max_x =  squares[i][0].x ;
        int min_y =  squares[i][0].y ;
        int max_y =  squares[i][0].y ;
        for (int point_idx = 0; point_idx < squares[i].size(); ++point_idx) {
            min_x = min_x < squares[i][point_idx].x ? squares[i][point_idx].x : min_x;
            max_x = max_x > squares[i][point_idx].x ? squares[i][point_idx].x : max_x;

            min_y = min_y < squares[i][point_idx].y ? squares[i][point_idx].y : min_y;
            max_y = max_y > squares[i][point_idx].y ? squares[i][point_idx].y : max_y;

        }

        if(pose==LEFT){
            rect_area = cv::Rect(
                        min_x,
                        min_y,
                        (max_x - min_x),
                        (max_y - min_y)) ;

        }

        else
        {
            rect_area = cv::Rect(
                        (image_width-pad_rect) + min_x,
                        min_y,
                        (max_x - min_x),
                        (max_y - min_y)) ;

            //            std::cout<< i <<" : " << squares[i] << "***" << p_1 << std::endl;
        }



        rect_vector.push_back(rect_area);

    }

}

double AnswerSheet::findAngle(const std::vector<std::vector<cv::Point> >& squares_left , const std::vector<std::vector<cv::Point> >& squares_right)
{


    int i = eye_number -1 ;
    cv::Point p_1 = squares_left[i][1] ;
    if(squares_left[i][0].x <= squares_left[i][1].x && squares_left[i][0].y <= squares_left[i][1].y) {
        p_1 = squares_left[i][0] ;
    }
    cv::Point p_2 = squares_right[i][1] ;
    if(squares_right[i][0].x <= squares_right[i][1].x && squares_right[i][0].y <= squares_right[i][1].y) {
        p_2 = squares_right[i][0] ;
    }
    //        std::cout<<p_2 << "-" << p_1 << std::endl ;
    return atan (double(p_2.y-p_1.y)/((p_2.x+image_width-pad_rect)-p_1.x) * 180 / PI);


}

void AnswerSheet::rotateRect(cv::Rect& rect, const cv::Mat & rot)
{
    std::vector<cv::Point2f> rot_roi_points;
    std::vector<cv::Point2f> roi_points = {
        {rect.x,              rect.y},
        {rect.x + rect.width, rect.y + rect.height}
    };
    transform(roi_points, rot_roi_points, rot);
    cv::Rect result= cv::Rect((int) round(rot_roi_points[0].x),
            (int) round(rot_roi_points[0].y),
            (int) round(rot_roi_points[1].x-rot_roi_points[0].x),
            (int) round(rot_roi_points[1].y-rot_roi_points[0].y)) ;
    rect = result ;

}

cv::Mat AnswerSheet::ProcessImage (cv::Mat img_process,QString table_name,std::string out_path_orginal,std::string out_path_processd,std::string out_path_error){
    AnswerSheet::OMRResult OMRresult ;

    cv::Mat img_resize,img_resized_omitcolors,img_resize_out ;
    double resize_factor = 1 ;

    if (img_process.cols!=resize_size.width || img_process.rows !=resize_size.height ) {
        resize_factor =  (double) img_process.cols /  (double)  resize_size.width;
        cv::resize(img_process,img_resize,resize_size) ;
    }
    else {
        img_process.copyTo(img_resize);
    }
    img_resize.copyTo(img_resized_omitcolors);
    omitColors(img_resized_omitcolors);

    cv::Rect left_rect = cv::Rect(0,0,pad_rect,img_resize.rows);
    cv::Rect right_rect = cv::Rect(img_resize.cols-pad_rect,0,pad_rect,img_resize.rows);

    std::vector<cv::Rect> left_eye_,right_eye_;


    cv::vector<cv::vector<cv::Point>> squares_left,squares_right;
    int a = findSquares(img_resized_omitcolors, squares_left,left_rect);
    int b = findSquares(img_resized_omitcolors, squares_right,right_rect);

    if (a==eye_number && b==eye_number){
        sortSquares(squares_left,left_eye_,LEFT);
        sortSquares(squares_right,right_eye_,RIGHT);
        double angle = findAngle(squares_left,squares_right);
        rotateImage(img_resize,angle,left_eye_,right_eye_);
        rotateImage(img_resized_omitcolors,angle,left_eye_,right_eye_);

        //        cv::resize(img_resize,img_resize,resize_size) ;
        //        cv::resize(img_resized_omitcolors,img_resized_omitcolors,resize_size) ;



        int c = findSquares(img_resized_omitcolors, squares_left,left_rect);
        int d = findSquares(img_resized_omitcolors, squares_right,right_rect);
        sortSquares(squares_left,left_eye_,LEFT);
        sortSquares(squares_right,right_eye_,RIGHT);

        img_resize.copyTo(img_resize_out);

        //        drawSquares(img_resize_out, left_eye_,cv::Scalar(255,0,0));
        //        drawSquares(img_resize_out, right_eye_,cv::Scalar(255,0,0));

        if(c == d and c== a ) {


            OMRresult = readChoices(img_resize_out,img_resized_omitcolors,left_eye_,right_eye_) ;
        }
        else {
            std::string file_name ;
            file_name = out_path_error+ currentDateTime() + ".jpg";
            cv::imwrite(file_name,img_process) ;
            return  empty;
        }

        //read barcode
        std::string barcode = " " ;
        cv::Mat gray_barcode;

        cv::Mat(img_process(cv::Rect(barcodeX*resize_factor,barcodeY*resize_factor,barcodeWidth*resize_factor,barcodeHeight*resize_factor))).copyTo(gray_barcode);
        cv::cvtColor(gray_barcode,gray_barcode,cv::COLOR_BGR2GRAY) ;
        //        cv::imwrite("ZXing.jpg", gray_barcode);

        int resultPointCount= 0 ;
        try {
            mtx.lock();
            Ref<LuminanceSource> source = MatSource::create(gray_barcode);
            Ref<Binarizer> binarizer(new GlobalHistogramBinarizer(source));
            Ref<BinaryBitmap> bitmap(new BinaryBitmap(binarizer));
            Ref<Result> result(reader->decode(bitmap, DecodeHints(DecodeHints::TRYHARDER_HINT)));
            resultPointCount = result->getResultPoints()->size();

            if (resultPointCount > 0) {
                barcode =  result->getText()->getText();
                cv::putText(img_resize_out, barcode, cv::Point(barcodeX,barcodeY+barcodeHeight), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar( 255, 0, 0 ),2);
            }
            mtx.unlock();
        }      catch (const ReaderException& e) {
            std::cout << e.what() << " (ignoring)" << std::endl;
        } catch (const zxing::IllegalArgumentException& e) {
            std::cout << e.what() << " (ignoring)" << std::endl;
        } catch (const zxing::Exception& e) {
            std::cout << e.what() << " (ignoring)" << std::endl;
        } catch (const std::exception& e) {
            std::cout << e.what() << " (ignoring)" << std::endl;
        }



        if (resultPointCount == 0)  {
            barcode= currentDateTime() ;
        }
        std::string file_name_org , file_name_proc ;

        file_name_proc = out_path_processd + barcode + ".jpg";
        cv::imwrite(file_name_proc,img_resize_out) ;

        file_name_org = out_path_orginal + barcode + ".jpg";
        cv::imwrite(file_name_org,img_process) ;



        QSqlQuery query;
        QString querytxt = "INSERT INTO " + table_name + " ( code, answers,orginalFilePath,processedFilePath,date,maxblackness) "
                                                         "VALUES ( :code, :answers , :orginalFilePath ,:processedFilePath,:date,:maxblackness)" ;
        query.prepare(querytxt);
        query.bindValue(":code", resultPointCount == 0 ? " " : QString::fromStdString( barcode));
        query.bindValue(":answers", QString::fromStdString(OMRresult.choices));
        query.bindValue(":orginalFilePath", QString::fromStdString(file_name_org));
        query.bindValue(":processedFilePath", QString::fromStdString(file_name_proc));
        query.bindValue(":date", QString::fromStdString(currentDateTime()));
        query.bindValue(":maxblackness",OMRresult.blackness);

        query.exec();

        //                std::cout <<OMRresult.choices << std::endl;

        return img_resize_out ;



    }
    else {

        std::string file_name ;
        file_name = out_path_error+ currentDateTime() + ".jpg";
        cv::imwrite(file_name,img_process) ;
        return  empty;



    }
}

AnswerSheet::OMRResult AnswerSheet::readChoices(cv::Mat& img_org ,cv::Mat& img ,std::vector<cv::Rect> left_eye_,std::vector<cv::Rect> right_eye_){
    cv::Mat threshold ;
    //    img.copyTo(temp);
    int ques_counter = 0 ;  int row = 0; int column=0;
    std::string answer_string = "" ;

    cv::cvtColor(img,threshold,cv::COLOR_RGB2GRAY) ;
    cv::threshold(threshold,threshold,220,255,cv::THRESH_BINARY_INV) ;

    //    int max_values [numberOfQuestions] ;
    int min = 1000 ;
    int max = 100 ;
    while (ques_counter< numberOfQuestions) {

        int choice_value [choiceNumber] ;
        for (int choice = 0; choice < choiceNumber; ++choice) {
            cv::Rect c1 = cv::Rect(left_eye_[row+referenceEye].x+ distanceWidth+(choice * distanceChoiceChoice)+(column*columnDistance),
                    ((left_eye_[row+referenceEye].y+distanceHeight)+(right_eye_[row+referenceEye].y+distanceHeight))/2  ,
                    choiceWidth,choiceHeight) ;
            cv::Mat choice_roi = threshold(c1);
            choice_value[choice] = cv::sum(choice_roi)[0]/c1.area()  ;
            if (cv::sum(choice_roi)[0]/c1.area() > max ) max = cv::sum(choice_roi)[0]/c1.area() ;
            if (cv::sum(choice_roi)[0]/c1.area() < min ) min = cv::sum(choice_roi)[0]/c1.area() ;
            //            std::cout << choice_value[choice] << "- max = " << max  << std::endl ;
        }
        //        int* max_choice_value = std::max_element(choice_value,choice_value+choiceNumber);
        //        if(*max_choice_value > thresh_choice_value) {
        //            max_values[ques_counter] = *max_choice_value ;

        //        }
        //        else {
        //            max_values[ques_counter] = 0 ;
        //        }
        ques_counter++ ;
        row++ ;
        if(row+referenceEye == left_eye_.size() ) {
            column++ ;
            row = 0;

        }
    }
    //    std::cout << "max = " << max << " min = " << min << std::endl ;

    /*
    int sums = 0 ;
    int max_numbers = 0 ;
    for (int var = 0; var < numberOfQuestions; ++var) {
        if(max_values[var]!=0) {
            sums+= max_values[var] ;
            max_numbers+=1 ;
        }

    }
    int threshold_value=0 ;
    if (max_numbers!=0) {
        threshold_value = (sums/max_numbers) ;
        threshold_value-= threshold_value/2 ;
    }

    std::cout << "Mean" <<threshold_value << std::endl ;
    if(threshold_value<(thresh_choice_value/2)) {
        threshold_value = thresh_choice_value/2 ;
    }
    */

    ques_counter = 0 ;   row = 0;  column=0;


    while (ques_counter< numberOfQuestions) {
        bool ischoiced = false ; bool twochoice = false ;
        int selected_choice = 0 ;

        int choice_value [choiceNumber] ;
        for (int choice = 0; choice < choiceNumber; ++choice) {
            cv::Rect c1 = cv::Rect(left_eye_[row+referenceEye].x+ distanceWidth+(choice * distanceChoiceChoice)+(column*columnDistance),
                    ((left_eye_[row+referenceEye].y+distanceHeight)+(right_eye_[row+referenceEye].y+distanceHeight))/2   ,
                    choiceWidth,choiceHeight) ;
            cv::Mat choice_roi = threshold(c1);
            choice_value[choice] = (((cv::sum(choice_roi)[0]/c1.area())-min)/(max-min)) * 100 ; // normalize choice blackness
            //            std::cout << (ques_counter+1) << "-" << (choice+1) << "=" << choice_value[choice] << std::endl ;
        }

        int* max_choice = std::max_element(choice_value,choice_value+choiceNumber);
        if (*max_choice>thresh_choice_value) { //choiced
            int max_idx = std::distance(choice_value, max_choice) ;
            for (int choice = 0; choice < choiceNumber; ++choice) {
                if ( choice!= max_idx &&  ((*max_choice-choice_value[choice])<(thresh_choice_value/(3)) || choice_value[choice]>75)) {
                    twochoice=true ;
                    cv::Rect c1 = cv::Rect(left_eye_[row+referenceEye].x+ distanceWidth+(choice * distanceChoiceChoice)+(column*columnDistance),
                            ((left_eye_[row+referenceEye].y+distanceHeight)+(right_eye_[row+referenceEye].y+distanceHeight))/2  ,
                            choiceWidth,choiceHeight) ;

                    cv::rectangle(img_org,c1,cv::Scalar(0,0,255),2);
                    c1 = cv::Rect(left_eye_[row+referenceEye].x+ distanceWidth+(max_idx * distanceChoiceChoice)+(column*columnDistance),
                            ((left_eye_[row+referenceEye].y+distanceHeight)+(right_eye_[row+referenceEye].y+distanceHeight))/2   ,
                            choiceWidth,choiceHeight) ;

                    cv::rectangle(img_org,c1,cv::Scalar(0,0,255),2);
                }


            }
            if (!twochoice) {
                selected_choice = max_idx+1 ;
                ischoiced = true ;
                cv::Rect c1 = cv::Rect(left_eye_[row+referenceEye].x+ distanceWidth+(max_idx * distanceChoiceChoice)+(column*columnDistance),
                        ((left_eye_[row+referenceEye].y+distanceHeight)+(right_eye_[row+referenceEye].y+distanceHeight))/2   ,
                        choiceWidth,choiceHeight) ;

                cv::rectangle(img_org,c1,cv::Scalar(0,255,0),2);

            }


        }
        else {
            for (int choice = 0; choice < choiceNumber; ++choice) {
                cv::Rect c1 = cv::Rect(left_eye_[row+referenceEye].x+ distanceWidth+(choice * distanceChoiceChoice)+(column*columnDistance),
                        ((left_eye_[row+referenceEye].y+distanceHeight)+(right_eye_[row+referenceEye].y+distanceHeight))/2   ,
                        choiceWidth,choiceHeight) ;
                cv::rectangle(img_org,c1,cv::Scalar(255,0,0),2);
            }

        }

        if (twochoice) {
            answer_string+='*';
        }
        else if (ischoiced) {
            answer_string+=std::to_string(selected_choice) ;
        }
        else {
            answer_string+=' ';
        }


        ques_counter++ ;
        row++ ;

        if(row+referenceEye == left_eye_.size() ) {
            column++ ;
            row = 0;

        }
    }
    AnswerSheet::OMRResult result= {answer_string,max} ;
    return result;




}



