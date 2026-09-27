#include <iostream>
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


// filter blur not completed
void blur(Image& currentImage){
	Image temp = currentImage;
	for (int i = 1 ; i < currentImage.width - 1 ; i++){
		for (int j =  1; j < currentImage.height - 1 ; j++){
			for (int k = 0 ; k < currentImage.channels ; k++){
				temp(i,j,k) = (currentImage(i-1,j-1,k) + currentImage(i,j-1,k) + currentImage(i+1,j-1,k) + currentImage(i-1,j,k) + currentImage(i,j,k) + currentImage(i+1,j,k) + currentImage(i-1,j+1,k) + currentImage(i,j+1,k) + currentImage(i+1,j+1,k))/9;
			}
		}
	}
	currentImage = temp;
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

void handleApplyFilter(Image& currentImage, bool& isModified){
	int filterChoice = 0;
	while(filterChoice != 10){
		cout << "\n============================================================================================\n\n";
		cout << "---                                     Filters Menu                                     ---\n\n";
		cout << "1- Invert Filter                       -                        ";
		cout << "2- Darken or Lighten Filter\n\n";
		cout << "3- Blur Filter                         -                        ";
		cout << "4- Infrared Filter\n\n";
		cout << "5- AddFrame Filter                     -                        ";
		cout << "6- GrayScale Filter\n\n";
		cout << "7- BlackAndWhite Filter                -                        ";
		cout << "8- Rotate FIlter\n\n";
		cout << "9- Flip Filter                         -                        ";
		cout << "10- Back to Main Menu\n";
		cout << "============================================================================================\n\n";
		cout << "Choose filter number: ";
		cin >> filterChoice;
		switch(filterChoice){
			case 1:
				cout << "\nApplying Invert Filter...\n";
				invert(currentImage);
				isModified = true;
				cout << "Invert applied successfully!\n";
				break;
			case 2:
				cout << "\nApplying Darken or Lighten Filter...\n";
				darkenAndLighten(currentImage);
				isModified = true;
				cout << "Darken or Lighten applied successfully!\n";
				break;
			case 3:
				cout << "\nApplying Blur Filter...\n";
				blur(currentImage);
				isModified = true;
				cout << "Blur applied successfully!\n";
				break;
			case 4:
				cout << "\nApplying Infrared Filter...\n";
				infrared(currentImage);
				isModified = true;
				cout << "Infrared applied successfully!\n";
				break;
			case 5:
				cout << "\nApplying AddFrame Filter...\n";
				addFrame(currentImage);
				isModified = true;
				cout << "AddFrame applied successfully!\n";
				break;
			case 6:
				cout << "\nApplying GrayScale Filter...\n";
				grayScale(currentImage);
				isModified = true;
				cout << "GrayScale applied successfully!\n";
				break;
			case 7:
				cout << "\nApplying BlackAndWhite Filter...\n";
				blackAndWhite(currentImage);
				isModified = true;
				cout << "BlackAndWhite applied successfully!\n";
				break;
			case 8:
				cout << "\nApplying Rotate Filter...\n";
				rotate(currentImage);
				isModified = true;
				cout << "Rotate applied successfully!\n";
				break;
			case 9:
				cout << "\nApplying Flip Filter...\n";
				flip(currentImage);
				isModified = true;
				cout << "Flip applied successfully!\n";
				break;
			case 10:
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
    	cout << "\n---                          Welcome to Image Processing System                          ---\n\n";
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
