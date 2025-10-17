/**
 * Example OpenCV application for RISC-V bare-metal
 * Demonstrates basic image processing operations
 */

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <stdio.h>

using namespace cv;

// Helper function to print matrix info
void print_mat_info(const char* name, const Mat& m) {
    printf("%s: %dx%d, channels=%d, type=%d\n", 
           name, m.cols, m.rows, m.channels(), m.type());
}

// Example 1: Basic matrix operations
void example_matrix_ops() {
    printf("\n=== Example 1: Matrix Operations ===\n");
    
    // Create a 3x3 matrix
    Mat m = (Mat_<float>(3, 3) << 
        1, 2, 3,
        4, 5, 6,
        7, 8, 9);
    
    print_mat_info("Matrix m", m);
    
    // Matrix multiplication
    Mat result = m * m.t();  // m times its transpose
    print_mat_info("m * m.t()", result);
    
    // Scalar operations
    Mat scaled = m * 2.0;
    printf("Element [0,0] after scaling by 2: %.1f\n", 
           scaled.at<float>(0, 0));
}

// Example 2: Image creation and manipulation
void example_image_ops() {
    printf("\n=== Example 2: Image Operations ===\n");
    
    // Create a grayscale image (320x240)
    const int width = 320;
    const int height = 240;
    Mat img(height, width, CV_8UC1);
    
    // Fill with a gradient pattern
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            img.at<uint8_t>(y, x) = (x + y) % 256;
        }
    }
    
    print_mat_info("Original image", img);
    
    // Region of Interest (ROI)
    Rect roi(width/4, height/4, width/2, height/2);
    Mat roi_img = img(roi);
    print_mat_info("ROI", roi_img);
    
    // Check some statistics
    Scalar mean_val = mean(img);
    printf("Image mean value: %.2f\n", mean_val[0]);
}

// Example 3: Image filtering
void example_filtering() {
    printf("\n=== Example 3: Image Filtering ===\n");
    
    // Create a small test image
    Mat src = (Mat_<uint8_t>(5, 5) << 
        10, 20, 30, 40, 50,
        15, 25, 35, 45, 55,
        20, 30, 40, 50, 60,
        25, 35, 45, 55, 65,
        30, 40, 50, 60, 70);
    
    print_mat_info("Source", src);
    
    // Gaussian blur
    Mat blurred;
    GaussianBlur(src, blurred, Size(3, 3), 1.0);
    print_mat_info("After Gaussian blur", blurred);
    
    printf("Original center: %d, Blurred center: %d\n",
           (int)src.at<uint8_t>(2, 2),
           (int)blurred.at<uint8_t>(2, 2));
}

// Example 4: Geometric transformations
void example_transforms() {
    printf("\n=== Example 4: Geometric Transforms ===\n");
    
    // Create a small image
    Mat src = Mat::eye(4, 4, CV_8UC1) * 255;
    print_mat_info("Identity matrix image", src);
    
    // Resize
    Mat resized;
    resize(src, resized, Size(8, 8), 0, 0, INTER_NEAREST);
    print_mat_info("After resize 2x", resized);
    
    // Rotation matrix (45 degrees around center)
    Point2f center(src.cols / 2.0f, src.rows / 2.0f);
    Mat rot_mat = getRotationMatrix2D(center, 45, 1.0);
    
    printf("Rotation matrix created: %dx%d\n", 
           rot_mat.cols, rot_mat.rows);
}

// Example 5: Drawing operations
void example_drawing() {
    printf("\n=== Example 5: Drawing Operations ===\n");
    
    // Create a blank canvas
    Mat canvas = Mat::zeros(100, 100, CV_8UC1);
    
    // Draw a rectangle
    rectangle(canvas, Point(10, 10), Point(50, 50), 
              Scalar(255), -1);  // Filled
    
    // Draw a circle
    circle(canvas, Point(75, 75), 20, Scalar(128), 2);
    
    // Draw a line
    line(canvas, Point(0, 0), Point(100, 100), Scalar(64), 1);
    
    print_mat_info("Canvas with drawings", canvas);
    
    // Count non-zero pixels
    int count = countNonZero(canvas);
    printf("Non-zero pixels: %d (%.1f%%)\n", 
           count, count * 100.0 / (100 * 100));
}

// Example 6: Morphological operations
void example_morphology() {
    printf("\n=== Example 6: Morphological Ops ===\n");
    
    // Create a binary image
    Mat src = Mat::zeros(10, 10, CV_8UC1);
    rectangle(src, Point(2, 2), Point(7, 7), Scalar(255), -1);
    
    print_mat_info("Binary image", src);
    
    // Erosion
    Mat eroded;
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    erode(src, eroded, kernel);
    print_mat_info("After erosion", eroded);
    
    // Dilation
    Mat dilated;
    dilate(src, dilated, kernel);
    print_mat_info("After dilation", dilated);
}

// Example 7: Memory efficiency test
void example_memory_test() {
    printf("\n=== Example 7: Memory Test ===\n");
    
    // Allocate several images
    const int num_images = 5;
    Mat images[num_images];
    
    for (int i = 0; i < num_images; i++) {
        images[i] = Mat::zeros(64, 64, CV_8UC1);
        printf("Allocated image %d: %dx%d (%zu bytes)\n",
               i, images[i].cols, images[i].rows,
               images[i].total() * images[i].elemSize());
    }
    
    // Release them (automatic with Mat destructor)
    printf("Images will be freed automatically\n");
}

int main() {
    printf("=======================================\n");
    printf("OpenCV Bare-Metal Example Application\n");
    printf("=======================================\n");
    
    try {
        example_matrix_ops();
        example_image_ops();
        example_filtering();
        example_transforms();
        example_drawing();
        example_morphology();
        example_memory_test();
        
        printf("\n=======================================\n");
        printf("All examples completed successfully!\n");
        printf("=======================================\n");
        
    } catch (const cv::Exception& e) {
        printf("\nERROR: OpenCV exception caught\n");
        printf("  Message: %s\n", e.what());
        printf("  Code: %d\n", e.code);
        return 1;
    } catch (...) {
        printf("\nERROR: Unknown exception caught\n");
        return 1;
    }
    
    return 0;
}

