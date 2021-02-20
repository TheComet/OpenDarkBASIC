#include "odb-cli/Actions.hpp"
#include "odb-cli/AST.hpp"
#include "odb-cli/Banner.hpp"
#include "odb-cli/BuildInfo.hpp"
#include "odb-cli/Commands.hpp"
#include "odb-cli/Codegen.hpp"
#include "odb-cli/Log.hpp"
#include "odb-cli/SDK.hpp"
#include "odb-sdk/Log.hpp"
#include "odb-sdk/Str.hpp"
#include <cstring>
#include <queue>

struct ActionHandler
{
    HandlerFunc func;
    std::vector<std::string> args;
    int priority;
};

struct ActionCompare
{
    bool operator()(const ActionHandler& a, const ActionHandler& b) {
        return a.priority > b.priority;
    }
};

typedef std::priority_queue<ActionHandler, std::vector<ActionHandler>, ActionCompare> ActionQueue;

static bool printHelp(const std::vector<std::string>& args);

// clang-format off
static const Action actions_[] = {
    { "help",          'h',"",                           {0,  0},  2, false, &printHelp, "Print this help text"},
    { "no-banner",     'b',"",                           {0,  0},  0, false, &disableBanner, "Don't print the cool ASCII art banner"},
    { "no-color",      'c',"",                           {0,  0},  0, false, &disableColor, "Disables color escape sequences for all messages"},
    { "",               0, "",                           {0,  0},  1, true,  &printBanner, ""},
    { "commit-hash",    0, "",                           {0,  0},  3, false, &printCommitHash, "Prints the git commit hash"},
    { "version",        0, "",                           {0,  0},  3, false, &printVersion, "Prints the version"},
    { "output-type",    0, "<obj|exe|llvm-ir|llvm-bc>",  {1,  1},  3, false, &setOutputType, "Specify the file type generated by the --output flag. Can be either an executable, object file, LLVM IR or LLVM bitcode. Defaults to 'object'."},
    { "sdkroot",        0, "<path>",                     {1,  1},  3, false, &setSDKRootDir, "Tell the compiler where to find the SDK (plugins and DB runtime)"},
    { "sdktype",        0, "<odb|dbpro>",                {1,  1},  3, false, &setSDKType, "Specify if the SDK is the original DBPro SDK, or if it is the ODB reimplementation"},
    { "arch",           0, "<i386|x86_64|aarch64>",      {1,  1},  3, false, &setArch, "Specify the output architecture if the output type is an executable or object file. Defaults to 'x86_64' for the ODB SDK, or 'i386' for the DBPro SDK."},
    { "platform",       0, "<windows|macos|linux>",      {1,  1},  3, false, &setPlatform, "Specify the output platform if the output type is an executable or object file. Defaults to the current platform for the ODB SDK, or 'windows' for the DBPro SDK."},
    { "plugins",        0, "<path|file> [path|file...]", {1, -1},  3, false, &setAdditionalPluginsDir, "Add additional directories to scan for thirdparty plugins. Note that ODB and DBPro plugins are not compatible with each other."},
    { "",               0, "",                           {0,  0},  4, true,  &initSDK, ""},
    { "print-sdkroot",  0, "",                           {0,  0},  5, false, &printSDKRootDir, "Prints the location of the SDK"},
    { "",               0, "",                           {0,  0},  5, true,  &loadCommands, ""},
    { "dump-cmd-json",  0, "[file]",                     {0,  1},  6, false, &dumpCommandsJSON, "Dump all commands (and their type/argument info) to JSON format. The default file is stdout."},
    { "dump-cmd-ini",   0, "[file]",                     {0,  1},  6, false, &dumpCommandsINI, "Dump all commands (and their type/argument info) to INI format. The default file is stdout."},
    { "dump-cmd-names", 0, "[file]",                     {0,  1},  6, false, &dumpCommandNames, "Dump all command names in alphabetical order. The default file is stdout."},
    { "",               0, "",                           {0,  0},  6, true,  &initCommandMatcher, ""},
    { "dba",           'i',"<file> [files...]",          {1, -1},  7, false, &parseDBA, "Parse DBA source file(s). The first file listed will become the 'main' file, i.e. where execution starts."},
    { "dump-ast-dot",   0, "[file]",                     {0,  1},  8, false, &dumpASTDOT, "Dump AST to Graphviz DOT format. The default file is stdout."},
    { "dump-ast-json",  0, "[file]",                     {0,  1},  8, false, &dumpASTJSON, "Dump AST to JSON format. The default file is stdout"},
    { "output",        'o',"[file]",                     {1,  1},  8, false, &output, "Output file."},
};
// clang-format on

static ActionQueue actionQueue_;
static std::string programName_;

#define N_ACTIONS (sizeof(actions_) / sizeof(*actions_))

// ----------------------------------------------------------------------------
static bool printHelp(const std::vector<std::string>& args)
{
    const int WRAP = 80;
    const int DOC_INDENT = 8;

    auto& log = odb::Log::info;
    using C = odb::Log::Color;

    // ------------------------------------------------------------------------
    // Usage
    // ------------------------------------------------------------------------

    log.print(C::FG_BRIGHT_WHITE, "Usage:\n");
    log.print(C::FG_BRIGHT_YELLOW, "  %s ", programName_.c_str());
    log.print("[");
    log.print(C::FG_BRIGHT_GREEN, "options");
    log.print("]\n\n");

    // ------------------------------------------------------------------------
    // Examples
    // ------------------------------------------------------------------------

    log.print(C::FG_BRIGHT_WHITE, "Examples:\n");

    // Compile dba to exe
    log.print(C::FG_BRIGHT_YELLOW, "  Compiling a .dba file to an executable\n");
    log.print("    %s ", programName_.c_str());
    log.print(C::FG_BRIGHT_GREEN, "--dba");
    log.print(" source.dba ");
    log.print(C::FG_BRIGHT_GREEN, "--output");
    log.print(" program.exe\n");
    log.print("    %s ", programName_.c_str());
    log.print(C::FG_BRIGHT_GREEN, "--dba");
    log.print(" source1.dba source2.dba ");
    log.print(C::FG_BRIGHT_GREEN, "--output");
    log.print(" program.exe\n\n");

    // Using the DBPro SDK
    log.print(C::FG_BRIGHT_YELLOW, "  Using the DBPro SDK instead of ODB\n");
    log.print("    %s ", programName_.c_str());
    log.print(C::FG_BRIGHT_GREEN, "--sdktype");
    log.print(" dbpro ");
    log.print(C::FG_BRIGHT_GREEN, "--sdkroot");
    log.print(" <");
    log.print(C::FG_BRIGHT_YELLOW, "path/to/dbp/installation");
    log.print("> ");
    log.print(C::FG_BRIGHT_GREEN, "--dba");
    log.print(" source.dba ");
    log.print(C::FG_BRIGHT_GREEN, "--output");
    log.print(" program.exe\n\n");

    // Rendering
    log.print(C::FG_BRIGHT_YELLOW, "  Converting your program into a graph\n");
    log.print("    %s ", programName_.c_str());
    log.print(C::FG_BRIGHT_GREEN, "--dba");
    log.print(" source.dba ");
    log.print(C::FG_BRIGHT_GREEN, "--dump-ast-dot");
    log.print(" | dot -Tpdf > out.pdf\n\n");

    log.print(C::FG_BRIGHT_WHITE, "Available options:\n");

    for (int i = 0; i != N_ACTIONS; ++i)
    {
        // Implicit actions are invisible to the user
        if (actions_[i].implicit)
            continue;

        // Print short option, if it exists
        if (actions_[i].shortOption)
        {
            log.print(C::FG_BRIGHT_YELLOW, "  -%c", actions_[i].shortOption);
            log.print(", ");
        }
        else
            log.print("      ");

        // Print full option
        log.print(C::FG_BRIGHT_GREEN, "--%s ", actions_[i].fullOption);

        // Format argument documentation, if any
        {
            odb::ColorState state(odb::Log::info, C::FG_BRIGHT_YELLOW);
            for (const char* p = actions_[i].argDoc; *p; ++p)
            {
                if (*p == '<' || *p == '>' || *p == '[' || *p == ']' || *p == '|' || *p == '.')
                    log.putc(C::FG_WHITE, *p);
                else
                    log.putc(*p);
            }
        }

        std::vector<std::string> lines;
        odb::str::justifyWrap(&lines, std::string(actions_[i].doc), WRAP - DOC_INDENT);
        for (auto line = lines.begin(); line != lines.end(); ++line)
        {
            log.putc('\n');
            for (int j = 0; j != DOC_INDENT; ++j)
                log.putc(' ');
            log.print("%s", line->c_str());
        }
        log.putc('\n');
        log.putc('\n');
    }

    return false;
}

// ----------------------------------------------------------------------------
static int parseFullOption(int argc, char** argv, ActionQueue* queue)
{
    char* str = &argv[0][2];  // skip "--"
    auto processTable = [str, argc, argv](ActionQueue* queue) -> int
    {
        for (int i = 0; i != N_ACTIONS; ++i)
            if (strcmp(str, actions_[i].fullOption) == 0)
            {
                if (argc <= actions_[i].argRange.l)
                {
                    odb::Log::info.print("Error: Option %s expects at least %d argument%s\n", argv[0], actions_[i].argRange.l, actions_[i].argRange.l == 1 ? "" : "s");
                    return -1;
                }

                ActionHandler handler;
                handler.func = actions_[i].handler;
                handler.priority = actions_[i].priority;
                for (int arg = 0; arg != actions_[i].argRange.h && arg != argc - 1 && argv[arg + 1][0] != '-'; ++arg)
                    handler.args.push_back(argv[arg + 1]);
                queue->push(handler);
                return handler.args.size() + 1;
            }
        return 0;
    };

    int argsProcessed;
    while (true)
    {
        if ((argsProcessed = processTable(queue)) > 0)
            return argsProcessed;
        else if (argsProcessed == -1)
            break;

        if (argsProcessed == 0)
            odb::Log::info.print("Error: Unrecognized command line option `%s`\n", argv[0]);

        break;
    }

    return 0;
}

// ----------------------------------------------------------------------------
static int parseShortOptions(int argc, char** argv, ActionQueue* queue)
{
    auto processTable = [argc, argv](ActionQueue* queue, const char* str) -> int
    {
        for (int i = 0; i != N_ACTIONS; ++i)
            if (actions_[i].shortOption == *str)
            {
                if (actions_[i].argRange.l > 0 && str[1] != '\0')
                {
                    odb::Log::info.print("Option `-%c` must be at end of short option list (before `-%c`)\n", *str, str[1]);
                    return -1;
                }
                if (argc <= actions_[i].argRange.l)
                {
                    odb::Log::info.print("Error: Option %s expects at least %d argument%s\n", argv[0], actions_[i].argRange.l, actions_[i].argRange.l == 1 ? "" : "s");
                    return -1;
                }

                ActionHandler handler;
                handler.func = actions_[i].handler;
                handler.priority = actions_[i].priority;
                if (str[1] == '\0')
                {
                    for (int arg = 0; arg != actions_[i].argRange.h && arg != argc - 1 && argv[arg + 1][0] != '-'; ++arg)
                        handler.args.push_back(argv[arg + 1]);
                }
                queue->push(handler);
                return handler.args.size() + 1;
            }

        return 0;
    };

    for (char* str = &argv[0][1]; *str; ++str)
    {
        int argsProcessed;
        if ((argsProcessed = processTable(queue, str)) > 1)
            return argsProcessed;
        else if (argsProcessed == 1)
            continue;

        if (argsProcessed == 0)
            odb::Log::info.print("Error: Unrecognized command line option `-%c`\n", *str);
        return 0;
    }

    return 1;
}

// ----------------------------------------------------------------------------
static int parseOption(int argc, char** argv, ActionQueue* queue)
{
    if (argv[0][0] == '-')
    {
        if (argv[0][1] == '-')
            return parseFullOption(argc, argv, queue);
        else
            return parseShortOptions(argc, argv, queue);
    }

    odb::Log::info.print("Error: Unrecognized command line option `%s`\n", argv[0]);
    return 0;
}

// ----------------------------------------------------------------------------
static void addImplicitActions(ActionQueue* queue, int lastPriority)
{
    if (queue->empty())
        return;

    for (int i = 0; i != N_ACTIONS; ++i)
    {
        if (actions_[i].implicit == false)
            continue;

        // Only add an implicit action if there is an action in the queue that
        // depends on it. These would be actions that strictly have a lower
        // value
        if (actions_[i].priority >= lastPriority)
            continue;

        ActionHandler handler;
        handler.func = actions_[i].handler;
        handler.priority = actions_[i].priority;
        queue->push(handler);
    }
}

// ----------------------------------------------------------------------------
static void addBanner(ActionQueue* queue)
{
    for (int i = 0; i != N_ACTIONS; ++i)
        if (actions_[i].handler == &printBanner)
        {
            ActionHandler handler;
            handler.func = actions_[i].handler;
            handler.priority = actions_[i].priority;
            queue->push(handler);
            return;
        }
}

// ----------------------------------------------------------------------------
static void addHelp(ActionQueue* queue)
{
    for (int i = 0; i != N_ACTIONS; ++i)
        if (actions_[i].handler == &printHelp)
        {
            ActionHandler handler;
            handler.func = actions_[i].handler;
            handler.priority = actions_[i].priority;
            queue->push(handler);
            return;
        }
}

// ----------------------------------------------------------------------------
bool parseCommandLine(int argc, char** argv)
{
    ActionQueue queue;
    programName_ = argv[0];

    for (int i = 1; i < argc; )
    {
        int processed = parseOption(argc - i, &argv[i], &queue);
        if (processed == 0)
            return false;
        i += processed;
    }

    // Find the priority of the action that will be executed last
    ActionQueue copy(queue);
    int lastPriority = 0;
    while (!copy.empty())
    {
        if (lastPriority < copy.top().priority)
            lastPriority = copy.top().priority;
        copy.pop();
    }

    addImplicitActions(&queue, lastPriority);

    // Be nice and show help if no meaningful actions are going to be run
    if (queue.empty() || lastPriority < 2)  // Help action has priority 2
    {
        addBanner(&queue);
        addHelp(&queue);
    }

    // Process all actions
    while (!queue.empty())
    {
        const ActionHandler& handler = queue.top();
        if ((handler.func)(handler.args) == false)
            return false;
        queue.pop();
    }

    return true;
}
