#include "../include/pch.h"
#include "../include/drawing.h"
#include "../include/events.h"

namespace Drawing {
    Spherical camera(3.0f, 1.0f, 0.2f);
    sf::Vector3f scenePosition{ 0.0f, 0.0f, 0.0f };
    bool perspectiveProjection = true;
    bool drawLoadDataInputs = true;
    bool drawUserGuide = true;
    Histogram3D histogram3D;
    void (*drawingFunction)() = &drawDummyScene;

    void initOpenGL() {
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);
        glLineWidth(2.0f);
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
        glTranslatef(scenePosition.x, scenePosition.y, scenePosition.z);
        drawingFunction();
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
                    drawingFunction = &drawHistogram;
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

    void drawDummyScene() {
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

    void drawHistogram() {
        for (int i = 0; i < histogram3D.trueBins.size(); ++i) {
            for (int j = 0; j < histogram3D.trueBins[i].size(); ++j) {
                if (histogram3D.trueBins[i][j].valuesCount != 0) {
                    drawBar(histogram3D.trueBins[i][j].leftBottomNearPoint,
                        histogram3D.trueBins[i][j].rightTopFarPoint,
                        Histogram3D::red, histogram3D.trueBins[i][j].drawTopFace);
                }
                if (histogram3D.falseBins[i][j].valuesCount != 0) {
                    drawBar(histogram3D.falseBins[i][j].leftBottomNearPoint,
                        histogram3D.falseBins[i][j].rightTopFarPoint,
                        Histogram3D::blue, histogram3D.falseBins[i][j].drawTopFace);
                }
            }
        }
        if (histogram3D.isGridWanted) {
            drawGrid3D();
        }
    }

    void drawBar(sf::Vector3f const &leftBottomNearPoint, sf::Vector3f const& rightTopFarPoint, sf::Vector3f const& color, bool drawTopFace) {
        glColor3f(color.x, color.y, color.z);
        glBegin(GL_TRIANGLE_STRIP);
            glVertex3f(leftBottomNearPoint.x, leftBottomNearPoint.y, leftBottomNearPoint.z);
            glVertex3f(leftBottomNearPoint.x, rightTopFarPoint.y, leftBottomNearPoint.z);
            glVertex3f(rightTopFarPoint.x, leftBottomNearPoint.y, leftBottomNearPoint.z);
            glVertex3f(rightTopFarPoint.x, rightTopFarPoint.y, leftBottomNearPoint.z);
            glVertex3f(rightTopFarPoint.x, leftBottomNearPoint.y, rightTopFarPoint.z);
            glVertex3f(rightTopFarPoint.x, rightTopFarPoint.y, rightTopFarPoint.z);
            glVertex3f(leftBottomNearPoint.x, leftBottomNearPoint.y, rightTopFarPoint.z);
            glVertex3f(leftBottomNearPoint.x, rightTopFarPoint.y, rightTopFarPoint.z);
            glVertex3f(leftBottomNearPoint.x, leftBottomNearPoint.y, leftBottomNearPoint.z);
            glVertex3f(leftBottomNearPoint.x, rightTopFarPoint.y, leftBottomNearPoint.z);
        glEnd();
        glColor3f(0, 0, 0);
        glBegin(GL_LINE_STRIP);
            glVertex3f(leftBottomNearPoint.x, leftBottomNearPoint.y, leftBottomNearPoint.z);
            glVertex3f(leftBottomNearPoint.x, rightTopFarPoint.y, leftBottomNearPoint.z);
            glVertex3f(rightTopFarPoint.x, rightTopFarPoint.y, leftBottomNearPoint.z);
            glVertex3f(rightTopFarPoint.x, leftBottomNearPoint.y, leftBottomNearPoint.z);
            glVertex3f(leftBottomNearPoint.x, leftBottomNearPoint.y, leftBottomNearPoint.z);
        glEnd();
        glBegin(GL_LINE_STRIP);
            glVertex3f(rightTopFarPoint.x, rightTopFarPoint.y, leftBottomNearPoint.z);
            glVertex3f(rightTopFarPoint.x, rightTopFarPoint.y, rightTopFarPoint.z);
            glVertex3f(rightTopFarPoint.x, leftBottomNearPoint.y, rightTopFarPoint.z);
            glVertex3f(rightTopFarPoint.x, leftBottomNearPoint.y, leftBottomNearPoint.z);
        glEnd();
        glBegin(GL_LINE_STRIP);
            glVertex3f(rightTopFarPoint.x, rightTopFarPoint.y, rightTopFarPoint.z);
            glVertex3f(leftBottomNearPoint.x, rightTopFarPoint.y, rightTopFarPoint.z);
            glVertex3f(leftBottomNearPoint.x, leftBottomNearPoint.y, rightTopFarPoint.z);
            glVertex3f(rightTopFarPoint.x, leftBottomNearPoint.y, rightTopFarPoint.z);
        glEnd();
        glBegin(GL_LINE_STRIP);
            glVertex3f(leftBottomNearPoint.x, rightTopFarPoint.y, rightTopFarPoint.z);
            glVertex3f(leftBottomNearPoint.x, rightTopFarPoint.y, leftBottomNearPoint.z);
            glVertex3f(leftBottomNearPoint.x, leftBottomNearPoint.y, leftBottomNearPoint.z);
            glVertex3f(leftBottomNearPoint.x, leftBottomNearPoint.y, rightTopFarPoint.z);
        glEnd();
        if (drawTopFace) {
            glColor3f(color.x, color.y, color.z);
            glBegin(GL_TRIANGLE_STRIP);
                glVertex3f(leftBottomNearPoint.x, rightTopFarPoint.y, leftBottomNearPoint.z);
                glVertex3f(rightTopFarPoint.x, rightTopFarPoint.y, leftBottomNearPoint.z);
                glVertex3f(leftBottomNearPoint.x, rightTopFarPoint.y, rightTopFarPoint.z);
                glVertex3f(rightTopFarPoint.x, rightTopFarPoint.y, rightTopFarPoint.z);
            glEnd();
        }
    }

    void drawGrid3D() {
        static constexpr float eps = 0.001f;
		glColor3f(0.7f, 0.7f, 0.7f);
        const float xStep = histogram3D.trueBins[1][0].leftBottomNearPoint.x - histogram3D.trueBins[0][0].leftBottomNearPoint.x;
        const float zStep = histogram3D.trueBins[0][1].leftBottomNearPoint.z - histogram3D.trueBins[0][0].leftBottomNearPoint.z;
		bool NeedToFlipX = camera.theta > std::numbers::pi && camera.theta < 2 * std::numbers::pi;
		bool NeedToFlipZ = camera.theta > std::numbers::pi / 2 && camera.theta < 3 * std::numbers::pi / 2;
		glBegin(GL_LINES);
            // horizontal grid
            for (float x = Histogram3D::xLow - histogram3D.gridBinsStep[0] * xStep;
                    x <= Histogram3D::xHigh + histogram3D.gridBinsStep[0] * xStep + eps;
                    x += histogram3D.gridBinsStep[0] * xStep) {
                glVertex3f(x, Histogram3D::minHeight - 0.2f, Histogram3D::zLow - histogram3D.gridBinsStep[1] * zStep);
                glVertex3f(x, Histogram3D::minHeight - 0.2f, Histogram3D::zHigh + histogram3D.gridBinsStep[1] * zStep);
            }
            for (float z = Histogram3D::zLow - histogram3D.gridBinsStep[1] * zStep;
                    z <= Histogram3D::zHigh + histogram3D.gridBinsStep[1] * zStep + eps;
                    z += histogram3D.gridBinsStep[1] * zStep) {
                glVertex3f(Histogram3D::xLow - histogram3D.gridBinsStep[0] * xStep, Histogram3D::minHeight - 0.2f, z);
                glVertex3f(Histogram3D::xHigh + histogram3D.gridBinsStep[0] * xStep, Histogram3D::minHeight - 0.2f, z);
            }
        glEnd();
        // vertical grid x variable
        if (NeedToFlipX) {
            glPushMatrix();
            glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        }
        glBegin(GL_LINES);
            for (float x = Histogram3D::xLow - histogram3D.gridBinsStep[0] * xStep;
                    x <= Histogram3D::xHigh + histogram3D.gridBinsStep[0] * xStep + eps;
                    x += histogram3D.gridBinsStep[0] * xStep) {
                glVertex3f(x, Histogram3D::minHeight - 0.2f, Histogram3D::zLow - histogram3D.gridBinsStep[1] * zStep);
                glVertex3f(x, Histogram3D::maxHeight + 0.1f, Histogram3D::zLow - histogram3D.gridBinsStep[1] * zStep);
            }
            for (float y = Histogram3D::minHeight - 0.2f;
                    y <= Histogram3D::maxHeight + 0.1f + eps;
                    y += histogram3D.gridHeightStep) {
                glVertex3f(Histogram3D::xLow - histogram3D.gridBinsStep[0] * xStep, y, Histogram3D::zLow - histogram3D.gridBinsStep[1] * zStep);
                glVertex3f(Histogram3D::xHigh + histogram3D.gridBinsStep[0] * xStep, y, Histogram3D::zLow - histogram3D.gridBinsStep[1] * zStep);
            }
        glEnd();
        if (NeedToFlipX) {
            glPopMatrix();
        }
        //vertical grid z variable
        if (NeedToFlipZ) {
            glPushMatrix();
            glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        }
        glBegin(GL_LINES);
            for (float z = Histogram3D::zLow - histogram3D.gridBinsStep[1] * zStep;
                    z <= Histogram3D::zHigh + histogram3D.gridBinsStep[1] * zStep + eps;
                    z += histogram3D.gridBinsStep[1] * zStep) {
                glVertex3f(Histogram3D::xLow - histogram3D.gridBinsStep[0] * xStep, Histogram3D::minHeight - 0.2f, z);
                glVertex3f(Histogram3D::xLow - histogram3D.gridBinsStep[0] * xStep, Histogram3D::maxHeight + 0.1f, z);
            }
            for (float y = Histogram3D::minHeight - 0.2f;
                    y <= Histogram3D::maxHeight + 0.1f + eps;
                    y += histogram3D.gridHeightStep) {
                glVertex3f(Histogram3D::xLow - histogram3D.gridBinsStep[0] * xStep, y, Histogram3D::zLow - histogram3D.gridBinsStep[1] * zStep);
                glVertex3f(Histogram3D::xLow - histogram3D.gridBinsStep[0] * xStep, y, Histogram3D::zHigh + histogram3D.gridBinsStep[1] * zStep);
            }
        glEnd();
        if (NeedToFlipZ) {
            glPopMatrix();
        }
    }
}