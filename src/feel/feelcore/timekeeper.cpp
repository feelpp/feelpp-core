//!

#include <fstream>

#include <feel/feelcore/timekeeper.hpp>


namespace Feel::Core
{

class Timekeeper::Impl
{
public:
    static thread_local std::vector<std::string> M_context;
    //! Get the thread local context
    std::vector<std::string> const& getContext() const noexcept { return M_context; }
    //! Get the thread local context
    void setContext( std::vector<std::string> const& ctx ) { M_context = ctx; }
    //! Push a new context level
    void pushContext( std::string const& name ) { M_context.push_back( name ); };
    //! Pop the last context level
    void popContext() {
        if ( M_context.empty() )
        {
            log::warn("Timekeeper popContext called without matching pushContext.");
            return;
        }
        M_context.pop_back();
    }
};

thread_local std::vector<std::string> Timekeeper::Impl::M_context;



std::unique_ptr<Timekeeper> Timekeeper::S_instance = nullptr;
std::once_flag Timekeeper::S_onceFlag;

Timekeeper::Timekeeper()
    :
    M_pimpl(new Impl())
{}

Timekeeper::~Timekeeper() { delete M_pimpl; }


Timekeeper*
Timekeeper::instance()
{
    std::call_once( S_onceFlag, []() {
        S_instance = std::unique_ptr<Timekeeper>( new Timekeeper() );
    } );
    return S_instance.get();
}

std::vector<std::string> const&
Timekeeper::getContext() const noexcept
{
    return M_pimpl->getContext();
}

void
Timekeeper::setContext( std::vector<std::string> const& ctx )
{
    M_pimpl->setContext( ctx );
}

void
Timekeeper::pushContext( std::string const& name )
{
    M_pimpl->pushContext( name );
}

void
Timekeeper::popContext()
{
    M_pimpl->popContext();
}


void
Timekeeper::recordTime( double time )
{
    std::unique_lock<std::shared_mutex> lock( M_mutex );
    bool isRoot = true;
    nl::json * current = &M_times;

    for ( std::string const& key : this->getContext() )
    {
        if ( key.empty() )
            continue;

        if ( isRoot )
        {
            if ( !current->contains( key ) )
                (*current)[key] = nl::json::object();
            current = &(*current)[key];
            isRoot = false;
        }
        else
        {
            if ( !current->contains( "subsections" ) )
                (*current)["subsections"] = nl::json::object();

            nl::json & subsections = (*current)["subsections"];
            if ( !subsections.contains( key ) )
                subsections[key] = nl::json::object();

            current = &subsections[key];
        }
    }

    if ( !current->contains( "times" ) )
        (*current)["times"] = nl::json::object();

    nl::json & times = (*current)["times"];

    if ( times.contains( "elapsed" ) ||  times.contains( "elapsed_sum" ))
    {
        double elapsed =  times.contains( "elapsed_sum" ) ? times.at( "elapsed_sum" ) : times.at( "elapsed" );
        times["elapsed_sum"] = elapsed + time;
        times["count"] = times.value( "count", 1 ) + 1;
        times["min"] = std::min( times.value( "min", elapsed ),time );
        times["max"] = std::max( times.value( "max", elapsed ),time);
        times.erase( "elapsed" );
    }
    else
        times["elapsed"] = time;
}

void
Timekeeper::save( fs::path const& filename ) const
{
    if ( ! Timekeeper::instance()->isEnabled() ) return;

    std::ofstream file( filename );
    if ( !file.is_open() )
        throw std::runtime_error( "Could not open file to save timekeeper data: " + filename.string() );

    //TODO: Evaluate memory overhead and copy cost (snapshot) VS locking the dump.
    nl::json timesSnapshot;
    {
        // Protects against concurrent recordTime() calls mutating M_times
        // Shared lock sufficient because save() is readonly
        std::shared_lock<std::shared_mutex> lock( M_mutex );
        timesSnapshot = M_times;
    }
    file << timesSnapshot.dump( 4 );
}


void
Timer::tic( std::string const& name, std::string const& msg, int verboseLvl )
{
    if ( ! Timekeeper::instance()->isEnabled() ) return;
    if ( verboseLvl > 0 && !msg.empty() )
        log::info( "Starting: {}", msg );

    Timekeeper::instance()->pushContext( name );
    M_timeStack.push( std::make_tuple( Clock::now(), msg, verboseLvl ) );
}

double
Timer::toc()
{
    if ( ! Timekeeper::instance()->isEnabled() ) return 0.0;

    if ( M_timeStack.empty() )
    {
        log::warn( "Timer toc called without matching tic." );
        return 0.0;
    }

    auto & [ start, msg, verboseLvl ] = M_timeStack.top();

    double elapsed = std::chrono::duration<double>(  Clock::now() - start ).count();

    if ( verboseLvl > 0 && !msg.empty() )
        log::info( "{} done in {} s", msg, elapsed );

    Timekeeper::instance()->recordTime( elapsed );
    Timekeeper::instance()->popContext( );

    M_timeStack.pop();
    return elapsed;
}


} // namespace Feel::Core
