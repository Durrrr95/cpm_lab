#include "IpsPipeline.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>


IpsPipeline::IpsPipeline()
{
    undistortPointsFn = std::make_shared<UndistortPoints>(
        std::vector<double>{4.351151e+00, -4.863260e+00, -1.183271e-01, 1.094383e+00, 4.861627e-01, 1.418471e-01, -6.700942e-01, 3.049470e-01, -4.362105e-01, 1.852765e-01, -7.676803e-02, -7.289893e-02, -1.313038e-01, -8.945004e-02, -5.608838e-02},
        std::vector<double>{-3.409322e-01, 5.226075e-01, 4.909991e+00, -3.901540e-01, -5.209485e-01, -1.093096e+00, 6.016393e-02, 5.101132e-01, -3.097603e-01, 5.542172e-01, -2.875253e-02, 5.073364e-02, 1.783163e-01, 1.005458e-01, 1.114947e-01}
    );

    VehiclePointSet dummy;
    detectVehiclesFn = std::make_shared<DetectVehicles>(dummy);
}


void IpsPipeline::apply(LedPoints led_points)
{
    std::cout << "recvd " << led_points.led_points().size() << " LEDs" << std::endl;

    FloorPoints floorPoints = undistortPointsFn->apply(led_points);

    VehiclePoints vehiclePoints = detectVehiclesFn->apply(floorPoints);




    // Visualization
    {
        const int vis_img_width = 1700;
        const int vis_img_height = 1000;
        cv::Mat image(vis_img_height, vis_img_width, CV_8UC3, cv::Scalar(255,255,255));

        // Transformation from floor coordinates
        // to visualization image coordiantes.
        auto transform = [](cv::Point2d floor_point)->cv::Point{
            return cv::Point(
                (floor_point.x - 2.25) * 220 + vis_img_width/2,
                vis_img_height/2 - ((floor_point.y - 2.0) * 220)
            );
        };

        // Draw outline box
        cv::line(image, transform(cv::Point2d(0, 0)), transform(cv::Point2d(4.5, 0)),cv::Scalar(0,0,0),3);
        cv::line(image, transform(cv::Point2d(4.5, 4.0)), transform(cv::Point2d(4.5, 0)),cv::Scalar(0,0,0),3);
        cv::line(image, transform(cv::Point2d(4.5, 4.0)), transform(cv::Point2d(0, 4.0)),cv::Scalar(0,0,0),3);
        cv::line(image, transform(cv::Point2d(0,0)), transform(cv::Point2d(0, 4.0)),cv::Scalar(0,0,0),3);


        // Draw floor points
        for(auto point : floorPoints.points)
        {
            cv::circle(image,transform(point),3,cv::Scalar(0,0,255),-1,cv::LINE_AA);
        }


        // Show image
        cv::imshow("IPS Visualization", image);
        if(cv::waitKey(1) == 27) // close on escape key
        {
            exit(0);
        }

    }
}
