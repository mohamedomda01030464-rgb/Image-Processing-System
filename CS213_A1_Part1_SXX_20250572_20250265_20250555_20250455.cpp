#include <iostream>
#include <string>
#include <stdexcept>
#include "Image_Class.h"

using namespace std;

void Grayscale(){
	
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
    cout << "1-Load Image\n";
    cout << "2-Apply Filters\n";
    cout << "3-Save Image\n";
    cout << "4-Exit\n";
    int choose;
    cout << "choose the service number";
    cin >> choose;
    while(choose != 4){
	    switch(choose){
	    	case 1:
	    		if ()
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
	    		break;
	    	case 2:
	    		
	    		break;
	    		
	    	case 3:
	    		
	    		break;
	    	default:
	    		cout << "Error inputs, Please choose valid number";
				break;
		}
	}
	return 0;
}
