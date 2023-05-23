#include "Box.h"

Box::Box(Scene*& scene, unsigned int width, unsigned int height)
    : Scene(scene, width, height), m_shadowFBO(4096, 4096)
{
    updateWidthHeight(width, height);
    
    m_camera = Camera({ 0.0f, 0.0f, 5.0f }, { 0.0f, 0.0f, 0.0f });
    EventManager::getInstance().addHandler(&m_camera);
    // enable depth testing
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    m_lights.push_back(std::move(std::make_unique<PointLight>(0, glm::vec3(1.5f, 2.0f, 1.0f))));
    m_lights.push_back(std::move(std::make_unique<DirectionalLight>(1, glm::vec3(0.0f, 2.0f, 0.0f))));
    m_lights.push_back(std::move(std::make_unique<Spotlight>(2, glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f))));

    // load shaders
    m_shaders.resize(4);
    m_shaders[0].load("base_shader.vert", "phong.frag");
    m_shaders[1].load("base_shader.vert", "blinn.frag");
    m_shaders[2].load("base_shader.vert", "cook-torrance.frag");
    m_shaders[3].load("base_shader.vert", "toon.frag");
    m_postprocessShader.load("postprocess.vert", "postprocess.frag");
    m_toonPostProcessShader.load("postprocess.vert", "toon_postprocess.frag");
    m_shadowShader.load("shadowmap.vert", "shadowmap.frag");
    m_textureDisplayShader.load("postprocess.vert", "texture_display.frag");

    m_postProcessUI.addShaders({ &m_shaders[0], &m_shaders[1], &m_shaders[2], &m_shaders[3], &m_postprocessShader, &m_toonPostProcessShader });
    m_postProcessUI.setUniforms();
    // setting uniforms
    // TODO: get screen size from config class?

    for (auto& shader : m_shaders) {
        shader.bind();
        shader.setInt("u_numLights", 3);
        for (auto& light : m_lights) {
            light->setUniforms(shader);
        }
    }

    // setup meshes
    std::vector<glm::mat4> wall_transforms{
        glm::translate(glm::vec3(-2.0f, 0.0f, 0.0f)) * glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
        glm::translate(glm::vec3(0.0f, 0.0f, -2.0f)),
        glm::translate(glm::vec3(2.0f, 0.0f, 0.0f)) * glm::rotate(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
        glm::translate(glm::vec3(0.0f, 2.0f, 0.0f)) * glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        glm::translate(glm::vec3(0.0f, -2.0f, 0.0f)) * glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
    };
    for (size_t i = 0; i < wall_transforms.size(); ++i) {
        MaterialMesh m;
        m.materials.push_back(std::move(std::make_unique<PhongMaterial>()));
        m.materials.push_back(std::move(std::make_unique<BlinnMaterial>()));
        m.materials.push_back(std::move(std::make_unique<CookTorranceMaterial>()));
        m.materials.push_back(std::move(std::make_unique<ToonMaterial>()));
        for (auto& material : m.materials) {
            material->disableHighlights();
            switch (i)
            {
            case 0: // left wall - red
                material->setColor({ 0.92f, 0.25f, 0.20f });
                material->setAmbient({ 0.92f, 0.25f, 0.20f });
                break;
            case 2: // right wall - blue
                material->setColor({ 0.2, 0.51, 0.92 });
                material->setAmbient({ 0.2, 0.51, 0.92 });
                break;
            default:
                material->setColor({ 0.8f, 0.8f, 0.8f });
                material->setAmbient({ 0.8f, 0.8f, 0.8f });
            }
        }
        m.modelMatrix = wall_transforms[i];
        m.mesh = std::unique_ptr<Mesh>(Mesh::getPlane(4.0f, 4.0f));
        m_wallMeshes.push_back(std::move(m));
    }

    MaterialMesh sphere;
    sphere.modelMatrix = glm::translate(glm::vec3(0.75f, -1.25f, 0.75f));
    sphere.mesh = std::unique_ptr<Mesh>(Mesh::getSphere(0.75f, 5));
    sphere.materials.push_back(std::move(std::make_unique<PhongMaterial>()));
    sphere.materials.push_back(std::move(std::make_unique<BlinnMaterial>()));
    sphere.materials.push_back(std::move(std::make_unique<CookTorranceMaterial>()));
    sphere.materials.push_back(std::move(std::make_unique<ToonMaterial>()));
    for (auto& material : sphere.materials) {
        material->setColor({ 1.0f, 0.0f, 0.0f });
        material->setAmbient({ 1.0f, 0.0f, 0.0f });
    }
    sphere.name = "Sphere";
    m_meshes.push_back(std::move(sphere));

    MaterialMesh cone;
    cone.modelMatrix = glm::translate(glm::vec3(-0.75f, -0.5f, -0.75f));
    cone.mesh = std::unique_ptr<Mesh>(Mesh::getCone(0.75, 2.0f, 60));
    cone.materials.push_back(std::move(std::make_unique<PhongMaterial>()));
    cone.materials.push_back(std::move(std::make_unique<BlinnMaterial>()));
    cone.materials.push_back(std::move(std::make_unique<CookTorranceMaterial>()));
    cone.materials.push_back(std::move(std::make_unique<ToonMaterial>()));
    for (auto& material : cone.materials) {
        material->setColor({ 0.0f, 1.0f, 0.0f });
        material->setAmbient({ 0.0f, 1.0f, 0.0f });
    }
    cone.name = "Cone";
    m_meshes.push_back(std::move(cone));

    MaterialMesh cube;
    cube.modelMatrix = glm::translate(glm::vec3(-0.75f, -1.25f, -0.75f)) * glm::rotate(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cube.mesh = std::unique_ptr<Mesh>(Mesh::getCube(1.5f, 1.5f, 1.5f));
    cube.materials.push_back(std::move(std::make_unique<PhongMaterial>()));
    cube.materials.push_back(std::move(std::make_unique<BlinnMaterial>()));
    cube.materials.push_back(std::move(std::make_unique<CookTorranceMaterial>()));
    cube.materials.push_back(std::move(std::make_unique<ToonMaterial>()));
    for (auto& material : cube.materials) {
        material->setColor({ 1.0f, 1.0f, 0.0f });
        material->setAmbient({ 1.0f, 1.0f, 0.0f });
    }
    cube.name = "Cube";
    m_meshes.push_back(std::move(cube));

    // set up HDR framebuffer
    

    // set up shadow related variables
    m_lights[1]->setShadow(true);
    m_lights[2]->setShadow(true);
    m_lights[0]->setShadow(true);
    
    // for every light that casts shadow create a depth buffer
    for (size_t i = 0; i < m_lights.size(); ++i) {
        // create cube map for point light
        if (m_lights[i]->getType() == Light::Type::POINT) {
            unsigned int id = m_shadowFBO.addDepthAttachment(GL_TEXTURE_CUBE_MAP);
            // set the depth maps starting from slot 8 
            glActiveTexture(GL_TEXTURE8 + i);
            glBindTexture(GL_TEXTURE_CUBE_MAP, id);
        }
        else {
            unsigned int id = m_shadowFBO.addDepthAttachment(GL_TEXTURE_2D);
            // set the depth maps starting from slot 8 
            glActiveTexture(GL_TEXTURE8 + i);
            glBindTexture(GL_TEXTURE_2D, id);
        }
        m_lights[i]->setShadowTextureSlot(8 + i);
    }
    
    m_shadowFBO.create(); // create the shadow framebuffer

    m_lights[1]->setViewProjectionParameters(Light::ViewProjectionParameters().directional(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 12.0f, 2.8f));
}

Box::~Box()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    EventManager::getInstance().removeHandler(&m_camera);
}

void Box::onRender()
{
    static double time = glfwGetTime();
    // update camera position and uniforms
    m_camera.update(glfwGetTime() - time);
    time = glfwGetTime();
    m_shaders[m_modelIndex].bind();
    m_shaders[m_modelIndex].setMat4("u_viewMatrix", m_camera.getMatrix());
    m_shaders[m_modelIndex].setVec3("u_viewPos", m_camera.getPosition());

    /******************
    * SHADOW PASS
    ******************/

    // setup viewport and framebuffer
    glViewport(0, 0, 4096, 4096);
    m_shadowFBO.bind();
    // enable depth testing and face culling
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    m_shadowShader.bind();

    auto renderSceneShadowPass = [this]() {
        // draw mesh
        glCullFace(GL_BACK);
        for (const auto& mesh : m_meshes) {
            m_shadowShader.setMat4("u_modelMatrix", mesh.modelMatrix);
            mesh.mesh->draw(m_shadowShader);
        }

        glCullFace(GL_BACK);
        // draw box
        for (const auto& wall : m_wallMeshes) {
            m_shadowShader.setMat4("u_modelMatrix", wall.modelMatrix);
            wall.mesh->draw(m_shadowShader);
        }
    };

    for (size_t i = 0, shadowTextureIndex = 0; i < m_lights.size(); ++i) {
        // if light has no shadows then pass or
        // if light doesnt need shadow update, then pass but increment shadowTextureIndex
        if (!m_lights[i]->getShadow() || !m_lights[i]->getShadowNeedsRender()){
            shadowTextureIndex++;
            continue;
        }

        // set far plane and light position for this light (used to write distance from light to fragment in texture)
        m_shadowShader.setVec3("u_lightPos", m_lights[i]->getPosition());
        m_shadowShader.setFloat("u_farPlane", m_lights[i]->getFarPlane());


        // if it is point light render scene for each face
        if (m_lights[i]->getType() == Light::Type::POINT) { 
            for (int faceIndex = 0; faceIndex < 6; ++faceIndex) {
                m_shadowShader.setMat4("u_lightSpaceMatrix", m_lights[i]->getLightSpaceMatrix()[faceIndex]);        
                m_shadowFBO.activateDepthAttachment(shadowTextureIndex, GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex);
                glClear(GL_DEPTH_BUFFER_BIT);
                renderSceneShadowPass();
            }
        }
        else {
            m_shadowShader.setMat4("u_lightSpaceMatrix", m_lights[i]->getLightSpaceMatrix()[0]);
            // activate the depth attachment for this light
            m_shadowFBO.activateDepthAttachment(shadowTextureIndex);
            glClear(GL_DEPTH_BUFFER_BIT);
            renderSceneShadowPass();
        }

        m_lights[i]->resetShadowNeedsRender();
        shadowTextureIndex++;
    }

    /******************
    * LIGHTING PASS
    ******************/
    glViewport(0, 0, m_width, m_height);
    m_hdrFBO->bind();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glm::vec3 clearColor = m_postProcessUI.getGammaCorrection() ? glm::pow(glm::vec3(0.1f), glm::vec3(2.2f)) : glm::vec3(0.1f);
    glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_shaders[m_modelIndex].bind();
    
    m_shaders[m_modelIndex].setMat4("u_projMatrix", m_projMatrix);
    if (m_wireframeEnabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    // lights
    for (size_t i = 0; i < m_lights.size(); ++i) {
        m_lights[i]->setUniforms(m_shaders[m_modelIndex]);
        m_lights[i]->draw(m_shaders[m_modelIndex]);
    }

    // draw box
    for (const auto& wall : m_wallMeshes) {
        wall.materials[m_modelIndex]->setUniforms(m_shaders[m_modelIndex]);
        m_shaders[m_modelIndex].setMat4("u_modelMatrix", wall.modelMatrix);
        wall.mesh->draw(m_shaders[m_modelIndex]);
    }

    // draw meshes
    for (const auto& mesh : m_meshes) {
        mesh.materials[m_modelIndex]->setUniforms(m_shaders[m_modelIndex]);
        m_shaders[m_modelIndex].setMat4("u_modelMatrix", mesh.modelMatrix);
        mesh.mesh->draw(m_shaders[m_modelIndex]);
    }

    if (m_wireframeEnabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // apply postprocessing and write to another FBO
    m_outputFBO->bind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    if (m_modelIndex != 3) {
        m_screenQuadRenderer.render(m_hdrFBO->getColorAttachment(0), m_postprocessShader);
    }
    else {
        m_screenQuadRenderer.renderToon(m_hdrFBO->getColorAttachment(0), m_hdrFBO->getColorAttachment(1), m_toonPostProcessShader);
    }

    // bind default framebuffer
    m_outputFBO->unbind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    m_screenQuadRenderer.render(m_outputFBO->getColorAttachment(0), m_textureDisplayShader);
}

void Box::onRenderImGui()
{
    // back button
    if (ImGui::Button("Back")) {
        Scene* temp = m_currentScene;
        m_currentScene = new SceneMenu(m_currentScene, m_width, m_height);
        delete temp;
        return;
    }

    // render UI for every light
    for (size_t i = 0; i < m_lights.size();++i) {
        ImGui::PushID(m_lights[i].get());
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Set header color
        if (ImGui::CollapsingHeader(m_lights[i]->m_name.c_str())) {
            // render combox for light type
            int type = (int)m_lights[i]->getType();
            bool hadShadow = m_lights[i]->getShadow();
            // recreate depth buffers for shadowmapping
            unsigned int textureType = GL_TEXTURE_2D;
            if (ImGui::Combo("Type", &type, "Point\0Directional\0Spotlight\0\0")) {
                switch (type)
                {
                case 0:
                    m_lights[i] = std::move(std::make_unique<PointLight>(i, m_lights[i]->getPosition()));
                    textureType = GL_TEXTURE_CUBE_MAP;
                    break;
                case 1:
                    m_lights[i] = std::move(std::make_unique<DirectionalLight>(i, m_lights[i]->getPosition()));
                    m_lights[i]->setViewProjectionParameters(Light::ViewProjectionParameters().directional(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 12.0f, 2.8f));
                    break;
                case 2:
                    m_lights[i] = std::move(std::make_unique<Spotlight>(i, m_lights[i]->getPosition(), glm::vec3(0.0f)));
                    break;
                }
                m_lights[i]->setShadow(hadShadow);
                // recreate depth buffers for shadowmapping
                unsigned int id = m_shadowFBO.addDepthAttachmentAtSlot(i, textureType);
                // set the depth maps starting from slot 8 
                glActiveTexture(GL_TEXTURE8 + i);
                glBindTexture(textureType, id);
                m_lights[i]->setShadowTextureSlot(8 + i);
            }
            // render the rest of UI
            m_lights[i]->imGuiRender();
        }
        ImGui::PopStyleColor();
        ImGui::PopID();
        ImGui::NewLine();
    }

    if (ImGui::Combo("Lighting model", &m_modelIndex, "Phong\0Blinn-Phong\0Cook-Torrance\0Toon\0\0")) {
        m_shaders[m_modelIndex].bind();
    }
    ImGui::NewLine();

    // render UI for materials
    for (auto& mesh : m_meshes) {
        ImGui::PushID(mesh.materials[m_modelIndex].get());
        if (ImGui::CollapsingHeader(mesh.name.c_str())) {
            mesh.materials[m_modelIndex]->imGuiRender();
        }
        ImGui::PopID();
    }

    // button to reset camera position and orientation
    if (ImGui::Button("Reset camera")) {
        m_camera.setPosition({ 0.0f, 0.0f, 5.0f });
        m_camera.setTarget({ 0.0f, 0.0f, 0.0f });
    }

    if (ImGui::Button("Screenshot")) {
        m_outputFBO->saveColorAttachmentToPNG(0);
    }

    m_postProcessUI.onRenderImGui();

    // enable/disable wireframes, for debug
    ImGui::Checkbox("Show wireframe", &m_wireframeEnabled);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

void Box::updateWidthHeight(unsigned width, unsigned height)
{
    m_width = width;
    m_height = height;
    m_hdrFBO = std::make_unique<Framebuffer>(width, height);
    m_hdrFBO->addColorAttachament(GL_TEXTURE_2D, GL_RGBA16F);
    m_hdrFBO->addColorAttachament(GL_TEXTURE_2D, GL_RGBA16F);
    m_hdrFBO->addDepthAttachment(GL_RENDERBUFFER);
    m_hdrFBO->create();
    // setup output FBO (after postprocessing)
    m_outputFBO = std::make_unique<Framebuffer>(width, height);
    m_outputFBO->addColorAttachament(GL_TEXTURE_2D, GL_RGB);
    m_outputFBO->create();

    m_projMatrix = glm::infinitePerspective(glm::radians(60.0f), 1.0f * m_width / m_height, 0.1f);
    // set new width/height in toon post process shader
    m_toonPostProcessShader.setFloat("u_textureWidth", m_width);
    m_toonPostProcessShader.setFloat("u_textureHeight", m_height);
}
