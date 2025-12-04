#include "../include/pch.h"
#include "../include/drawing.h"
#include "../include/events.h"

namespace Drawing {
    Spherical camera(3.0f, 1.0f, 0.2f);
    sf::Vector3f scenePosition{ 0.0f, 0.0f, 0.0f };
    bool perspectiveProjection = false;
    bool drawLoadDataInputs = true;
    bool drawUserGuide = true;
    bool drawHistogramInputs = false;
    Histogram3D histogram3D;
    ImFont* bigFont = nullptr;
    std::function<void()> drawingFunction;
    sf::Shader dummySceneShader;

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
                drawHistogramInputs = true;
                drawingFunction = &drawHistogram;
            }
        };
        ImGui::End();
    }

    void drawHistogramInputsWindow() {
        static int gridSquaresNumbers[3]{ 10, 10, 10 };
        static int newBinsNumbers[2]{ 10, 10 };
        int sectioningUpperLimitColumn2 = 0;
        if (histogram3D.drawingReady) {
            sectioningUpperLimitColumn2 = histogram3D.trueBins.front().size();
        }
        ImGui::Begin("Modify histogram");
            if (!histogram3D.drawingReady) {
                ImGui::Text("Unavailable until data is loaded.");
            }
            ImGui::BeginDisabled(!histogram3D.drawingReady);
                ImGui::PushItemWidth(150);
                if (ImGui::InputInt3("Number of grid's squares for \nx/y variable and height", gridSquaresNumbers)) {
                    Events::updateGridSquaresNumbers(gridSquaresNumbers);
                }
                ImGui::PushItemWidth(250);
                ImGui::InputText("x label", histogram3D.gridLabels[0], 128);
                ImGui::InputText("y label", histogram3D.gridLabels[1], 128);
                ImGui::Checkbox("Show grid", &histogram3D.isGridWanted);
            ImGui::EndDisabled();
            ImGui::Separator();
            if (!histogram3D.drawingReady) {
                ImGui::Text("Unavailable until data is loaded.");
            }
            else if (!histogram3D.sectioningReady) {
                ImGui::Text("Unavailable. Data is still being sorted.");
            }
            ImGui::BeginDisabled(!histogram3D.drawingReady);
                ImGui::InputInt2("New bins number for\nx/y variable", newBinsNumbers);
                if (ImGui::Button("Rebin histogram")) {
                    Events::rebinHistogram(newBinsNumbers);
                }
                ImGui::Separator();
                ImGui::SliderInt2("Histogram sectioning\n(x variable)", histogram3D.drawingLimitsColumn1, 0, histogram3D.trueBins.size());
                ImGui::SliderInt2("Histogram sectioning\n(y variable)", histogram3D.drawingLimitsColumn2, 0, sectioningUpperLimitColumn2);
            ImGui::EndDisabled();
        ImGui::End();
    }

    void drawUserGuideWindow() {
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

    void drawDummyScene(sf::RenderWindow& window, sf::Time& time) {
        GLfloat projectionMatrix[16], modelViewMatrix[16], MVP_Matrix[16];
        glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
        glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                float cell = 0.f;
                for (int k = 0; k < 4; ++k) {
                    cell += projectionMatrix[k * 4 + col] * modelViewMatrix[row * 4 + k];
                }
                MVP_Matrix[row * 4 + col] = cell;
            }
        }
        static std::vector<sf::Vertex> vertices = []() { 
            int constexpr xStepsNumber = 100;
            int constexpr zStepsNumber = 100;
            float constexpr xStep = (Histogram3D::xHigh - Histogram3D::xLow) / xStepsNumber;
            float constexpr zStep = (Histogram3D::zHigh - Histogram3D::zLow) / zStepsNumber;
            std::vector<sf::Vertex> vertices;
            for (int i = 0; i <= xStepsNumber; ++i)
                vertices.push_back(sf::Vertex{ {Histogram3D::xLow + xStep * i, Histogram3D::zLow + zStep * 0} });
            for (int j = 1; j <= zStepsNumber; ++j)
                vertices.push_back(sf::Vertex{ {Histogram3D::xLow + xStep * xStepsNumber, Histogram3D::zLow + zStep * j} });
            for (int i = xStepsNumber; i >= 0; --i)
                vertices.push_back(sf::Vertex{ {Histogram3D::xLow + xStep * i, Histogram3D::zLow + zStep * zStepsNumber} });
            for (int j = zStepsNumber; j >= 0; --j)
                vertices.push_back(sf::Vertex{ {Histogram3D::xLow + xStep * 0, Histogram3D::zLow + zStep * j} });
            vertices.push_back(sf::Vertex{ {Histogram3D::xLow + xStep * 0, Histogram3D::zLow + zStep * 0} });
            return vertices;
        }();
        window.pushGLStates();
        dummySceneShader.setUniform("time", time.asSeconds());
        dummySceneShader.setUniform("MVP", sf::Glsl::Mat4(MVP_Matrix));
        window.draw(vertices.data(), vertices.size(), sf::LineStrip, &dummySceneShader);
        window.popGLStates();
    }

    void drawHistogram() {
        for (int i = histogram3D.drawingLimitsColumn1[0]; i < histogram3D.drawingLimitsColumn1[1]; ++i) {
            for (int j = histogram3D.drawingLimitsColumn2[0]; j < histogram3D.drawingLimitsColumn2[1]; ++j) {
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

    void drawBar(sf::Vector3f const& leftBottomNearPoint, sf::Vector3f const& rightTopFarPoint, sf::Vector3f const& color, bool drawTopFace) {
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
        static constexpr float margin = 0.2f;
        glColor3f(0.7f, 0.7f, 0.7f);
        const float xStep = histogram3D.trueBins[1][0].leftBottomNearPoint.x - histogram3D.trueBins[0][0].leftBottomNearPoint.x;
        const float zStep = histogram3D.trueBins[0][1].leftBottomNearPoint.z - histogram3D.trueBins[0][0].leftBottomNearPoint.z;
        bool needToFlipX = camera.theta > std::numbers::pi && camera.theta < 2 * std::numbers::pi;
        bool needToFlipZ = camera.theta > std::numbers::pi / 2 && camera.theta < 3 * std::numbers::pi / 2;
        glBegin(GL_LINES);
        // horizontal grid
        for (float x = Histogram3D::xLow;
            x <= Histogram3D::xHigh + eps;
            x += histogram3D.gridBinsStep[0] * xStep) {
            glVertex3f(x, Histogram3D::minHeight - 0.2f, Histogram3D::zLow - margin);
            glVertex3f(x, Histogram3D::minHeight - 0.2f, Histogram3D::zHigh + margin);
        }
        for (float z = Histogram3D::zLow;
            z <= Histogram3D::zHigh + eps;
            z += histogram3D.gridBinsStep[1] * zStep) {
            glVertex3f(Histogram3D::xLow - margin, Histogram3D::minHeight - 0.2f, z);
            glVertex3f(Histogram3D::xHigh + margin, Histogram3D::minHeight - 0.2f, z);
        }
        glEnd();
        // vertical grid x variable
        if (needToFlipX) {
            glPushMatrix();
            glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        }
        glBegin(GL_LINES);
        for (float x = Histogram3D::xLow;
            x <= Histogram3D::xHigh + eps;
            x += histogram3D.gridBinsStep[0] * xStep) {
            glVertex3f(x, Histogram3D::minHeight - 0.2f, Histogram3D::zLow - margin);
            glVertex3f(x, Histogram3D::maxHeight + 0.1f, Histogram3D::zLow - margin);
        }
        for (float y = Histogram3D::minHeight;
            y <= Histogram3D::maxHeight + eps;
            y += histogram3D.gridHeightStep) {
            glVertex3f(Histogram3D::xLow - margin, y, Histogram3D::zLow - margin);
            glVertex3f(Histogram3D::xHigh + margin, y, Histogram3D::zLow - margin);
        }
        glEnd();
        if (needToFlipX) {
            glPopMatrix();
        }
        // vertical grid z variable
        if (needToFlipZ) {
            glPushMatrix();
            glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        }
        glBegin(GL_LINES);
        for (float z = Histogram3D::zLow;
            z <= Histogram3D::zHigh + eps;
            z += histogram3D.gridBinsStep[1] * zStep) {
            glVertex3f(Histogram3D::xLow - margin, Histogram3D::minHeight - 0.2f, z);
            glVertex3f(Histogram3D::xLow - margin, Histogram3D::maxHeight + 0.1f, z);
        }
        for (float y = Histogram3D::minHeight;
            y <= Histogram3D::maxHeight + eps;
            y += histogram3D.gridHeightStep) {
            glVertex3f(Histogram3D::xLow - margin, y, Histogram3D::zLow - margin);
            glVertex3f(Histogram3D::xLow - margin, y, Histogram3D::zHigh + margin);
        }
        glEnd();
        if (needToFlipZ) {
            glPopMatrix();
        }
    }

    ScreenPoint mapWorldCoordinatesToScreen(sf::Vector3f worldPoint, GLdouble* modelMatrix, GLdouble* projectionMatrix, GLint* viewport) {
        GLdouble windowX, windowY, windowZ;
        int result = gluProject(worldPoint.x, worldPoint.y, worldPoint.z, modelMatrix, projectionMatrix, viewport, &windowX, &windowY, &windowZ);
        if (result == GL_TRUE && windowZ < 1.0) {
            float imguiCoordinatesY = (float)viewport[3] - (float)windowY;
            return { sf::Vector2f(windowX, imguiCoordinatesY), true };
        }
        else {
            return { sf::Vector2f(0.f, 0.f), false };
        }
    }

    void drawGridTicksValues() {
        static constexpr float eps = 0.001f;
        static constexpr float margin = 0.2f;
        static constexpr float ticksMargin = 0.1f;
        static constexpr float minTicksValuesStep = 0.15f;
        const float xStep = histogram3D.trueBins[1][0].leftBottomNearPoint.x - histogram3D.trueBins[0][0].leftBottomNearPoint.x;
        const float zStep = histogram3D.trueBins[0][1].leftBottomNearPoint.z - histogram3D.trueBins[0][0].leftBottomNearPoint.z;
        bool needToFlipX = camera.theta > std::numbers::pi && camera.theta < 2 * std::numbers::pi;
        bool needToFlipZ = camera.theta > std::numbers::pi / 2 && camera.theta < 3 * std::numbers::pi / 2;

        GLdouble modelMatrix[16];
        GLdouble projectionMatrix[16];
        GLint viewport[4];

        glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
        glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
        glGetIntegerv(GL_VIEWPORT, viewport);

        auto drawList = ImGui::GetBackgroundDrawList();
        float const xTicksStep = std::max(histogram3D.gridBinsStep[0] * xStep, minTicksValuesStep);
        float const yTicksStep = std::max(histogram3D.gridHeightStep, minTicksValuesStep / 3 * 2);
        float const zTicksStep = std::max(histogram3D.gridBinsStep[1] * zStep, minTicksValuesStep);
        float const zForXTicks = needToFlipX ? Histogram3D::zLow - margin - ticksMargin : Histogram3D::zHigh + margin + ticksMargin;
        float const xForZTicks = needToFlipZ ? Histogram3D::xLow - margin - ticksMargin : Histogram3D::xHigh + margin + ticksMargin;
        float const xForYTicks = needToFlipX ? Histogram3D::xHigh + margin + ticksMargin : Histogram3D::xLow - margin - ticksMargin;
        float const zForYTicks = needToFlipZ ? Histogram3D::zLow - margin - ticksMargin : Histogram3D::zHigh + margin + ticksMargin;
		float const zForXTicksLabel = needToFlipX ? Histogram3D::zLow - margin - ticksMargin * 2 : Histogram3D::zHigh + margin + ticksMargin * 2;
		float const xForZTicksLabel = needToFlipZ ? Histogram3D::xLow - margin - ticksMargin * 2 : Histogram3D::xHigh + margin + ticksMargin * 2;
		float const xForYTicksLabel = needToFlipX ? Histogram3D::xHigh + margin + ticksMargin * 3 : Histogram3D::xLow - margin - ticksMargin * 3;
		float const zForYTicksLabel = needToFlipZ ? Histogram3D::zLow - margin - ticksMargin * 3 : Histogram3D::zHigh + margin + ticksMargin * 3;
        for (float x = Histogram3D::xLow;
                x <= Histogram3D::xHigh + eps;
                x += xTicksStep) {
            double const value = (x - Histogram3D::xLow) / (Histogram3D::xHigh - Histogram3D::xLow) *
                (histogram3D.trueBins.back().back().maxColumn1 - histogram3D.trueBins.front().front().minColumn1);
            drawGridTickValue(drawList, value, { x, Histogram3D::minHeight - 0.2f, zForXTicks }, modelMatrix, projectionMatrix, viewport);
        }
        for (float z = Histogram3D::zLow;
                z <= Histogram3D::zHigh + eps;
                z += zTicksStep) {
            double const value = (z - Histogram3D::zLow) / (Histogram3D::zHigh - Histogram3D::zLow) *
                (histogram3D.trueBins.back().back().maxColumn2 - histogram3D.trueBins.front().front().minColumn2);
            drawGridTickValue(drawList, value, { xForZTicks, Histogram3D::minHeight - 0.2f, z }, modelMatrix, projectionMatrix, viewport);
        }
        for (float y = Histogram3D::minHeight;
                y <= Histogram3D::maxHeight + eps;
                y += yTicksStep) {
            double const value = (y - Histogram3D::minHeight) / (Histogram3D::maxHeight - Histogram3D::minHeight) * histogram3D.highestValuesCount;
            drawGridTickValue(drawList, value, { xForYTicks, y, zForYTicks }, modelMatrix, projectionMatrix, viewport);
        }
        sf::Vector3f xLabelCenterWorldPoint = { Histogram3D::xLow + (Histogram3D::xHigh - Histogram3D::xLow) / 2.f, Histogram3D::minHeight - 0.3f, zForXTicksLabel};
		sf::Vector3f xLabelCenterTranslatedAlongXWorldPoint = { xLabelCenterWorldPoint.x + 0.1f, xLabelCenterWorldPoint.y, xLabelCenterWorldPoint.z };
		ScreenPoint xLabelCenterScreenPoint = mapWorldCoordinatesToScreen(xLabelCenterWorldPoint, modelMatrix, projectionMatrix, viewport);
		ScreenPoint xLabelCenterTranslatedAlongXScreenPoint = mapWorldCoordinatesToScreen(xLabelCenterTranslatedAlongXWorldPoint, modelMatrix, projectionMatrix, viewport);
        float xLabelAngle = std::atan2f(xLabelCenterTranslatedAlongXScreenPoint.position.y - xLabelCenterScreenPoint.position.y,
            xLabelCenterTranslatedAlongXScreenPoint.position.x - xLabelCenterScreenPoint.position.x);
        if(needToFlipX) {
            xLabelAngle += std::numbers::pi;
		}
        drawGridLabel(drawList, histogram3D.gridLabels[0], xLabelCenterScreenPoint, xLabelAngle);
		sf::Vector3f zLabelCenterWorldPoint = { xForZTicksLabel, Histogram3D::minHeight - 0.3f, Histogram3D::zLow + (Histogram3D::zHigh - Histogram3D::zLow) / 2.f };
		sf::Vector3f zLabelCenterTranslatedAlongZWorldPoint = { zLabelCenterWorldPoint.x, zLabelCenterWorldPoint.y, zLabelCenterWorldPoint.z + 0.1f };
		ScreenPoint zLabelCenterScreenPoint = mapWorldCoordinatesToScreen(zLabelCenterWorldPoint, modelMatrix, projectionMatrix, viewport);
        ScreenPoint zLabelCenterTranslatedAlongZScreenPoint = mapWorldCoordinatesToScreen(zLabelCenterTranslatedAlongZWorldPoint, modelMatrix, projectionMatrix, viewport);
        float zLabelAngle = std::atan2f(zLabelCenterTranslatedAlongZScreenPoint.position.y - zLabelCenterScreenPoint.position.y,
            zLabelCenterTranslatedAlongZScreenPoint.position.x - zLabelCenterScreenPoint.position.x);
        if(!needToFlipZ) {
			zLabelAngle += std::numbers::pi;
            }
        drawGridLabel(drawList, histogram3D.gridLabels[1], zLabelCenterScreenPoint, zLabelAngle);
		sf::Vector3f yLabelCenterWorldPoint = { xForYTicksLabel, Histogram3D::minHeight + (Histogram3D::maxHeight - Histogram3D::minHeight) / 2.f, zForYTicksLabel };
        ScreenPoint yLabelCenterScreenPoint = mapWorldCoordinatesToScreen(yLabelCenterWorldPoint, modelMatrix, projectionMatrix, viewport);
        drawGridLabel(drawList, histogram3D.gridLabels[2], yLabelCenterScreenPoint, 0);
    }

    void drawGridTickValue(ImDrawList* drawList, double value, sf::Vector3f worldPoint, GLdouble* modelMatrix, GLdouble* projectionMatrix, GLint* viewport) {
        ScreenPoint screenPoint = mapWorldCoordinatesToScreen(worldPoint, modelMatrix, projectionMatrix, viewport);
        if (screenPoint.isInFrontOfCamera) {
            std::string label = std::format("{:.2}", value);
            ImVec2 textSize = ImGui::CalcTextSize(label.c_str());
            ImVec2 textPosition = ImVec2(screenPoint.position.x - textSize.x * 0.5f, screenPoint.position.y);
            drawList->AddText(textPosition, IM_COL32(0, 0, 0, 255), label.c_str());
        }
    }

    void drawGridLabel(ImDrawList* drawList, const char* label, ScreenPoint screenPoint, float angleRadians) {
		float oldFontSize = ImGui::GetFontSize();
        ImGui::PushFont(bigFont);
        float newFontSize = ImGui::GetFontSize();
        ImVec2 textSize = ImGui::CalcTextSize(label);
        ImVec2 textPosition = ImVec2(screenPoint.position.x - textSize.x * 0.5f, screenPoint.position.y - textSize.y * 0.5f);
        int vertexOldLastIndex = drawList->VtxBuffer.Size;
        drawList->AddText(textPosition, IM_COL32(0, 0, 0, 255), label);
        int vertexNewLastIndex = drawList->VtxBuffer.Size;
        float sin = std::sin(angleRadians);
        float cos = std::cos(angleRadians);
        for (int i = vertexOldLastIndex; i < vertexNewLastIndex; ++i) {
            ImDrawVert& vertex = drawList->VtxBuffer[i];
            float xDelta = vertex.pos.x - screenPoint.position.x;
            float yDelta = vertex.pos.y - screenPoint.position.y;
			xDelta *= oldFontSize / newFontSize * 1.25;
			yDelta *= oldFontSize / newFontSize * 1.25;
            vertex.pos.x = (xDelta * cos - yDelta * sin) + screenPoint.position.x;
            vertex.pos.y = (xDelta * sin + yDelta * cos) + screenPoint.position.y;
        }
        ImGui::PopFont();
    }
}