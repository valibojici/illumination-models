#include "TextureScene.h"

TextureScene::TextureScene(std::unique_ptr<Scene>& scene, unsigned int width, unsigned int height) :
    Scene(scene, width, height)
{
    updateWidthHeight(width, height);
    m_camera = Camera({ 0.0f, 1.0f, 5.0f }, { 0.0f, 0.0f, 0.0f });
    EventManager::getInstance().addHandler(&m_camera);
    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_lights.push_back(std::move(std::make_unique<PointLight>(0, glm::vec3(-5.0f, 1.5f, -0.5f))));
    m_lights.push_back(std::move(std::make_unique<PointLight>(1, glm::vec3(0.0f, 1.5f, -0.5f))));

    // load shaders
    m_shaders.resize(3);
    m_shaders[0].load("base_shader.vert", "phong.frag");
    m_shaders[1].load("base_shader.vert", "blinn.frag");
    m_shaders[2].load("base_shader.vert", "cook-torrance.frag");
    m_postprocessShader.load("postprocess.vert", "postprocess.frag");
    m_textureDisplayShader.load("postprocess.vert", "texture_display.frag");
    
    m_postProcessUI.addShaders({ &m_shaders[0], &m_shaders[1], &m_shaders[2], &m_postprocessShader });
    m_postProcessUI.setUniforms();

    for (auto& shader : m_shaders) {
        shader.bind();
        shader.setInt("u_numLights", m_lights.size());
        for (auto& light : m_lights) {
            light->setUniforms(shader);
        }
    }

    // load textures
    m_textures.push_back({}); // no textures

    m_textures.push_back({
       TextureManager::get().getTexture("textures/wood_floor/diffuse.png", Texture::Type::DIFFUSE),
       TextureManager::get().getTexture("textures/wood_floor/normal.png", Texture::Type::NORMAL),
       TextureManager::get().getTexture("textures/wood_floor/roughness.png", Texture::Type::ROUGHNESS),
        });

    m_textures.push_back({
        TextureManager::get().getTexture("textures/chain_floor/diffuse.png", Texture::Type::DIFFUSE),
        TextureManager::get().getTexture("textures/chain_floor/normal.png", Texture::Type::NORMAL),
        TextureManager::get().getTexture("textures/chain_floor/roughness.png", Texture::Type::ROUGHNESS),
        TextureManager::get().getTexture("textures/chain_floor/metallic.png", Texture::Type::METALLIC),
        TextureManager::get().getTexture("textures/chain_floor/opacity.png", Texture::Type::OPACITY),
        });
   
    m_textures.push_back({
        TextureManager::get().getTexture("textures/art_deco/diffuse.png", Texture::Type::DIFFUSE),
        TextureManager::get().getTexture("textures/art_deco/normal.png", Texture::Type::NORMAL),
        TextureManager::get().getTexture("textures/art_deco/metallic.png", Texture::Type::METALLIC),
        TextureManager::get().getTexture("textures/art_deco/roughness.png", Texture::Type::ROUGHNESS),
        });

    m_textures.push_back({
        TextureManager::get().getTexture("textures/metal/diffuse.png", Texture::Type::DIFFUSE, false),
        TextureManager::get().getTexture("textures/metal/normal.png", Texture::Type::NORMAL, false),
        TextureManager::get().getTexture("textures/metal/metallic.png", Texture::Type::METALLIC, false),
        TextureManager::get().getTexture("textures/metal/roughness.png", Texture::Type::ROUGHNESS, false),
        });

    m_textures.push_back({
        TextureManager::get().getTexture("textures/wood_platform/diffuse.png", Texture::Type::DIFFUSE),
        TextureManager::get().getTexture("textures/wood_platform/normal.png", Texture::Type::NORMAL),
        TextureManager::get().getTexture("textures/wood_platform/roughness.png", Texture::Type::ROUGHNESS),
        TextureManager::get().getTexture("textures/wood_platform/metallic.png", Texture::Type::METALLIC),
        TextureManager::get().getTexture("textures/wood_platform/opacity.png", Texture::Type::OPACITY),
        });

    m_textures.push_back({
        TextureManager::get().getTexture("textures/grass/diffuse.png", Texture::Type::DIFFUSE),
        TextureManager::get().getTexture("textures/grass/normal.png", Texture::Type::NORMAL),
        TextureManager::get().getTexture("textures/grass/roughness.png", Texture::Type::ROUGHNESS),
        TextureManager::get().getTexture("textures/grass/metallic.png", Texture::Type::METALLIC),
        });
    
    MaterialMesh object;
    object.modelMatrix = glm::translate(glm::vec3(-5.0f, 0.0f, 0.0f)) * glm::rotate(glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    object.materials.push_back(std::move(std::make_unique<PhongMaterial>()));
    object.materials.push_back(std::move(std::make_unique<BlinnMaterial>()));
    object.materials.push_back(std::move(std::make_unique<CookTorranceMaterial>()));
    object.name = "Object 1";
    object.textureScaleX = 2.0f;
    object.textureScaleY = 2.0f;
    object.textureIndex = 1;
    object.mesh = std::move(std::unique_ptr<Mesh>(Mesh::getPlane(4, 4)));
    m_materialMeshes.push_back(std::move(object));

    object = MaterialMesh();
    object.modelMatrix = glm::rotate(glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    object.materials.push_back(std::move(std::make_unique<PhongMaterial>()));
    object.materials.push_back(std::move(std::make_unique<BlinnMaterial>()));
    object.materials.push_back(std::move(std::make_unique<CookTorranceMaterial>()));
    object.name = "Object 2";
    object.textureScaleX = 2.0f;
    object.textureScaleY = 2.0f;
    object.textureIndex = 2;
    object.mesh = std::move(std::unique_ptr<Mesh>(Mesh::getPlane(4, 4)));
    m_materialMeshes.push_back(std::move(object));

    
}

TextureScene::~TextureScene()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    EventManager::getInstance().removeHandler(&m_camera);
}

void TextureScene::onRender()
{
    static double time = glfwGetTime();
    
    m_hdrFBO.bind();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::vec3 clearColor = m_postProcessUI.getGammaCorrection() ? glm::pow(glm::vec3(0.1f), glm::vec3(2.2f)) : glm::vec3(0.1f);
    glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
    glViewport(0, 0, m_width, m_height);

    // update camera position and uniforms
    m_camera.update(glfwGetTime() - time);
    time = glfwGetTime();
    for (auto& shader : m_shaders) {
        shader.setMat4("u_viewMatrix", m_camera.getMatrix());
        shader.setVec3("u_viewPos", m_camera.getPosition());
        shader.setMat4("u_projMatrix", m_projMatrix);
    }

    if (m_wireframeEnabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    // lights
    for (auto& light : m_lights) {
        light->draw(m_shaders[0]);
        for (auto& shader : m_shaders) {
            light->setUniforms(shader);
        }
    }

    // draw mesh
    for (auto& mesh : m_materialMeshes) {
        Shader& shader = m_shaders[mesh.modelIndex];
        mesh.materials[mesh.modelIndex]->setUniforms(shader);
        shader.setMat4("u_modelMatrix", mesh.modelMatrix);
        if (mesh.currentMesh == 1) {
            // if sphere scale X axis by PI
            shader.setFloat("u_textureScaleX", glm::pi<float>() * mesh.textureScaleX);
            shader.setFloat("u_textureScaleY", mesh.textureScaleY);
        }
        else {
            shader.setFloat("u_textureScaleX", mesh.textureScaleX);
            shader.setFloat("u_textureScaleY", mesh.textureScaleY);
        }
        mesh.mesh->setTextures(m_textures[mesh.textureIndex]);
        mesh.mesh->draw(shader);
    }

    if (m_wireframeEnabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // apply postprocessing and write to another FBO
    m_outputFBO.bind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    m_screenQuadRenderer.render(m_hdrFBO.getColorAttachment(0), m_postprocessShader);

    // bind default framebuffer
    m_outputFBO.unbind(); 
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    m_screenQuadRenderer.render(m_outputFBO.getColorAttachment(0), m_textureDisplayShader);
}

void TextureScene::onRenderImGui()
{
    // back button
    if (renderImGuiBackButton()) {
        return; // exit if button is clicked
    }

    // render UI for every light
    for (size_t i = 0; i < m_lights.size(); ++i) {
        ImGui::PushID(m_lights[i].get());
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Set header color
        if (ImGui::CollapsingHeader(m_lights[i]->m_name.c_str())) {
            // render combox for light type
            int type = (int)m_lights[i]->getType();
            if (ImGui::Combo("Type", &type, "Point\0Directional\0Spotlight\0\0")) {
                switch (type)
                {
                case 0:
                    m_lights[i] = std::move(std::make_unique<PointLight>(i, m_lights[i]->getPosition()));
                    break;
                case 1:
                    m_lights[i] = std::move(std::make_unique<DirectionalLight>(i, m_lights[i]->getPosition()));
                    break;
                case 2:
                    m_lights[i] = std::move(std::make_unique<Spotlight>(i, m_lights[i]->getPosition(), glm::vec3(0.0f)));
                    break;
                }
            }
            // render the rest of UI
            m_lights[i]->imGuiRender();
        }
        ImGui::PopStyleColor();
        ImGui::PopID();
        ImGui::NewLine();
    }
 
    ImGui::NewLine();

    // render UI for materials
    static int subdivisions[2] = { 5, 5 };
    for (size_t i = 0; i < m_materialMeshes.size(); ++i) {
        auto& mesh = m_materialMeshes[i];
        ImGui::PushID(mesh.materials[mesh.modelIndex].get());
        if (ImGui::CollapsingHeader(mesh.name.c_str())) {
            // render combo box for texture types
            if (ImGui::Combo("Texture", &mesh.textureIndex, "No texture\0Wood\0Chain\0Art Deco\0Rusted metal\0Wood platform\0Grass\0\0")) {
                // if no texture show UI for presets
                for (auto& material : mesh.materials) {
                    material->setShowPresetsUI(mesh.textureIndex == 0);
                    if (mesh.textureIndex != 0) material->defaultParameters(); // if texture => use default parameters
                }
            }
            if (ImGui::Combo("Mesh", &mesh.currentMesh, "Plane\0Sphere\0\0")) {
                mesh.mesh = mesh.currentMesh == 0 ?
                    std::move(std::unique_ptr<Mesh>(Mesh::getPlane(4, 4))) :
                    std::move(std::unique_ptr<Mesh>(Mesh::getSphere(2, subdivisions[i])));
            }
            ImGui::NewLine();
            ImGui::Combo("Lighting model", &mesh.modelIndex, "Phong\0Blinn-Phong\0Cook-Torrance\0\0");
            // render sphere subdivisons UI
            if (mesh.currentMesh == 1) {    // if mesh is sphere
                if (ImGui::SliderInt("Subdivisions", &subdivisions[i], 0, 8)) {
                    mesh.mesh = std::move(std::unique_ptr<Mesh>(Mesh::getSphere(2, subdivisions[i])));
                }
            }
             
            mesh.materials[mesh.modelIndex]->imGuiRender();
        }
        ImGui::NewLine();
        ImGui::PopID();
    }

    // button to reset camera position and orientation
    if (ImGui::Button("Reset camera")) {
        m_camera.setPosition({ 0.0f, 0.0f, 5.0f });
        m_camera.setTarget({ 0.0f, 0.0f, 0.0f });
    }

    if (ImGui::Button("Screenshot")) {
        m_outputFBO.saveColorAttachmentToPNG(0);
    }

    m_postProcessUI.onRenderImGui();

    // enable/disable wireframes, for debug
    ImGui::Checkbox("Show wireframe", &m_wireframeEnabled);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

void TextureScene::updateWidthHeight(unsigned int width, unsigned int height)
{
    m_width = width;
    m_height = height;
    m_hdrFBO = Framebuffer(width, height);
    m_hdrFBO.addColorAttachament(GL_TEXTURE_2D, GL_RGB16F);
    m_hdrFBO.addDepthAttachment(GL_TEXTURE_2D);
    m_hdrFBO.create();
    // setup output FBO (after postprocessing)
    m_outputFBO = Framebuffer(width, height);
    m_outputFBO.addColorAttachament(GL_TEXTURE_2D, GL_RGB);
    m_outputFBO.create();
    m_projMatrix = glm::infinitePerspective(glm::radians(60.0f), 1.0f * m_width / m_height, 0.1f);
}
