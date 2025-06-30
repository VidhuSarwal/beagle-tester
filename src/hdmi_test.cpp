
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <chrono>

bool isColorful(const cv::Mat& frame) {
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    cv::Scalar lower_red1(0, 100, 100), upper_red1(10, 255, 255);
    cv::Scalar lower_red2(160, 100, 100), upper_red2(180, 255, 255);
    cv::Scalar lower_blue(100, 100, 100), upper_blue(130, 255, 255);
    cv::Scalar lower_green(40, 100, 100), upper_green(70, 255, 255);

    cv::Mat mask_red1, mask_red2, mask_blue, mask_green;
    cv::inRange(hsv, lower_red1, upper_red1, mask_red1);
    cv::inRange(hsv, lower_red2, upper_red2, mask_red2);
    cv::inRange(hsv, lower_blue, upper_blue, mask_blue);
    cv::inRange(hsv, lower_green, upper_green, mask_green);

    cv::Mat mask_red = mask_red1 | mask_red2;

    int total_pixels = cv::countNonZero(mask_red) +
                       cv::countNonZero(mask_blue) +
                       cv::countNonZero(mask_green);

    return total_pixels > 5000;
}

int main(int argc, char** argv) {
    bool save_frame = (argc > 1 && std::string(argv[1]) == "-s");

    cv::VideoCapture cap("/dev/video0", cv::CAP_V4L2);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open /dev/video0\n";
        return 1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_FPS, 5);

    std::cout << "Waiting for colorful activity (for up to 20 seconds)...\n";

    auto start_time = std::chrono::steady_clock::now();
    cv::Mat frame;

    while (true) {
        cap >> frame;
        if (frame.empty()) continue;

        if (isColorful(frame)) {
            std::cout << "Colorful activity detected!\n";
            if (save_frame) cv::imwrite("detected_frame.jpg", frame);
            cap.release();
            return 0;
        }

        if (std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - start_time).count() >= 20) {
            std::cout << "Timeout: No color detected in 20 seconds.\n";
            break;
        }

        cv::waitKey(200);
    }

    cap.release();
    return 1;
}
