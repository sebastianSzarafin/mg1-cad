#ifndef ESPERT_SANDBOX_C0BEZIERSURFACECOMPONENT_HH
#define ESPERT_SANDBOX_C0BEZIERSURFACECOMPONENT_HH

#include "C0BezierSurfaceInfo.hh"
#include "Espert.hh"
#include "Interfaces/Interfaces.hh"
#include "Layers/Objects/Point/PointComponent.hh"
#include "Utils/Enums.hh"

using namespace esp;

namespace mg1
{
  class C0BezierSurfaceComponent : public IComponent, public IEventable
  {
   private:
    static const int s_patch_size   = 4;
    static const int s_patch_offset = 3;

   protected:
    std::shared_ptr<C0BezierSurfaceInfo> m_info;

    std::vector<uint32_t> m_control_points{};
    std::vector<uint32_t> m_surface_indices{};
    std::vector<uint32_t> m_control_line_indices{};

    SurfaceType m_type;
    int m_patches_u; // surface division (horizontal)
    int m_patches_v; // surface division (vertical)
    int m_points_u;
    int m_points_v;
    int m_vertex_count;
    bool m_wrap_u;

   public:
    C0BezierSurfaceComponent(int id, Scene* scene, CreateSurfaceData data);
    C0BezierSurfaceComponent(int id, Scene* scene);
    ~C0BezierSurfaceComponent() = default;

    std::tuple<std::vector<Vertex>, std::vector<uint32_t>> reconstruct();
    void set_dirty_flag();

    void translate(glm::vec3 position);
    void remove();

    inline C0BezierSurfaceInfo* get_info() { return m_info.get(); }
    inline bool display_control_line() const { return m_info->display_control_line(); }
    inline std::tuple<int, int> get_tesselation_level() { return { m_info->m_tess_u, m_info->m_tess_v }; }
    inline std::vector<uint32_t>& get_surface_indices() { return m_surface_indices; }
    inline std::vector<uint32_t>& get_control_line_indices() { return m_control_line_indices; }

    virtual void handle_event(GuiCheckboxChangedEvent& event);
    virtual void handle_event(CursorRotChangedEvent& event);
    virtual void handle_event(CursorScaleChangedEvent& event);

   protected:
    virtual std::vector<uint32_t> create_control_points(CreateSurfaceData data);
    virtual inline const int patch_size() { return s_patch_size; }
    virtual inline const int patch_offset() { return s_patch_offset; }

    std::vector<Vertex> generate_patches();
    std::vector<uint32_t> generate_surface_indices();
    std::vector<uint32_t> generate_control_line_indices();

    static std::vector<PointInfo*> create_point_infos(std::vector<uint32_t>& control_points);
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_C0BEZIERSURFACECOMPONENT_HH
