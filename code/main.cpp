/*
 * Acid Cam v2 - OpenCV Edition
 * written by Jared Bruni ( http://lostsidedead.com )
 * GPL
 *
 *	 ac2 arguments:
 *	 -t transition [Transition variable]
 *	 -l input_file.avi [Use video instead of camera]
 *	 -d device [Capture device index]
 *	 -x  [Disable recording]
 *	 -f fps [Frames per second]
 *	 -p alpha [Second pass alpha level]
 *	 -o output.avi [Output avi filename]
 *	 -s seed [Random generator seed]
 *	 -i imgfile.jpg [Image for blend with image functions]
 *	 -v imglist.txt [Image list for image functions]
 *	 -w width [Webcam Resolution Width]
 *	 -h height [Webcam Resolution Height]
 *   -a output_name [Save file prefix]
 *
 *   To compile use:
 *   $ g++ -O3 fractal.cpp ac.cpp main.cc -o ac2 `pkg-config opencv --cflags --libs`
 *
 */

#include"ac.h"
#include<unistd.h>

void ProcFrame(cv::Mat &frame);
void BuildImages(std::string filename);
void TrackbarCallback(int pos, void *ptr);
void IsNegativeCallback(int pos, void *ptr);
void IsNegativeFlash(int pos, void *ptr);
void Pass2Enable(int pos, void *ptr);
void ReverseColors(int pos, void *ptr);
void SlideShow(int pos, void *ptr);
void SlideRand(int pos, void *ptr);
void StrobeIt(int pos, void *ptr);
void BlurFirst(int pos, void *ptr);
void BlurSecond(int pos, void *ptr);

inline std::string boolStr(bool b) { if(b) return "True"; return "False"; };

int main(int argc, char **argv) {

	std::cout << "Acid Cam " << ac::version << " ..\n";
	std::string input_file, add_path="default";
	int capture_device = 0;
	int capture_width = 0, capture_height = 0, frame_count = 0;
	srand(static_cast<unsigned int>(time(0)));
	std::cout << argv[0] << "\n-v list.txt [Image list text file]\n-t translation_speed [Transition variable]\n-l input_file.avi [Input file name (avi)]\n-d device_num [Capture Device Index]\n-w width [Capture Device Width]\n-h height [Capture Device Height]\n-f fps [Record frames per second]\n-p alpha [Pass 2 blend alpha]\n-o output.avi [Output filename]\n-s seed [Random seed value]\n-x [Disable recording]\n-i image_file.jpg [Image for blending functions]\n-a path [Save prefix]\n\n";
	if (argc > 1) {
		int opt;
		while ((opt = getopt(argc, argv, "v:t:l:w:h:d:f:o:xp:s:i:a:")) != -1) {
			switch (opt) {
			case 't':
				ac::translation_variable = atof(optarg);
				ac::tr = ac::translation_variable;
				break;
			case 'l':
				input_file = optarg;
				break;
			case 'w':
				capture_width = atoi(optarg);
				break;
			case 'h':
				capture_height = atoi(optarg);
				break;
			case 'd':
				capture_device = atoi(optarg);
				break;
			case 'o':
				ac::fileName = optarg;
				break;
			case 'f':
				ac::fps_force = true;
				ac::fps = atof(optarg);
				break;
			case 'x':
				ac::noRecord = true;
				break;
			case 'p':
				ac::pass2_alpha = atof(optarg);
				break;
			case 's': {
				unsigned int seed = atoi(optarg);
				srand(seed);
			}
				break;
			case 'i': {
				ac::blendW_frame = cv::imread(optarg);
				if(!ac::blendW_frame.data) {
					std::cerr << "AC: could not load image" << optarg << "\n";
					ac::blendW = false;
				}
				else {
					ac::blendW = true;
					std::cout << "AC: loaded image " << optarg << "\n";
				}
			}
				break;
			case 'v': {
				BuildImages(optarg);
			}
				break;
			case 'a':
				add_path = optarg;
				break;
			}
		}
	}

	try {
		cv::VideoCapture capture;
		if(input_file.size() == 0) capture = cv::VideoCapture(capture_device);
		else
			capture = cv::VideoCapture(input_file);

		if (!capture.isOpened()) {
			std::cerr << "Error could not open Camera device..\n";
			return 0;
		} else
			std::cout << "Acid Cam " << ac::version << " Capture device opened..\n";

		int aw = capture.get(CV_CAP_PROP_FRAME_WIDTH);
		int ah = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
		std::cout << "Resolution: " << aw << "x" << ah << "\n";
		if(ac::fps_force == false && input_file.size() != 0) ac::fps = capture.get(CV_CAP_PROP_FPS);
		std::cout << "FPS: " << ac::fps << "\n";
		cv::Mat frame;
		capture.read(frame);
		cv::Size frameSize = frame.size();
		if(capture_width != 0 && capture_height != 0) {
			capture.set(CV_CAP_PROP_FRAME_WIDTH, capture_width);
			capture.set(CV_CAP_PROP_FRAME_HEIGHT, capture_height);
			std::cout << "Resolution set to " << capture_width << "x" << capture_height << "\n";
			frameSize = cv::Size(capture_width, capture_height);
		}

		cv::VideoWriter writer;
		if(ac::noRecord == false) {
			writer = cv::VideoWriter(ac::fileName, CV_FOURCC('X','V','I','D'),  ac::fps, frameSize, true);
			if(writer.isOpened() == false) {
				std::cerr << "Error video file could not be created.\n";
				exit(0);
			}
		}

		if(ac::noRecord == false)
			std::cout << "Now recording .. \n";
		else
			std::cout << "Recording disabled ..\n";

		int slider_value = 0, slider_neg = 0, slider_neg_f = 0, slider_pass2 = 0, slider_rev = 0, slider_show = 0, slider_rand = 0, slider_strobe = 0, slider_blur_f = 0, slider_blur_s = 0;
		cv::namedWindow("Acid Cam v2", 1);
		/*cv::namedWindow("Controls");
		cv::createTrackbar("Change Filter", "Acid Cam v2",&slider_value, ac::draw_max, TrackbarCallback);
		cv::createTrackbar("Filter Negative", "Controls", &slider_neg, 1, IsNegativeCallback);
		cv::createTrackbar("Negative Flash", "Controls", &slider_neg_f,1, IsNegativeFlash);
		cv::createTrackbar("Reverse Colors", "Controls", &slider_rev, 1, ReverseColors);
		cv::createTrackbar("Slide Show", "Controls", &slider_show, 1, SlideShow);
		cv::createTrackbar("Slide Rand", "Controls", &slider_rand, 1, SlideRand);
		cv::createTrackbar("Strobe ", "Controls", &slider_strobe, 1, StrobeIt);
		cv::createTrackbar("Blur First", "Controls", &slider_blur_f, 1, BlurFirst);
		cv::createTrackbar("Blur Second", "Controls", &slider_blur_s, 1, BlurSecond);
		cv::createTrackbar("Pass 2 Enable", "Controls", &slider_pass2,1,Pass2Enable);*/
		bool active = true;
		int key = 0x0;
		//cv::moveWindow("Controls", 0, 0);

		while(active == true) {
			cv::Mat frame;
			if(capture.read(frame) == false) {
				active = false;
				break;
			}
			ProcFrame(frame);
			++frame_count;
			imshow("Acid Cam v2", frame);

			if(ac::noRecord == false) {
				writer.write(frame);
			}

			if( (key = cv::waitKey(30)) ) {
				switch(key) {
				case 't':
					ac::resetAll();
					break;
				case 'a':
				{
					static unsigned int index = 0;
					std::ostringstream stream;
					stream << add_path << "_" <<  (++index) << ".Acid.Cam.Image.Uncompressed." << ac::draw_strings[ac::draw_offset] << ".bmp";
					imwrite(stream.str(), frame);
					std::cout << "Saved uncompressed image: " << stream.str() << "\n";
				}
					break;
				case 'd':
				case 's':
				{
					static unsigned int index = 0;
					std::ostringstream stream;
					stream << add_path << "_" << (++index) << ".Acid.Cam.Image." << ac::draw_strings[ac::draw_offset] << ((key == 's') ? ".jpg" : ".png");
					imwrite(stream.str(), frame);
					std::cout << "Took snapshot: " << stream.str() << "\n";
				}
					break;
				case 'x':
					ac::alpha = 1.0f;
					ac::tr = 0.1f;
					std::cout << "Reset Alpha..\n";
					break;
				case 'l':
					std::cout << "Draw Offset Increased\n";
                        
                        if(ac::draw_strings[ac::draw_offset] == "Alpha Flame Filters") {
                            if(current_filterx < 36) {
                                ++current_filterx;
                                break;
                            } else ++ac::draw_offset;
                        }
                        
					if(ac::draw_offset < ac::draw_max)
						++ac::draw_offset;

					std::cout << "Filter set to: " << ac::draw_strings[ac::draw_offset] << "\n";
					break;
				case 'o':
                        
                        if(ac::draw_strings[ac::draw_offset] == "Alpha Flame Filters") {
                            if(current_filterx > 0) --current_filterx;
                                else if(ac::draw_offset > 0) ac::draw_offset--;
                            break;
                        }
                        
					std::cout << "Draw Offset Decreased\n";
					if(ac::draw_offset > 0)
						--ac::draw_offset;

					std::cout << "Filter set to: " << ac::draw_strings[ac::draw_offset] << "\n";;
					break;
				case 'n':
					ac::isNegative = !ac::isNegative;
					std::cout << "Is Negative: " << boolStr(ac::isNegative) << "\n";
					break;
				case 'r':
					ac::iRev = !ac::iRev;
					if(ac::iRev == true) ac::isNegative = true;
					std::cout << "Reverse Colors: " << boolStr(ac::iRev) << "\n";
					break;
				case 27:
					active = false;
					std::cout << "Quitting..\n";
					break;
				case 'p':
					ac::pass2_enabled = !ac::pass2_enabled;
					std::cout << "Pass2 Enabled: "<< boolStr(ac::pass2_enabled) <<  "\n";
					break;
				case 'h':
					ac::slide_Show = !ac::slide_Show;
					std::cout << "Slide Show: " <<  boolStr(ac::slide_Show) << "\n";
					break;
				case 'j':
					ac::slide_Rand = !ac::slide_Rand;
					std::cout << "Slide Show Random: " << boolStr(ac::slide_Rand) << "\n";
					break;
				case 'b':
					ac::strobe_It = !ac::strobe_It;
					std::cout << "Strobe It: " << boolStr(ac::strobe_It) << "\n";
					break;
				case 'c':
					if(ac::switch_Back == true) {
						ac::switch_Back = false;
					} else {
						ac::switch_Back = true;
						ac::isNegative = false;
					}
					std::cout << "switch: " << boolStr(ac::switch_Back) << "\n" << "is Negative: " << boolStr(ac::isNegative) << "\n";
					break;
				case 'f':
					ac::blur_First = !ac::blur_First;
					std::cout << "Blur First: " << boolStr(ac::blur_First) << "\n";
					break;
				case 'g':
					ac::blur_Second = !ac::blur_Second;
					std::cout << "Blur Second: " << boolStr(ac::blur_Second) << "\n";
					break;
                case 'y':
                        if(current_filterx < 36)
                        current_filterx++;
                        break;
                case 'u':
                        if(current_filterx > 0) --current_filterx;
                        break;
				}
			}
		}
		if(ac::noRecord == false) std::cout << "Wrote to video file: " << ac::fileName << "\n";
		std::cout << frame_count << " Total frames\n";
		std::cout << (frame_count/ac::fps) << " Seconds\n";
		return 0;
	}
	catch(std::exception &e) {
		std::cerr << e.what() << " was thrown!\n";
	}
	catch(...) {
		std::cerr << "Unknown error thrown.\n";
	}
}

void TrackbarCallback(int pos, void *ptr) {
	ac::draw_offset = pos;
	std::cout << "Filter set to: " << ac::draw_strings[ac::draw_offset] << "\n";;
}

void IsNegativeCallback(int pos, void *ptr) {
	if(pos == 1) ac::isNegative = true;
	else ac::isNegative = false;
	std::cout << "isNegative: " << boolStr(ac::isNegative) << "\n";
}

void IsNegativeFlash(int pos, void *ptr) {
	if(ac::switch_Back == true) {
		ac::switch_Back = false;
	} else {
		ac::switch_Back = true;
		ac::isNegative = false;
	}
	std::cout << "switch: " << boolStr(ac::switch_Back) << "\n" << "is Negative: " << boolStr(ac::isNegative) << "\n";
}

void Pass2Enable(int pos, void *ptr) {
	if(pos == 1) ac::pass2_enabled = true;
	else ac::pass2_enabled = false;
	std::cout << "Pass2 Enabled: "<< boolStr(ac::pass2_enabled) <<  "\n";
}

void ReverseColors(int pos, void *ptr) {
	if(pos == 1) ac::iRev = true;
	else ac::iRev = false;

	if(ac::iRev == true) ac::isNegative = true;
	std::cout << "Reverse Colors: " << boolStr(ac::iRev) << "\n";
}

void SlideShow(int pos, void *ptr) {
	if(pos == 1) ac::slide_Show = true;
	else ac::slide_Show = false;
	std::cout << "Slide Show: " <<  boolStr(ac::slide_Show) << "\n";
}

void SlideRand(int pos, void *ptr) {
	if(pos == 1) ac::slide_Rand = true;
	else ac::slide_Rand = false;
	std::cout << "Slide Show Random: " << boolStr(ac::slide_Rand) << "\n";
}

void StrobeIt(int pos, void *ptr) {
	if(pos == 1) ac::strobe_It = true;
	else ac::strobe_It = false;
	std::cout << "Strobe It: " << boolStr(ac::strobe_It) << "\n";
}

void BlurFirst(int pos, void *ptr) {
	if(pos == 1) ac::blur_First = true;
	else ac::blur_First = false;
	std::cout << "Blur First set to: " << boolStr(ac::blur_First) << "\n";
}
void BlurSecond(int pos, void *ptr) {
	if(pos == 1) ac::blur_Second = true;
	else ac::blur_Second = false;
	std::cout << "Blur Second set to: " << boolStr(ac::blur_Second) << "\n";
}

void custom_filter(cv::Mat &frame) {
    
}


void ProcFrame(cv::Mat &frame) {
	static int offset = 0;
	if(ac::pass2_enabled == true)
		ac::orig_frame = frame;

	if(ac::blur_First == true) {
		cv::Mat temp;
		cv::GaussianBlur(frame, temp,cv::Size(5, 5), 0, 0, 0);
		frame = temp;
	}

	if(ac::slide_Show == false)
	ac::draw_func[ac::draw_offset](frame);
	else {
		if(ac::slide_Rand == true) ac::draw_func[rand()%ac::draw_max](frame);
		else ac::draw_func[offset](frame);
		++offset;
		if(offset >= 7)
			offset = 0;
	}
	if(ac::switch_Back == true) {
		ac::isNegative = !ac::isNegative;
	}

	if(ac::blur_Second == true) {
		cv::Mat temp;
		cv::GaussianBlur(frame, temp,cv::Size(5, 5), 0, 0, 0);
		frame = temp;
	}

	if(ac::pass2_enabled == true) {
		ac::Pass2Blend(frame);
	}
}

void BuildImages(std::string filename) {
	std::fstream file;
	file.open(filename.c_str(), std::ios::in);
	if (!file.is_open()) {
		std::cerr << "Error opening list file: " << filename << "\n";
		exit(0);
	}
	std::vector<std::string> images;
	while (!file.eof()) {
		std::string str;
		std::getline(file, str);
		images.push_back(str);
	}
	file.close();
	if (images.size() < 4) {
		std::cerr << "Error four images are required\n";
		exit(0);
	}
	for(int i = 0; i < 4; ++i) {
		ac::image_files[i] = cv::imread(images[i]);
		if(!ac::image_files[i].data) {
			std::cerr << "Error could not load image: " << images[i] << "\n";
			exit(0);
		} else std::cout <<  images[i] << " Successfully loaded.\n";
	}
	ac::images_Enabled = true;
	std::cout << "Images enabled..\n";
}

