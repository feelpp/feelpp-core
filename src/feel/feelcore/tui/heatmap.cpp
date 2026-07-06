//!

#include <feel/feelcore/tui/heatmap.hpp>

namespace Feel::Core::tui
{

void
HeatmapNode::Render( ftxui::Screen & screen )
{
    double valueRange = std::max( M_maxValue - M_minValue, 0.001 );

    for ( int y = box_.y_min; y <= box_.y_max; ++y )
    {
        int baseYTop = ( y - box_.y_min ) * 2;
        int baseYBot = baseYTop + 1;

        if ( baseYTop >= M_height ) continue;

        for ( int x = box_.x_min; x <= box_.x_max; ++x )
        {
            int pixelX = x - box_.x_min;

            if ( pixelX >= M_width ) continue;

            auto valTop = M_sampler( pixelX, baseYTop );
            auto valBot = M_sampler( pixelX, baseYBot );

            ftxui::Color colorTop = ftxui::Color::Black;
            if ( valTop )
                colorTop = getHeatmapColor( ( *valTop - M_minValue) / valueRange );

            ftxui::Color colorBot = ftxui::Color::Black;
            if ( valBot )
                colorBot = getHeatmapColor( ( *valBot - M_minValue ) / valueRange );

            auto & pixel = screen.PixelAt(x, y);
            pixel.character = "▀";
            pixel.foreground_color = colorTop;
            pixel.background_color = colorBot;
        }
    }
}

ftxui::Color
HeatmapNode::getHeatmapColor( double normalizedValue )
{
    double v = std::clamp( normalizedValue, 0.0, 1.0 );
    return ftxui::Color::HSV( static_cast<uint8_t>( (1. - v) * 170. ), 255, 255 );
}



ftxui::Element
HeatmapComponent::OnRender()
{
    if ( !M_sampler )
        return ftxui::text("No heatmap data") | ftxui::center | ftxui::border;

    auto pixelToWorldSampler = [ this ]( int pixelX, int pixelY ) -> std::optional<double>
    {
        double worldX = ( pixelX - M_pan.x() ) / M_zoom;
        double worldY = ( pixelY - M_pan.y() ) / M_zoom;

        return M_sampler( worldX, worldY );
    };

    auto rasterNode = std::make_shared<HeatmapNode>( M_width, M_height, M_minValue, M_maxValue, pixelToWorldSampler );

    return ftxui::Element(rasterNode) | ftxui::reflect(M_box) |  ftxui::center;
}


}
