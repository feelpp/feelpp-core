//!

#pragma once

#include <ftxui/component/component_base.hpp>
#include <feel/feelcore/feelcore.hpp>

#include <ftxui/component/event.hpp>


namespace Feel::Core::tui
{


//! A viewport base class that allows zooming and panning of a 2D space.
//! Designed to serve as a base class for canvas-like components or nodes that require camera-like functionality.
class FEELPP_CORE_EXPORT Viewport
    : public ftxui::ComponentBase
{
public:
    Viewport( int width = 80, int height = 80,
              int scaleX = 2, int scaleY = 4,
              bool allowZoom = false, bool allowPan = false )
    : M_width( width ), M_height( height ),
      M_scaleX( scaleX ), M_scaleY( scaleY ),
      M_allowZoom( allowZoom ), M_allowPan( allowPan )
    {}


    //! Handle mouse events for zooming and panning the viewport.
    bool OnEvent( ftxui::Event event ) override;

    //! Reset the viewport to its default zoom and pan settings.
    void resetView()
    {
        M_zoom = 1.0;
        M_pan = { 0,0 };
        M_isDragging = false;
    }

    //! Get the UI box representing the viewport area on the screen.
    ftxui::Box const& box() const noexcept { return M_box; }

    //! Check if a given screen coordinate is inside the viewport box.
    bool isInsideBox( int x, int y ) const { return M_box.Contain( x,y ); }

    //! Fit the viewport to a specified bounding box in world coordinates, optionally flipping the Y-axis.
    void fitToBoundingBox( double minX, double minY, double maxX, double maxY, bool flipY = true );

    //! Convert world coordinates to screen coordinates based on the current zoom and pan settings.
    std::pair<double, double> worldToScreen( double worldX, double worldY ) const;

    //! Convert screen coordinates to world coordinates based on the current zoom and pan settings.
    std::pair<double,double> screenToWorld( int x, int y ) const;

protected:
    template <typename T, std::size_t N>
    struct Point
    {
        T& operator[]( std::size_t index ) { return data[index]; }
        T const& operator[]( std::size_t index ) const { return data[index]; }

        T& x() { return data[0]; }
        T const& x() const { return data[0]; }

        T& y() { return data[1]; }
        T const& y() const { return data[1]; }

        std::array<T,N> data;
    };


    ftxui::Box M_box;

    int M_width, M_height;

    int M_scaleX, M_scaleY;

    bool M_allowZoom, M_allowPan;
    double M_zoom = 1.0;
    Point<double,2> M_pan = { 0., 0. };
    bool M_isDragging = false;
    Point<int,2> M_lastMousePos = { 0,0 };

    bool M_flipY = false;
};


}
