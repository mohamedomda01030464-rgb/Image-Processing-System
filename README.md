=============================================================================
 LUMINA STUDIO  -  Image Filter Application (C++ / Qt Widgets)
=============================================================================

1. ABOUT THE PROJECT
-----------------------------------------------------------------------------
Lumina Studio is a desktop application that loads an image, applies one or
more filters to it, shows the ORIGINAL (Before) and the RESULT (After) side
by side, and saves the result to a file.

The application is written in C++ and its graphical interface is built with
the Qt Widgets library. The image filters themselves are plain C++ functions
that work directly on the pixels of the image (they do not depend on Qt).

The project was originally a console program (Image Processing System). It was
then converted to a Qt GUI application with the same filters, plus extra
features (Before/After preview, Delete Filter, Multi Filters mode, dialogs).


2. TEAM MEMBERS AND WORK BREAKDOWN
-----------------------------------------------------------------------------
Course : CS213 Object Oriented Programming, FCAI, Cairo University
Doctor : Dr. Mohammad El-Ramly
Assignment 1 - Part 2 (Beast Level: 18 filters + GUI)

| Name | ID | Section | Filters (original menu numbers) |
| :--- | :---: | :---: | :--- |
| **Mohamed Adel** | 20250572 | S19 , S20 | 4 (Frame), 8 (Resize), 12 (Blur), 16 (Infrared) |
| **Ziad Waleed** | 20250265 | S19 , S20 | 1 (GrayScale), 5 (Flip), 9 (Merge), 13 (Land Of Wano) |
| **Amr Maher** | 20250455 | S19 , S20 | 2 (Black And White), 6 (Rotate), 10 (Detect Edges), 14 (Cinema / Old TV) |
| **Mohamed Hassaan** | 20250555 | S19 , S20 | 3 (Invert), 7 (Darken/Lighten), 11 (Crop), 15 (Wano Purple) |

Extra filters (Beast Level):
- 17 Oil Painting : Amr Maher & Mohamed Hassaan
- 18 Skew Image   : Mohamed Adel & Ziad Waleed & Amr Maher
GUI (Qt Widgets) : made by all team members.
Menu : made by all team members.


3. FEATURES
-----------------------------------------------------------------------------
- Load an image (PNG, JPG, JPEG, BMP, TGA).
- 18 filters, available from a drop-down list ("Choose Filter"). The list
  shows 21 entries because Flip, Add Frame and Darken/Lighten are split
  into two entries each (Horizontal/Vertical, Simple/Fancy, Darken/Lighten).
- Before / After preview that resizes together with the window.
- Filters that need extra values ask the user through small dialog windows.
- Multi Filters mode: apply several filters one after another.
- Delete Filter: go back to the original image.
- Clear_Img: remove the current image and load another one.
- Save the result (PNG, JPG, BMP, TGA) with a "Save As" dialog.
- Warning before closing the program if there are unsaved changes.


4. THE FILTERS
-----------------------------------------------------------------------------
| Name | What it does | Asks the user for |
| :--- | :--- | :--- |
| **Invert** | Reverses every color (255 - value) | - |
| **Darken** | Halves the brightness | - |
| **Lighten** | Multiplies the brightness by 1.5 | - |
| **Blur** | 3x3 average blur, repeated N times | number of passes |
| **Infrared** | Infrared-like red/pink look | - |
| **GrayScale** | Average of R, G, B (gray image) | - |
| **Black And White** | Pure black/white using threshold 128 | - |
| **Flip Horizontal** | Mirrors the image left <-> right | - |
| **Flip Vertical** | Mirrors the image top <-> bottom | - |
| **Rotate** | Rotates the image clockwise | angle (90/180/270) |
| **Resize** | Changes width and height (nearest neighbor) | new width, height |
| **Crop** | Cuts a rectangle from the image | x, y, width, height |
| **Wano Purple** | Purple color tone | - |
| **Oil Painting** | Oil-painting look (dominant intensity level in each neighborhood) | radius, levels |
| **Skew** | Horizontal shear (leans the image) | angle (-60 to 60) |
| **Detect Edges** | Sobel edge detection (black edges on white background, threshold 90) | - |
| **Cinema (Old TV)** | Old TV look: row jitter, RGB shift, contrast/saturation, scanlines, glitch bar, noise | - |
| **Simple Frame** | Solid color frame around the image | thickness, color |
| **Fancy Frame** | Blue frame with a white inner stripe | outer size, stripe |
| **Land Of Wano** | Warm color tone | - |
| **Merge Images** | Blends the current image with a second image (average of both). If the sizes differ: resize both, merge common area, or place the second image at (x, y) | second image file, and options when sizes differ |


5. REQUIREMENTS
-----------------------------------------------------------------------------
- Qt 6 with the Widgets module (developed with Qt 6.11.2, MinGW 64-bit kit).
- Qt Creator (recommended) and CMake.
- A C++17 compiler (std::clamp and std::make_unique are used).
- The following header files must be in the project folder:
      Image_Class.h
      stb_image.h
      stb_image_write.h


6. PROJECT FILES
-----------------------------------------------------------------------------
main.cpp         Entry point: creates the QApplication and shows the window.
widget.h         Declaration of the Widget class (slots, members).
widget.cpp       All the filters + the application logic (load, save,
                 preview, buttons, filter list, dialogs, styling).
widget.ui        The window design (buttons and image frames) from Qt Designer.
CMakeLists.txt   Build configuration.
Image_Class.h    Image class used to read/write images and access pixels.
stb_image.h      Image loading library (used by Image_Class.h).
stb_image_write.h Image saving library (used by Image_Class.h).

Generated automatically (do not edit, do not submit):
ui_widget.h, the "build" folder, CMakeLists.txt.user


7. HOW TO BUILD AND RUN
-----------------------------------------------------------------------------
1. Copy the whole project folder to a path WITHOUT spaces and WITHOUT Arabic
   letters (example: C:\project\filter).
2. Open Qt Creator.
3. File > Open File or Project... and choose CMakeLists.txt.
4. Select the kit "Desktop Qt 6.x.x MinGW 64-bit" and press "Configure Project".
5. Build > Rebuild All.
6. Press Run (Ctrl+R).

To run the .exe outside Qt Creator on another computer, open the "Qt 6.x.x
(MinGW 64-bit)" command prompt, go to the build folder and run:
     windeployqt <name-of-the-exe>.exe
This copies the required Qt DLL files next to the executable.


8. HOW TO USE
-----------------------------------------------------------------------------
1. Press "Load" and choose an image. It appears in the "Before" frame.
2. Choose a filter from the drop-down list "Choose Filter".
   - If the filter needs values, a small dialog opens. Fill it and press OK
     (Cancel leaves the image unchanged).
3. The result appears in the "After" frame.
4. Multi Filters button:
   - OFF (default): every filter is applied on the ORIGINAL image.
   - ON: every filter is applied on top of the previous result.
5. "Delete Filter" returns the image to the original.
6. "Save" opens a Save As dialog. Choose a name and a type (PNG, JPG, BMP, TGA).
7. "Clear_Img" removes the image so you can load a new one.
8. When closing the program with unsaved changes, it asks if you want to save.


9. HOW IT WORKS (SHORT)
-----------------------------------------------------------------------------
- The image is stored as an array of pixels, 3 bytes (R, G, B) per pixel.
  Pixel (x, y) starts at index (y * width + x) * 3.
- Each filter is a function that receives the image by reference and changes
  it. Filters never use cin/cout; values come as parameters.
- All filters are listed in one table (filterList). Each entry has a name and
  a function. The drop-down list is filled from this table, so adding a new
  filter only needs: (1) a new function, (2) a new entry in filterList.
- A filter is always applied on a COPY of the image. If the user cancels a
  dialog, the current image is not changed.
- Buttons are connected to functions (slots) with Qt signals and slots.
- The Before/After frames display QPixmap objects converted from the image.


10. NOTES AND LIMITATIONS
-----------------------------------------------------------------------------
- Use image file names with lowercase extensions (photo.jpg, not photo.JPG).
- Avoid Arabic letters in folder or file names (the image library may fail to
  open them on Windows).
- Large images with Oil Painting or Blur can take a few seconds. Build in
  Release mode for better speed.
- Cinema (Old TV) uses rand() without srand(), so its random effect is the same
  every time the program starts.
- A console (non-Qt) version of the program with the same filters is also
  available (single .cpp file, runs on Visual Studio with Image_Class.h only).


11. TOOLS AND VERSIONS USED
-----------------------------------------------------------------------------
- Operating system : Windows (version: ______)
- IDE              : Qt Creator (version: ______)
- Qt version       : Qt 6.11.2 (Widgets module)
- Kit / compiler   : Desktop Qt 6.11.2 MinGW 64-bit (g++ version: ______)
- Build system     : CMake (version: ______) with Ninja
- C++ standard     : C++17
- Libraries        : Image_Class.h, stb_image.h, stb_image_write.h
                     (given by the course, header-only, no installation)
- Version control  : Git and GitHub (repository: ______)

NOTE: The zip file does not contain Qt, any exe file or the build folder.
Qt must be installed on the computer that builds the project.


12. LINKS
-----------------------------------------------------------------------------
- Video demo     : https://drive.google.com/file/d/1IEW_Q1nd4QhYRIfc3sNJkpmH0b5FxbfG/view?usp=drive_link
- Shared document: https://drive.google.com/file/d/1N144Y_a0XhXApLEpGXPOR6dQljDKTvyO/view?usp=drive_link
- GitHub repo    : https://github.com/mohamedomda01030464-rgb/Image-Processing-System

=============================================================================
