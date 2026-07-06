//!

#include <feel/feelcore/tui/viewport.hpp>



namespace Feel::Core::tui
{



bool
Viewport::OnEvent( ftxui::Event event )
{
    if ( !event.is_mouse() )
        return false;

    auto mouse = event.mouse();
    if ( !isInsideBox( mouse.x, mouse.y ) )
        return false;


    if ( M_allowZoom )
    {
        double scale = 1.0;

        if ( mouse.button == ftxui::Mouse::WheelUp )
            scale = 1.15;
        if ( mouse.button == ftxui::Mouse::WheelDown )
            scale = 1.0 / 1.15;

        if ( scale != 1.0 )
        {
            double sx = ( mouse.x - M_box.x_min ) * M_scaleX;
            double sy = ( mouse.y - M_box.y_min ) * M_scaleY;

            double mathSy = M_flipY ? (M_height - sy - 1.0) : sy;

            M_pan.x() = sx - ( sx - M_pan.x() ) * scale;
            M_pan.y() = mathSy - ( mathSy - M_pan.y() ) * scale;

            M_zoom *= scale;
            return true;
        }
    }

    if ( M_allowPan )
    {
        if ( mouse.button == ftxui::Mouse::Right )
        {
            if ( mouse.motion == ftxui::Mouse::Pressed )
            {
                M_isDragging = true;
                M_lastMousePos = { mouse.x, mouse.y };
                return true;
            }
            else if ( mouse.motion == ftxui::Mouse::Moved && M_isDragging )
            {
                int deltaX = ( mouse.x - M_lastMousePos.x() ) * M_scaleX;
                int deltaY = ( mouse.y - M_lastMousePos.y() ) * M_scaleY;
                M_pan.x() += deltaX;
                M_pan.y() += M_flipY ? -deltaY : deltaY;

                M_lastMousePos = { mouse.x, mouse.y };
                return true;
            }
            else if ( mouse.motion == ftxui::Mouse::Released )
            {
                M_isDragging = false;
                return true;
            }
        }
    }

    return false;
}

void
Viewport::fitToBoundingBox( double minX, double minY, double maxX, double maxY, bool flipY )
{
    M_flipY = flipY;

    double rangeX = std::max( maxX - minX, 1.0 );
    double rangeY = std::max( maxY - minY, 1.0 );

    M_zoom = std::min( M_width / rangeX, M_height / rangeY ) * 0.90;

    M_pan.x() = ( M_width - ( rangeX * M_zoom) ) / 2.0 - ( minX * M_zoom );
    M_pan.y() = ( M_height - ( rangeY * M_zoom) ) / 2.0 - ( minY * M_zoom );
}



std::pair<double, double>
Viewport::worldToScreen( double worldX, double worldY ) const
{
    double screenX = worldX * M_zoom + M_pan.x();
    double screenY = worldY * M_zoom + M_pan.y();

    if (M_flipY)
        screenY = M_height - screenY - 1.0;

    return { screenX, screenY };
}



std::pair<double,double>
Viewport::screenToWorld( int x, int y ) const
{
    int xBox = ( x - M_box.x_min ) * M_scaleX;
    int yBox = ( y - M_box.y_min ) * M_scaleY;

    if ( M_flipY )
        yBox = M_height - yBox - 1;

    double worldX = ( xBox - M_pan.x() ) / M_zoom;
    double worldY = ( yBox - M_pan.y() ) / M_zoom;

    return { worldX, worldY };
}


}

