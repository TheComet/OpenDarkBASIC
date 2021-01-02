#include "odb-compiler/cli/Actions.hpp"
#include "odb-compiler/cli/AST.hpp"
#include "odb-compiler/cli/Banner.hpp"
#include "odb-compiler/cli/Commands.hpp"
#include "odb-compiler/cli/SDK.hpp"
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

static const Action actions_[] = {
    { "no-banner",     'n',"",                           {0,  0},  0, false, &disableBanner, "Don't print the cool ASCII art banner"},
    { "",               0, "",                           {0,  0},  1, true,  &printBanner, ""},
    { "help",          'h',"",                           {0,  0},  2, false, &printHelp, "Print this help text"},
    { "sdkroot",        0, "<path>",                     {1,  1},  3, false, &setSDKRootDir, "Tell the compiler where to find the SDK (plugins and DB runtime)"},
    { "sdktype",        0, "<odb|dbpro>",                {1,  1},  3, false, &setSDKType, "Specify if the SDK is the original DBPro SDK, or if it is the ODB reimplementation"},
    { "plugins",        0, "<path|file> [path|file...]", {1, -1},  3, false, &setAdditionalPluginsDir, "Add additional directories to scan for thirdparty plugins"},
    { "print-sdkroot",  0, "",                           {0,  0},  4, false, &printSDKRootDir, "Prints the location of the SDK"},
    { "",               0, "",                           {0,  0},  4, true,  &initSDK, ""},
    { "",               0, "",                           {0,  0},  5, true,  &loadCommands, ""},
    { "dump-cmd-json",  0, "[file]",                     {0,  1},  6, false, &dumpCommandsJSON, "Dump all commands (and their type/argument info) to JSON format. The default file is stdout."},
    { "dump-cmd-ini",   0, "[file]",                     {0,  1},  6, false, &dumpCommandsINI, "Dump all commands (and their type/argument info) to INI format. The default file is stdout."},
    { "dump-cmd-names", 0, "[file]",                     {0,  1},  6, false, &dumpCommandNames, "Dump all command names in alphabetical order. The default file is stdout."},
    { "",               0, "",                           {0,  0},  6, true,  &initCommandMatcher, ""},
    { "parse-dba",      0, "<file> [files...]",          {1, -1},  7, false, &parseDBA, "Parse DBA source file(s). The first file listed will become the 'main' file, i.e. where execution starts."},
    { "dump-ast-dot",   0, "[file]",                     {0,  1},  8, false, &dumpASTDOT, "Dump AST to Graphviz DOT format. The default file is stdout."},
    { "dump-ast-json",  0, "[file]",                     {0,  1},  9, false, &dumpASTJSON, "Dump AST to JSON format. The default file is stdout"},
};

static ActionQueue actionQueue_;
static std::string programName_;

#define N_ACTIONS (sizeof(actions_) / sizeof(*actions_))

// ----------------------------------------------------------------------------
static bool printHelp(const std::vector<std::string>& args)
{
    fprintf(stderr, "Usage: %s [options]\n", programName_.c_str());
    fprintf(stderr, "Available options:\n");

    for (int i = 0; i != N_ACTIONS; ++i)
    {
        int padding = 40;

        if (actions_[i].implicit)
            continue;

        if (actions_[i].shortOption)
            padding -= fprintf(stderr, "  -%c, ", actions_[i].shortOption);
        else
            padding -= fprintf(stderr, "      ");

        padding -= fprintf(stderr, "--%s", actions_[i].fullOption);

        if (actions_[i].argDoc)
            padding -= fprintf(stderr, " %s", actions_[i].argDoc);

        while (padding-- > 0)
            putc(' ', stderr);

        fprintf(stderr, "%s\n", actions_[i].doc);
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
                    fprintf(stderr, "Error: Option %s expects at least %d argument%s\n", argv[0], actions_[i].argRange.l, actions_[i].argRange.l == 1 ? "" : "s");
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
            fprintf(stderr, "Error: Unrecognized command line option `%s`\n", argv[0]);

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
                    fprintf(stderr, "Option `-%c` must be at end of short option list (before `-%c`)\n", *str, str[1]);
                    return -1;
                }
                if (argc <= actions_[i].argRange.l)
                {
                    fprintf(stderr, "Error: Option %s expects at least %d argument%s\n", argv[0], actions_[i].argRange.l, actions_[i].argRange.l == 1 ? "" : "s");
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
            fprintf(stderr, "Error: Unrecognized command line option `-%c`\n", *str);
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

    fprintf(stderr, "Error: Unrecognized command line option `%s`\n", argv[0]);
    return 0;
}

// ----------------------------------------------------------------------------
static void addImplicitActions(ActionQueue* queue)
{
    for (int i = 0; i != N_ACTIONS; ++i)
    {
        if (actions_[i].implicit == false)
            continue;

        ActionHandler handler;
        handler.func = actions_[i].handler;
        handler.priority = actions_[i].priority;
        queue->push(handler);
    }
}

// ----------------------------------------------------------------------------
static void addHelp(ActionQueue* queue)
{
    for (int i = 0; i != N_ACTIONS; ++i)
        if (strcmp(actions_[i].fullOption, "help") == 0)
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

    // Go through all command line arguments and split them up into the "global"
    // list and the "sequential" list. One group of commands only toggle some
    // flags (such as --no-banner) whereas the other group of commands needs to
    // be executed in the order they appear in.
    for (int i = 1; i < argc; )
    {
        int processed = parseOption(argc - i, &argv[i], &queue);
        if (processed == 0)
            return false;
        i += processed;
    }

    addImplicitActions(&queue);

    // Be nice and show help if no args were specified
    if (argc == 1)
        addHelp(&queue);

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
