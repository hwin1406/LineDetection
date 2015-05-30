#include "VideoProcessor.h"

/// Konstruktor/Destruktor
VideoProcessor::VideoProcessor(string VideoFile_or_CameraStream)
{
	// Videodatei oder Camerastream öffnen
	m_vid.open(VideoFile_or_CameraStream);

	// Prüfen ob erfolgreich geladen, wenn ja alles initialisieren
	if (m_vid.isOpened() == true)
	{
		// erfolgreich geöffnet
			m_is_opened=true;
			m_eof=false;

		// Initialisierungs-Frame laden (wird nicht verarbeitet)
			m_vid >> m_frame;
			this  -> p_refresh();

		// Videoeigenschaften		
			m_width=		m_vid.get(CV_CAP_PROP_FRAME_WIDTH);
			m_height=		m_vid.get(CV_CAP_PROP_FRAME_HEIGHT);
			m_fps=			m_vid.get(CV_CAP_PROP_FPS);
			m_total_frames=	m_vid.get(CV_CAP_PROP_FRAME_COUNT);

		// Helligkeit und Kontrast g(x)=alpha*x+beta
			m_alpha=	1;
			m_beta=		0;

		// ROI
			m_ROI_cut_on=false;
			m_ROI_mark_on=false;

		// Gauss Weichzeichner
			m_gauss_on=	false;

		// Thresholding
			m_trs_on=	false;

		// Graustufenumwandlung
			m_gs_on=	false;
	}	
	else
	{
		// nicht erfolgreich geöffnet
			m_is_opened=false;
			// Fenstergröße für Error Frame initialisieren
			m_width=640;
			m_height=480;
	}
}

VideoProcessor::~VideoProcessor(void)
{
}

/// Gauss Funktionen
void VideoProcessor::enable_Gauss(void)
{
	m_gauss_on=true;
	// Standard Gauss Filter mit 3x3 Matrix
	m_Gauss_Matrix=Size(3,3);
}

void VideoProcessor::disable_Gauss(void)
{
	m_gauss_on=false;
}

void VideoProcessor::set_Gauss_size(int Gauss_Kernel_Size)
{
	// Kernelsize muss ungerade sein (Matrixdimension)
	if (Gauss_Kernel_Size%2 == 0)
		++Gauss_Kernel_Size;

	m_Gauss_Matrix=Size(Gauss_Kernel_Size,Gauss_Kernel_Size);
}

/// Region of Interest Funktionen
void VideoProcessor::enable_ROI_cut(void)
{
	m_ROI_cut_on=true;

	// Standardmäßig alles in ROI
	m_ROI_x0=			0;
	m_ROI_y0=			0;
	m_ROI_width=		m_width;			
	m_ROI_height=		m_height;
}

void VideoProcessor::disable_ROI_cut(void)
{
	m_ROI_cut_on=		false;
}

void VideoProcessor::enable_ROI_marking(void)
{
	m_ROI_mark_on=		true;
	// Standard Lininendicke:5; Standardfarbe: rot
	m_ROI_bordersize=	5;
	m_ROI_bordercolor=	Scalar(0,0,255);	

	// ToDo:	setter für Farbe und Liniendicke
}

void VideoProcessor::disable_ROI_marking(void)
{
	m_ROI_mark_on=		false;
}

void VideoProcessor::set_ROI_x0(int Region_of_interest_x0)
{
	// Wertebereich prüfen
		if (Region_of_interest_x0 > m_width)
			Region_of_interest_x0=m_width;
		if (Region_of_interest_x0 < 0)
			Region_of_interest_x0=0;

	m_ROI_x0=Region_of_interest_x0;
}

void VideoProcessor::set_ROI_y0(int Region_of_interest_y0)
{
	// Wertebereich prüfen
		if (Region_of_interest_y0 > m_height)
			Region_of_interest_y0=m_height;
		if (Region_of_interest_y0 < 0)
			Region_of_interest_y0=0;

	m_ROI_y0=Region_of_interest_y0;
}

void VideoProcessor::set_ROI_width(int Region_of_interest_width)
{
	// Wertebereich prüfen
		if (Region_of_interest_width < 1)
			Region_of_interest_width=1;
		// Vorgegebene Breite liegt außerhalb des Bildes
		if (m_ROI_x0+Region_of_interest_width > m_width)
			Region_of_interest_width=m_width-m_ROI_x0;

	m_ROI_width=Region_of_interest_width;
}

void VideoProcessor::set_ROI_height(int Region_of_interest_height)
{
	// Wertebereich prüfen
		if (Region_of_interest_height < 1)
			Region_of_interest_height=1;
		// Vorgegebene Breite liegt außerhalb des Bildes
		if (m_ROI_y0+Region_of_interest_height > m_height)
			Region_of_interest_height=m_height-m_ROI_y0;

	m_ROI_height=Region_of_interest_height;
}

/// Thresholding Funktionen
void VideoProcessor::enable_tresholding(void)
{
	m_trs_on=true;
	// Standardverfahren + Threshold Wert
	m_trs_val=200;	
	/*
	1 = Binary
	2 = Binary INV
	3 = Trunc
	4 = To Zero
	5 = To Zero INV
	siehe: docs.opencv.org/modules/imgproc/doc/miscellaneous_transformations.html?#threshold
	*/
	m_trs_type=1;	
}

void VideoProcessor::disable_tresholding(void)
{
	m_trs_on=false;
}

void VideoProcessor::set_trs_value(int Threshold_Value)
{
	// Wertebereich prüfen
	if (Threshold_Value < 0)
		Threshold_Value=0;

	m_trs_val=Threshold_Value;
}

void VideoProcessor::set_trs_type(int Threshold_Type)
{
	// Wertebereich prüfen
	if (Threshold_Type < 1 || Threshold_Type > 5)
		Threshold_Type=1;

	m_trs_type=Threshold_Type;
}

/// Graustufen Umwandlung
void VideoProcessor::enable_gs_conv(void)
{
	m_gs_on=true;
}

void VideoProcessor::disable_gs_conv(void)
{
	m_gs_on=false;
}

/// Frame Ausgabe
Mat VideoProcessor::getFrame(void)
{
	if (m_is_opened)
	{
		// Wenn Ende der Datei schon erreicht wurde
		if (m_eof)
			return this->p_throw_error_frame
				("Error: End of File already reached");

		// Versuche nächsten Frame zu laden
			m_vid >> m_frame;

		// Prüfen ob Frame geladen werden konnte
			if (m_frame.empty())
			{
				m_eof=true;
				return this->p_throw_error_frame
					("End of File reached or Camera Stream interrupted");
			}
			else
			{
				// Timecode etc. aktualisieren
					this -> p_refresh();
				// Backup des unverarbeiteten Frames machen
					m_frame.copyTo(m_raw_frame);
			}

		// Helligkeit und Kontrast anpassen
			// alpha=1 und beta=0 behält Originalbild bei
			// d.h. y(x)=alpha*x+beta
			if (m_alpha!=1 || m_beta!=0)
				m_frame.convertTo(m_frame,-1,m_alpha,m_beta);

		// Gauss Weichzeichner
			if (m_gauss_on)
				GaussianBlur(m_frame,m_frame,m_Gauss_Matrix,0);

		// Region of Interest
			if (m_ROI_cut_on)
			{
				// Rechteck mit Eckpunkten und Abmessungen definieren
					Rect ROI(m_ROI_x0,m_ROI_y0,m_ROI_width,m_ROI_height);

				// Nur ROI behalten
					// Wirft Fehler
					m_frame=m_frame(ROI);

				// Größe des Ausgabe Frames anpassen
					m_width=m_ROI_width;
					m_height=m_ROI_height;
			}

			if (m_ROI_mark_on)
			{
				// Rechteck mit Eckpunkten und Abmessungen definieren
					Rect ROI(m_ROI_x0,m_ROI_y0,m_ROI_width,m_ROI_height);

				// Rechteck in Frame einzeichnen
					rectangle(m_frame,ROI,m_ROI_bordercolor,m_ROI_bordersize);
			}

		// Graustufen Umwandlung
			if (m_gs_on)
				cvtColor(m_frame,m_frame,CV_BGR2GRAY);

		// Thresholding
			if (m_trs_on)
				threshold(m_frame,m_frame,m_trs_val,255,m_trs_type);
				// 255 = Anzahl der Farbwerte pro Kanal bei 8bit Kodierung

		// Bearbeiteten Frame ausgeben
			return m_frame;
	}

	else
	{
		return this->p_throw_error_frame
			("Error: Couldn't open Video File or Camera");
	}
}

void VideoProcessor::p_refresh(void)
	// aktualisiert die aktuellen Videoinformationen
{
	m_cur_pos=		m_vid.get(CV_CAP_PROP_POS_MSEC);
	m_cur_frameID=	m_vid.get(CV_CAP_PROP_POS_FRAMES);
	m_cur_pos_rel=	m_vid.get(CV_CAP_PROP_POS_AVI_RATIO);
}

Mat VideoProcessor::p_throw_error_frame(string Error_Message)
{
	// Fehler Frame erzeugen
		Mat error_frame=Mat::zeros(m_height,m_width, CV_8UC3);
	// Ankerpunkt für Fehlertext in Bildmitte
		Point error_anker = Point(m_width/2,m_height/2);
	// Text  ausgeben
		putText(error_frame,Error_Message,error_anker,	
			FONT_HERSHEY_SIMPLEX,12,Scalar(0,0,0)); // Farbe: schwarz

	return error_frame;
}
