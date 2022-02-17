
int main(){
    const std::vector<int> possibleNumbers{2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
    const int classes = possibleNumbers.size();
    const int train_samples = 1;

    const int ImageSize = sizex * sizey;
    const char pathToImages[] = ".";

    Mat trainData = Mat(classes * train_samples, ImageSize, CV_32FC1);
    Mat trainClasses = Mat(classes * train_samples, 1, CV_32FC1);

    namedWindow("single", WINDOW_AUTOSIZE);
    namedWindow("all", WINDOW_AUTOSIZE);

    char file[255];
    for (int i = 0; i < possibleNumbers.size() * train_samples; ++i) {
        // sprintf(file, "%s/%d.png", pathToImages, i);
        sprintf(file, "%d.png", possibleNumbers[i % possibleNumbers.size()]);
        Mat src = imread(file, IMREAD_COLOR);
        std::cout << "Reading file: " << file << std::endl;
        if (!src.data) {
            std::cout << "File " << file << " not found\n";
            exit(1);
        }

        src.convertTo(src, CV_32FC4);
        DEB(src.type());
        DEB(src.size());

        Mat img  = Mat::zeros(Size(src.cols, src.rows), CV_32FC1);
        {
            // Split the image into different channels
            Mat rgbChannels[3];
            split(src, rgbChannels);
            DEB(rgbChannels[0].size());
            DEB(rgbChannels[1].size());
            DEB(rgbChannels[2].size());
            // cv::add(rgbChannels[0], rgbChannels[1], img);
            // cv::add(img, rgbChannels[2], img);
            img = (rgbChannels[0] + rgbChannels[1] + rgbChannels[2]);
        }
        DEB(img.type());
        DEB(img.size());


        // Mat outfile;
        // PreProcessImage(&img, &outfile, sizex, sizey);

        for (int r = 0; r < img.rows; r++) {
            for (int c = 0; c < img.cols; c++) { trainData.at<float>(i, r * img.rows + c) = img.at<float>(r, c); }
        }

        trainClasses.at<float>(i) = possibleNumbers[i % possibleNumbers.size()];
    }

    // DEB(trainData.at<float>(0,0));
    // DEB(trainData.data[0]);
    // DEB(trainData.at<float>(1,0));
    // DEB(trainData.at<float>(0,2));
    // DEB(trainData.at<float>(3,4));
    // DEB(trainData.at<float>(4, 12));
    // DEB(trainData.size());

    Ptr kNearest = ml::KNearest::create();
    kNearest->setIsClassifier(true);
    kNearest->setDefaultK(12);
    kNearest->setAlgorithmType(cv::ml::KNearest::Types::BRUTE_FORCE);

    // DEB(trainData.rows);
    // DEB(trainData.cols);

    Ptr trainingData = ml::TrainData::create(trainData, ml::SampleTypes::ROW_SAMPLE, trainClasses);
    kNearest->train(trainingData, 0);

    int wrong = 0;
    int right = 0;
    for (int i = 0; i < possibleNumbers.size(); ++i) {
        std::random_device rd_;
        int iSecret = possibleNumbers[rd_() % 12];
        std::cout <<"Trying to guess number: "<< iSecret << std::endl;
        sprintf(file, "%d.png", iSecret);
        Mat src = imread(file, IMREAD_COLOR);
        if (!src.data) {
            std::cout << "File " << file << " not found\n";
            exit(1);
        }

        src.convertTo(src, CV_32FC4);
        DEB(src.type());
        DEB(src.size());

        Mat img  = Mat::zeros(Size(src.cols, src.rows), CV_32FC1);
        {
            // Split the image into different channels
            Mat rgbChannels[3];
            split(src, rgbChannels);
            DEB(rgbChannels[0].size());
            DEB(rgbChannels[1].size());
            DEB(rgbChannels[2].size());
            // cv::add(rgbChannels[0], rgbChannels[1], img);
            // cv::add(img, rgbChannels[2], img);
            img = (rgbChannels[0] + rgbChannels[1] + rgbChannels[2]);
        }
        DEB(img.type());
        DEB(img.size());

        // single-precision floating-point matrix of `<number_of_samples> * k` size
        // Mat stagedImage = Mat(1, ImageSize, CV_32FC1);
        // PreProcessImage(&img, &stagedImage, sizex, sizey);

        Mat sample2 = Mat(1, ImageSize, CV_32FC1);
        for (int r = 0; r < img.rows; r++) {
            for (int c = 0; c < img.cols; c++) { 
                sample2.at<float>(1, r * img.rows + c) = img.at<float>(r, c); }
        }
        img.~Mat();

        DEB(sample2.type());
        DEB(sample2.size());


        Mat matResults;
        float detectedClass = kNearest->findNearest(sample2, 1, matResults);
        sample2.~Mat();
        DEB(matResults);
        matResults.~Mat();
        DEB(detectedClass);
        if (iSecret != (int)((detectedClass))) {
            std::cout << "Falsch. Ist " << iSecret << " aber geraten ist " << (int)((detectedClass)) << std::endl;
            // exit(1);
            wrong++;
            continue;
        }
        right++;
        std::cout << "Richtig " << (int)((detectedClass)) << "\n";
        // imshow("single", img);
        waitKey(0);
    }

    DEB(wrong);
    DEB(right);
}
