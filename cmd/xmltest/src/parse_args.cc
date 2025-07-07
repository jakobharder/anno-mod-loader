
#define MODLOADER_VERSION "GU17.1"

#include "./parse_args.h"

#include <memory>
#include <string>

using namespace std;

static void printUsage(FILE* out)
{
    fprintf(out, "xmltest using modloader %s\n", MODLOADER_VERSION);

    fprintf(out, "\nUsage: xmltest.exe [options] target-xml patch-xml|GUID\n\n");
    fprintf(out, "-c=<command>  patch (default): output target-xml with patch-xml applied.\n");
    fprintf(out, "              show: output asset with GUID from target-xml.\n");
    fprintf(out, "              diff: output assets before and after patching.\n");
    fprintf(out, "\n");
    fprintf(out, "-p=<path>     Apply mods before testing patch-xml.\n");
    fprintf(out, "              Multiple are allowed.\n");
    fprintf(out, "-m=<path>     Specify mod path. Default: working directory\n");
    fprintf(out, "              Multiple are allowed.\n");
    fprintf(out, "-i=<relpath>  Read patch content from stdin. File is needed for relative path to mod.\n");
    fprintf(out, "-o            Output file. Default: patched.{xml,fc,cfg,bin}\n");
    fprintf(out, "-s            Skip output.\n");
    fprintf(out, "-v            Verbose.\n");
}

static bool invalidUsage(const std::string& pArg)
{
    fprintf(stderr, "Invalid argument: %s\n", pArg.c_str());
    printUsage(stderr);
    return false;
}

bool parseArguments(int argc, const char* argv[], XmltestParameters& params)
{
    if (argc < 3) {
        printUsage(stderr);
        return false;
    }

    params.command = XmltestParameters::Command::Patch;
    params.skipOutput = false;
    params.useStdin = false;
    params.verbose = false;
    for (int i = 0; i < argc; i++) {
		if (argv[i] == std::string("-v")) {
			params.verbose = true;
        }
    }

    if (params.verbose) {
        printf("xmltest using modloader %s\n", MODLOADER_VERSION);
    }

    char lastMode = 0;
    for (int i = 1; i < argc; i++) {
        std::string pArg = argv[i];
        if (pArg[0] == '-') {
            if (pArg.length() <= 1 || lastMode != 0) {
                return invalidUsage(pArg);
            }
            switch (pArg[1]) {
                case 'v': {
                    break;
                }
                case 'i': {
                    if (params.useStdin) {
                        return invalidUsage(pArg);
                    }
                    params.useStdin = true;
                    lastMode = 'i';
                    break;
                }
                case 's': {
                    if (params.skipOutput || !params.outputFile.empty()) {
                        return invalidUsage(pArg);
                    }
                    params.skipOutput = true;
                    break;
                }
                case 'o': {
                    if (params.skipOutput || !params.outputFile.empty()) {
                        return invalidUsage(pArg);
                    }
                    lastMode = 'o';
                    break;
                }
                case 'c':
                case 'p':
                case 'm': {
                    lastMode = pArg[1];
                    break;
                }
                default: {
                    return invalidUsage(pArg);
                }
            }
        }
        else if (lastMode != 0) {
            switch (lastMode) {
                case 'c': {
                    if (std::string(pArg) == "patch") {
                        params.command = XmltestParameters::Command::Patch;
                    }
                    else if (std::string(pArg) == "diff") {
                        params.command = XmltestParameters::Command::Diff;
                    }
                    else if (std::string(pArg) == "show") {
                        params.command = XmltestParameters::Command::Show;
                    }
                    else {
                        return invalidUsage(pArg);
                    }
                    break;
                }
                case 'o': {
                    params.outputFile = pArg;
                    break;
                }
                case 'm': {
                    params.modPaths.emplace_back(pArg);
                    break;
                }
                case 'p': {
                    params.prepatchPaths.emplace_back(pArg);
                    break;
                }
                case 'i': {
                    params.stdinPath = pArg;
                    break;
                }
                default: {
                    return invalidUsage(pArg);
                }
            }
            lastMode = 0;
        }
        else {
            if (params.targetPath.empty()) {
                params.targetPath = pArg;
            }
            else if (params.patchPath.empty()) {
                params.patchPath = pArg;
            }
            else {
                return invalidUsage(pArg);
            }
        }
    }

    if (lastMode != 0) {
        printUsage(stderr);
        return false;
    }

    if (params.patchPath.empty()) {
        fprintf(stderr, "Specify target and patch file.\n");
        printUsage(stderr);
        return false;
    }

    if (params.modPaths.empty()) {
        params.modPaths.emplace_back(std::filesystem::current_path());
    }

    if (params.outputFile.empty()) {
        params.outputFile = "patched" + params.targetPath.extension().string();
    }

    return true;
}
