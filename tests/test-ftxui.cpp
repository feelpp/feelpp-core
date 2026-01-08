//!

#include <iostream>

#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include <feel/feelcore/environment.hpp>

TEST_CASE( "Test Screen Basic", "[FTXUI]" )
{
    using namespace ftxui;
    Element document = hbox({
            text("left")   | border,
            text("middle") | border | flex,
            text("right")  | border,
        });
    auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    Render(screen, document);
    screen.Print();
}

TEST_CASE( "Test Gauge", "[FTXUI]" )
{
    using namespace ftxui;
    using namespace std::chrono_literals;

    std::string reset_position;
    for (float percentage = 0.0f; percentage <= 1.0f; percentage += 0.01f) {
        std::string data_downloaded =
            std::to_string(int(percentage * 5000)) + "/5000";
        auto document = hbox({
                text("downloading:"),
                gauge(percentage) | flex,
                text(" " + data_downloaded),
            });
        auto screen = Screen(100, 1);
        Render(screen, document);
        std::cout << reset_position;
        screen.Print();
        reset_position = screen.ResetPosition();

        std::this_thread::sleep_for(0.01s);
    }
    std::cout << std::endl;
}


int main( int argc, char* argv[])
{
    auto env = Feel/*::Core*/::createEnvironment( argc, argv );
    return Catch::Session().run( argc, argv );
}

