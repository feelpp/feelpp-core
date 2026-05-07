//!

#include <ftxui/component/event.hpp>

#include <feel/feelcore/tui/fileinput.hpp>

namespace Feel::Core::ftxui
{


std::string
FileInputComponent::longestCommonPrefix( std::vector<std::string> const& strings )
{
    if ( strings.empty() ) return "";
    if ( strings.size() == 1 ) return strings[0];

    std::string prefix = strings[0];
    for ( std::size_t i = 1; i < strings.size(); ++i )
    {
        std::size_t j = 0;
        while ( j < prefix.length() && j < strings[i].length() && prefix[j] == strings[i][j] )
            j++;

        prefix = prefix.substr(0, j);
    }
    return prefix;
}

std::string
FileInputComponent::expandTilde( std::string const& pathStr )
{
    if (pathStr.empty()) return ".";
    if (pathStr[0] == '~')
    {
        const char* home = std::getenv("HOME");

        if (!home) // Fallback for Windows
            home = std::getenv("USERPROFILE"); 

        if (home)
        {
            std::string expanded = home;
            if (pathStr.length() > 1 && (pathStr[1] == '/' || pathStr[1] == '\\'))
                expanded += pathStr.substr(1);

            return expanded;
        }
    }
    return pathStr;
}


bool
FileInputComponent::onReturn()
{
    M_autocompleteState.cycling = false;
    if ( M_options.on_enter )
        M_options.on_enter();

    return true;
}


bool
FileInputComponent::handleAutocomplete( Event event )
{
    std::string currentInput = *M_content;

    if ( !M_autocompleteState.cycling || currentInput != M_autocompleteState.last_input )
        findMatches( currentInput, event );
    else if ( M_autocompleteState.cycling )
        cycleMatches( event );
    
    return true;
}




void
FileInputComponent::findMatches( std::string const& currentInput, Event event )
{
    M_autocompleteState.cycling = false;

    std::string parentStr;
    std::string prefix;

    auto lastSep = currentInput.find_last_of( "/\\" );
    if ( lastSep == std::string::npos )
    {
        parentStr = "";
        prefix = currentInput;
    }
    else
    {
        parentStr = currentInput.substr( 0, lastSep + 1 );
        prefix = currentInput.substr( lastSep + 1 );
    }

    std::string searchPathStr = expandTilde( parentStr );
    fs::path searchDir( searchPathStr );

    std::error_code ec;
    if ( !fs::exists( searchDir, ec ) || !fs::is_directory( searchDir, ec ) )
        return; 

    bool showHidden = ( !prefix.empty() && prefix[0] == '.' );

    std::vector<std::string> matches;
    for ( auto const& entry : fs::directory_iterator( searchDir, ec ) )
    {
        std::string filename = entry.path().filename().string();

        if ( filename.length() >= prefix.length() && filename.substr( 0, prefix.length() ) == prefix )
            if ( showHidden || filename[0] != '.' )
                matches.push_back( filename );
    }

    if ( matches.empty() ) return;

    std::sort( matches.begin(), matches.end() );
    std::string lcp = longestCommonPrefix(matches);

    if ( lcp.length() > prefix.length() || matches.size() == 1 )
    {
        std::string newPath = parentStr + lcp;

        if ( matches.size() == 1  && fs::is_directory( searchDir / lcp, ec ) )
            newPath += "/";

        *M_content = newPath;
        M_autocompleteState.last_input = newPath;

        M_input->OnEvent( Event::End );
        return;
    }
    else
    {
        M_autocompleteState.cycling = true;
        M_autocompleteState.matches = matches;
        M_autocompleteState.parent_str = parentStr;

        if ( event == Event::TabReverse )
            M_autocompleteState.cycle_index = matches.size() - 1;
        else
            M_autocompleteState.cycle_index = 0;

        cycleMatches( event );
    }
}


void
FileInputComponent::cycleMatches( Event event )
{
    std::string match = M_autocompleteState.matches[M_autocompleteState.cycle_index];
    std::string newPath = M_autocompleteState.parent_str + match;

    std::string searchPathStr = expandTilde(M_autocompleteState.parent_str);
    fs::path searchDir(searchPathStr);
    fs::path fullP = searchDir / match;
    std::error_code ec;
    if (fs::is_directory(fullP, ec))
        newPath += "/";

    *M_content = newPath;
    M_autocompleteState.last_input = newPath;

    M_input->OnEvent(Event::End);

    if ( event == Event::Tab )
        M_autocompleteState.cycle_index = (M_autocompleteState.cycle_index + 1) % M_autocompleteState.matches.size();
    else if (event == Event::TabReverse)
    {
        if ( M_autocompleteState.cycle_index == 0 )
            M_autocompleteState.cycle_index =  M_autocompleteState.matches.size() - 1;
        else
            --M_autocompleteState.cycle_index;
    }
}


bool
FileInputComponent::OnEvent( Event event )
{
    if ( event.is_mouse() || event == Event::Custom )
        return ComponentBase::OnEvent( event );

    if ( event == Event::Return )
        return onReturn();

    if ( event != Event::Tab && event != Event::TabReverse)
    {
        M_autocompleteState.cycling = false;
        return M_input->OnEvent( event );
    }

    return handleAutocomplete( event );
}


} //namespace Feel::Core::ftxui
