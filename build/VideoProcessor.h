#pragma once

// Standard Library Includes
#include <string>

// OpenCV Includes
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

using namespace std;
using namespace cv;

class VideoProcessor
{
public:
	/// Konstruktor
		VideoProcessor					(string VideoFile_or_CameraStream);

	/// Destruktor
		~VideoProcessor					(void);

	/// Selbsttest
		bool		selfcheck			(void) {return this -> 
														m_is_opened && !m_eof	;};

	/// Videoeigenschaften
		// Getter
				// Time Code
			double	get_cur_Positon		(void) {return this -> m_cur_pos		;};
			double	get_cur_FrameID		(void) {return this -> m_cur_frameID	;};
				// Bei Videofile: relative Position im Intervall [0 1]
			double	get_cur_Position_rel(void) {return this -> m_cur_pos_rel	;};
			int		get_width			(void) {return this -> m_width			;};
			int		get_height			(void) {return this -> m_height			;};
			double	get_fps				(void) {return this -> m_fps			;};
				// Bei Videofile: Gesamtzahl der Frames
			int		get_total_frames	(void) {return this -> m_total_frames	;};

	/// Bildverarbeitung
		/* 	Helligkeit und Kontrast mit linearer Transformation anpassen, siehe:
		docs.opencv.org/doc/tutorials/core/basic_linear_transform/basic_linear_transform.html
		*/
			void	disable_alpha_beta	(void)	{m_alpha	=1;		m_beta=0	;};

			void	set_alpha			(double alpha)
												{m_alpha	=alpha				;};
			void	set_beta			(double beta)
												{m_beta		=beta				;};

		/* 	Gauss-Weichzeichner zur Rauschunterdrückung, siehe:
		docs.opencv.org/doc/tutorials/imgproc/gausian_median_blur_bilateral_filter/
		gausian_median_blur_bilateral_filter.html
		*/
			void	enable_Gauss		(void); // Gauss Weichzeichner an/aus
			void	disable_Gauss		(void);

				// Stärke des Weichzeichners einstellen (Größe der Gauss Matrix)
			void	set_Gauss_size		(int	Gauss_Kernel_Size);

		//	Region of Interest
			void	enable_ROI_cut		(void);	// Auf ROI zuschneiden an/aus
			void	disable_ROI_cut		(void);
			void	enable_ROI_marking	(void); // ROI im Bild markieren an/aus
			void	disable_ROI_marking	(void);

				// Abmessungen der ROI; Achtung: Koordinatenursprung oben links!
			void	set_ROI_x0			(int	Region_of_interest_x0);
			void	set_ROI_y0			(int	Region_of_interest_y0);
			void	set_ROI_width		(int	Region_of_interest_width);
			void	set_ROI_height		(int	Region_of_interest_height);

			void	set_ROI_bordersize	(int	Region_of_interest_bordersize_for_marking)
				{this -> m_ROI_bordersize	=	Region_of_interest_bordersize_for_marking	;};

			void	set_ROI_bordercolor	(Scalar Region_of_interest_bordercolor_for_marking)
				{this -> m_ROI_bordercolor	=	Region_of_interest_bordercolor_for_marking	;};
			void	set_ROI_bordercolor	(int	blue, int	green, int	red)
				{this -> m_ROI_bordercolor	=	Scalar(blue,green,red)						;};

		/* 	"Schwellenwertverfahren" oder Thresholding, siehe:
		de.wikipedia.org/wiki/Schwellenwertverfahren
		docs.opencv.org/doc/tutorials/imgproc/threshold/threshold.html
		*/
			void	enable_tresholding	(void);
			void	disable_tresholding	(void);

			void	set_trs_value		(int Threshold_Value);
			void	set_trs_type		(int Threshold_Type);

		// Graustufen-Umwandlung			
			void	enable_gs_conv		(void);
			void	disable_gs_conv		(void);

	/// Frame ausgeben
			Mat		getFrame			(void);	// Fertig bearbeiteter Frame
			Mat		getOverview			(void);	// Übersichts Frame

private:
	/// Klasse richtig initialisiert/gesund
		bool			m_is_opened;	// Erfolgreich geöffnet
		bool			m_eof;			// End of File

	/// Hauptdatenstrukturen
		VideoCapture	m_vid;			// Datenstruktur für Video
		
		Mat				m_frame;		// bearbeitete Frame
		Mat				m_raw_frame;	// unbearbeiteter Frame
		Mat				m_ov_frame;		// Overview Frame

	/// Videoeigenschaften

		// Current position of the video file in milliseconds or video capture timestamp
			double		m_cur_pos;

		// 0-based index of the frame to be decoded/captured next
			double		m_cur_frameID;

		// Relative position of the video file: 0 - start of the film, 1 - end of the film
			double		m_cur_pos_rel;

		// Width of the frames in the video stream
			int			m_width;

		// Height of the frames in the video stream.
			int			m_height;

		// Frame Rate
			double		m_fps;

		// Number of frames in the video file
			int			m_total_frames;

	/// Bildverarbeitung

		// Helligkeit und Kontrast
			double		m_alpha;
			double		m_beta;

		// Gauss Weichzeichner
			bool		m_gauss_on;
			Size		m_Gauss_Matrix;

		// Region of Interest
			bool		m_ROI_cut_on;
			bool		m_ROI_mark_on;

			int			m_ROI_x0;
			int			m_ROI_y0;
			int			m_ROI_width;
			int			m_ROI_height;

			int			m_ROI_bordersize;
			Scalar		m_ROI_bordercolor;

		// Thresholding
			bool		m_trs_on;

			int			m_trs_val;
			int			m_trs_type;

		// Graustufenumwandlung
			bool		m_gs_on;

	/// Private Funktionen
		// Refresh des Timecodes etc. nach einlesen eines neuen Frames
			void		p_refresh(void);

		// Frame mit Error Nachricht erzeugen
			Mat			p_throw_error_frame(string Error_Message);
};

