#include "../include/pch.h"
#include "../include/drawing.h"
#include "../include/events.h"

namespace Drawing {
    Spherical camera(3.0f, 1.0f, 0.2f);
    bool perspectiveProjection = true;
    bool drawLoadDataInputs = true;
    bool drawUserGuide = true;
    Histogram3D histogram3D;

    void initOpenGL() {
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        glEnable(GL_DEPTH_TEST);
    }

    void reshapeScreen(sf::Vector2u size) {
        glViewport(0, 0, (GLsizei)size.x, (GLsizei)size.y);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        if (perspectiveProjection) {
            gluPerspective(45., (GLdouble)size.x / (GLdouble)size.y, 0.1, 100.0);
        }
        else {
            float magic_number = static_cast<float>(camera.distance / 2.2);
            glOrtho(-magic_number * ((GLdouble)size.x / (GLdouble)size.y), magic_number * ((GLdouble)size.x / (GLdouble)size.y), -magic_number, magic_number, -3., 12.0);
        }
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void drawScene() {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        Spherical northOfCamera(camera.distance, camera.theta, camera.phi + 0.01f);
        gluLookAt(camera.getX(), camera.getY(), camera.getZ(),
            0.0, 0.0, 0.0,
            northOfCamera.getX(), northOfCamera.getY(), northOfCamera.getZ());

        //uklad
        glBegin(GL_LINES);
            glColor3f(1.0, 0.0, 0.0); glVertex3f(0, 0, 0); glVertex3f(1.0, 0, 0);
            glColor3f(0.0, 1.0, 0.0); glVertex3f(0, 0, 0); glVertex3f(0, 1.0, 0);
            glColor3f(0.0, 0.0, 1.0); glVertex3f(0, 0, 0); glVertex3f(0, 0, 1.0);
        glEnd();

        //Linie przerywane
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(2, 0xAAAA);
        glBegin(GL_LINES);
            glColor3f(1.0, 0.0, 0.0); glVertex3f(0, 0, 0); glVertex3f(-1.0, 0, 0);
            glColor3f(0.0, 1.0, 0.0); glVertex3f(0, 0, 0); glVertex3f(0, -1.0, 0);
            glColor3f(0.0, 0.0, 1.0); glVertex3f(0, 0, 0); glVertex3f(0, 0, -1.0);
        glEnd();
        glDisable(GL_LINE_STIPPLE);

        //szescian
        glLineWidth(2.0);
        glColor3f(0, 0, 0);
        glBegin(GL_LINES);
            for (unsigned char i = 0; i < 2; i++) {
                for (unsigned char j = 0; j < 2; j++) {
                    glVertex3f(-0.3f + 0.6f * (i ^ j), -0.3f + 0.6f * j, -0.3f); glVertex3f(-0.3f + 0.6f * (i ^ j), -0.3f + 0.6f * j, 0.3f);
                    glVertex3f(-0.3f, -0.3f + 0.6f * (i ^ j), -0.3f + 0.6f * j); glVertex3f(0.3f, -0.3f + 0.6f * (i ^ j), -0.3f + 0.6f * j);
                    glVertex3f(-0.3f + 0.6f * (i ^ j), -0.3f, -0.3f + 0.6f * j); glVertex3f(-0.3f + 0.6f * (i ^ j), 0.3f, -0.3f + 0.6f * j);
                }
            }
        glEnd();
    }

    void drawFileLoadUserInputs() {
        static char fileDelimiter[2]{};
        static char fileColumn1[128]{};
        static char fileColumn2[128]{};
        static char fileBoolColumn[128]{};
        static bool skipFirstRow = false;
        static bool fileUseColumnNames = true;
        static int binsNumber[2]{};
        ImGui::Begin("Load csv data");
            ImGui::PushItemWidth(20);
            ImGui::InputText("Single character column delimiter", fileDelimiter, 2);
            ImGui::PushItemWidth(150);
            if (fileUseColumnNames) {
                ImGui::InputText("First column name", fileColumn1, 128);
                ImGui::InputText("Second column name", fileColumn2, 128);
                ImGui::InputText("Boolean column name", fileBoolColumn, 128);
                ImGui::Checkbox("Use column names", &fileUseColumnNames);
            }
            else {
                ImGui::InputText("First column index", fileColumn1, 128);
                ImGui::InputText("Second column index", fileColumn2, 128);
                ImGui::InputText("Boolean column index", fileBoolColumn, 128);
                ImGui::Checkbox("Use column names", &fileUseColumnNames);
                ImGui::Checkbox("Skip first row", &skipFirstRow);
            }
            ImGui::InputInt2("Number of histogram\nbins for x/y variable", binsNumber);
            if (ImGui::Button("Select file and load")) {
                bool dataLoaded = Events::loadHistogramData(fileDelimiter, fileColumn1, fileColumn2, fileBoolColumn, skipFirstRow, !fileUseColumnNames, binsNumber);
                if (dataLoaded) {
                    drawLoadDataInputs = false;
                }
            };
        ImGui::End();
    }

    void drawUserGuideBox() {
        static std::ifstream userGuideFile("user_guide.txt");
        static std::string userGuideContent = "Missing user_guide.txt file!";
        if (userGuideFile.is_open()) {
            std::stringstream buffer;
            buffer << userGuideFile.rdbuf();
            userGuideContent = buffer.str();
            userGuideFile.close();
        }
        ImGui::Begin("User Guide");
            ImGui::Text(userGuideContent.c_str());
        if (ImGui::Button("Hide")) {
            Drawing::drawUserGuide = false;
        }
        ImGui::End();
    }
}