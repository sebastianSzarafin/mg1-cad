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

    const SurfaceType m_type;
    const int m_patches_u; // surface division (horizontal)
    const int m_patches_v; // surface division (vertical)

   public:
    C0BezierSurfaceComponent(int id, Scene* scene, CreateSurfaceData data);
    ~C0BezierSurfaceComponent() = default;

    virtual std::tuple<std::vector<Vertex>, std::vector<uint32_t>> reconstruct();
    virtual void set_dirty_flag();

    void translate(glm::vec3 position);
    void remove();

    inline C0BezierSurfaceInfo* get_info() { return m_info.get(); }
    inline bool display_control_line() const { return m_info->display_control_line(); }

    inline int points_u() { return s_patch_size + s_patch_offset * (m_patches_u - 1) - (wrap_u() ? 1 : 0); }
    inline int points_v() { return s_patch_size + s_patch_offset * (m_patches_v - 1); }
    inline bool wrap_u() { return m_type == SurfaceType::Cylinder; }
    inline std::tuple<int, int> get_tesselation_level() { return { m_info->m_tess_u, m_info->m_tess_v }; }

    virtual void handle_event(CursorRotChangedEvent& event);
    virtual void handle_event(CursorScaleChangedEvent& event);

   protected:
    std::vector<uint32_t> create_control_points(CreateSurfaceData data);

    static std::vector<uint32_t> get_surface_indices(uint32_t vertex_count);
    static std::vector<PointInfo*> create_point_infos(std::vector<uint32_t>& control_points);

   private:
    std::vector<Vertex> generate_patches();
  };
} // namespace mg1

#endif // ESPERT_SANDBOX_C0BEZIERSURFACECOMPONENT_HH
