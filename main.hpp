#ifndef MAIN_HPP
#define MAIN_HPP
#include <stdio.h>
#include <curl/curl.h>
#include <QApplication>
#include <QPushButton>
#include <QWidget>
#include <QString>
#include <QTimer>
#include <QLabel>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QDebug>
#include <windows.h>
#include <thread>
#include <mutex>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;



/** Struct to store clipboard data and a mutex to avoid data race. */
struct ClipboardData {
    std::string data;
    std::mutex mutex;
};

class CanvasForCoord : public QWidget {
    Q_OBJECT
    private:
        int x;
        int y;
        int widthRec = 20;
        int heightRec = 10;
        QPoint mousePos;

    public:
        /** Constructor for the canvas used to configure the window coordinates. */
        CanvasForCoord(QWidget *parent = nullptr);

    protected:
        /** Handles the paint event for the canvas. 
         * @param event Pointer to the QPaintEvent object containing event data.
        */
        void paintEvent(QPaintEvent *event) override;
        /** Handles the mouse press event for the canvas. */
        void mousePressEvent(QMouseEvent *event) override;
        /** Handles the key press event 'esc' for the canvas. */
        void keyPressEvent(QKeyEvent *event) override;
        /** Configures the coordinates for the elements. 
         * @param x The x coordinate.
         * @param y The y coordinate.
        */
        void configCoords(int x, int y);
        
    signals:
        /** Emitted when the coordinates are changed. 
         * @param x The new x coordinate for the Main Window.
         * @param y The new y coordinate for the Main Window.
        */
        void coordsChanged(int x, int y);

        /** Emitted when the configuration window is closed. */
        void windowClosed();

    };

class MainWindow : public QWidget {
    Q_OBJECT
    private:
        int widthM = 400;
        int heightM = 300;
        QPushButton button;
        QPushButton hideButton;
        CanvasForCoord* canvas;
        QTimer *timer;
        QLabel* infoLabel;
        QLabel* definitionLabel;
        ClipboardData* clipboardData;

    public:
        /** Constructor for the main window. 
         * @param data Pointer to the ClipboardData structure where clipboard data is stored.
         * @param parent Pointer to the parent widget (default is nullptr).
        */
        MainWindow(ClipboardData* data, QWidget *parent = nullptr);
        /** Sets the text for the info label. */
        //change this name!
        void setText(const QString& text);

        /** Sets the clipboard data. */
        void setClipboardData(ClipboardData* data);

        /** Updates the coordinates of the main window. */
        void updateCoords(int x, int y);
        
        /** Pops up the main window. */
        void popWindow();

    protected:
        /** Handles when 'esc' key is pressed in Main Window */
        void keyPressEvent(QKeyEvent *event) override;

    private:
        /** Creates the button for the configuration of the window coordinates. */
        void createConfigButton();
        /** Sets the coordinates for the configuration button of the window. */
        void buttonCoords();
        /** Creates the hide button. */
        void createHideButton();
        /** Sets the coordinates for the hide button. */
        void hideButtonCoords();
        /** Creates the label for displaying the chosen word */
        void createInfoLabel();
        /** Creates the label for displaying the word definition. */
        void createDefinitionLabel();
};

/**
 * Iterates all definitions in the JSON and appends some of them into the definitions string.
 * @param jsonData The JSON data received from the API.
 * @param definitions Pointer to the string where the definitions will be stored.
 */
void getAllDefs(const json& jsonData, std::string* definitions);

/**
 * Parses the API to a json format using the nlohmann library and extracts the definitions of the word.
 * @param output The raw JSON string received from the API. 
 * @return The parsed JSON data.
 */
std::string apiOutput(const std::string& output);

/**
 * Thread to handle the hotkey event and clipboard operations.
 * @param clipboardData Pointer to the ClipboardData structure to store clipboard data.
 * @param mainWindow Pointer to the MainWindow UI.
 */
void hotKeyThread(ClipboardData* clipboardData, MainWindow* mainWindow);

/**
 * Callback function for libcurl to write the response data into a string.
 * @param contents Pointer to the data received from the API.
 * @param size Size of each data element.
 * @param nmemb Number of data elements.
 * @param userp Pointer to the string where the response data will be stored.
 * @return The total size of the data written to the string.
 */
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);

/** 
 * Fetches the definition of a word from the API.
 * @param word The word for which to fetch the definition.
 * @return The definition of the word.
 **/
std::string fetchWordDefinition(const std::string& word);

#endif