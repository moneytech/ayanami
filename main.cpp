#define _CRT_SECURE_NO_WARNINGS
#include "gl.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>
#include <TextEditor.h>
#include <imfilebrowser.h>

#include "ayanami.h"
#include "log.h"
#include "random.h"

void lecb(const char* m) {
  fprintf(stderr, "LUA ERROR %s\n", m);
}

int main(int argc, char *argv[]) {
  // Create window.
  const int glfw_init_result = glfwInit();
  assert(glfw_init_result == GLFW_TRUE);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  GLFWwindow *win = glfwCreateWindow(1024u, 768u, "ayanami", nullptr, nullptr);
  assert(win);
  glfwMakeContextCurrent(win);

  // Set up ImGui.
  ImGui::SetCurrentContext(ImGui::CreateContext());
  ImGui_ImplGlfw_InitForOpenGL(win, true);
  ImGui::StyleColorsDark();
  // Apply custom style.
  ImGuiStyle& gui_style        = ImGui::GetStyle();
  gui_style.WindowRounding     = 0.0f;
  gui_style.ScrollbarRounding  = 0.0f;
  gui_style.FrameBorderSize    = 1.0f;
  gui_style.ScrollbarSize      = 20.0f;
  gui_style.WindowTitleAlign.x = 0.5f;
  gui_style.TabRounding        = 0.0f;
  gui_style.TabBorderSize      = 0.0f;
  // Initialize.
  const bool imgui_gl_init_result  = ImGui_ImplOpenGL3_Init() &&
                                     ImGui_ImplOpenGL3_CreateDeviceObjects() &&
                                     ImGui_ImplOpenGL3_CreateFontsTexture();
  assert(imgui_gl_init_result);

  // App state.
  lua_env                  lua {lecb};
  TextEditor               scene_editor;
  std::string              scene_script_path;
  ImGui::FileBrowser       open_dialog { ImGuiFileBrowserFlags_CloseOnEsc };
  ImGui::FileBrowser       save_tga_dialog { 
      ImGuiFileBrowserFlags_EnterNewFilename |
      ImGuiFileBrowserFlags_CloseOnEsc };
  ImGui::FileBrowser       save_scn_dialog { 
      ImGuiFileBrowserFlags_EnterNewFilename |
      ImGuiFileBrowserFlags_CloseOnEsc };
  std::unique_ptr<ayanami> rt;
  std::unique_ptr<scene>   scn;
  uint32_t                 fb_size[] { 1000u, 500u };
  framebuffer              fb { fb_size[0], fb_size[1] };
  ayanami::render_params   rp { 8u,
                                10u,
                                10u,
                                100u
                              };
  GLuint                   image_texture = GL_NONE;
  log_messages             l;
  int                      bvh_strat = 0;
  std::chrono::time_point  render_start = std::chrono::system_clock::now();
  auto save = [&scene_script_path, &scene_editor]() {
    FILE              *f    = fopen(scene_script_path.c_str(), "wb");
    const std::string &text = scene_editor.GetText();
    fwrite(text.c_str(), 1u, text.size(), f);
    fclose(f);
  };
  auto save_as = [&save_scn_dialog]() {
    save_scn_dialog.Open();
  };
  TextEditor::LanguageDefinition lua_def = TextEditor::LanguageDefinition::Lua();
  lua_def.mAutoIndentation = true;
  scene_editor.SetLanguageDefinition(lua_def);

  // Main event loop.
  bool show_child = false;
  l.append("ayanami raytracer");
  l.append("=================");
  while (!glfwWindowShouldClose(win)) {
    int win_width = 0, win_height = 0;
    glfwGetFramebufferSize(win, &win_width, &win_height);
    ImGui::GetIO().DisplaySize.x = (float)win_width;
    ImGui::GetIO().DisplaySize.y = (float)win_height;
    glfwPollEvents();
    ImGui::NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::SetNextWindowPos(ImVec2(.0f, .0f));
    ImGui::SetNextWindowSize(ImVec2(win_width, win_height));
    ImGui::Begin("bg", nullptr, ImGuiWindowFlags_NoDecoration |
                                ImGuiWindowFlags_NoBackground |
                                ImGuiWindowFlags_NoBringToFrontOnFocus);
    if (image_texture != GL_NONE) {
      ImGui::Image((ImTextureID)(intptr_t)image_texture,
                    ImVec2(fb.width(), fb.height()),
                    ImVec2(0, 1),
                    ImVec2(1, 0));
    } else {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
      ImGui::Text("no image available yet");
      ImGui::PopStyleColor();
    }
    ImGui::End();
    ImGui::Begin("scene editor", nullptr, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("scene")) {
        if(ImGui::MenuItem("open...", "ctrl-o")) {
          open_dialog.Open();
        }
        if(ImGui::MenuItem("save", "ctrl-s")) {
          if (!scene_script_path.empty()) {
            save();
          } else {
            save_as();
          }
        }
        if(ImGui::MenuItem("save as...")) {
          save_as();
        }
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("render")) {
        if (ImGui::MenuItem("start rendering", "f12", nullptr, rt.get() == nullptr)) {
          l.append("started rendering");
          l.append("  ", fb_size[0], "x", fb_size[1], "@", rp.nsamples, "spp");
          l.append("  ", rp.ntilesx, "x", rp.ntilesy, " tiles; ", rp.nworkers, " workers");
          fb = framebuffer{ fb_size[0], fb_size[1] };
          set_seed(15677);
          scn = std::make_unique<scene>(lua,
                                        (float)fb.width() / (float)fb.height(),
                                        scene_editor.GetText().c_str(),
                                        scene_editor.GetText().length(),
                                        static_cast<bvh_node::cons_strat>(bvh_strat));
          l.append("  bvh cons ", scn->get_stats().bvh_construction);
          l.append("  lua ", scn->get_stats().lua_runtime);
          render_start = std::chrono::system_clock::now();
          rt = std::make_unique<ayanami>(fb, *scn, rp);
          if (image_texture != GL_NONE) {
            glDeleteTextures(1, &image_texture);
            image_texture = GL_NONE;
          }
        }
        if (ImGui::MenuItem("re-render", "f11", nullptr, rt.get() == nullptr && scn.get() != nullptr)) {
          scn->rebuild_bvh(static_cast<bvh_node::cons_strat>(bvh_strat));
          l.append("  bvh cons ", scn->get_stats().bvh_construction);
          render_start = std::chrono::system_clock::now();
          rt = std::make_unique<ayanami>(fb, *scn, rp);
          if (image_texture != GL_NONE) {
            glDeleteTextures(1, &image_texture);
            image_texture = GL_NONE;
          }
        } if (ImGui::MenuItem("save as tga...", "ctrl-e", nullptr, image_texture != GL_NONE)) {
          save_tga_dialog.Open();
        }
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }
    save_tga_dialog.Display();
    if (save_tga_dialog.HasSelected()) {
      fb.save(save_tga_dialog.GetSelected().string().c_str());
      save_tga_dialog.ClearSelected();
    }
    save_scn_dialog.Display();
    if (save_scn_dialog.HasSelected()) {
      scene_script_path = save_scn_dialog.GetSelected().string();
      save();
      save_scn_dialog.ClearSelected();
    }


    open_dialog.Display();
    if (open_dialog.HasSelected()) {
      scene_script_path = open_dialog.GetSelected().string();
      FILE *f = fopen(scene_script_path.c_str(), "r");
      if (f) {
        fseek(f, 0, SEEK_END);
        std::string buf (ftell(f), ' ');
        fseek(f, 0, 0);
        fread(buf.data(), 1u, buf.size(), f);
        fclose(f);
        scene_editor.SetText(buf);
        l.append("loaded scene script file from\n [", scene_script_path, "]");
      } else {
        l.append("ERROR: failed to open scene script file ", scene_script_path);
      }
      open_dialog.ClearSelected();
    }

    ImGui::Columns(2u, nullptr, true);
    ImGui::SetColumnWidth(0, 256.0f);
    ImGui::BulletText("parallelism");
    ImGui::Indent();
    ImGui::InputScalar("workers", ImGuiDataType_U32, &rp.nworkers);
    ImGui::InputScalarN("tiles", ImGuiDataType_U32, &rp.ntilesx, 2);
    ImGui::Unindent();
    ImGui::BulletText("quality");
    ImGui::Indent();
    ImGui::InputScalarN("imgsize", ImGuiDataType_U32, fb_size, 2);
    ImGui::InputScalar("spp", ImGuiDataType_U32, &rp.nsamples);
    ImGui::Unindent();
    ImGui::BulletText("bvh");
    ImGui::Indent();
    const char *bvh_strat_names[] = {
      "max unique elements",
      "random axis"
    };
    ImGui::Combo("strat", &bvh_strat, bvh_strat_names, 2);
    ImGui::Unindent();
    ImGui::BulletText("log");
    ImGui::Indent();
    ImGui::BeginChild("log");
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    for (const std::string &s : l) {
      ImGui::Text(s.c_str());
    }
    ImGui::PopStyleColor();
    ImGui::SetScrollHereY();
    ImGui::EndChild();
    ImGui::Unindent();
    if (rt) {
      ImGui::Text("waiting for render");
      if (rt->finished()) {
        std::chrono::time_point         render_end = std::chrono::system_clock::now();
        std::chrono::duration<double>   render_dur = render_end - render_start;
        l.append("render finished ", render_dur.count(), "sec.");
        rt->join();
        glGenTextures(1, &image_texture);
        glBindTexture(GL_TEXTURE_2D, image_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGB8,
                     fb.width(), fb.height(),
                     0,
                     GL_BGR,
                     GL_UNSIGNED_BYTE,
                     fb.data());
        rt.reset(nullptr);
      }
    }
    ImGui::NextColumn();
    ImGui::BeginChild("scened");
    if (!scene_script_path.empty()) {
      ImGui::Text(scene_script_path.c_str());
    }
    scene_editor.Render("scene_script_editor");
    ImGui::EndChild();
    ImGui::NextColumn();
    ImGui::End();
    ImGui::EndFrame();
    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
    glClear(GL_COLOR_BUFFER_BIT);
    if (draw_data) {
      ImGui_ImplOpenGL3_RenderDrawData(draw_data);
    }
    glfwSwapBuffers(win);
  }

  // Teardown.
  ImGui_ImplOpenGL3_DestroyFontsTexture();
  ImGui_ImplOpenGL3_DestroyDeviceObjects();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  glfwDestroyWindow(win);
  glfwTerminate();
  return 0;
}
