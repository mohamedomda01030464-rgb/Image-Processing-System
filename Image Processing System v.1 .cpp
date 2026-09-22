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
		cout << "\nThe current image has unsaved changes. Do you want to save it first? (y/n)";
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

void darken(Image& currentImage) {

    for (int i = 0; i < currentImage.width; i++) {
        for (int j = 0; j < currentImage.height; j++) {
            for (int k = 0; k < currentImage.channels; k++) {

                currentImage(i, j, k) = currentImage(i, j, k)*0.5;

            }
        }
    }
    cout << "Done (Darken Filter)\n";
}

void lighten(Image& currentImage) {
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
	cout << "Done (Samurais Filter Applied!)\n";
}

void addFrame(Image& currentImage){
	int choice ;
	cout << "\n--- Frame Filter Menu ---\n";
	cout << "1- Simple Frame\n";
	cout << "2- Fancy Frame\n";
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
	}
	cout << "\nDone (Frame Added Successfully!)\n";
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
}

void handleApplyFilter(Image& currentImage, bool& isModified){
	int filterChoice = 0;
	while(filterChoice != 8){
		cout << "\n============================================================================================\n\n";
		cout << "---                                     Filters Menu                                     ---\n\n";
		cout << "1- Invert Filter                         -                        ";
		cout << "2- Darken Filter\n\n";
		cout << "3- Lighten Filter                        -                        ";
		cout << "4- Blur Filter\n\n";
		cout << "5- Infrared Filter                       -                        ";
		cout << "6- AddFrame Filter\n\n";
		cout << "7- GrayScale Filter                      -                        ";
		cout << "8- Back to Main Menu\n\n";
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
				cout << "\nApplying Darken Filter...\n";
				darken(currentImage);
				isModified = true;
				cout << "Darken applied successfully!\n";
				break;
			case 3:
				cout << "\nApplying Lighten Filter...\n";
				lighten(currentImage);
				isModified = true;
				cout << "Lighten applied successfully!\n";
				break;
			case 4:
				cout << "\nApplying Blur Filter...\n";
				blur(currentImage);
				isModified = true;
				cout << "Blur applied successfully!\n";
				break;
			case 5:
				cout << "\nApplying Infrared Filter...\n";
				infrared(currentImage);
				isModified = true;
				cout << "Infrared applied successfully!\n";
				break;
			case 6:
				cout << "\nApplying AddFrame Filter...\n";
				addFrame(currentImage);
				isModified = true;
				cout << "AddFrame applied successfully!\n";
				break;
			case 7:
				cout << "\nApplying GrayScale Filter...\n";
				grayScale(currentImage);
				isModified = true;
				cout << "GrayScale applied successfully!\n";
				break;
			case 8:
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
