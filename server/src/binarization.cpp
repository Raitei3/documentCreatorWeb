#include "../headers/Binarization.hpp"

/* Code provided by Antoine BILLY */

double calcLocalStats (cv::Mat &im, cv::Mat &map_m, cv::Mat &map_s, int winx, int winy) {
    /* Computes global stats of the input image to perform Wolf & Jolion segmentation */
    cv::Mat im_sum, im_sum_sq;
    cv::integral(im,im_sum,im_sum_sq,CV_64F);


	double max_s = 0;	
	int wxh	= winx/2;
	int wyh	= winy/2;
	int x_firstth= wxh;
	int y_lastth = im.rows-wyh-1;
	int y_firstth= wyh;
	double winarea = winx*winy;

	for	(int j = y_firstth ; j<=y_lastth; j++){   
        double m,s,sum,sum_sq;
        sum = im_sum.at<double>(j-wyh+winy,winx) - im_sum.at<double>(j-wyh,winx) - im_sum.at<double>(j-wyh+winy,0) + im_sum.at<double>(j-wyh,0);
        sum_sq = im_sum_sq.at<double>(j-wyh+winy,winx) - im_sum_sq.at<double>(j-wyh,winx) - im_sum_sq.at<double>(j-wyh+winy,0) + im_sum_sq.at<double>(j-wyh,0);

        m  = sum / winarea;
        s  = sqrt ((sum_sq - m*sum)/winarea);
        if (s > max_s) max_s = s;

        map_m.fset(x_firstth, j, m);
        map_s.fset(x_firstth, j, s);

        // Shift the window, add and remove new/old values to the histogram
        for (int i=1 ; i <= im.cols-winx; i++) {

            // Remove the left old column and add the right new column
            sum -= im_sum.at<double>(j-wyh+winy,i) - im_sum.at<double>(j-wyh,i) - im_sum.at<double>(j-wyh+winy,i-1) + im_sum.at<double>(j-wyh,i-1);
            sum += im_sum.at<double>(j-wyh+winy,i+winx) - im_sum.at<double>(j-wyh,i+winx) - im_sum.at<double>(j-wyh+winy,i+winx-1) + im_sum.at<double>(j-wyh,i+winx-1);

            sum_sq -= im_sum_sq.at<double>(j-wyh+winy,i) - im_sum_sq.at<double>(j-wyh,i) - im_sum_sq.at<double>(j-wyh+winy,i-1) + im_sum_sq.at<double>(j-wyh,i-1);
            sum_sq += im_sum_sq.at<double>(j-wyh+winy,i+winx) - im_sum_sq.at<double>(j-wyh,i+winx) - im_sum_sq.at<double>(j-wyh+winy,i+winx-1) + im_sum_sq.at<double>(j-wyh,i+winx-1);

            m  = sum / winarea;
            s  = sqrt ((sum_sq - m*sum)/winarea);
            if (s > max_s) max_s = s;

            map_m.fset(i+wxh, j, m);
            map_s.fset(i+wxh, j, s);
        }
    }
    im_sum.release();
    im_sum_sq.release();
    return max_s;
}

void NiblackSauvolaWolfJolion (cv::Mat im, cv::Mat output, NiblackVersion version, double dR,
	int winx, int winy, double k) {

	if(im.channels() > 1)
        cv::cvtColor(im, im, CV_RGB2GRAY);
	
	double m, s, max_s;
	double th=0;
	double min_I, max_I;
	int wxh	= winx/2;
	int wyh	= winy/2;
	int x_firstth= wxh;
	int x_lastth = im.cols-wxh-1;
	int y_lastth = im.rows-wyh-1;
    int y_firstth= wyh;

	// Create local statistics and store them in a double matrices
    cv::Mat map_m = cv::Mat::zeros (im.rows, im.cols, CV_32F);
    cv::Mat map_s = cv::Mat::zeros (im.rows, im.cols, CV_32F);
	max_s = calcLocalStats (im, map_m, map_s, winx, winy);
	
	minMaxLoc(im, &min_I, &max_I);
			
    cv::Mat thsurf (im.rows, im.cols, CV_32F);
			
	// Create the threshold surface, including border processing
	// ----------------------------------------------------

	for	(int j = y_firstth ; j<=y_lastth; j++) {

		// NORMAL, NON-BORDER AREA IN THE MIDDLE OF THE WINDOW:
		for	(int i=0 ; i <= im.cols-winx; i++) {

			m  = map_m.fget(i+wxh, j);
    		s  = map_s.fget(i+wxh, j);

    		// Calculate the threshold
    		switch (version) {

    			case NIBLACK:
    				th = m + k*s;
    				break;

    			case SAUVOLA:
	    			th = m * (1 + k*(s/dR-1));
	    			break;

    			case WOLFJOLION:
    				th = m + k * (s/max_s-1) * (m-min_I);
    				break;
    				
    			default:
    				std::cerr << "Unknown threshold type in ImageThresholder::surfaceNiblackImproved()\n";
    				exit (1);
    		}
    		
    		thsurf.fset(i+wxh,j,th);

    		if (i==0) {
        		// LEFT BORDER
        		for (int i=0; i<=x_firstth; ++i)
                	thsurf.fset(i,j,th);

        		// LEFT-UPPER CORNER
        		if (j==y_firstth)
        			for (int u=0; u<y_firstth; ++u)
        			for (int i=0; i<=x_firstth; ++i)
        				thsurf.fset(i,u,th);

        		// LEFT-LOWER CORNER
        		if (j==y_lastth)
        			for (int u=y_lastth+1; u<im.rows; ++u)
        			for (int i=0; i<=x_firstth; ++i)
        				thsurf.fset(i,u,th);
    		}

			// UPPER BORDER
			if (j==y_firstth)
				for (int u=0; u<y_firstth; ++u)
					thsurf.fset(i+wxh,u,th);

			// LOWER BORDER
			if (j==y_lastth)
				for (int u=y_lastth+1; u<im.rows; ++u)
					thsurf.fset(i+wxh,u,th);
		}

		// RIGHT BORDER
		for (int i=x_lastth; i<im.cols; ++i)
        	thsurf.fset(i,j,th);

  		// RIGHT-UPPER CORNER
		if (j==y_firstth)
			for (int u=0; u<y_firstth; ++u)
			for (int i=x_lastth; i<im.cols; ++i)
				thsurf.fset(i,u,th);

		// RIGHT-LOWER CORNER
		if (j==y_lastth)
			for (int u=y_lastth+1; u<im.rows; ++u)
			for (int i=x_lastth; i<im.cols; ++i)
				thsurf.fset(i,u,th);
	}
	
	for	(int y=0; y<im.rows; ++y) 
	for	(int x=0; x<im.cols; ++x) 
	{
    	if (im.uget(x,y) >= thsurf.fget(x,y))
    	{
    		output.uset(x,y,255);
    	}
    	else
    	{
    	    output.uset(x,y,0);
    	}
    }
    map_m.release();
    map_s.release();
    thsurf.release();
}
