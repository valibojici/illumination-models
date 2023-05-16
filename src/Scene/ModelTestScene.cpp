#include "ModelTestScene.h"
static glm::vec3 trans = glm::vec3(1.56f, -1.1f, 0.62f);
ModelTestScene::ModelTestScene(Scene*& scene, unsigned int width, unsigned int height)
    : Scene(scene, width, height), m_shadowFBO(2048, 2048)
{
    updateWidthHeight(width, height);

    m_camera = Camera({ 0.0f, 0.0f, 5.0f }, { 0.0f, 0.0f, 0.0f });
    EventManager::getInstance().addHandler(&m_camera);
    // enable depth testing
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    m_lights.push_back(std::move(std::make_unique<PointLight>(0, glm::vec3(1.56f, -0.76f, 0.47f), glm::vec3(1.0f, 0.95f, 0.87f))));
    m_lights.push_back(std::move(std::make_unique<PointLight>(1, glm::vec3(0.0f, 1.4f, 0.0f))));
    m_lights.push_back(std::move(std::make_unique<PointLight>(2, glm::vec3(0.0f))));
    m_lights[0]->setIntensity(2.0f);
    m_lights[1]->setIntensity(1.5f);
    m_lights[0]->disableDraw();
    m_lights[1]->disableDraw();
    m_lights[2]->disable();
    m_lights[2]->disableDraw();

    // load shaders
    m_shader.load("base_shader.vert", "cook-torrance.frag");
    m_postprocessShader.load("postprocess.vert", "postprocess.frag");
    m_shadowShader.load("shadowmap_v2.vert", "shadowmap_v2.frag");

    m_postProcessUI.addShaders({ &m_shader, &m_postprocessShader });
    m_postProcessUI.setUniforms();
    // setting uniforms
    m_shader.setMat4("u_projMatrix", m_projMatrix);
    m_shader.setInt("u_numLights", m_lights.size());
    for (auto& light : m_lights) {
        light->setUniforms(m_shader);
    }
    
    // setup default uniform values
    m_material.setUniforms(m_shader);

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
        m.textureScaleX = m.textureScaleY = 2.0f;
        m.mesh = std::unique_ptr<Mesh>(Mesh::getPlane(4.0f, 4.0f));
        if (i == 4) {
            m.mesh->setTextures({
                TextureManager::get().getTexture("textures/wood_floor/diffuse.png", Texture::Type::DIFFUSE),
                TextureManager::get().getTexture("textures/wood_floor/normal.png", Texture::Type::NORMAL),
                TextureManager::get().getTexture("textures/wood_floor/roughness.png", Texture::Type::ROUGHNESS),
                TextureManager::get().getTexture("textures/wood_floor/metallic.png", Texture::Type::METALLIC),
                });
        }
        else if (i == 3) {
            m.mesh->setTextures({
                TextureManager::get().getTexture("textures/ceiling/diffuse.png", Texture::Type::DIFFUSE),
                TextureManager::get().getTexture("textures/ceiling/normal.png", Texture::Type::NORMAL),
                TextureManager::get().getTexture("textures/ceiling/roughness.png", Texture::Type::ROUGHNESS),
                TextureManager::get().getTexture("textures/ceiling/metallic.png", Texture::Type::METALLIC),
                });
        }
        else {
            m.mesh->setTextures({
                TextureManager::get().getTexture("textures/art_deco/diffuse.png", Texture::Type::DIFFUSE),
                TextureManager::get().getTexture("textures/art_deco/normal.png", Texture::Type::NORMAL),
                TextureManager::get().getTexture("textures/art_deco/roughness.png", Texture::Type::ROUGHNESS),
                TextureManager::get().getTexture("textures/art_deco/metallic.png", Texture::Type::METALLIC),
                });
        }
        m.modelMatrix = wall_transforms[i];
       
        m_wallMeshes.push_back(std::move(m));
    }

    // set up shadow related variables
    m_lights[1]->setShadow(true);
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

    // set view proj parameters for lights
    m_lights[0]->setViewProjectionParameters(Light::ViewProjectionParameters().point(1.0f, 0.02f, 12.0f));
    m_lights[1]->setViewProjectionParameters(Light::ViewProjectionParameters().point(1.0f, 0.02f, 12.0f));
    m_lights[2]->setViewProjectionParameters(Light::ViewProjectionParameters().point(1.0f, 0.1f, 12.0f));


    // load models
    m_models.resize(6);
    m_models[0].load("models/chair/chair.dae");
    m_models[0].m_modelMatrix = glm::translate(glm::vec3(0.5f, -1.52f, -0.1f)) * glm::scale(glm::vec3(0.5f));
    m_models[1].load("models/desk/desk.dae");
    m_models[1].m_modelMatrix = glm::translate(glm::vec3(1.46f, -1.55f, 0.0f)) * glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::vec3(1.0f));
    m_models[2].load("models/lamp/lamp.obj");
    m_models[2].m_modelMatrix = glm::translate(glm::vec3(1.56f, -1.1f, 0.62f)) * glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::vec3(0.04f));
    m_models[3].load("models/lightbulb/lightbulb.gltf");
    m_models[3].m_modelMatrix = glm::translate(glm::vec3(0.0f, 2.0f, 0.0f)) * glm::scale(glm::vec3(0.01f));
    m_models[4].load("models/bookshelf/bookshelf.fbx");
    m_models[4].m_modelMatrix = glm::translate(glm::vec3(-1.5f, -2.0f, -0.14f)) 
        * glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) 
        * glm::scale(glm::vec3(0.012f));

    m_models[5].load("models/books/books.gltf");
    m_models[5].m_modelMatrix = glm::translate(glm::vec3(1.6f, -1.1f, 0.0f)) * glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::vec3(0.1f));
}

ModelTestScene::~ModelTestScene()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    EventManager::getInstance().removeHandler(&m_camera);
}

void ModelTestScene::onRender()
{
    static double time = glfwGetTime();
    // update camera position and uniforms
    m_camera.update(glfwGetTime() - time);
    time = glfwGetTime();
    
    m_shader.setMat4("u_viewMatrix", m_camera.getMatrix());
    m_shader.setVec3("u_viewPos", m_camera.getPosition());

    /******************
    * SHADOW PASS
    ******************/

    // setup viewport and framebuffer
    glViewport(0, 0, 2048, 2048);
    m_shadowFBO.bind();
    // enable depth testing and face culling
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    m_shadowShader.bind();

    auto renderSceneShadowPass = [this]() {
        glCullFace(GL_BACK);
        // draw box
        for (const auto& wall : m_wallMeshes) {
            m_shadowShader.setMat4("u_modelMatrix", wall.modelMatrix);
            wall.mesh->draw(m_shadowShader);
        }

        // draw models
        for (auto& model : m_models) {
            m_shadowShader.setMat4("u_modelMatrix", model.m_modelMatrix);
            model.draw(m_shadowShader);
        }
    };

    for (size_t i = 0, shadowTextureIndex = 0; i < m_lights.size(); ++i) {
        // if light has no shadows then pass or
        // if light doesnt need shadow update, then pass but increment shadowTextureIndex
        if (!m_lights[i]->getShadow() || !m_lights[i]->getShadowNeedsRender()) {
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
    m_shader.setMat4("u_projMatrix", m_projMatrix);
    m_material.setUniforms(m_shader);

    if (m_wireframeEnabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    // lights
    for (size_t i = 0; i < m_lights.size(); ++i) {
        m_lights[i]->setUniforms(m_shader);
        m_lights[i]->draw(m_shader);
    }

    // draw box
    for (const auto& wall : m_wallMeshes) {
        //wall.materials[m_modelIndex]->setUniforms(m_shader);
        m_shader.setMat4("u_modelMatrix", wall.modelMatrix);
        m_shader.setFloat("u_textureScaleX", wall.textureScaleX);
        m_shader.setFloat("u_textureScaleY", wall.textureScaleY);
        wall.mesh->draw(m_shader);
    }
    m_shader.setFloat("u_textureScaleX", 1.0f);
    m_shader.setFloat("u_textureScaleY", 1.0f);

    // draw models
    for (auto& model : m_models) {
        m_shader.setMat4("u_modelMatrix", model.m_modelMatrix);
        model.draw(m_shader);
    }

    if (m_wireframeEnabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    m_hdrFBO->unbind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    m_screenQuadRenderer.render(m_hdrFBO->getColorAttachment(0), m_postprocessShader);
}

void ModelTestScene::onRenderImGui()
{
    // back button
    if (ImGui::Button("Back")) {
        Scene* temp = m_currentScene;
        m_currentScene = new SceneMenu(m_currentScene, m_width, m_height);
        delete temp;
        return;
    }

    // render UI for every light
    for (auto& light : m_lights) {
        light->imGuiRender();
    }

    ImGui::DragFloat3("Trnaslte", &trans.x, 0.01f);

    m_material.imGuiRender();

    // button to reset camera position and orientation
    if (ImGui::Button("Reset camera")) {
        m_camera.setPosition({ 0.0f, 0.0f, 5.0f });
        m_camera.setTarget({ 0.0f, 0.0f, 0.0f });
    }

    m_postProcessUI.onRenderImGui();

    // enable/disable wireframes, for debug
    ImGui::Checkbox("Show wireframe", &m_wireframeEnabled);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

void ModelTestScene::updateWidthHeight(unsigned int width, unsigned int height)
{
    m_width = width;
    m_height = height;
    m_hdrFBO = std::make_unique<Framebuffer>(width, height);
    m_hdrFBO->addColorAttachament(GL_TEXTURE_2D, GL_RGBA16F);
    m_hdrFBO->addDepthAttachment(GL_RENDERBUFFER);
    m_hdrFBO->create();
    m_projMatrix = glm::infinitePerspective(glm::radians(60.0f), 1.0f * m_width / m_height, 0.1f);
}