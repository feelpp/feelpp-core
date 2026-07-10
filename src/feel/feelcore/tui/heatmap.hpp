//!

#pragma once

#include <ftxui/component/component_base.hpp>

#include <feel/feelcore/feelcore.hpp>
#include <feel/feelcore/tui/viewport.hpp>


namespace Feel::Core::tui
{

class HeatmapNode : public ftxui::Node
{
    using PixelSampler = std::function<std::optional<double>(int, int)>;

public:
    HeatmapNode( int width, int height, double minValue, double maxValue, PixelSampler sampler )
        : M_width(width), M_height(height), M_minValue(minValue), M_maxValue(maxValue), M_sampler(sampler) {}

    void ComputeRequirement() override
    {
        requirement_.min_x = M_width;
        requirement_.min_y = M_height / 2;
    }

    void Render( ftxui::Screen & screen ) override;

private:

    ftxui::Color getHeatmapColor(double normalizedValue);

private:
    int M_width, M_height;
    double M_minValue, M_maxValue;
    PixelSampler M_sampler;
};



class HeatmapComponent
    : public Viewport
{
public:
    HeatmapComponent( int width, int height, bool allowZoom = true, bool allowPan = true )
        : Viewport( width, height, 1, 2, allowZoom, allowPan )
    { }

    void setData( std::function<std::optional<double>( double, double )> sampler, double minValue, double maxValue )
    {
        M_sampler = sampler;
        M_minValue = minValue;
        M_maxValue = maxValue;
    }

    ftxui::Element OnRender() override;


private:
    double M_minValue = 0.0;
    double M_maxValue = 1.0;
    std::function<std::optional<double>( double, double )> M_sampler;
};




}
