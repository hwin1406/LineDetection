#include <iostream>
#include <string>  

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

#include "VideoProcessor.h"

using namespace std;
using namespace cv;

/// Global Variables

	// Sliders - Initial Values
		int alpha_slider	=100;
		int kernel_slider	=0;
		int width_slider	=0;
		int height_slider	=0;
		int x0_slider		=0;
		int y0_slider		=0;
		int trs_slider		=0;
		int trs_type_slider	=0;
		int gs_slider		=0;

	// Sliders - Max Values
		const int alpha_slider_max		=100;
		const int kernel_slider_max		=100;
		const int width_slider_max		=640;
		const int height_slider_max		=480;
		const int x0_slider_max			=640;
		const int y0_slider_max			=480;
		const int trs_slider_max		=500;
		const int trs_type_slider_max	=5;
		const int gs_slider_max			=1;

/// Function Headers

	// Slider Callbacks
		void on_alpha_trackbar		( int, void* p_VideoCapture);
		void on_kernel_trackbar		( int, void* p_VideoCapture);
		void on_width_trackbar		( int, void* p_VideoCapture);
		void on_height_trackbar		( int, void* p_VideoCapture);
		void on_x0_trackbar			( int, void* p_VideoCapture);
		void on_y0_trackbar			( int, void* p_VideoCapture);
		void on_trs_trackbar		( int, void* p_VideoCapture);
		void on_trs_type_trackbar	( int, void* p_VideoCapture);
		void on_gs_trackbar			( int, void* p_VideoCapture);

int main(int argc, char *argv[])
{	
	/// Video Datei laden
		const string VideoFile = argv[1];
		VideoProcessor vid(VideoFile);
	
		// Videowiedergabe mit Quell-FPS synchronisieren
		const int delay = 1000 / vid.get_fps();

	/// GUI erstellen
		namedWindow("Output",WINDOW_AUTOSIZE);

		// Pointer zum VideoProcessor erstellen, um in Callback Funktionen Zugriff zu haben
			VideoProcessor *p_vid = &vid;

			// Create Trackbars
				// Alpha
					char TrackbarName_alpha[50];
					sprintf( TrackbarName_alpha, "Alpha x %d", alpha_slider_max );
					createTrackbar( TrackbarName_alpha, "Output", &alpha_slider, 
						alpha_slider_max, on_alpha_trackbar, p_vid );

				// Gauss Kernel Size
					char TrackbarName_kernel[50];
					sprintf( TrackbarName_kernel, "Kernelsize %d", kernel_slider_max );
					createTrackbar( TrackbarName_kernel, "Output", &kernel_slider, 
						kernel_slider_max, on_kernel_trackbar, p_vid );

				// ROI Breite
					char TrackbarName_width[50];
					sprintf( TrackbarName_width, "Width %d", width_slider_max );
					createTrackbar( TrackbarName_width, "Output", &width_slider, 
						width_slider_max, on_width_trackbar, p_vid );

				// ROI Höhe
					char TrackbarName_height[50];
					sprintf( TrackbarName_height, "Height %d", height_slider_max );
					createTrackbar( TrackbarName_height, "Output", &height_slider, 
						height_slider_max, on_height_trackbar, p_vid );

				// ROI x0 (Koordinatenursprung oben links!)
					char TrackbarName_x0[50];
					sprintf( TrackbarName_x0, "x0 %d", x0_slider_max );
					createTrackbar( TrackbarName_x0, "Output", &x0_slider, 
						x0_slider_max, on_x0_trackbar, p_vid );

				// ROI y0
					char TrackbarName_y0[50];
					sprintf( TrackbarName_y0, "y0 %d", y0_slider_max );
					createTrackbar( TrackbarName_y0, "Output", &y0_slider, 
						y0_slider_max, on_y0_trackbar, p_vid );

				// Treshold
					char TrackbarName_trs[50];
					sprintf( TrackbarName_trs, "trs %d", trs_slider_max );
					createTrackbar( TrackbarName_trs, "Output", &trs_slider, 
						trs_slider_max, on_trs_trackbar, p_vid );

				// Threshold Type 
					char TrackbarName_trs_type[50];
					sprintf( TrackbarName_trs_type, "trs_type %d", trs_type_slider_max );
					createTrackbar( TrackbarName_trs_type, "Output", &trs_type_slider, 
						trs_type_slider_max, on_trs_type_trackbar, p_vid );

				// Grayscale an/aus
					char TrackbarName_gs[50];
					sprintf( TrackbarName_gs, "Grayscale on/off %d", gs_slider_max );
					createTrackbar( TrackbarName_gs, "Output", &gs_slider, 
						gs_slider_max, on_gs_trackbar, p_vid );

	/// Video abspielen
		Mat Frame;
		while (vid.selfcheck())
		{
			// Nächsten Frame holen und bearbeiten
				Frame = vid.getFrame();
			// verarbeiteten Frame anzeigen und waitkey Funktion damit Videowiedergabe
				imshow("Output", Frame);
			// delay warten (Synchronisierung) 
				waitKey(delay);
		}			
		return 0;
	}


/// Slider Callbacks 

void on_alpha_trackbar( int, void* p_VideoCapture )
{
	 // Slider Wert auslesen
		double alpha	=	(double) alpha_slider/alpha_slider_max;
	 // Void Pointer in VideoProcessor Pointer umwandeln
		VideoProcessor	*p_video_casted = static_cast<VideoProcessor*>(p_VideoCapture);
	 // Zugehörige Funktion aufrufen
		p_video_casted	->	set_alpha(alpha);
}

void on_kernel_trackbar( int, void* p_VideoCapture )
{
	 // Slider Wert auslesen
		int KernelSize = (int) kernel_slider;
	 // Void Pointer in VideoProcessor Pointer umwandeln
		VideoProcessor	*p_video_casted = static_cast<VideoProcessor*>(p_VideoCapture);
	 // Zugehörige Funktion aufrufen
		p_video_casted -> set_Gauss_size(KernelSize);  
}

void on_width_trackbar( int, void* p_VideoCapture )
{
	 // Slider Wert auslesen
		int ROI_width = (int) width_slider;
	 // Void Pointer in VideoProcessor Pointer umwandeln
		VideoProcessor	*p_video_casted = static_cast<VideoProcessor*>(p_VideoCapture);
	 // Zugehörige Funktion aufrufen
		p_video_casted -> set_ROI_width(ROI_width);
}

void on_height_trackbar( int, void* p_VideoCapture )
{
	 // Slider Wert auslesen
		int ROI_height = (int) height_slider;
	 // Void Pointer in VideoProcessor Pointer umwandeln
		VideoProcessor	*p_video_casted = static_cast<VideoProcessor*>(p_VideoCapture);
	 // Zugehörige Funktion aufrufen
		p_video_casted -> set_ROI_height(ROI_height);
}

void on_x0_trackbar( int, void* p_VideoCapture )
{
	 // Slider Wert auslesen
		int ROI_x0 = (int) x0_slider;
	 // Void Pointer in VideoProcessor Pointer umwandeln
		VideoProcessor	*p_video_casted = static_cast<VideoProcessor*>(p_VideoCapture);
	 // Zugehörige Funktion aufrufen
		p_video_casted -> set_ROI_x0(ROI_x0);
}

void on_y0_trackbar( int, void* p_VideoCapture )
{
	 // Slider Wert auslesen
		int ROI_y0 = (int) y0_slider;
	 // Void Pointer in VideoProcessor Pointer umwandeln
		VideoProcessor	*p_video_casted = static_cast<VideoProcessor*>(p_VideoCapture);
	 // Zugehörige Funktion aufrufen
		p_video_casted -> set_ROI_y0(ROI_y0);
}

void on_trs_trackbar( int, void* p_VideoCapture )
{
	 // Slider Wert auslesen
		int threshold_value = (int) trs_slider;
	 // Void Pointer in VideoProcessor Pointer umwandeln
		VideoProcessor	*p_video_casted = static_cast<VideoProcessor*>(p_VideoCapture);
	 // Zugehörige Funktion aufrufen
		p_video_casted -> set_trs_value(threshold_value);
}

void on_trs_type_trackbar( int, void* p_VideoCapture )
{
	 // Slider Wert auslesen
		int trs_type = (int) trs_type_slider;
	 // Void Pointer in VideoProcessor Pointer umwandeln
		VideoProcessor	*p_video_casted = static_cast<VideoProcessor*>(p_VideoCapture);
	 // Zugehörige Funktion aufrufen
		if (trs_type==0)
			p_video_casted -> disable_tresholding();
		else
			p_video_casted -> set_trs_type(trs_type);
}

void on_gs_trackbar( int, void* p_VideoCapture )
{
	 // Slider Wert auslesen
		bool gs = (bool) gs_slider;
	 // Void Pointer in VideoProcessor Pointer umwandeln
		VideoProcessor	*p_video_casted = static_cast<VideoProcessor*>(p_VideoCapture);
	 // Zugehörige Funktion aufrufen
		if (gs)
			p_video_casted -> enable_gs_conv();
		else
			p_video_casted -> disable_gs_conv();
}
