#include "traceTrajectory.h"

int traceTrajectory() {
    /***** DECLARATIONS *****/
    FILE *currentFile = NULL;
    char *video_path = (char*) malloc(sizeof (char) * MAX_PATH);
    char *pattern_path = (char*) malloc(sizeof (char) * MAX_PATH);
    char *center_path = (char*) malloc(sizeof (char) * MAX_PATH);
    CvCapture* capture = 0;
    bwimage_t *image_target, *image_pattern, *image_center;
    Pixel peak;
    error_e retval = EEA_OK;

    /***** MAIN PROCEDURE *****/
    do {
        /*** Find the paths to the images that need to be processed ***/
        currentFile = fopen("paths_center.txt", "r"); // file containing
                                                      // the paths to the images
        if (currentFile != NULL) {
            fgetstr(video_path, MAX_PATH, currentFile);
            fgetstr(pattern_path, MAX_PATH, currentFile);
            fgetstr(center_path, MAX_PATH, currentFile);
        } else {
            printf("paths_center.txt cannot be opened");
            exit(EXIT_FAILURE);
        }
        fclose(currentFile);

        /*** Load the input images ***/
        image_target = EEACreateImage();
        image_pattern = EEACreateImage();
        retval = EEALoadImage(pattern_path, image_pattern);
        if (retval != EEA_OK) break;
        // free paths
        free(video_path);
        free(pattern_path);

        /*** Initialize the video frame capture ***/
        capture = cvCreateFileCapture("samples/parabolic_drop_256.avi");
        if (!capture) {
            return -1;
        }
        IplImage *frame = cvQueryFrame(capture); //Init the video reading
        double fps = cvGetCaptureProperty(
                capture,
                CV_CAP_PROP_FPS
                );
        CvSize size = cvSize(
                (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH),
                (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT)
                );
        // Convert to grayscale frame
        IplImage *frameGray = cvCreateImage(size, IPL_DEPTH_8U, 1);

        /*** Process the images ***/
        bwimage_t *image_temp = createBlackBwimage(image_pattern->height,
                                                   image_pattern->width);
        Pixel center;
        copyImage(image_temp, image_pattern);
        trim(&image_temp, 0.08);
        findCenter(&center, image_temp);
        EEAFreeImage(image_temp);
        cvCvtColor(frame, frameGray, CV_RGB2GRAY);
        iplImageToBwimage(&image_target, frameGray);
        locateShape(image_target, image_pattern, 0.08, &peak);
        createCenterImage(&image_center, image_target->height,
                                image_target->width, peak, center);
        while (1) {
            frame = cvQueryFrame(capture);
            if (!frame) break;
            // Convert to grayscale frame
            cvCvtColor(frame, frameGray, CV_RGB2GRAY);
            iplImageToBwimage(&image_target, frameGray);
            locateShape(image_target, image_pattern, 0.08, &peak);
            addPosition(image_center, peak, center);
        }
        cvReleaseImage(&frame);
        cvReleaseImage(&frameGray);
        cvReleaseCapture(&capture);

        /*** Write the output images at the specified path ***/
        if (EEA_OK != (retval = EEADumpImage(center_path, image_center))) break;
        free(center_path);
    } while (0);

    /***** FREE HEAP MEMORY *****/
    EEAFreeImage(image_target);
    EEAFreeImage(image_pattern);
    EEAFreeImage(image_center);

    /***** ERROR HANDLING *****/
    switch (retval) {
        case EEA_OK:
            break;
        case EEA_ENOFILE:
            fprintf(stderr, "Cannot open file\n");
            break;
        case EEA_EBADBPS:
            fprintf(stderr, "Number of bits per sample must be equal to 8\n");
            break;
        case EEA_EBADPHOTOMETRIC:
            fprintf(stderr, "Not a colormap image\n");
            break;
        case EEA_ENOCOLORMAP:
            fprintf(stderr, "Image does not have a colormap\n");
            break;
        case EEA_ENOTGRAY:
            fprintf(stderr, "At least one entry in the colormap is not gray\n");
        case EEA_ENOMEM:
            fprintf(stderr, "Cannot allocate memory\n");
            break;
        default:
            ; /* Can't happen  */
    }

    return(EXIT_SUCCESS);
}
