// This file is designed to applying different filters on images and called Image Processing System

// This is link google drive that contain docs file and video that explain how to work this program -> https://drive.google.com/drive/folders/1rnZG3cXsE96pX53ffWOF_3bQUNUzLdNN?usp=drive_link

/*
-----------------------------------------------------------------------------
Course : CS213 Object Oriented Programming, FCAI, Cairo University
Doctor : Dr. Mohammad El-Ramly
Assignment 1 - Part 2 (Beast Level: 18 filters + GUI)

Name                 ID          Section     Filters (original menu numbers)
-------------------  ----------  ----------  ------------------------------
Mohamed Adel         20250572    S19 , S20   4 (Frame), 8 (Resize), 12 (Blur), 16 (Infrared)
Ziad Waleed          20250265    S19 , S20   1 (GrayScale), 5 (Flip), 9 (Merge), 13 (Land Of Wano)
Amr Maher            20250455    S19 , S20   2 (Black And White), 6 (Rotate), 10 (Detect Edges), 14 (Cinema / Old TV)
Mohamed Hassaan      20250555    S19 , S20   3 (Invert), 7 (Darken/Lighten), 11 (Crop), 15 (Wano Purple)

Extra filters (Beast Level):
- 17 Oil Painting : Amr Maher & Mohamed Hassaan
- 18 Skew Image   : Mohamed Adel & Ziad Waleed & Amr Maher
GUI (Qt Widgets) : made by all team members.
Menu : made by all team members.
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <stdexcept>
#include "Image_Class.h"

using namespace std;

void handleSaveImage(Image& currentImage, string& currentPath, bool& isModified){
	int saveChoice;
	cout << "\n---                                     Save Options                                     ---\n\n";
	cout << "1- Save : " << currentPath << ")\n";
	cout << "2- Save As\n\n";
	cout << "Choose Option: ";
	cin >> saveChoice;
	
	string targetPath = currentPath;
	
	if (saveChoice == 2){
		cout << "\nEnter new image path/filename (e.g., output.png): ";
		cin >> targetPath;
	}else if (saveChoice != 1){
		cout << "\nInvalid choice! Defaulting to original file path.\n";
	}
	bool savedSuccessfully = false;
	while(!savedSuccessfully){
		try {
			if (currentImage.saveImage(targetPath)){
				cout << "Emage saved successfully to: " << targetPath << "\n\n";
				isModified = false;
				savedSuccessfully = true;
				currentPath = targetPath;
			}
		}
		catch (const invalid_argument& e){
			cout << "Error saving image: " << e.what() << "\n\n";
			cout << "Please enter a valid target path/filename (or type '0' to cancel): ";
			cin >> targetPath;
			if (targetPath == "0"){
				cout << "Saved operation canceled.\n\n";
				break;
			}
		
		}
	}
}

void handleLoadNewImage(Image& currentImage, string& currentPath, bool& isImageLoaded, bool& isModified){
	if (isModified){
		char ans;
		cout << "\nThe current image has unsaved changes. Do you want to save it first? (y/n) ";
		cin >> ans;
		if (ans == 'y' || ans == 'Y'){
			handleSaveImage(currentImage,currentPath,isModified);
		}
	}
	string newPath = "";
	bool loadedSuccessfully = false;
	while(!loadedSuccessfully){
		cout << "\nPlease enter the new image path: ";
		cin >> newPath;
		if (newPath == "0"){
			cout << "Loading new image canceled.\n\n";
			break;
		}
		try{
			if (currentImage.loadNewImage(newPath)){
				loadedSuccessfully = true;
				currentPath = newPath;
				isModified = false;
				isImageLoaded = true;
				cout << "\nNew image loaded successfully!\n\n";
			}
		}
		catch (const invalid_argument& e){
			cout << "Error: " << e.what() << ". Please try again.\n";
		}
	}
}

void handleExit(Image& currentImage, string& currentPath, bool& isModified){
	if (isModified){
		char ans;
		cout << "\nYou have unsaved changes! Do you want to save before exiting? (y/n)";
		cin >> ans;
		if (ans == 'y' || ans == 'Y'){
			handleSaveImage(currentImage, currentPath, isModified);
		}
	}
	cout << "\nExiting program... Goodbye!\n";
}

void invert(Image& currentImage) {
	
    for (int i = 0; i < currentImage.width; i++) {
        for (int j = 0; j < currentImage.height; j++) {
            for (int k = 0; k < currentImage.channels; k++) {

                currentImage(i, j, k) = 255 - currentImage(i, j, k);

            }
        }
    }
    cout << "Done (Invert Filter)\n";
}

void darkenAndLighten(Image& currentImage) {
	int choose;
	while(choose != 1 && choose != 2){
		cout << "\n---                                     Filters Menu                                     ---\n\n";
		cout << "1- Darken mode                         -                        ";
		cout << "2- Lighten mode\n\n";
		cout << "Enter Your Choice (1 or 2): ";
		cin >> choose;
		if (choose == 1){
			for (int i = 0; i < currentImage.width; i++) {
        		for (int j = 0; j < currentImage.height; j++) {
            		for (int k = 0; k < currentImage.channels; k++) {

                		currentImage(i, j, k) = currentImage(i, j, k)*0.5;

		            }
		        }
		    }
    		cout << "Done (Darken Filter)\n";
		}
		else if (choose == 2){
			for (int i = 0; i < currentImage.width; i++) {
        		for (int j = 0; j < currentImage.height; j++) {
            		for (int k = 0; k < currentImage.channels; k++) {

		              int newvalue= currentImage(i, j, k) * 1.5;
		              if (newvalue > 255) {
		                  newvalue = 255;
		              }
              			currentImage(i, j, k) = newvalue;

		            }
		        }
			}
    		cout << "Done (Lighten Filter)\n";
		}
		else {
			cout << "Invalid Input, Please choose correct number\n";
		}
	}
    
}

void blur(Image& currentImage){
	int pass;
	cout << "\nChoose Blur Degree: ";
	cin >> pass;
	cout << "\n";
	for (int c = 1 ; c <= pass ; c++){
		Image temp = currentImage;
		for (int i = 1 ; i < currentImage.width - 1 ; i++){
			for (int j =  1; j < currentImage.height - 1 ; j++){
				for (int k = 0 ; k < currentImage.channels ; k++){
					int sum = (currentImage(i-1,j-1,k) + currentImage(i,j-1,k) + currentImage(i+1,j-1,k) + currentImage(i-1,j,k) + currentImage(i,j,k) + currentImage(i+1,j,k) + currentImage(i-1,j+1,k) + currentImage(i,j+1,k) + currentImage(i+1,j+1,k));
					temp(i,j,k) = sum/9;
				}
			}
		}
		currentImage = temp;
		cout << "Pass " << c << " Completed!\n";
	}
	cout << "Done (Blur Filter)\n";
}

void infrared(Image& currentImage){
	for (int i = 0 ; i < currentImage.width ; i++){
		for (int j = 0 ; j < currentImage.height ; j++){
			int r = currentImage(i , j , 0);
			int g = currentImage(i , j , 1);
			int b = currentImage(i , j , 2);
			
			currentImage(i , j , 0) = 255;
			currentImage(i , j , 1) = 255 - g;
			currentImage(i , j , 2) = 255 - b;
		}
	}
	cout << "Done (Infrared Filter)\n";
}

void addFrame(Image& currentImage){
	int choice ;
	while(choice != 1 && choice != 2){
		cout << "\n---                                 Frame Filter Menu                                  ---\n\n";
		cout << "1- Simple Frame                        -                        ";
		cout << "2- Fancy Frame\n\n";
		cout << "Enter Your Choice (1 or 2): ";
		cin >> choice;
		
		int width = currentImage.width;
		int height = currentImage.height;
		
		if (choice == 1){
			int frameSize, colorChoice;
			cout << "Enter frame thickness (from 10 to 50): ";
			cin >> frameSize;
			
			cout << "Select frame color:\n";
			cout << "1- Blue\n2- Red\n3- Green\n4- White\nChoice: ";
			cin >> colorChoice;
			
			int r = 0 , g = 0 , b = 0;
			if (colorChoice == 1){r = 0 ; g = 0 ; b = 255;}
			else if (colorChoice == 2){r = 255 ; g = 0 ; b = 0;}
			else if (colorChoice == 3){r = 0 ; g = 255 ; b = 0;}
			else if (colorChoice == 4){r = 255 ; g = 255 ; b = 255;}
			
			for (int i = 0 ; i < width ; i++){
				for (int j = 0 ; j < height ; j++){
					if (i < frameSize || j < frameSize || i >= width - frameSize || j >= height - frameSize){
						currentImage(i , j , 0) = r;
						currentImage(i , j , 1) = g;
						currentImage(i , j , 2) = b;
					}
				}
			}
			cout << "\nDone (AddFrame Filter)\n";
		}
		else if (choice == 2){
			int outerSize , innerStripeSize;
			cout << "Enter outer frame thickness (e.g., 10): ";
			cin >> outerSize;
			cout << "Enter inner thickness (e.g., 3): ";
			cin >> innerStripeSize;
			
			int startStripe = outerSize/2 - innerStripeSize/2;
	        int endStripe = outerSize/2 + innerStripeSize/2;
			for (int j = 0; j < height; j++) {
	            for (int i = 0; i < width; i++) {
	                if (i < outerSize || j < outerSize || i >= width - outerSize || j >= height - outerSize){
						if ((i >= startStripe && i < endStripe) || (i >= width - endStripe && i < width - startStripe) || (j >= startStripe && j < endStripe) || (j >= height - endStripe && j < height - startStripe)){
							currentImage(i , j , 0) = 255;
							currentImage(i , j , 1) = 255;
							currentImage(i , j , 2) = 255;
						}
						else{
							currentImage(i, j, 0) = 0;
					        currentImage(i, j, 1) = 100;
					        currentImage(i, j, 2) = 255;
						}
					}
					
	            }
	        }
	        cout << "\nDone (AddFrame Filter)\n";
		}
		else{
			cout << "Invalid Input, Please choose correct number\n";
		}
	}
}

void grayScale(Image& currentImage){
	int width = currentImage.width;
	int height = currentImage.height;
	for (int i = 0;i < height; i++) {
		for (int j = 0; j < width; j++) {
			int r = currentImage.getPixel(j, i, 0);
			int g = currentImage.getPixel(j, i, 1);
			int b = currentImage.getPixel(j, i, 2);

			int avr = (r + g + b) / 3;
			currentImage.setPixel(j,i,0,avr);
			currentImage.setPixel(j,i,1,avr);
			currentImage.setPixel(j,i,2,avr);
		}
	}
	cout << "Done (GrayScale Filter)\n";
}

void blackAndWhite(Image& currenImage){
    for (int i = 0; i < currenImage.height; i++)
    {
        for (int j = 0; j < currenImage.width; j++)
        {
            int r = currenImage.getPixel(j, i, 0);
            int g = currenImage.getPixel(j, i, 1);
            int b = currenImage.getPixel(j, i, 2);

            int gray = (r + g + b) / 3;

            if (gray >= 128)
                gray = 255;
            else
                gray = 0;

            currenImage.setPixel(j, i, 0, gray);
            currenImage.setPixel(j, i, 1, gray);
            currenImage.setPixel(j, i, 2, gray);
        }
    }

    cout << "Done (BlackAndWhite Filter)" << endl;
}

void flip(Image& currentImage) {
    int width = currentImage.width;
    int height = currentImage.height;
    int choose;
	while(choose != 1 && choose != 2){
		cout << "\n---                                 Flip Filter Menu                                  ---\n\n";
		cout << "1- Horizontal Flip                     -                        ";
		cout << "2- Vertical Flip\n\n";
		cout << "Enter Your Choice (1 or 2): ";
		cin >> choose;
		if (choose == 1){
		    for (int i = 0; i < height; i++) {
		        for (int j = 0; j < width / 2; j++) {
		            for (int k = 0; k < currentImage.channels; k++) {
		                swap(currentImage(j, i, k), currentImage(width - 1 - j, i, k));
		            }
		        }
		    }
		    cout << "Done (Flip Horizontal Filter)\n";
		}
		else if (choose == 2){
			for (int i = 0; i < height / 2; i++) {
		        for (int j = 0; j < width; j++) {
		            for (int k = 0; k < currentImage.channels; k++) {
		                swap(currentImage(j, i, k), currentImage(j, height - 1 - i, k));
		            }
		        }
		    }
		    cout << "Done (Flip Vertical Filter)\n";
		}
		else {
			cout << "Invalid Input, Please choose correct number\n";
		}
	}
}

void rotate(Image& currentImage){
    int oldWidth = currentImage.width;
    int oldHeight = currentImage.height;
	int angle;
	while(angle != 360 && angle != 90 && angle != 270 && angle != 180){
		cout << "Please choose angle (e.g., 90 or 180 or 270 or 360): ";
		cin >> angle;
	    if (angle == 360){
	        cout << "Done (Rotate Filter)\n";
	    }
	
	    else if (angle == 90 || angle == 270){
	        Image rotatedImage(oldHeight, oldWidth);
	
	        for (int i = 0; i < oldHeight; i++)
	        {
	            for (int j = 0; j < oldWidth; j++)
	            {
	                for (int k = 0; k < currentImage.channels; k++)
	                {
	                    if (angle == 90)
	                    {
	                        rotatedImage.setPixel(
	                            oldHeight - 1 - i,
	                            j,
	                            k,
	                            currentImage.getPixel(j, i, k)
	                        );
	                    }
	                    else
	                    {
	                        rotatedImage.setPixel(
	                            i,
	                            oldWidth - 1 - j,
	                            k,
	                            currentImage.getPixel(j, i, k)
	                        );
	                    }
	                }
	            }
	        }
	
	        currentImage = rotatedImage;
	        cout << "Done (Rotate Filter)\n";
	    }
	    else if (angle == 180){
	        Image rotatedImage(oldWidth, oldHeight);
	
	        for (int i = 0; i < oldHeight; i++)
	        {
	            for (int j = 0; j < oldWidth; j++)
	            {
	                for (int k = 0; k < currentImage.channels; k++)
	                {
	                    rotatedImage.setPixel(
	                        oldWidth - 1 - j,
	                        oldHeight - 1 - i,
	                        k,
	                        currentImage.getPixel(j, i, k)
	                    );
	                }
	            }
	        }
	
	        currentImage = rotatedImage;
	        cout << "Done (Rotate Filter)\n";
	    }
	    else{
	    	cout << "Invalid Input, Please choose correct number\n";
		}
	}
	    
}

void reSize(Image& currentImage){
	int oldWidth = currentImage.width;
	int oldHeight = currentImage.height;
	int newWidth , newHeight;
	cout << "Width: " ;
	cin >> newWidth;
	cout << "Height: ";
	cin >> newHeight;
	Image newImage(newWidth , newHeight);
	float facHeight = (float) oldHeight / newHeight;
	float facWidth = (float) oldWidth / newWidth;
	for (int i = 0 ; i < newWidth ; i++){
		for (int j = 0 ; j < newHeight ; j++){
			int I = min((int) (i * facWidth) , oldWidth - 1);
			int J = min((int) (j * facHeight), oldHeight - 1);
			newImage(i,j,0) = currentImage(I,J,0);
			newImage(i,j,1) = currentImage(I,J,1);
			newImage(i,j,2) = currentImage(I,J,2);
		}
	}
	currentImage = newImage;
	cout << "Done (ReSize Filter)\n";
}

void crop(Image& currentImage) {
	int x, y, w, h;
	cout << "Current Dimensions: " << currentImage.width << " x " << currentImage.height << "\n";

	cout << "Enter starting point X (upper left corner): ";
	cin >> x;
	cout << "Enter starting point Y (upper left corner): ";
	cin >> y;
	cout << "Enter crop Width (W): ";
	cin >> w;
	cout << "Enter crop Height (H): ";
	cin >> h;

	if (x < 0 || y < 0 || w <= 0 || h <= 0 || (x + w) > currentImage.width || (y + h) > currentImage.height) {
		cout << "Error: Crop dimensions are out of bounds or invalid!\n";
	}
	else{
		Image croppedImage(w, h);
	
		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				for (int k = 0; k < currentImage.channels; k++) {
					croppedImage(i, j, k) = currentImage(x + i, y + j, k);
				}
			}
		}
	
		currentImage = croppedImage;
		cout << "Done (Crop Filter)\n";
	}
}

void wanoPurple(Image& currentImage) {
	for (int i = 0; i < currentImage.width; i++) {
		for (int j = 0; j < currentImage.height; j++) {
			int r = currentImage(i, j, 0);
			int g = currentImage(i, j, 1);
			int b = currentImage(i, j, 2);

			int newR = min(255, (int)(r * 1.1));
			int newG = (int)(g * 0.5);
			int newB = min(255, (int)(b * 1.1));

			currentImage(i, j, 0) = newR;
			currentImage(i, j, 1) = newG;
			currentImage(i, j, 2) = newB;
		}
	}
	cout << "Done (WanoPurple Filter)\n";
}

unsigned char clamp255(float v) {
    return (unsigned char)max(0.0f, min(255.0f, v));
}
inline int clamp(int v, int low, int high) {
    if (v < low) return low;
    if (v > high) return high;
    return v;
}

void detected(Image& currentImage){
    int height = currentImage.height;
    int width  = currentImage.width;
    const int threshold = 90;

   
    vector<vector<int>> gray(width, vector<int>(height));
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            int r = currentImage(i, j, 0);
            int g = currentImage(i, j, 1);
            int b = currentImage(i, j, 2);
            int avg = (r + g + b) / 3;
            gray[i][j] = avg;
        }
    }


    int gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

   
    Image result(width, height);
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            for (int z = 0; z < 3; z++){
                result(i, j, z) = 255;
            }
        }
    }

    
    for (int i = 1; i < width - 1; i++){
        for (int j = 1; j < height - 1; j++){
            int sumx = 0, sumy = 0;
            for (int m = -1; m <= 1; m++){
                for (int n = -1; n <= 1; n++){
                    sumx += gray[i + m][j + n] * gx[m + 1][n + 1];
                    sumy += gray[i + m][j + n] * gy[m + 1][n + 1];
                }
            }
            int magnitude = abs(sumx) + abs(sumy);
            int value = (magnitude > threshold) ? 0 : 255;
            for (int k = 0; k < 3; k++){
                result(i, j, k) = value;
            }
        }
    }

   
    currentImage = result;
    cout << "Done (Detect Image Edges Filter)\n";
}

void jitterRows(Image& img, int maxShift, int rate) {
    Image copy = img;
    for (int y = 0; y < img.height; y++) {
        if (rand() % rate != 0) continue;           // only 1 row out of every "rate" rows
        int off = rand() % (2 * maxShift + 1) - maxShift;
        for (int x = 0; x < img.width; x++) {
            int sx = clamp(x - off, 0, img.width - 1);
            for (int c = 0; c < 3; c++)
                img(x, y, c) = copy(sx, y, c);
        }
    }
}

void rgbShift(Image& img, int shift) {
    Image copy = img;
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            int xr = clamp(x - shift, 0, img.width - 1);
            int xb = clamp(x + shift, 0, img.width - 1);
            img(x, y, 0) = copy(xr, y, 0);   // red
            img(x, y, 2) = copy(xb, y, 2);   // blue
        }
    }
}

void contrastSaturation(Image& img, float contrast, float saturation) {
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            float r = (img(x, y, 0) - 128) * contrast + 128;
            float g = (img(x, y, 1) - 128) * contrast + 128;
            float b = (img(x, y, 2) - 128) * contrast + 128;
            float gray = 0.299f * r + 0.587f * g + 0.114f * b;
            img(x, y, 0) = clamp255(gray + (r - gray) * saturation);
            img(x, y, 1) = clamp255(gray + (g - gray) * saturation);
            img(x, y, 2) = clamp255(gray + (b - gray) * saturation);
        }
    }
}

void scanlines(Image& img, int step, float darkness) {
    for (int y = 0; y < img.height; y += step)
        for (int x = 0; x < img.width; x++)
            for (int c = 0; c < 3; c++)
                img(x, y, c) = clamp255(img(x, y, c) * darkness);
}

void glitchBar(Image& img, int barHeight) {
    for (int y = 0; y < barHeight && y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y, 0) = clamp255(img(x, y, 0) * 0.4f + 255 * 0.6f);
            img(x, y, 1) = clamp255(img(x, y, 1) * 0.4f + 120 * 0.6f);
            img(x, y, 2) = clamp255(img(x, y, 2) * 0.4f + 255 * 0.6f);
        }
    }
}

void addNoise(Image& img, int strength) {
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            int n = rand() % (2 * strength + 1) - strength;
            for (int c = 0; c < 3; c++)
                img(x, y, c) = clamp255(img(x, y, c) + n);
        }
    }
}

void cinema(Image& img) {
    jitterRows(img, 12, 25);
    rgbShift(img, 4);
    contrastSaturation(img, 1.2f, 1.3f);
    scanlines(img, 3, 0.6f);
    glitchBar(img, max(2, img.height / 100));
    addNoise(img, 8);
    cout << "Done (Old TV Filter)\n";
}

void resizeTo(Image& img, int newWidth, int newHeight) {
    Image result(newWidth, newHeight);
    float facWidth = (float)img.width / newWidth;
    float facHeight = (float)img.height / newHeight;
    for (int i = 0; i < newWidth; i++) {
        for (int j = 0; j < newHeight; j++) {
            int I = min((int)(i * facWidth), img.width - 1);
            int J = min((int)(j * facHeight), img.height - 1);
            for (int k = 0; k < 3; k++)
                result(i, j, k) = img(I, J, k);
        }
    }
    img = result;
}

void mergeImages(Image& currentImage) {
    Image secondImage;
    string path;
    bool loaded = false;

    while (!loaded) {
        cout << "\nEnter the path of the second image (or 0 to cancel): ";
        cin >> path;
        if (path == "0") {
            cout << "Merge canceled.\n";
            return;
        }
        try {
            if (secondImage.loadNewImage(path)) {
                loaded = true;
                cout << "Second image loaded successfully!\n";
            }
        }
        catch (const invalid_argument& e) {
            cout << "Error: " << e.what() << ". Please try again.\n";
        }
    }

    Image a = currentImage;
    Image b = secondImage;
    int newWidth, newHeight;

    if (a.width == b.width && a.height == b.height) {
        newWidth = a.width;
        newHeight = a.height;
    }
    else {
        int choice = 0;
        while (choice < 1 || choice > 3) {
            cout << "\nThe images have different sizes:\n";
            cout << "1- Resize to the biggest width and height\n";
            cout << "2- Merge the common area only\n";
            cout << "3- Place the second image on the first at a chosen position\n";
            cout << "Choice: ";
            cin >> choice;
        }

        if (choice == 3) {
            int x, y;
            cout << "Enter X where the second image starts (0 = left edge): ";
            cin >> x;
            cout << "Enter Y where the second image starts (0 = top edge): ";
            cin >> y;

            int startX = max(0, x);
            int startY = max(0, y);
            int endX = min(a.width, x + b.width);
            int endY = min(a.height, y + b.height);

            if (startX >= endX || startY >= endY) {
                cout << "The images do not overlap! Merge canceled.\n";
                return;
            }

            Image result = a;
            for (int i = startX; i < endX; i++)
                for (int j = startY; j < endY; j++)
                    for (int k = 0; k < 3; k++)
                        result(i, j, k) = (a(i, j, k) + b(i - x, j - y, k)) / 2;

            currentImage = result;
            cout << "Done (Merge Filter)\n";
            return;
        }

        if (choice == 1) {
            newWidth = max(a.width, b.width);
            newHeight = max(a.height, b.height);
            resizeTo(a, newWidth, newHeight);
            resizeTo(b, newWidth, newHeight);
        }
        else {
            newWidth = min(a.width, b.width);
            newHeight = min(a.height, b.height);
        }
    }

    Image result(newWidth, newHeight);
    for (int i = 0; i < newWidth; i++)
        for (int j = 0; j < newHeight; j++)
            for (int k = 0; k < 3; k++)
                result(i, j, k) = (a(i, j, k) + b(i, j, k)) / 2;

    currentImage = result;
    cout << "Done (Merge Filter)\n";
}

void landOfWano(Image& currentImage) {
    for (int i = 0; i < currentImage.width; i++) {
        for (int j = 0; j < currentImage.height; j++) {
            int r = currentImage(i, j, 0) * 1.2;
            int g = currentImage(i, j, 1) * 1.1;
            int b = currentImage(i, j, 2) * 0.9;

            currentImage(i, j, 0) = min(255, r);
            currentImage(i, j, 1) = min(255, g);
            currentImage(i, j, 2) = min(255, b);
        }
    }
    cout << "Done (Land Of Wano Filter)\n";
}

void oilPainting(Image& currentImage){
    int radius, levels;
    cout << "Enter brush radius (1 to 10): ";
    cin >> radius;
    cout << "Enter intensity levels (4 to 50, lower = more painted look): ";
    cin >> levels;
    radius = clamp(radius, 1, 10);
    levels = clamp(levels, 4, 50);

    const int w = currentImage.width;
    const int h = currentImage.height;
    Image src = currentImage;
    vector<int> cnt(levels), sr(levels), sg(levels), sb(levels);

    for (int y = 0; y < h; y++){
        for (int x = 0; x < w; x++){
            fill(cnt.begin(), cnt.end(), 0);
            fill(sr.begin(), sr.end(), 0);
            fill(sg.begin(), sg.end(), 0);
            fill(sb.begin(), sb.end(), 0);

            for (int dy = -radius; dy <= radius; dy++){
                int ny = clamp(y + dy, 0, h - 1);
                for (int dx = -radius; dx <= radius; dx++){
                    int nx = clamp(x + dx, 0, w - 1);
                    int r = src(nx, ny, 0);
                    int g = src(nx, ny, 1);
                    int b = src(nx, ny, 2);
                    int bin = ((r + g + b) / 3) * (levels - 1) / 255;
                    cnt[bin]++;
                    sr[bin] += r;
                    sg[bin] += g;
                    sb[bin] += b;
                }
            }

            int best = 0;
            for (int i = 1; i < levels; i++)
                if (cnt[i] > cnt[best]) best = i;

            currentImage(x, y, 0) = sr[best] / cnt[best];
            currentImage(x, y, 1) = sg[best] / cnt[best];
            currentImage(x, y, 2) = sb[best] / cnt[best];
        }
    }
    cout << "Done (Oil Painting Filter)\n";
}

void skewImage(Image& currentImage){
    int angle;
    cout << "Enter skew angle in degrees (-60 to 60, negative skews left): ";
    cin >> angle;
    if (angle < -60 || angle > 60 || angle == 0){
        cout << "Invalid angle! Skew canceled.\n";
        return;
    }

    const double pi = 3.14159265358979323846;
    const double t = tan(angle * pi / 180.0);
    const int w = currentImage.width;
    const int h = currentImage.height;
    const int extra = (int)ceil(fabs(t) * (h - 1));
    const int newW = w + extra;

    Image result(newW, h);
    for (int x = 0; x < newW; x++)
        for (int y = 0; y < h; y++)
            for (int k = 0; k < 3; k++)
                result(x, y, k) = 255;          // white background

    for (int y = 0; y < h; y++){
        double shift = (t >= 0) ? fabs(t) * (h - 1 - y) : fabs(t) * y;
        int s = (int)lround(shift);
        for (int x = 0; x < w; x++)
            for (int k = 0; k < 3; k++)
                result(x + s, y, k) = currentImage(x, y, k);
    }
    currentImage = result;
    cout << "Done (Skew Filter)\n";
}

void handleApplyFilter(Image& currentImage, bool& isModified){
	int filterChoice = 0;
	while(filterChoice != 19){
		cout << "\n============================================================================================\n\n";
		cout << "---                                     Filters Menu                                     ---\n\n";
		cout << "1- GrayScale Filter                    -                        ";
		cout << "2- BlackAndWhite Filter\n\n";
		cout << "3- Invert Filter                       -                        ";
		cout << "4- AddFrame Filter\n\n";
		cout << "5- Flip Filter                         -                        ";
		cout << "6- Rotate FIlter\n\n";
		cout << "7- Darken or Lighten Filter            -                        ";
		cout << "8- ReSize Filter\n\n";
		cout << "9- MergeImage Filter                   -                        ";
		cout << "10- Detected Filter\n\n";
		cout << "11- Crop Filter                        -                        ";
		cout << "12- Blur Filter\n\n";
		cout << "13- LandOfWano                         -                        ";
		cout << "14- Cinema Filter\n\n";
		cout << "15- WanoPurple Filter                  -                        ";
		cout << "16- Infrared Filter\n\n";
		cout << "17- Oil Painting Filter                -                        ";
		cout << "18- Skew Filter\n\n";
		cout << "19- Back to Main Menu\n\n";
		cout << "============================================================================================\n\n";
		cout << "Choose filter number: ";
		cin >> filterChoice;
		switch(filterChoice){
			case 1:
				cout << "\nApplying GrayScale Filter...\n";
				grayScale(currentImage);
				isModified = true;
				cout << "GrayScale applied successfully!\n";
				break;
			case 2:
				cout << "\nApplying BlackAndWhite Filter...\n";
				blackAndWhite(currentImage);
				isModified = true;
				cout << "BlackAndWhite applied successfully!\n";
				break;
			case 3:
				cout << "\nApplying Invert Filter...\n";
				invert(currentImage);
				isModified = true;
				cout << "Invert applied successfully!\n";
				break;
			case 4:
				cout << "\nApplying AddFrame Filter...\n";
				addFrame(currentImage);
				isModified = true;
				cout << "AddFrame applied successfully!\n";
				break;
			case 5:
				cout << "\nApplying Flip Filter...\n";
				flip(currentImage);
				isModified = true;
				cout << "Flip applied successfully!\n";
				break;
			case 6:
				cout << "\nApplying Rotate Filter...\n";
				rotate(currentImage);
				isModified = true;
				cout << "Rotate applied successfully!\n";
				break;
			case 7:
				cout << "\nApplying Darken or Lighten Filter...\n";
				darkenAndLighten(currentImage);
				isModified = true;
				cout << "Darken or Lighten applied successfully!\n";
				break;
			case 8:
				cout << "\nApplying ReSize Filter...\n";
				reSize(currentImage);
				isModified = true;
				cout << "ReSize applied successfully!\n";
				break;
			case 9:
				cout << "\nApplying MergeImage Filter...\n";
				mergeImages(currentImage);
				isModified = true;
				cout << "MergeImage applied successfully!\n";
				break;
			case 10:
				cout << "\nApplying Detected Filter...\n";
				detected(currentImage);
				isModified = true;
				cout << "Detected applied successfully!\n";
				break;
			case 11:
				cout << "\nApplying Crop Filter...\n";
				crop(currentImage);
				isModified = true;
				cout << "Crop applied successfully!\n";
				break;
			case 12:
				cout << "\nApplying Blur Filter...\n";
				blur(currentImage);
				isModified = true;
				cout << "Blur applied successfully!\n";
				break;
			case 13:
				cout << "\nApplying LandOfWano Filter...\n";
				landOfWano(currentImage);
				isModified = true;
				cout << "LandOfWano applied successfully!\n";
				break;
			case 14:
				cout << "\nApplying Cinema Filter...\n";
				cinema(currentImage);
				isModified = true;
				cout << "Cinema applied successfully!\n";
				break;
			case 15:
				cout << "\nApplying WanoPurple Filter...\n";
				wanoPurple(currentImage);
				isModified = true;
				cout << "WanoPurple applied successfully!\n";
				break;
			case 16:
				cout << "\nApplying Infrared Filter...\n";
				infrared(currentImage);
				isModified = true;
				cout << "Infrared applied successfully!\n";
				break;
			case 17:
				cout << "\nApplying Oil Painting Filter...\n";
				oilPainting(currentImage);
				isModified = true;
				cout << "Oil Painting applied successfully!\n";
				break;
			case 18:
				cout << "\nApplying Skew Filter...\n";
				skewImage(currentImage);
				isModified = true;
				break;
			case 19:
				cout << "Returning to Main Menu...\n";
				break;
			default:
				cout << "Invalid choice! Please select a valid filter number.\n";
				break;
		}
	}
}

int main() {
    Image currentImage;
    string currentPath = "";
    bool isImageLoaded = false;
    bool isModified = false;
	
    while (!isImageLoaded) {
        cout << "Please enter image path: ";
        cin >> currentPath;

        try {
            if (currentImage.loadNewImage(currentPath)) {
                isImageLoaded = true;
                cout << "\nImage loaded successfully!\n\n";
            }
        } 
        catch (const invalid_argument& e) {
            cout << "Error: " << e.what() << ". Please try again.\n";
            isImageLoaded = false;
        }
    }
    int choose = 0;
    while(choose != 4){
    	cout << "\n---                          Welcome to Image Processing System                          ---\n\n";
    	cout << "============================================================================================\n\n";
        cout << "1- Load New Image                        -                        ";
        cout << "2- Apply Filters\n\n";
        cout << "3- Save Image                            -                        ";
        cout << "4- Exit\n";
        cout << "\n============================================================================================\n\n";
        cout << "Choose service number: ";
        cin >> choose;
	    switch(choose){
	    	case 1:
	    		handleLoadNewImage(currentImage, currentPath, isImageLoaded, isModified);
	    		break;
	    	case 2:
	    		handleApplyFilter(currentImage, isModified);
	    		break;
	    		
	    	case 3:
	    		handleSaveImage(currentImage, currentPath, isModified);
	    		break;
	    	case 4:
				handleExit(currentImage, currentPath, isModified);
				break;
	    	default:
	    		cout << "\nInvalid choice! Please select a valid number from 1 to 4.\n\n";
				break;
		}
	}
	return 0;
}

