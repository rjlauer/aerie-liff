/*!
 * @filename CommandLineConfigurator.h
 * @brief A command-line parser.
 * @author Segev BenZvi
 * @date 17 Sep 2010
 * @version $Id: CommandLineConfigurator.h 27790 2015-11-17 20:55:41Z sybenzvi $
 */

#ifndef HAWCNEST_COMMAND_LINE_CONFIGURATOR_H_INCLUDED
#define HAWCNEST_COMMAND_LINE_CONFIGURATOR_H_INCLUDED

#include <boost/program_options.hpp>

#include <string>

namespace po = boost::program_options;

class OptionGroup {

  public:

    OptionGroup(const std::string& name);

    template<class T>
    void AddOption(const std::string& name,
                   const std::string& desc);

    template<class T>
    void AddOption(const std::string& name, const T& init,
                   const std::string& desc);

    void AddFlag(const std::string& name, const std::string& desc);

    template<class T>
    void AddMultiOption(const std::string& name, const std::string& desc);

    po::options_description& GetOptionsDescription() { return opts_; }

  private:

    po::options_description opts_;

    bool hasPositionalOption_;    ///< Check for >1 positional option request
    bool hasMultiOption_;         ///< Check for multi-options don't mix
                                  ///< with positional options

};

/*!
 * @class CommandLineConfigurator
 * @author Segev BenZvi
 * @date 17 Sep 2010
 * @ingroup hawcnest_api
 * @brief Class to parse command line arguments
 *
 * This class provides a small, uniform interface for parsing command line
 * arguments and setting up options.  It wraps the boost::program_options
 * library, so it shares many of the same advantages, such as automatic lexical
 * casting of data and automatic generation of help/usage messages.  For
 * example:
 *
 * <ul>
 *   <li> The interface is significantly smaller and easier to use than getopt
 *   </li>
 *   <li> It parses arguments and lexically casts them to the desired type, 
 *   including containers (e.g., vector<string>, for example).  If the cast
 *   fails, a boost::program_options exception is raised
 *   </li>
 *   <li> It can handle regular options and positional arguments, making calls
 *   like this trival to parse:
 *   \code
 *     $> progname -i -v --output=out.root in1.txt in2.txt in3.txt
 *   \endcode
 *   </li>
 * </ul>
 *
 * The interface also has the benefit of hiding a few obnoxious details in the
 * boost::program_options library.  The known problems of positional arguments
 * in boost are not possible here: the class is set up to prevent the user from
 * making time-wasting mistakes.
 */
class CommandLineConfigurator {

  public:

    CommandLineConfigurator(const std::string& dscr = "");

   ~CommandLineConfigurator();

    /// Parse the command line; return false if some syntax error is found
    bool ParseCommandLine(int argc, char* argv[]);

    /// Add an option group that handles user options
    OptionGroup& AddOptionGroup(const std::string& name);

    /// Add an option; the template argument gives the data type stored.
    /// To add short and long option names, specify the name as "option,o"
    template<class T>
    void AddOption(const std::string& name, const std::string& desc);

    /// Add a command line option with a default value; the template argument
    /// gives the data type to be stored
    template<class T>
    void AddOption(const std::string& name, const T& init, const std::string& desc);

    /// Add a list of options (--option opt1 opt2 ...) ; the template argument
    /// gives the data type stored. It is supposed to be a STL vector to hold
    /// all given options. Don't mix AddMultiOption with AddPositionalOption.
    /// To add short and long option names, specify the name as "option,o"
    template<class T>
    void AddMultiOption(const std::string& name, const std::string& desc);

    /// Add a command line option of the form "progname file.txt", so that the 
    /// option string does not have to be specified
    template<class T>
    void AddPositionalOption(const std::string& name, const std::string& desc);

    /// Retreive data corresponding to an option name 
    template<class T>
    const T& GetArgument(const std::string& name) const;

    /// Add a simple flag option (like -l in the UNIX ls command)
    void AddFlag(const std::string& name, const std::string& desc);

    /// Check if a given flag option has been set
    bool HasFlag(const std::string& name) const;

  private:

    typedef std::map<std::string, OptionGroup*> GroupMap;
    typedef GroupMap::iterator GroupMapIterator;
    typedef GroupMap::const_iterator ConstGroupMapIterator;

    /// Short description of program
    std::string progDscr_;

    /// Generic command-line options common to all programs
    po::options_description gopts_;

    /// User configuration options
    po::options_description copts_;

    /// Additional option groups (for nice help menu printouts)
    std::vector<std::string> groupNames_;
    GroupMap groups_;

    /// Positional options set by users (such as a list of input file names)
    po::positional_options_description popts_;

    /// Option variable-value map
    po::variables_map vm_;

    bool hasPositionalOption_;    ///< Check for >1 positional option request
    bool hasMultiOption_;         ///< Check for multi-options don't mix
                                  ///< with positional options
    int verbosity_;               ///< Verbosity level for logging system
    unsigned int fpExceptMask_;   ///< Floating-point exception mask

};

#include <hawcnest/CommandLineConfigurator-inl.h>

#endif // HAWCNEST_COMMAND_LINE_CONFIGURATOR_H_INCLUDED

