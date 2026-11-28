"""Build a native xmltest for local development without Bazel (macOS/Linux)."""

import argparse
import os
from pathlib import Path
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if sys.platform == "win32":
        parser.error("Use the Bazel Windows build described in CONTRIBUTING.md")
    root = args.source.resolve()
    output = args.output.resolve()
    includes = [
        root / "cmd/xmltest/src",
        root / "libs/xml-operations/include",
        root / "third_party/pugixml/src",
        root / "third_party/spdlog/include",
        root / "third_party/json/single_include",
    ]
    # The original 1800 CLI includes this header even though it does not use it.
    if '"absl/strings/str_cat.h"' in (root / "cmd/xmltest/src/main.cc").read_text():
        abseil = [Path("/opt/homebrew/opt/abseil/include"), Path("/usr/local/include"), Path("/usr/include")]
        installed = next((p for p in abseil if (p / "absl/strings/str_cat.h").exists()), None)
        if installed is None:
            parser.error("The original 1800 CLI requires Abseil headers; install Abseil or use Bazel")
        includes.append(installed)
    if not (root / "third_party/pugixml/src/pugixml.cpp").exists():
        parser.error("Initialize the source checkout's pinned pugixml and spdlog submodules first")
    sources = sorted((root / "cmd/xmltest/src").glob("*.cc"))
    sources += sorted((root / "libs/xml-operations/src").glob("*.cc"))
    sources.append(root / "third_party/pugixml/src/pugixml.cpp")
    output.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run([
        os.environ.get("CXX", "c++"), "-std=c++17", "-O2", "-pthread",
        *[f"-I{p}" for p in includes], *map(str, sources), "-o", str(output),
    ], check=True)
    subprocess.run([str(output), "--version"], check=True)


if __name__ == "__main__":
    main()
