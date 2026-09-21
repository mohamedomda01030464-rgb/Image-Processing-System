#include <iostream>
#include <string>
#include <stdexcept>
#include "Image_Class.h"

using namespace std;

void handleSaveImage(Image& currentImage, string& currentPath, bool& isModified){
	int saveChoice;
	cout << "\n--- Save Options ---\n";
	cout << "1- Save : " << currentPath << ")\n";
	cout << "2- Save As\n";
	cout << "Choose Option: ";
	cin >> saveChoice;
	
	string targetPath = currentPath;
	
	if (saveChoice == 2){
		cout << "Enter new image path/filename (e.g., output.png): ";
		cin >> targetPath;
	}else if (saveChoice != 1){
		cout << "Invalid choice! Defaulting to original file path.\n";
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
		cout << "The current image has unsaved changes. Do you want to save it first? (y/n)";
		cin >> ans;
		if (ans == 'y' || ans == 'Y'){
			handleSaveImage(currentImage,currentPath,isModified);
		}
	}
	string newPath = "";
	bool loadedSuccessfully = false;
	while(!loadedSuccessfully){
		cout << "Please enter the new image path ";
		cin >> newPath;
		try{
			if (currentImage.loadNewImage(newPath)){
				loadedSuccessfully = true;
				currentPath = newPath;
				isModified = false;
				isImageLoaded = true;
				cout << "New image loaded successfully!\n\n";
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

void handleApplyFilter(Image& currentImage, bool& isModified){
	int filterChoice = 0;
	while(filterChoice != 4){
		cout << "\n==============================\n";
		cout << "--- Filters Menu ---\n";
		cout << "1- Invert Filter\n";
		cout << "2- Darken Filter\n";
		cout << "3- Lighten Filter\n";
		cout << "4- Back to Main Menu\n";
		cout << "==============================\n";
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
                cout << "Image loaded successfully!\n";
            }
        } 
        catch (const invalid_argument& e) {
            cout << "Error: " << e.what() << ". Please try again.\n";
            isImageLoaded = false;
        }
    }
    int choose = 0;
    while(choose != 4){
    	cout << "===========================\n";
        cout << "1- Load New Image\n";
        cout << "2- Apply Filters\n";
        cout << "3- Save Image\n";
        cout << "4- Exit\n";
        cout << "===========================\n";
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
